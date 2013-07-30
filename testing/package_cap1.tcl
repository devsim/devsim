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

lappend auto_path .
package require dsBase
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

createSolution $device $region Potential


set efield "(Potential@n0 - Potential@n1)*EdgeInverseLength"
# rewrite so use a getter on existing expression
createEdgeModel            $device $region ElectricField $efield
createEdgeModelDerivatives $device $region ElectricField $efield Potential

set_parameter -device $device -region $region -name topbias -value 1.0e-0
set_parameter -device $device -region $region -name botbias -value 0.0

set dfield "Permittivity*ElectricField"
createEdgeModel            $device $region PotentialEdgeFlux $dfield
createEdgeModelDerivatives $device $region PotentialEdgeFlux $dfield Potential

equation -device $device -region $region -name PotentialEquation -variable_name Potential -node_model "" \
    -edge_model "PotentialEdgeFlux" -time_node_model "" -variable_update default 

set conteq "Potential - topbias"
createNodeModel           $device $region "topnode_model" $conteq
createNodeModelDerivative $device $region "topnode_model" $conteq Potential

set conteq "Potential - botbias"
createNodeModel           $device $region "botnode_model" $conteq
createNodeModelDerivative $device $region "botnode_model" $conteq Potential

set conteq "Permittivity*ElectricField"
createContactEdgeModel $device top "contactcharge_edge_top"    $conteq
createContactEdgeModel $device bot "contactcharge_edge_bot" $conteq

contact_equation -device $device -contact "top" -name "PotentialEquation" -variable_name Potential \
			-node_model topnode_model -edge_model "" \
			-node_charge_model "" -edge_charge_model "contactcharge_edge_top" \
			-node_current_model ""   -edge_current_model ""

contact_equation -device $device -contact "bot" -name "PotentialEquation" -variable_name Potential \
			-node_model botnode_model -edge_model "" \
			-node_charge_model "" -edge_charge_model "contactcharge_edge_bot" \
			-node_current_model ""   -edge_current_model ""

solve -type dc -absolute_error 1.0 -relative_error 1e-10 -maximum_iterations 30

puts [get_contact_charge -device $device -contact top -equation PotentialEquation]
puts [get_contact_charge -device $device -contact bot -equation PotentialEquation]

