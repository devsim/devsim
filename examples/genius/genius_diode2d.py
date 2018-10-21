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

from devsim import *
from python_packages.simple_physics import *

device="pn2d"
region="Silicon"

#load in the mesh created
load_devices    (file="pn2d.msh")

###
### Set physical parameters
###
SetSiliconParameters(device, region, 300)

####
#### NetDoping
####
# donors and acceptors from genius
node_model(device=device, region=region, name="NetDoping", equation="nd-na;")

CreateSiliconPotentialOnly(device, region)
for i in get_contact_list(device=device):
  set_parameter(device=device, name=GetContactBiasName(i), value=0.0)
  CreateSiliconPotentialOnlyContact(device, region, i)


####
#### Initial DC solution
####

solve(type="dc", absolute_error=1.0, relative_error=1e-12, maximum_iterations=30)
write_devices(file="pn2d_equil.dat", type="tecplot")

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

#
#
#solve(type="dc", absolute_error=1e10, relative_error=1e-10, maximum_iterations=50)
##
v = 0.0
while v < 0.501:
  set_parameter(device=device, name=GetContactBiasName("Anode"), value=v)
  solve(type="dc", absolute_error=1e3, relative_error=5e-2, maximum_iterations=20)
  PrintCurrents(device, "Anode")
  PrintCurrents(device, "Cathode")
  v += 0.025

write_devices(file="genius_diode2d.dat", type="tecplot")

