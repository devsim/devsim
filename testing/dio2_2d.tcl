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
#####
# dio1
#
# Make doping a step function
# print dat to text file for viewing in grace
# verify currents analytically
# in dio2 add recombination
#

####
#### Meshing
####
proc createMesh {device region} {
    create_2d_mesh -mesh dog
    add_2d_mesh_line -mesh dog -dir x -pos 0 -ps 1e-6
    add_2d_mesh_line -mesh dog -dir x -pos 0.5e-5 -ps 1e-8
    add_2d_mesh_line -mesh dog -dir x -pos 1e-5 -ps 1e-6
    add_2d_mesh_line -mesh dog -dir y -pos 0 -ps 1e-6
    add_2d_mesh_line -mesh dog -dir y -pos 1e-5 -ps 1e-6

    add_2d_mesh_line -mesh dog -dir x -pos -1e-8 -ps 1e-8
    add_2d_mesh_line -mesh dog -dir x -pos 1.001e-5 -ps 1e-8

    add_2d_region    -mesh dog -material Si -region $region
    add_2d_region    -mesh dog -material Si -region air1 -xl -1e-8 -xh 0
    add_2d_region    -mesh dog -material Si -region air2 -xl 1.0e-5 -xh 1.001e-5

    add_2d_contact   -mesh dog -name top -region $region -yl 0.8e-5 -yh 1e-5 -xl 0 -xh 0 -bloat 1e-10 -material metal
    add_2d_contact   -mesh dog -name bot -region $region -xl 1e-5 -xh 1e-5 -bloat 1e-10 -material metal

    finalize_mesh -mesh dog
    create_device -mesh dog -device $device
}



set device MyDevice
set region MyRegion

createMesh $device $region

setMaterialParameters $device $region

createSolution $device $region Potential

####
#### NetDoping
####
node_model -device $device -region $region -name Acceptors -equation "1.0e18*step(0.5e-5-x);"
node_model -device $device -region $region -name Donors -equation "1.0e18*step(x-0.5e-5);"
node_model -device $device -region $region -name NetDoping -equation "Donors-Acceptors;"
print_node_values -device $device -region $region -name NetDoping

createPotentialOnly $device $region


createPotentialOnlyContact $device $region top
createPotentialOnlyContact $device $region bot

####
#### Initial DC solution
####

solve -type dc -absolute_error 1.0 -relative_error 1e-12 -maximum_iterations 30

#print_node_values -device $device -region $region -name Potential
write_devices -file dio2_2d_potentialonly.flps -type floops

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
print_node_values -device $device -region $region -name Potential
print_node_values -device $device -region $region -name Electrons
print_node_values -device $device -region $region -name Holes

#
# This is to test the solution backup
#
set_parameter -device $device -region $region -name "topbias" -value 10
catch {solve -type dc -absolute_error 1e10 -relative_error 1e-10 -maximum_iterations 30} x

puts $x

for {set v 0.0} {$v < 0.51} {set v [expr $v + 0.1]} {
set_parameter -device $device -region $region -name "topbias" -value $v
solve -type dc -absolute_error 1e10 -relative_error 1e-10 -maximum_iterations 30

printCurrents $device "top" $v
printCurrents $device "bot" 0.0
}

print_node_values -device $device -region $region -name Electrons
print_node_values -device $device -region $region -name Holes
print_node_values -device $device -region $region -name AtContactNode
print_node_values -device $device -region $region -name botnodemodel
print_node_values -device $device -region $region -name botnodemodel:Potential
print_node_values -device $device -region $region -name topnodemodel
print_node_values -device $device -region $region -name topnodemodel:Potential
print_node_values -device $device -region $region -name Potential
#print_node_values -device $device -region $region -name chole_top
#print_node_values -device $device -region $region -name chole_bot
#print_node_values -device $device -region $region -name botnodeholes
#print_edge_values -device $device -region $region -name topnodeholecurrent
#print_edge_values -device $device -region $region -name botnodeholecurrent
#print_edge_values -device $device -region $region -name HoleCurrent
print_edge_values -device $device -region $region -name ElectronCurrent
print_edge_values -device $device -region $region -name HoleCurrent
print_node_values -device $device -region $region -name USRH
#print_node_values -device $device -region $region -name NodeVolume
#node_model -device $device -region $region -name "netsrh" \
	   -equation "USRH*NodeVolume;"
#print_node_values -device $device -region $region -name netsrh
write_devices -file dio2_2d_dd.flps -type floops
write_devices -file dio2_2d_dd -type vtk

