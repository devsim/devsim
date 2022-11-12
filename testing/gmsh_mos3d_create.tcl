# Copyright 2013 DEVSIM LLC
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

set device mos3d

set device_width    1.0e-4
set gate_width      1.0e-5
set diffusion_width 0.4

set air_thickness       1e-7
set oxide_thickness     1e-5
set gate_thickness      1e-5
set device_thickness    1e-4
set diffusion_thickness 1e-5

set x_diffusion_decay   1e-20
set y_diffusion_decay   1e-10

set bulk_doping         -1e15
set body_doping         -1e19
set drain_doping        1e19 
set source_doping       1e19 
set gate_doping         1e20

set y_channel_spacing     1e-8
set y_diffusion_spacing   1e-6
set y_gate_top_spacing    1e-8
set y_gate_mid_spacing    [expr $gate_thickness * 0.25]
set y_gate_bot_spacing    1e-8
set y_oxide_mid_spacing   [expr $oxide_thickness * 0.25]
set x_channel_spacing   1e-6
set x_diffusion_spacing 1e-5
set max_y_spacing       1e-4
set max_x_spacing       1e-2
set y_bulk_mid_spacing [expr $device_thickness * 0.25]
set y_bulk_bottom_spacing 1e-8

set x_bulk_left   0.0
set x_bulk_right  [expr $x_bulk_left + $device_width]
set x_center      [expr 0.5 * ($x_bulk_left + $x_bulk_right)]
set x_gate_left   [expr $x_center - 0.5 * ($gate_width)]
set x_gate_right  [expr $x_center + 0.5 * ($gate_width)]
set x_device_left [expr $x_bulk_left - $air_thickness]
set x_device_right [expr $x_bulk_right + $air_thickness]

set y_bulk_top      0.0
set y_oxide_top     [expr $y_bulk_top - $oxide_thickness]
set y_oxide_mid     [expr 0.5 * ($y_oxide_top + $y_bulk_top)]
set y_gate_top      [expr $y_oxide_top - $gate_thickness]
set y_gate_mid      [expr 0.5 * ($y_gate_top + $y_oxide_top)]
set y_device_top    [expr $y_gate_top - $air_thickness]
set y_bulk_bottom   [expr $y_bulk_top + $device_thickness]
set y_bulk_mid      [expr 0.5 * ($y_bulk_top + $y_bulk_bottom)]
set y_device_bottom [expr $y_bulk_bottom + $air_thickness]
set y_diffusion     [expr $y_bulk_top + $diffusion_thickness]

create_gmsh_mesh -file gmsh_mos3d.msh -mesh mos3d
add_gmsh_region    -mesh mos3d -gmsh_name "bulk"    -region "bulk" -material Silicon
add_gmsh_region    -mesh mos3d -gmsh_name "oxide"    -region "oxide" -material Silicon
add_gmsh_region    -mesh mos3d -gmsh_name "gate"    -region "gate" -material Silicon
add_gmsh_contact   -mesh mos3d -gmsh_name "drain_contact"  -region "bulk" -name "drain" -material "metal"
add_gmsh_contact   -mesh mos3d -gmsh_name "source_contact" -region "bulk" -name "source" -material "metal"
add_gmsh_contact   -mesh mos3d -gmsh_name "body_contact"   -region "bulk" -name "body" -material "metal"
add_gmsh_contact   -mesh mos3d -gmsh_name "gate_contact"   -region "gate" -name "gate" -material "metal"
add_gmsh_interface -mesh mos3d -gmsh_name "gate_oxide_interface" -region0 "gate" -region1 "oxide" -name "gate_oxide"
add_gmsh_interface -mesh mos3d -gmsh_name "bulk_oxide_interface" -region0 "bulk" -region1 "oxide" -name "bulk_oxide"
finalize_mesh -mesh mos3d
create_device -mesh mos3d -device mos3d

node_model -name NetDoping    -device $device -region gate -equation "$gate_doping;"
node_model -name DrainDoping  -device $device -region bulk -equation "0.25*$drain_doping*erfc((x-$x_gate_left)/$x_diffusion_decay)*erfc((y-$y_diffusion)/$y_diffusion_decay);"
node_model -name SourceDoping -device $device -region bulk -equation "0.25*$source_doping*erfc(-(x-$x_gate_right)/$x_diffusion_decay)*erfc((y-$y_diffusion)/$y_diffusion_decay);"
node_model -name BodyDoping -device $device -region bulk -equation "0.5*$body_doping*erfc(-(y-$y_bulk_bottom)/$y_diffusion_decay);"
node_model -name NetDoping    -device $device -region bulk -equation "DrainDoping + SourceDoping + $bulk_doping + BodyDoping;"

write_devices -file gmsh_mos3d_out.msh
write_devices -file gmsh_mos3d_out -type vtk

