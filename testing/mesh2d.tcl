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

set device MyDevice
set region MyRegion

set xmin 0.0
set xmax 1.0
set ymin 0.0
### not that ymid my not exactly line up with a 0.5 gathered through summation, so use closes bloat with triangle rule to claim ownership
set ymid1 0.5
set ymid2 0.6
set ymax 1.0
# have user use tcl variables instead of tags
# use dynamic cast to verify that command is accepted by our mesh class
#add_mesh_point
# future version allow reading mesh points and automatic triangulation
create_2d_mesh -mesh dog
add_2d_mesh_line -mesh dog -dir y -pos -0.001 -ps 0.001
add_2d_mesh_line -mesh dog -dir x -pos $xmin -ps 0.1
add_2d_mesh_line -mesh dog -dir x -pos $xmax -ps 0.1
add_2d_mesh_line -mesh dog -dir y -pos $ymin -ps 0.1
add_2d_mesh_line -mesh dog -dir y -pos $ymax -ps 0.1
#in order to capture interfaces
add_2d_mesh_line -mesh dog -dir y -pos +1.001 -ps 0.001
# color triangles not nodes or edges
# error if region doesn't have any triangles
# last add_region has highest precedence
# allow use of names or tags
# check my parser to verify that adding is possible (via dynamic_cast) 
# note this region won't have equation numbers
add_2d_region    -mesh dog -material gas     -region gas1  -yl -.001 -yh 0.0
add_2d_region    -mesh dog -material gas     -region gas2  -yl 1.0 -yh 1.001
add_2d_region    -mesh dog -material Oxide   -region r0 -xl $xmin -xh $xmax -yl $ymid1 -yh $ymin
add_2d_region    -mesh dog -material Silicon -region r1 -xl $xmin -xh $xmax -yl $ymid2 -yh $ymid1
add_2d_region    -mesh dog -material Silicon -region r2 -xl $xmin -xh $xmax -yl $ymid2   -yh $ymax

#warn when leftover triangles are not encapsulated by region
# not specifying bounding box means all intersecting nodes
#bounding box is optional
add_2d_interface -mesh dog -name i0 -region0 r0 -region1 r1
add_2d_interface -mesh dog -name i1 -region0 r1 -region1 r2 -xl 0 -xh 1 -yl $ymid2 -yh $ymid2 -bloat 1.0e-10
add_2d_contact -mesh dog -name top -region r0 -yl $ymin -yh $ymin -bloat 1.0e-10 -material metal
add_2d_contact -mesh dog -name bot -region r2 -yl $ymax -yh $ymax -bloat 1.0e-10 -material metal
#contact can only apply interface
finalize_mesh -mesh dog
create_device -mesh dog -device $device

node_model -device $device -name NetDoping -region gas1 -equation "1e18;"
node_model -device $device -name NetDoping -region r0 -equation "1e18;"
node_model -device $device -name NetDoping -region r1 -equation "-1e15;"
node_model -device $device -name NetDoping -region r2 -equation "-1e15;"
node_model -device $device -name NetDoping -region gas2 -equation "-1e15;"

interface_normal_model -device $device -region r0 -interface i0
interface_normal_model -device $device -region r1 -interface i0
interface_normal_model -device $device -region r1 -interface i1
interface_normal_model -device $device -region r2 -interface i1

#print_node_values -device $device -region r0 -name NodeVolume
#print_edge_values -device $device -region r0 -name EdgeCouple
#print_edge_values -device $device -region r1 -name EdgeCouple
#print_edge_values -device $device -region r2 -name EdgeCouple
set_parameter -name raxis_zero -value 0.0
set_parameter -name raxis_variable -value x
foreach region {r0 r1 r2} {
cylindrical_node_volume  -device $device -region $region
cylindrical_edge_couple  -device $device -region $region
cylindrical_surface_area -device $device -region $region
}


write_devices -file mesh2d.flps -type floops
write_devices -file mesh2d.msh -type devsim
write_devices -file mesh2d.dat -type tecplot
#write_devices -file mesh2d -type vtk

