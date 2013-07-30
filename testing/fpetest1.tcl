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

####
#### Meshing
####

create_1d_mesh -mesh dog
add_1d_mesh_line -mesh dog -pos 0 -ps 0.1 -tag top
add_1d_mesh_line -mesh dog -pos 1 -ps 0.1 -tag bot
add_1d_contact   -mesh dog -name top -tag top -material metal
add_1d_contact   -mesh dog -name bot -tag bot -material metal
add_1d_region    -mesh dog -material Si -region $region -tag1 top -tag2 bot
finalize_mesh -mesh dog
create_device -mesh dog -device $device

node_model -device $device -region $region -name test1 -equation "log(-1);"
catch { set x [get_node_model_values -device $device -region $region -name test1]} y
puts $y

node_model -device $device -region $region -name test2 -equation "log(x);"
catch { set x [get_node_model_values -device $device -region $region -name test2]} y
puts $y
