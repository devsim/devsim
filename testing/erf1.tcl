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

# basic linear circuit solved by itself

circuit_element -name V1 -n1 1 -n2 0 -value 1.0
circuit_element -name R1 -n1 1 -n2 cnode1 -value 5
circuit_element -name R2 -n1 cnode1 -n2 0 -value 5

solve -type dc -absolute_error 1.0 -relative_error 1e-14 -maximum_iterations 3

circuit_alter -name V1 -value 2.0
solve -type dc -absolute_error 1.0 -relative_error 1e-14 -maximum_iterations 30
create_1d_mesh -mesh dog
add_1d_mesh_line -mesh dog -pos 0 -ps 0.1 -tag top
add_1d_mesh_line -mesh dog -pos 1 -ps 0.1 -tag bot
add_1d_contact   -mesh dog -name top -tag top -material metal
add_1d_contact   -mesh dog -name bot -tag bot -material metal
add_1d_region    -mesh dog -material Si -region $region -tag1 top -tag2 bot
finalize_mesh -mesh dog
create_device -mesh dog -device $device

node_model -device $device -region $region -name erf -equation "erf(x);"
print_node_values -device $device -region $region -name erf
node_model -device $device -region $region -name d_erf_dx -equation "diff(erf(x),x);"
print_node_values -device $device -region $region -name d_erf_dx
node_model -device $device -region $region -name erfc -equation "erfc(x);"
print_node_values -device $device -region $region -name erfc
node_model -device $device -region $region -name d_erfc_dx -equation "diff(erfc(x),x);"
print_node_values -device $device -region $region -name d_erfc_dx

node_model -device $device -region $region -name d_abs_dx -equation "diff(abs(x),x);"
print_node_values -device $device -region $region -name d_abs_dx

set_parameter    -name param -value -1.0
node_model -device $device -region $region -name deptest -equation "param;"
print_node_values -device $device -region $region -name deptest
set_parameter    -name param -value 1.0
print_node_values -device $device -region $region -name deptest

node_model -device $device -region $region -name cdeptest -equation "cnode1;"
print_node_values -device $device -region $region -name cdeptest
set_circuit_node_value -node cnode1 -value -1.0
print_node_values -device $device -region $region -name cdeptest
