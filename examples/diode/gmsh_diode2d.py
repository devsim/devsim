# Copyright 2013 DEVSIM LLC
#
# SPDX-License-Identifier: Apache-2.0

from devsim import *
from devsim.python_packages.simple_physics import *
import diode_common

device="diode2d"
region="Bulk"

diode_common.Create2DGmshMesh(device, region)

# this is is the devsim format
write_devices    (file="gmsh_diode2d_out.msh")

diode_common.SetParameters(device=device, region=region)

####
#### NetDoping
####
node_model(device=device, region=region, name="Acceptors", equation="1.0e18*step(0.5e-5-y);")
node_model(device=device, region=region, name="Donors"   , equation="1.0e18*step(y-0.5e-5);")
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

solve(type="dc", absolute_error=1e10, relative_error=1e-10, maximum_iterations=50)

v = 0.0
while v < 0.51:
    set_parameter(device=device, name=GetContactBiasName("top"), value=v)
    solve(type="dc", absolute_error=1e10, relative_error=1e-10, maximum_iterations=30)
    PrintCurrents(device, "top")
    PrintCurrents(device, "bot")
    v += 0.1

write_devices(file="gmsh_diode2d.dat", type="tecplot")
write_devices(file="gmsh_diode2d_dd.msh", type="devsim")

