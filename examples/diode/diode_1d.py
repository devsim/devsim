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
from python_packages.simple_physics import *
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
  add_1d_mesh_line(mesh="dio", pos=0.5e-5, ps=1e-9, tag="mid")
  add_1d_mesh_line(mesh="dio", pos=1e-5, ps=1e-7, tag="bot")
  add_1d_contact  (mesh="dio", name="top", tag="top", material="metal")
  add_1d_contact  (mesh="dio", name="bot", tag="bot", material="metal")
  add_1d_region   (mesh="dio", material="Si", region=region, tag1="top", tag2="bot")
  finalize_mesh(mesh="dio")
  create_device(mesh="dio", device=device)

device="MyDevice"
region="MyRegion"

createMesh(device, region)

####
#### Set parameters for 300 K
####
SetSiliconParameters(device, region, 300)
set_parameter(device=device, region=region, name="taun", value=1e-8)
set_parameter(device=device, region=region, name="taup", value=1e-8)

####
#### NetDoping
####
CreateNodeModel(device, region, "Acceptors", "1.0e18*step(0.5e-5-x)")
CreateNodeModel(device, region, "Donors",    "1.0e18*step(x-0.5e-5)")
CreateNodeModel(device, region, "NetDoping", "Donors-Acceptors")
print_node_values(device=device, region=region, name="NetDoping")

####
#### Create Potential, Potential@n0, Potential@n1
####
CreateSolution(device, region, "Potential")

####
#### Create potential only physical models
####
CreateSiliconPotentialOnly(device, region)

####
#### Set up the contacts applying a bias
####
for i in get_contact_list(device=device):
  set_parameter(device=device, name=GetContactBiasName(i), value=0.0)
  CreateSiliconPotentialOnlyContact(device, region, i)


####
#### Initial DC solution
####
solve(type="dc", absolute_error=1.0, relative_error=1e-10, maximum_iterations=30)

####
#### drift diffusion solution variables
####
CreateSolution(device, region, "Electrons")
CreateSolution(device, region, "Holes")

####
#### create initial guess from dc only solution
####
set_node_values(device=device, region=region, name="Electrons", init_from="IntrinsicElectrons")
set_node_values(device=device, region=region, name="Holes",     init_from="IntrinsicHoles")

###
### Set up equations
###
CreateSiliconDriftDiffusion(device, region)
for i in get_contact_list(device=device):
  CreateSiliconDriftDiffusionAtContact(device, region, i)

###
### Drift diffusion simulation at equilibrium
###
solve(type="dc", absolute_error=1e10, relative_error=1e-10, maximum_iterations=30)

####
#### Ramp the bias to 0.5 Volts
####
v = 0.0
while v < 0.51:
  set_parameter(device=device, name=GetContactBiasName("top"), value=v)
  solve(type="dc", absolute_error=1e10, relative_error=1e-10, maximum_iterations=30)
  PrintCurrents(device, "top")
  PrintCurrents(device, "bot")
  v += 0.1

write_devices(file="diode_1d.dat", type="tecplot")
#import matplotlib
#import matplotlib.pyplot
#x=get_node_model_values(device=device, region=region, name="x")
#ymax = 10
#ymin = 10
#fields = ("Electrons", "Holes", "Donors", "Acceptors")
#for i in fields:
#    y=get_node_model_values(device=device, region=region, name=i)
#    if (max(y) > ymax):
#      ymax = max(y)
#    matplotlib.pyplot.semilogy(x, y)
#matplotlib.pyplot.xlabel('x (cm)')
#matplotlib.pyplot.ylabel('Density (#/cm^3)')
#matplotlib.pyplot.legend(fields)
#ymax *= 10
#matplotlib.pyplot.axis([min(x), max(x), ymin, ymax])
#matplotlib.pyplot.savefig("diode_1d_density.eps")
#
#matplotlib.pyplot.clf()
#edge_average_model(device=device, region=region, node_model="x", edge_model="xmid")
#xmid=get_edge_model_values(device=device, region=region, name="xmid")
#efields = ("ElectronCurrent", "HoleCurrent", )
#y=get_edge_model_values(device=device, region=region, name="ElectronCurrent")
#ymin=min(y)
#ymax=max(y)
#for i in efields:
#  y=get_edge_model_values(device=device, region=region, name=i)
#  if min(y) < ymin:
#    ymin = min(y)
#  elif max(y) > ymax:
#    ymax = max(y)
#  matplotlib.pyplot.plot(xmid, y)
#matplotlib.pyplot.xlabel('x (cm)')
#matplotlib.pyplot.ylabel('J (A/cm^2)')
#matplotlib.pyplot.legend(efields)
#matplotlib.pyplot.axis([min(x), max(x), 0.5*ymin, 2*ymax])
#matplotlib.pyplot.savefig("diode_1d_current.eps")
#print ymin
#print ymax
