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

#set tcl_precision 17
source mos_2d_create.tcl
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

createSolution $device gate Potential
createSolution $device bulk Potential
createSolution $device oxide Potential

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

solve -type dc -absolute_error 1.0e-13 -relative_error 1e-12 -maximum_iterations 30
solve -type dc -absolute_error 1.0e-13 -relative_error 1e-12 -maximum_iterations 30

write_devices -file mos_2d_potentialonly.flps -type floops
write_devices -file mos_2d_potentialonly -type vtk

createSolution $device gate Electrons
createSolution $device gate Holes
set_node_values -device $device -region gate -name Electrons -init_from IntrinsicElectrons
set_node_values -device $device -region gate -name Holes     -init_from IntrinsicHoles
createSiliconDriftDiffusion $device gate
createSiliconDriftDiffusionAtContact $device gate gate

createSolution $device bulk Electrons
createSolution $device bulk Holes
set_node_values -device $device -region bulk -name Electrons -init_from IntrinsicElectrons
set_node_values -device $device -region bulk -name Holes     -init_from IntrinsicHoles
createSiliconDriftDiffusion $device bulk


createSiliconDriftDiffusionAtContact $device bulk drain
createSiliconDriftDiffusionAtContact $device bulk source
createSiliconDriftDiffusionAtContact $device bulk body

solve -type dc -absolute_error 1.0e30 -relative_error 1e-5 -maximum_iterations 30

write_devices -file mos_2d_dd.flps -type floops
write_devices -file mos_2d_dd -type vtk

set_parameter -device $device -region bulk -name "drainbias" -value 0.0001
for {set v 0.0} {$v < 1.01} {set v [expr $v + 0.1]} {
set_parameter -device $device -region gate -name "gatebias" -value $v
solve -type dc -absolute_error 1e30 -relative_error 1e-4 -maximum_iterations 30

printCurrents $device "gate" [get_parameter -device $device -region gate -name "gatebias"]
printCurrents $device "drain" [get_parameter -device $device -region bulk -name "drainbias"]
printCurrents $device "source" 0.0
printCurrents $device "body" 0.0
}

write_devices -file "mos_2d_1.0_0.0" -type vtk

for {set v 1.0} {$v < 2.01} {set v [expr $v + 0.1]} {
set_parameter -device $device -region gate -name "gatebias" -value $v
solve -type dc -absolute_error 1e30 -relative_error 1e-4 -maximum_iterations 30

printCurrents $device "gate" [get_parameter -device $device -region gate -name "gatebias"]
printCurrents $device "drain" [get_parameter -device $device -region bulk -name "drainbias"]
printCurrents $device "source" 0.0
printCurrents $device "body" 0.0
}

write_devices -file "mos_2d_2.0_0.0" -type vtk

for {set v 0.01} {$v < 0.41} {set v [expr $v + 0.01]} {
puts "\n\nsetting $v\n\n"
set_parameter -device $device -region bulk -name "drainbias" -value $v
solve -type dc -absolute_error 1e30 -relative_error 1e-4 -maximum_iterations 30

printCurrents $device "gate" [get_parameter -device $device  -region gate  -name "gatebias"]
printCurrents $device "drain" [get_parameter -device $device -region bulk -name "drainbias"]
printCurrents $device "source" 0.0
printCurrents $device "body" 0.0
}

#write_devices -file "mos_2d_0.5_0.4.flps" -type floops
write_devices -file "mos_2d_2.0_0.4" -type vtk

