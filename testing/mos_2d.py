# Copyright 2013 DEVSIM LLC
#
# SPDX-License-Identifier: Apache-2.0

from devsim.python_packages.simple_physics import *
from devsim.python_packages.ramp import *
from devsim import *

import mos_2d_create

device = "mymos"
silicon_regions=("gate", "bulk")
oxide_regions=("oxide",)
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
    set_node_values(device=device, region=i, name="Electrons", init_from="IntrinsicElectrons")
    set_node_values(device=device, region=i, name="Holes",     init_from="IntrinsicHoles")
    CreateSiliconDriftDiffusion(device, i, "mu_n", "mu_p")

for c in contacts:
    tmp = get_region_list(device=device, contact=c)
    r = tmp[0]
    CreateSiliconDriftDiffusionAtContact(device, r, c)

solve(type="dc", absolute_error=1.0e30, relative_error=1e-5, maximum_iterations=30)

for r in silicon_regions:
    node_model(device=device, region=r, name="logElectrons", equation="log(Electrons)/log(10)")

write_devices(file="mos_2d_dd.msh", type="devsim")

with open("mos_2d_params.py", "w", encoding="utf-8") as ofh:
    ofh.write('import devsim\n')
    for p in get_parameter_list():
        if p in ('solver_callback', 'direct_solver', 'info'):
            continue
        v=repr(get_parameter(name=p))
        ofh.write('devsim.set_parameter(name="%s", value=%s)\n' % (p, v))
    for i in get_device_list():
        for p in get_parameter_list(device=i):
            v=repr(get_parameter(device=i, name=p))
            ofh.write('devsim.set_parameter(device="%s", name="%s", value=%s)\n' % (i, p, v))

    for i in get_device_list():
        for j in get_region_list(device=i):
            for p in get_parameter_list(device=i, region=j):
                v=repr(get_parameter(device=i, region=j, name=p))
                ofh.write('devsim.set_parameter(device="%s", region="%s", name="%s", value=%s)\n' % (i, j, p, v))

