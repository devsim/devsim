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

node_model -device $device -region $region -name NetDoping1 -equation "kahan3(1e20, -1e20, x);"
puts [get_node_model_values -device $device -region $region -name NetDoping1]
node_model -device $device -region $region -name NetDoping2 -equation "kahan4(1e20, -1e20, x,1e14);"
puts [get_node_model_values -device $device -region $region -name NetDoping2]

node_model -device $device -region $region -name NetDoping3 -equation "kahan3(x, 1e20, -1e20);"
puts [get_node_model_values -device $device -region $region -name NetDoping3]

node_model -device $device -region $region -name NetDoping4 -equation "kahan4(x, 1e20, -1e20, 1e14);"
puts [get_node_model_values -device $device -region $region -name NetDoping4]

puts [symdiff -expr "diff(kahan3(a,b,c),c);"]
puts [symdiff -expr "diff(kahan4(a,b,c,d),c);"]
puts [symdiff -expr "diff(kahan4(a,b,c,c),c);"]
