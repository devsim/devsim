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

from ds import *
import sys
sys.path.append('../../python_packages')
from simple_physics import *

device="diode3d"
region="Bulk"

#this reads in the gmsh format
create_gmsh_mesh (mesh="diode3d", file="gmsh_diode3d.msh")
add_gmsh_region  (mesh="diode3d", gmsh_name="Bulk",    region="Bulk", material="Silicon")
add_gmsh_contact (mesh="diode3d", gmsh_name="Base",    region="Bulk", name="top")
add_gmsh_contact (mesh="diode3d", gmsh_name="Emitter", region="Bulk", name="bot")
finalize_mesh    (mesh="diode3d")
create_device    (mesh="diode3d", device=device)

# this is is the devsim format
write_devices (file="gmsh_diode3d_out.msh")

###
### Set physical parameters
###
SetSiliconParameters(device, region, 300)

####
#### NetDoping
####
node_model(device=device, region=region, name="Acceptors", equation="1.0e18*step(0.5e-5-z);")
node_model(device=device, region=region, name="Donors",    equation="1.0e18*step(z-0.5e-5);")
node_model(device=device, region=region, name="NetDoping", equation="Donors-Acceptors;")

CreateSiliconPotentialOnly(device, region)
for i in get_contact_list(device=device):
  set_parameter(device=device, name=GetContactBiasName(i), value=0.0)
  CreateSiliconPotentialOnlyContact(device, region, i)


####
#### Initial DC solution
####

solve(type="dc", absolute_error=1.0, relative_error=1e-12, maximum_iterations=30)

####
#### drift diffusion
####
CreateSolution(device, region, "Electrons")
CreateSolution(device, region, "Holes")

####
#### create initial guess from dc only solution
####
set_node_values(device=device, region=region, name="Electrons", init_from="IntrinsicElectrons")
set_node_values(device=device, region=region, name="Holes",     init_from="IntrinsicHoles")

CreateSiliconDriftDiffusion(device, region)
for i in get_contact_list(device=device):
  CreateSiliconDriftDiffusionAtContact(device, region, i)


solve(type="dc", absolute_error=1e10, relative_error=1e-8, maximum_iterations=50)

v = 0.1
while v < 0.51:
  set_parameter(device=device, name=GetContactBiasName("top"), value=v)
  solve(type="dc", absolute_error=1e10, relative_error=1e-8, maximum_iterations=30)
  PrintCurrents(device, "top")
  PrintCurrents(device, "bot")
  v += 0.1

element_from_edge_model(edge_model="ElectricField",   device=device, region=region)
element_from_edge_model(edge_model="ElectronCurrent", device=device, region=region)
element_from_edge_model(edge_model="HoleCurrent",     device=device, region=region)

write_devices(file="gmsh_diode3d_dd.dat", type="tecplot")

