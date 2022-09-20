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

####
#### package test for drift diffusion
####
from devsim import *
from devsim.python_packages.simple_physics import *

device="MyDevice"
region="MyRegion"

#### TODO: make one where you pick bulk spacing, contact spacing, and junction spacing
####
#### Meshing
####
create_1d_mesh  (mesh="dog")
add_1d_mesh_line(mesh="dog", pos=0, ps=1e-6, tag="top")
add_1d_mesh_line(mesh="dog", pos=0.5e-5, ps=1e-7, tag="mid")
add_1d_mesh_line(mesh="dog", pos=1e-5, ps=1e-6, tag="bot")
add_1d_contact  (mesh="dog", name="top", tag="top", material="metal")
add_1d_contact  (mesh="dog", name="bot", tag="bot", material="metal")
add_1d_region   (mesh="dog", material="Si", region=region, tag1="top", tag2="bot")
finalize_mesh(mesh="dog")
create_device(mesh="dog", device=device)

####
#### Constants
####
SetSiliconParameters(device, region, 300)

####
#### Potential
####
CreateSolution(device, region, "Potential")

####
#### NetDoping
####
CreateNodeModel(device, region, "Acceptors", "1.0e17*step(0.5e-5-x)")
CreateNodeModel(device, region, "Donors",    "1.0e18*step(x-0.5e-5)")
CreateNodeModel(device, region, "NetDoping", "Donors-Acceptors")
#CreateNodeModel(device, region, "NetDoping", "1e18")

CreateSiliconPotentialOnly(device, region)

for i in get_contact_list(device=device):
    set_parameter(device=device, name=GetContactBiasName(i), value=0.0)
    CreateSiliconPotentialOnlyContact(device, region, i)

solve(type="dc", absolute_error=1.0, relative_error=1e-14, maximum_iterations=30)

###
### Electrons and Holes
###
CreateSolution(device, region, "Electrons")
CreateSolution(device, region, "Holes")

###
### Initialize initial guess
###
set_node_values(device=device, region=region, name="Electrons", init_from="IntrinsicElectrons")
set_node_values(device=device, region=region, name="Holes",     init_from="IntrinsicHoles")
CreateSiliconDriftDiffusion(device, region)
for i in get_contact_list(device=device):
    CreateSiliconDriftDiffusionAtContact(device, region, i)

solve(type="dc", absolute_error=1e10, relative_error=1e-12, maximum_iterations=30)
x= get_node_model_values(device=device, region=region, name="x")

#v= get_node_model_values(device=device, region=region, name="Potential")
##v= get_edge_model_values(device=device, region=region, name="ElectricField")
#import matplotlib
#import matplotlib.pyplot
#matplotlib.pyplot.plot(v)
#matplotlib.pyplot.savefig('foo.pdf')

v = 0.1
while v < 0.99:
    set_parameter(device=device, name=GetContactBiasName("top"), value=v)
    solve(type="dc", absolute_error=1e10, relative_error=1e-12, maximum_iterations=30)
    PrintCurrents(device, "top")
    PrintCurrents(device, "bot")
    v += 0.1



