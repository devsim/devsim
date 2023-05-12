# Copyright 2013 DEVSIM LLC
#
# SPDX-License-Identifier: Apache-2.0

import devsim
devsim.load_devices(file="mos_2d_dd.msh")
device=devsim.get_device_list()[0]
from devsim.python_packages.simple_physics import *

device = "mymos"
silicon_regions=("gate", "bulk")
oxide_regions=("oxide",)
regions = ("gate", "bulk", "oxide")
interfaces = ("bulk_oxide", "gate_oxide")


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


for i in silicon_regions:
    CreateSolution(device, i, "Electrons")
    CreateSolution(device, i, "Holes")
    set_node_values(device=device, region=i, name="Electrons", init_from="IntrinsicElectrons")
    set_node_values(device=device, region=i, name="Holes",     init_from="IntrinsicHoles")
    CreateSiliconDriftDiffusion(device, i, "mu_n", "mu_p")

for c in contacts:
    tmp = get_region_list(device=device, contact=c)
    r = tmp[0]
    CreateSiliconDriftDiffusionAtContact(device, r, c)

devsim.solve(type="dc", absolute_error=1.0e30, relative_error=1e-5, maximum_iterations=30)

