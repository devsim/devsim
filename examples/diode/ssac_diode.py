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

#### Small Signal simulation
from ds import *
import sys
sys.path.append('../../python_packages')
from simple_physics import *

#This requires a circuit element to integrated current
circuit_element(name="V1", n1=GetContactBiasName("top"), n2=0, value=0.0, acreal=1.0, acimag=0.0)


#####
# dio1
#
# Make doping a step function
# print dat to text file for viewing in grace
# verify currents analytically
# in dio2 add recombination
#

####
#### Meshing
####
def createMesh(device, region):
  create_1d_mesh(mesh="dio")
  add_1d_mesh_line(mesh="dio", pos=0, ps=1e-7, tag="top")
  add_1d_mesh_line(mesh="dio", pos=0.5e-5, ps=1e-8, tag="mid")
  add_1d_mesh_line(mesh="dio", pos=1e-5, ps=1e-7, tag="bot")
  add_1d_contact(mesh="dio", name="top", tag="top")
  add_1d_contact(mesh="dio", name="bot", tag="bot")
  add_1d_region(mesh="dio", material="Si", region=region, tag1="top", tag2="bot")
  finalize_mesh(mesh="dio")
  create_device(mesh="dio", device=device)



device="MyDevice"
region="MyRegion"

createMesh(device, region)

SetSiliconParameters(device, region, 300)

CreateSolution(device, region, "Potential")

####
#### NetDoping
####
node_model(device=device, region=region, name="Acceptors", equation="1.0e18*step(0.5e-5-x)")
node_model(device=device, region=region, name="Donors", equation="1.0e18*step(x-0.5e-5)")
node_model(device=device, region=region, name="NetDoping", equation="Donors-Acceptors")
#print_node_values device=device, region=region, name="NetDoping",

CreateSiliconPotentialOnly(device, region)


CreateSiliconPotentialOnlyContact(device, region, "top", True)
CreateSiliconPotentialOnlyContact(device, region, "bot")

set_parameter(device=device, name=GetContactBiasName("bot"), value=0)


####
#### Initial DC solution
####
solve(type="dc", absolute_error=1.0, relative_error=1e-12, maximum_iterations=30)

#print_node_values device=device, region=region, name="Potential",
#write_devices file="dio2_potentialonly",.flps type="floops",

#### need way to clear out old models and their derivatives

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
CreateSiliconDriftDiffusionAtContact(device, region, "top", True)
CreateSiliconDriftDiffusionAtContact(device, region, "bot", False)

v=0.0
while v < 0.51: 
  circuit_alter(name="V1", value=v)
  solve(type="dc", absolute_error=1e10, relative_error=1e-10, maximum_iterations=30)
  #TODO: get out circuit information
#  PrintCurrents(device, "top")
  PrintCurrents(device, "bot")
  solve(type="ac", frequency=1.0)
  cap=get_circuit_node_value(node="V1.I", solution="ssac_imag")/ (2*3.14159)
  print "capacitance {0} {1}".format(v, cap)
  v += 0.1

for x in get_circuit_node_list():
  for y in get_circuit_solution_list():
    z = get_circuit_node_value(node=x, solution=y)
    print("{0}\t{1}\t{2}".format(x, y, z))

