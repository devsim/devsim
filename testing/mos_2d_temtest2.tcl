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

set device mymos
load_devices -file mos_2d_dd.msh
#source mos_2d_create.tcl
# TODO: write out mesh, and then read back in as separate test
source mos_2d_physics.tcl
#setMaterialParameters $device gate
#setMaterialParameters $device bulk
#
#
#setMaterialParameters $device gate
#setMaterialParameters $device bulk
#setMaterialParameters $device $region
#
#createSolution $device gate  Potential
#createSolution $device bulk  Potential
#createSolution $device oxide Potential
#
#

#createSolution $device gate Potential
#createSolution $device bulk Potential
#createSolution $device oxide Potential

setSiliconParameters $device gate
setSiliconParameters $device bulk
setOxideParameters $device oxide

createSiliconPotentialOnly $device bulk
createSiliconPotentialOnly $device gate

createOxidePotentialOnly $device oxide

createSiliconPotentialOnlyContact $device gate gate
createSiliconPotentialOnlyContact $device bulk drain
createSiliconPotentialOnlyContact $device bulk source
createSiliconPotentialOnlyContact $device bulk body

createSiliconOxideInterface $device bulk_oxide
createSiliconOxideInterface $device gate_oxide

#solve -type dc -absolute_error 1.0e-13 -relative_error 1e-12 -maximum_iterations 30
#solve -type dc -absolute_error 1.0e-13 -relative_error 1e-12 -maximum_iterations 30

#write_devices -file mos_2d_potentialonly.flps -type floops
#write_devices -file mos_2d_potentialonly -type vtk

#createSolution $device gate Electrons
#createSolution $device gate Holes
#set_node_values -device $device -region gate -name Electrons -init_from IntrinsicElectrons
#set_node_values -device $device -region gate -name Holes     -init_from IntrinsicHoles
createSiliconDriftDiffusion $device gate
createSiliconDriftDiffusionAtContact $device gate gate

#createSolution $device bulk Electrons
#createSolution $device bulk Holes
#set_node_values -device $device -region bulk -name Electrons -init_from IntrinsicElectrons
#set_node_values -device $device -region bulk -name Holes     -init_from IntrinsicHoles
createSiliconDriftDiffusion $device bulk


createSiliconDriftDiffusionAtContact $device bulk drain
createSiliconDriftDiffusionAtContact $device bulk source
createSiliconDriftDiffusionAtContact $device bulk body
solve -type dc -absolute_error 1.0e30 -relative_error 1e-5 -maximum_iterations 30

element_from_edge_model -edge_model ElectricField -device $device -region bulk
element_from_edge_model -edge_model ElectricField -derivative Potential -device $device -region bulk

element_from_node_model -node_model x -device $device -region bulk
element_from_node_model -node_model y -device $device -region bulk
element_from_node_model -node_model Potential -device $device -region bulk
set x0 [get_element_model_values -device $device -region bulk -name "x@en0"]
set x1 [get_element_model_values -device $device -region bulk -name "x@en1"]
set x2 [get_element_model_values -device $device -region bulk -name "x@en2"]

set y0 [get_element_model_values -device $device -region bulk -name "y@en0"]
set y1 [get_element_model_values -device $device -region bulk -name "y@en1"]
set y2 [get_element_model_values -device $device -region bulk -name "y@en2"]

set p0 [get_element_model_values -device $device -region bulk -name "Potential@en0"]
set p1 [get_element_model_values -device $device -region bulk -name "Potential@en1"]
set p2 [get_element_model_values -device $device -region bulk -name "Potential@en2"]

set ex [get_element_model_values -device $device -region bulk -name "ElectricField_x"]
set ey [get_element_model_values -device $device -region bulk -name "ElectricField_y"]

set exn0 [get_element_model_values -device $device -region bulk -name "ElectricField_x:Potential@en0"]
set eyn0 [get_element_model_values -device $device -region bulk -name "ElectricField_y:Potential@en0"]
set exn1 [get_element_model_values -device $device -region bulk -name "ElectricField_x:Potential@en1"]
set eyn1 [get_element_model_values -device $device -region bulk -name "ElectricField_y:Potential@en1"]
set exn2 [get_element_model_values -device $device -region bulk -name "ElectricField_x:Potential@en2"]
set eyn2 [get_element_model_values -device $device -region bulk -name "ElectricField_y:Potential@en2"]

set names {x0 x1 x2 y0 y1 y2 p0 p1 p2 ex ey exn0 eyn0 exn1 eyn1 exn2 eyn2}

set len [llength $x0]
puts $len

puts -nonewline "#"
foreach i $names {
  puts -nonewline " $i"
#  puts
}
puts ""

for {set n 0} {$n < $len} {incr n} {
#  puts  $n
  foreach i $names {
#    puts "$i $n"
#    puts $$i
#    puts [subst $$i]
    set v [lindex [subst $$i] $n]
    puts -nonewline " $v"
  }
  puts ""
}


