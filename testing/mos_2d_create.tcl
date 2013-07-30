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

create_2d_mesh -mesh mos
add_2d_mesh_line -mesh mos -dir y -pos $y_device_top -ps $max_y_spacing
add_2d_mesh_line -mesh mos -dir y -pos $y_gate_top -ps $y_gate_top_spacing
add_2d_mesh_line -mesh mos -dir y -pos $y_gate_mid -ps $y_gate_mid_spacing
add_2d_mesh_line -mesh mos -dir y -pos $y_oxide_top -ps $y_gate_bot_spacing -ns $y_oxide_mid_spacing
add_2d_mesh_line -mesh mos -dir y -pos $y_oxide_mid -ps $y_oxide_mid_spacing
add_2d_mesh_line -mesh mos -dir y -pos $y_bulk_top -ns $y_oxide_mid_spacing -ps $y_channel_spacing
add_2d_mesh_line -mesh mos -dir y -pos $y_diffusion -ps $y_diffusion_spacing
add_2d_mesh_line -mesh mos -dir y -pos $y_bulk_mid -ps $y_bulk_mid_spacing
add_2d_mesh_line -mesh mos -dir y -pos $y_bulk_bottom -ns $y_bulk_bottom_spacing -ps $max_y_spacing
add_2d_mesh_line -mesh mos -dir y -pos $y_device_bottom -ps $max_y_spacing

add_2d_mesh_line -mesh mos -dir x -pos $x_center -ps $x_channel_spacing
add_2d_mesh_line -mesh mos -dir x -pos $x_gate_left -ps $x_channel_spacing
add_2d_mesh_line -mesh mos -dir x -pos $x_bulk_left -ps $x_diffusion_spacing
add_2d_mesh_line -mesh mos -dir x -pos $x_gate_right -ps $x_channel_spacing
add_2d_mesh_line -mesh mos -dir x -pos $x_bulk_right -ps $x_diffusion_spacing
add_2d_mesh_line -mesh mos -dir x -pos $x_device_right -ps $max_x_spacing
add_2d_mesh_line -mesh mos -dir x -pos $x_device_left -ps $max_x_spacing

add_2d_region -mesh mos -material Air     -region air
add_2d_region -mesh mos -material Silicon -region bulk -xl $x_bulk_left -xh $x_bulk_right -yl $y_bulk_bottom -yh $y_bulk_top
add_2d_region -mesh mos -material Silicon -region gate -xl $x_gate_left -xh $x_gate_right -yl $y_oxide_top -yh $y_gate_top
add_2d_region -mesh mos -material Oxide -region oxide -xl $x_gate_left -xh $x_gate_right -yl $y_bulk_top -yh $y_oxide_top
#add_2d_region -mesh mos -material Air -region air_top -xl $x_device_left -xh $x_device_right -yl $y_bulk_top -yh $y_device_top
#add_2d_region -mesh mos -material Air -region air_bottom -xl $x_device_left -xh $x_device_right -yl $y_device_bottom -yh $y_bulk_bottom
#add_2d_region -mesh mos -material Air -region air_left -xl $x_device_left -xh $x_bulk_left -yl $y_bulk_bottom -yh $y_bulk_top
#add_2d_region -mesh mos -material Air -region air_right -xl $x_bulk_right -xh $x_device_right -yl $y_bulk_bottom -yh $y_bulk_top

add_2d_contact -mesh mos -name gate -region gate -yl $y_gate_top -yh $y_gate_top -material metal
add_2d_contact -mesh mos -name body -region bulk -yl $y_bulk_bottom -yh $y_bulk_bottom -material metal
add_2d_contact -mesh mos -name source -region bulk -yl $y_bulk_top -yh $y_bulk_top -xl $x_device_left -xh $x_gate_left -material metal
add_2d_contact -mesh mos -name drain -region bulk -yl $y_bulk_top -yh $y_bulk_top -xl $x_gate_right -xh $x_device_right -material metal

add_2d_interface -mesh mos -name gate_oxide -region0 gate -region1 oxide
add_2d_interface -mesh mos -name bulk_oxide -region0 bulk -region1 oxide
finalize_mesh -mesh mos
create_device -mesh mos -device $device

#node_model -name myval -device $device -region gate -equation "1;"
#node_model -name myval -device $device -region oxide -equation "2;"
#node_model -name myval -device $device -region bulk -equation "3;"
#node_model -name myval -device $device -region air -equation "0;"

node_model -name NetDoping    -device $device -region gate -equation "$gate_doping;"
node_model -name DrainDoping  -device $device -region bulk -equation "0.25*$drain_doping*erfc((x-$x_gate_left)/$x_diffusion_decay)*erfc((y-$y_diffusion)/$y_diffusion_decay);"
node_model -name SourceDoping -device $device -region bulk -equation "0.25*$source_doping*erfc(-(x-$x_gate_right)/$x_diffusion_decay)*erfc((y-$y_diffusion)/$y_diffusion_decay);"
node_model -name BodyDoping -device $device -region bulk -equation "0.5*$body_doping*erfc(-(y-$y_bulk_bottom)/$y_diffusion_decay);"
node_model -name NetDoping    -device $device -region bulk -equation "DrainDoping + SourceDoping + $bulk_doping + BodyDoping;"

write_devices -file mos_2d -type vtk
write_devices -file mos_2d.flps -type floops

