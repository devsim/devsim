# Copyright 2013 Devsim LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

load_devices -file mesh1.msh
set x [get_device_list]
foreach y [get_region_list -device $x] {
    puts "$x $y"
}

set device MyDevice
set region MyRegion

set_parameter -device $device -region $region   -name "Permittivity" -value [expr  3.9*8.85e-14]
set_parameter -device $device -region $region   -name "ElectricCharge" -value 1.6e-19

puts [get_parameter -device $device -region $region -name "Permittivity" ]
puts [get_parameter -device $device -region $region -name "ElectricCharge" ]

node_solution -device $device -region $region -name Potential

edge_from_node_model -device $device -region $region -node_model Potential

edge_model -device $device -region $region -name ElectricField \
		 -equation "(Potential@n0 - Potential@n1)*EdgeInverseLength;"

edge_model -device $device -region $region -name "ElectricField:Potential@n0" \
		 -equation "EdgeInverseLength;"

edge_model -device $device -region $region -name "ElectricField:Potential@n1" \
		 -equation "-EdgeInverseLength;"

set_parameter -device $device -region $region -name topbias -value 1.0e-0
set_parameter -device $device -region $region -name bottombias -value 0.0

edge_model -device $device -region $region -name PotentialEdgeFlux -equation "Permittivity*ElectricField;"
edge_model -device $device -region $region -name PotentialEdgeFlux:Potential@n0 -equation "diff(Permittivity*ElectricField, Potential@n0);"
edge_model -device $device -region $region -name PotentialEdgeFlux:Potential@n1 -equation "-PotentialEdgeFlux:Potential@n0;"

equation -device $device -region $region -name PotentialEquation -variable_name Potential -node_model "" \
    -edge_model "PotentialEdgeFlux" -time_node_model "" -variable_update default 

set conteq "Permittivity*ElectricField;"
node_model -device $device -region $region -name "topnode_model"           -equation "Potential - topbias;"
node_model -device $device -region $region -name "topnode_model:Potential" -equation "1;"
edge_model -device $device -region $region -name "contactcharge_edge_top"  -equation $conteq
#
node_model -device $device -region $region -name "bottomnode_model"           -equation "Potential - bottombias;"
node_model -device $device -region $region -name "bottomnode_model:Potential" -equation "1;"
edge_model -device $device -region $region -name "contactcharge_edge_bottom"  -equation $conteq

contact_equation -device $device -contact "top" -name "PotentialEquation" -variable_name Potential \
			-node_model topnode_model -edge_model "" \
			-node_charge_model "" -edge_charge_model "contactcharge_edge_top" \
			-node_current_model ""   -edge_current_model ""

contact_equation -device $device -contact "bot" -name "PotentialEquation" -variable_name Potential \
			-node_model bottomnode_model -edge_model "" \
			-node_charge_model "" -edge_charge_model "contactcharge_edge_bottom" \
			-node_current_model ""   -edge_current_model ""



solve -type dc -absolute_error 1.0 -relative_error 1e-10 -maximum_iterations 30

puts [get_contact_charge -device $device -contact top -equation PotentialEquation]
puts [get_contact_charge -device $device -contact bot -equation PotentialEquation]

print_node_values -device MyDevice -region MyRegion -name NodeVolume
print_edge_values -device MyDevice -region MyRegion -name ElectricField
print_edge_values -device MyDevice -region MyRegion -name EdgeCouple
