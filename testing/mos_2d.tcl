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

element_from_edge_model -edge_model ElectricField -device $device -region bulk

#write_devices -file mos_2d_dd.flps -type floops
#write_devices -file mos_2d_dd -type vtk
write_devices -file mos_2d_dd.msh -type devsim
#write_devices -file mos_2d_dd.dat -type tecplot

set fh [open mos_2d_params.tcl w]

puts "GLOBAL:"
foreach param [get_parameter_list] {
  puts $fh [format "set_parameter -name %s -value %s" $param [get_parameter -name $param]]
}

set devicelist [get_device_list]
foreach i $devicelist {
  puts "DEVICE: $i"

  foreach param [get_parameter_list -device $i] {
    puts $fh [format "set_parameter -device %s -name %s -value %s" $i $param [get_parameter -device $i -name $param]]
  }

  set regionlist [get_region_list -device $i]
  foreach j $regionlist {
    set material [get_material -device $i -region $j]
    puts "  REGION: $j $material"
    foreach param [get_parameter_list -device $i -region $j] {
#      puts "$i $j $param"
      puts $fh [format "set_parameter -device %s -region %s -name %s -value %s" $i $j $param [get_parameter -device $i -region $j -name $param]]
    }
  }

close $fh

  set contactlist [get_contact_list -device $i]
  foreach j $contactlist {
    set contactregion [get_region_list -device $i -contact $j]
    puts "  CONTACT: $j ${contactregion}"
  }

  set interfacelist [get_interface_list -device $i]
  foreach j $interfacelist {
    set interfaceregions [get_region_list -device $i -interface $j]
    puts "  INTERFACE: $j $interfaceregions"
  }
}

