# Copyright 2013 DEVSIM LLC
#
# SPDX-License-Identifier: Apache-2.0

from devsim.python_packages.simple_physics import (
    GetContactBiasName,
    SetOxideParameters,
    SetSiliconParameters,
    CreateSiliconPotentialOnly,
    CreateSiliconPotentialOnlyContact,
    CreateSiliconDriftDiffusion,
    CreateSiliconDriftDiffusionAtContact,
    CreateOxidePotentialOnly,
    CreateSiliconOxideInterface,
)
from devsim.python_packages.ramp import rampbias, printAllCurrents
from devsim import (
    element_from_edge_model,
    get_contact_list,
    get_region_list,
    node_model,
    set_node_values,
    set_parameter,
    solve,
    write_devices,
)
from devsim.python_packages.model_create import CreateSolution

device = "mos2d"
silicon_regions = ("gate", "bulk")
oxide_regions = ("oxide",)
regions = ("gate", "bulk", "oxide")
interfaces = ("bulk_oxide", "gate_oxide")

for i in regions:
    CreateSolution(device, i, "Potential")

for i in silicon_regions:
    SetSiliconParameters(device, i, 300)
    CreateSiliconPotentialOnly(device, i)

for i in oxide_regions:
    SetOxideParameters(device, i, 300)
    CreateOxidePotentialOnly(device, i, "log_damp")

### Set up contacts
contacts = get_contact_list(device=device)
for i in contacts:
    tmp = get_region_list(device=device, contact=i)
    r = tmp[0]
    print("%s %s" % (r, i))
    CreateSiliconPotentialOnlyContact(device, r, i)
    set_parameter(device=device, name=GetContactBiasName(i), value=0.0)

for i in interfaces:
    CreateSiliconOxideInterface(device, i)

solve(type="dc", absolute_error=1.0e-13, relative_error=1e-12, maximum_iterations=30)
solve(type="dc", absolute_error=1.0e-13, relative_error=1e-12, maximum_iterations=30)
#
##write_devices -file gmsh_mos2d_potentialonly.flps -type floops
write_devices(file="gmsh_mos2d_potentialonly", type="vtk")

for i in silicon_regions:
    CreateSolution(device, i, "Electrons")
    CreateSolution(device, i, "Holes")
    set_node_values(
        device=device, region=i, name="Electrons", init_from="IntrinsicElectrons"
    )
    set_node_values(device=device, region=i, name="Holes", init_from="IntrinsicHoles")
    CreateSiliconDriftDiffusion(device, i, "mu_n", "mu_p")

for c in contacts:
    tmp = get_region_list(device=device, contact=c)
    r = tmp[0]
    CreateSiliconDriftDiffusionAtContact(device, r, c)

solve(type="dc", absolute_error=1.0e30, relative_error=1e-5, maximum_iterations=30)

for r in silicon_regions:
    node_model(
        device=device, region=r, name="logElectrons", equation="log(Electrons)/log(10)"
    )


for r in silicon_regions:
    element_from_edge_model(edge_model="ElectricField", device=device, region=r)
    element_from_edge_model(edge_model="ElectronCurrent", device=device, region=r)
    element_from_edge_model(edge_model="HoleCurrent", device=device, region=r)
#
rampbias(device, "gate", 0.5, 0.5, 0.001, 100, 1e-10, 1e30, printAllCurrents)
rampbias(device, "drain", 0.5, 0.1, 0.001, 100, 1e-10, 1e30, printAllCurrents)
#
write_devices(file="gmsh_mos2d_dd.dat", type="tecplot")
