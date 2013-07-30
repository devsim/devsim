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

circuit_element -name V1 -n1 topbias -n2 0 -value 0.0 -acreal 1.0 -acimag 0.0
#circuit_node_alias -node node1 -alias topbias
#circuit_node_alias -node 0 -alias botbias

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
    create_1d_mesh -mesh dog
    add_1d_mesh_line -mesh dog -pos 0 -ps 1e-7 -tag top
    add_1d_mesh_line -mesh dog -pos 0.5e-5 -ps 1e-8 -tag mid
    add_1d_mesh_line -mesh dog -pos 1e-5 -ps 1e-7 -tag bot
    add_1d_contact   -mesh dog -name top -tag top -material metal
    add_1d_contact   -mesh dog -name bot -tag bot -material metal
    add_1d_region    -mesh dog -material Si -region $region -tag1 top -tag2 bot
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
#print_node_values -device $device -region $region -name NetDoping

createPotentialOnly $device $region


createPotentialOnlyContactWithCircuit $device $region top
createPotentialOnlyContact $device $region bot



####
#### Initial DC solution
####
solve -type dc -absolute_error 1.0 -relative_error 1e-12 -maximum_iterations 30

#print_node_values -device $device -region $region -name Potential
#write_devices -file dio2_potentialonly.flps -type floops

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

createDriftDiffusionAtContactWithCircuit $device $region top
createDriftDiffusionAtContact $device $region bot


for {set v 0.0} {$v < 0.51} {set v [expr $v + 0.1]} {
circuit_alter -name V1 -value $v
solve -type dc -absolute_error 1e10 -relative_error 1e-10 -maximum_iterations 30

printCurrents $device "top" $v
printCurrents $device "bot" 0.0
printCharges $device "top" $v
printCharges $device "bot" 0.0

solve -type ac -frequency 1.0

set cap [expr [get_circuit_node_value -node V1.I -solution ssac_imag] / (2*3.14159)]
puts "capacitance $v $cap"

}
#write_devices -type tecplot -file test.dat

foreach x [get_circuit_node_list] {
    foreach y [get_circuit_solution_list] {
	foreach z [get_circuit_node_value -node $x -solution $y] {
	    puts "$x\t$y\t$z";
	}
    }
}
#write_devices -file test.dat -type tecplot

