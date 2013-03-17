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

source dio2_physics.tcl


set device resistor3d
set region Bulk

load_devices -file gmsh_resistor3d_out.msh

setMaterialParameters $device $region

createSolution $device $region Potential

####
#### NetDoping
####
node_model -device $device -region $region -name Acceptors -equation "0;"
node_model -device $device -region $region -name Donors -equation "1.0e18;"
node_model -device $device -region $region -name NetDoping -equation "Donors-Acceptors;"
#print_node_values -device $device -region $region -name NetDoping

createPotentialOnly $device $region


createPotentialOnlyContact $device $region top
createPotentialOnlyContact $device $region bot

####
#### Initial DC solution
####

solve -type dc -absolute_error 1.0 -relative_error 1e-12 -maximum_iterations 30

#print_node_values -device $device -region $region -name Potential
write_devices -file gmsh_resistor3d_potentialonly -type vtk

#### need way to clear out old models and their derivatives

####
#### drift diffusion
####
createSolution $device $region Electrons
createSolution $device $region Holes

####
#### create initial guess from dc only solution
####
set_node_values -device $device -region $region -name Electrons -init_from IntrinsicElectrons
set_node_values -device $device -region $region -name Holes     -init_from IntrinsicHoles

createDriftDiffusion $device $region

createDriftDiffusionAtContact $device $region top
createDriftDiffusionAtContact $device $region bot



set_parameter -device $device -region $region -name "topbias" -value 0
#print_node_values -device $device -region $region -name Potential
#print_node_values -device $device -region $region -name Electrons
#print_node_values -device $device -region $region -name Holes

#
# This is to test the solution backup
#
#set_parameter -device $device -region $region -name "topbias" -value 10
#catch {solve -type dc -absolute_error 1e10 -relative_error 1e-10 -maximum_iterations 30} x

#puts $x

for {set v 0.0} {$v < 0.11} {set v [expr $v + 0.1]} {
set_parameter -device $device -region $region -name "topbias" -value $v
solve -type dc -absolute_error 1e10 -relative_error 1e-10 -maximum_iterations 30

printCurrents $device "top" $v
printCurrents $device "bot" 0.0
}

#print_node_values -device $device -region $region -name Electrons
#print_node_values -device $device -region $region -name Holes
#print_node_values -device $device -region $region -name AtContactNode
#print_node_values -device $device -region $region -name botnodemodel
#print_node_values -device $device -region $region -name botnodemodel:Potential
#print_node_values -device $device -region $region -name topnodemodel
#print_node_values -device $device -region $region -name topnodemodel:Potential
#print_node_values -device $device -region $region -name Potential
#print_node_values -device $device -region $region -name chole_top
#print_node_values -device $device -region $region -name chole_bot
#print_node_values -device $device -region $region -name botnodeholes
#print_edge_values -device $device -region $region -name topnodeholecurrent
#print_edge_values -device $device -region $region -name botnodeholecurrent
#print_edge_values -device $device -region $region -name HoleCurrent
#print_edge_values -device $device -region $region -name ElectronCurrent
#print_edge_values -device $device -region $region -name HoleCurrent
#print_node_values -device $device -region $region -name USRH
#print_node_values -device $device -region $region -name NodeVolume
#node_model -device $device -region $region -name "netsrh" \
	   -equation "USRH*NodeVolume;"
#print_node_values -device $device -region $region -name netsrh
#write_devices -file gmsh_resistor3d_dd -type vtk

proc sum list {
  set sum 0
  foreach i $list {
      set sum [expr {$sum+$i}]
  }
  set sum
}


set x [sum [get_node_model_values -device $device -region $region -name NodeVolume]]
puts "Volume $x"

