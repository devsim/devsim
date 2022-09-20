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

import sys
from devsim import *

if not get_parameter(name='info')['extended_precision']:
    print("Extended precision support is not available with this version")
    sys.exit(0)

set_parameter(name = "extended_solver", value=True)
set_parameter(name = "extended_model", value=True)
set_parameter(name = "extended_equation", value=True)

from devsim.python_packages.simple_physics import *
import diode_common

device="diode3d"
region="Bulk"


diode_common.Create3DGmshMesh(device, region)

# this is is the devsim format
write_devices (file="gmsh_diode3d_out.msh")

diode_common.SetParameters(device=device, region=region)

####
#### NetDoping
####
node_model(device=device, region=region, name="Acceptors", equation="1.0e18*step(0.5e-5-z);")
node_model(device=device, region=region, name="Donors",    equation="1.0e18*step(z-0.5e-5);")
node_model(device=device, region=region, name="NetDoping", equation="Donors-Acceptors;")

diode_common.InitialSolution(device, region)


####
#### Initial DC solution
####
solve(type="dc", absolute_error=1.0, relative_error=1e-12, maximum_iterations=30)

###
### Drift diffusion simulation at equilibrium
###
diode_common.DriftDiffusionInitialSolution(device, region)

solve(type="dc", absolute_error=1e10, relative_error=1e-8, maximum_iterations=50)

v = 0.1
while v < 0.51:
    set_parameter(device=device, name=GetContactBiasName("top"), value=v)
    solve(type="dc", absolute_error=1e10, relative_error=1e-20, maximum_iterations=30)
    PrintCurrents(device, "top")
    PrintCurrents(device, "bot")
    v += 0.1

element_from_edge_model(edge_model="ElectricField",   device=device, region=region)
element_from_edge_model(edge_model="ElectronCurrent", device=device, region=region)
element_from_edge_model(edge_model="HoleCurrent",     device=device, region=region)

write_devices(file="gmsh_diode3d_dd.dat", type="tecplot")
write_devices(file="gmsh_diode3d_dd.msh", type="devsim")

#element_from_node_model(node_model="node_index", device=device, region=region)
#en0 = map(lambda x : int(x), get_element_model_values(name="node_index@en0", device=device, region=region))
#en1 = map(lambda x : int(x), get_element_model_values(name="node_index@en1", device=device, region=region))
#en2 = map(lambda x : int(x), get_element_model_values(name="node_index@en2", device=device, region=region))
#en3 = map(lambda x : int(x), get_element_model_values(name="node_index@en3", device=device, region=region))
#for i in range(len(en0)):
#  print "%d %d %d %d" % (en0[i], en1[i], en2[i], en3[i])

