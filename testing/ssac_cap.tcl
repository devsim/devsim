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

# cap device to test displacement current at contact
#add_circuit_node -name topbias -variable_update default
set device MyDevice
set region MyRegion

create_1d_mesh -mesh dog
add_1d_mesh_line -mesh dog -pos 0 -ps 0.1 -tag top
add_1d_mesh_line -mesh dog -pos 1 -ps 0.1 -tag bot
add_1d_contact   -mesh dog -name top -tag top -material metal
add_1d_contact   -mesh dog -name bot -tag bot -material metal
add_1d_region    -mesh dog -material Si -region $region -tag1 top -tag2 bot
finalize_mesh -mesh dog
create_device -mesh dog -device $device

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

#set_parameter -device $device -region $region -name topbias -value 1.0e-0
set_parameter -device $device -region $region -name bottombias -value 0.0

edge_model -device $device -region $region -name PotentialEdgeFlux -equation "Permittivity*ElectricField;"
edge_model -device $device -region $region -name PotentialEdgeFlux:Potential@n0 -equation "diff(Permittivity*ElectricField, Potential@n0);"
edge_model -device $device -region $region -name PotentialEdgeFlux:Potential@n1 -equation "-PotentialEdgeFlux:Potential@n0;"

equation -device $device -region $region -name PotentialEquation -variable_name Potential -node_model "" \
    -edge_model "PotentialEdgeFlux" -time_node_model "" -variable_update default 

set conteq "Permittivity*ElectricField;"
node_model -device $device -region $region -name "topnode_model"           -equation "Potential - topbias;"
node_model -device $device -region $region -name "topnode_model:Potential" -equation "1;"
node_model -device $device -region $region -name "topnode_model:topbias"   -equation "-1;"
edge_model -device $device -region $region -name "contactcharge_edge_top"  -equation $conteq
edge_model -device $device -region $region -name "contactcharge_edge_top:Potential@n0"  -equation "Permittivity*ElectricField:Potential@n0;"
edge_model -device $device -region $region -name "contactcharge_edge_top:Potential@n1"  -equation "Permittivity*ElectricField:Potential@n1;"

#
node_model -device $device -region $region -name "bottomnode_model"           -equation "Potential - bottombias;"
node_model -device $device -region $region -name "bottomnode_model:Potential" -equation "1;"
edge_model -device $device -region $region -name "contactcharge_edge_bottom"  -equation $conteq

contact_equation -device $device -contact "top" -name "PotentialEquation" -variable_name Potential \
			-node_model topnode_model -edge_model "" \
			-node_charge_model "" -edge_charge_model "contactcharge_edge_top" \
			-node_current_model ""   -edge_current_model "" -circuit_node topbias

contact_equation -device $device -contact "bot" -name "PotentialEquation" -variable_name Potential \
			-node_model bottomnode_model -edge_model "" \
			-node_charge_model "" -edge_charge_model "contactcharge_edge_bottom" \
			-node_current_model ""   -edge_current_model ""



circuit_element -name V1 -n1 1 -n2 0 -value 1.0 -acreal 1.0
circuit_element -name R1 -n1 topbias -n2 1 -value 1e3

solve -type dc -absolute_error 1.0 -relative_error 1e-10 -maximum_iterations 30

puts [get_contact_charge -device $device -contact top -equation PotentialEquation]
puts [get_contact_charge -device $device -contact bot -equation PotentialEquation]

solve -type ac -frequency 1e10
solve -type ac -frequency 1e15

