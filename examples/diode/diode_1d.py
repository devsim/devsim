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

from devsim import *
import devsim.python_packages.simple_physics as simple_physics
import diode_common
#####
# dio1
#
# Make doping a step function
# print dat to text file for viewing in grace
# verify currents analytically
# in dio2 add recombination
#

device="MyDevice"
region="MyRegion"

diode_common.CreateMesh(device=device, region=region)

diode_common.SetParameters(device=device, region=region)
set_parameter(device=device, region=region, name="taun", value=1e-8)
set_parameter(device=device, region=region, name="taup", value=1e-8)

diode_common.SetNetDoping(device=device, region=region)

print_node_values(device=device, region=region, name="NetDoping")

diode_common.InitialSolution(device, region)

# Initial DC solution
solve(type="dc", absolute_error=1.0, relative_error=1e-10, maximum_iterations=30)

diode_common.DriftDiffusionInitialSolution(device, region)
###
### Drift diffusion simulation at equilibrium
###
solve(type="dc", absolute_error=1e10, relative_error=1e-10, maximum_iterations=30)


####
#### Ramp the bias to 0.5 Volts
####
v = 0.0
while v < 0.51:
    set_parameter(device=device, name=simple_physics.GetContactBiasName("top"), value=v)
    solve(type="dc", absolute_error=1e10, relative_error=1e-10, maximum_iterations=30)
    simple_physics.PrintCurrents(device, "top")
    simple_physics.PrintCurrents(device, "bot")
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
