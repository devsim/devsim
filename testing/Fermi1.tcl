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

set_parameter -name "Nc" -value 1e22
node_model -device $device -region $region -name Electrons  -equation "2e15*(1 - x) + 1e10;"
node_model -device $device -region $region -name r  -equation "Electrons/Nc;"
node_model -device $device -region $region -name Eta -equation "InvFermi(r);"
node_model -device $device -region $region -name r2  -equation "Fermi(Eta);"

node_model -device $device -region $region -name Eta:r -equation "diff(InvFermi(r),r);"
node_model -device $device -region $region -name r2:Eta  -equation "diff(Fermi(Eta),Eta);"
node_model -device $device -region $region -name r3:Eta  -equation "dFermidx(Eta);"
# this one may be much more efficient since call to Fermi is expensive
node_model -device $device -region $region -name r4:Eta  -equation "1.0/dInvFermidx(r2);"

print_node_values -device $device -region $region -name Electrons
print_node_values -device $device -region $region -name r
print_node_values -device $device -region $region -name Eta
print_node_values -device $device -region $region -name r2
print_node_values -device $device -region $region -name Eta:r
print_node_values -device $device -region $region -name r2:Eta
print_node_values -device $device -region $region -name r3:Eta
print_node_values -device $device -region $region -name r4:Eta

set_parameter -name "Nc" -value 3.23e19
set_parameter -name "Nv" -value 1.83e19
set_parameter -name "Eg" -value 1.12
set_parameter -name "Vt" -value 0.0259
node_model -device $device -region $region -name "ni_f" -equation "(Nc*Nv)^0.5 * Fermi(-Eg/(2*Vt))";
node_model -device $device -region $region -name "ni_b" -equation "(Nc*Nv)^0.5 * exp(-Eg/(2*Vt))";
print_node_values -device $device -region $region -name "ni_f"
print_node_values -device $device -region $region -name "ni_b"
