# Copyright 2013 DEVSIM LLC
#
# SPDX-License-Identifier: Apache-2.0

# this test added specifically to create a mesh for gmsh_diode3d_equil.py

from devsim import node_model, solve, write_devices
import diode_common

device = "diode3d"
region = "Bulk"


diode_common.Create3DGmshMesh(device, region)

# this is is the devsim format
write_devices(file="gmsh_diode3d_out.msh")

diode_common.SetParameters(device=device, region=region)

####
#### NetDoping
####
node_model(
    device=device, region=region, name="Acceptors", equation="1.0e18*step(0.5e-5-z);"
)
node_model(
    device=device, region=region, name="Donors", equation="1.0e18*step(z-0.5e-5);"
)
node_model(device=device, region=region, name="NetDoping", equation="Donors-Acceptors;")

diode_common.InitialSolution(device, region)


####
#### Initial DC solution
####
solve(type="dc", absolute_error=1.0, relative_error=1e-12, maximum_iterations=30)

# element_from_edge_model(edge_model="ElectricField",   device=device, region=region)
# element_from_edge_model(edge_model="ElectricField",   device=device, region=region, derivative="Potential")

write_devices(file="gmsh_diode3d_equil.msh", type="devsim")
