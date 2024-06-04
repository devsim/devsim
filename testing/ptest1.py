# Copyright 2013 DEVSIM LLC
#
# SPDX-License-Identifier: Apache-2.0

from devsim import (
    create_device,
    get_contact_charge,
    get_edge_model_values,
    set_parameter,
    solve,
)
from cap_mesh import cap_mesh
from devsim.python_packages.simple_physics import (
    CreateOxideContact,
    SetOxideParameters,
    CreateOxidePotentialOnly,
)

region_name = "MyRegion"
device_name = "MyDevice"
material_name = "Oxide"

mesh_name = cap_mesh(region_name, material_name)
create_device(mesh=mesh_name, device=device_name)

SetOxideParameters(device_name, region_name, 300)
CreateOxidePotentialOnly(device_name, region_name)
CreateOxideContact(device_name, region_name, "top")
CreateOxideContact(device_name, region_name, "bot")

set_parameter(name="bot_bias", value=0.0)
# for i in (0.0, 1.0):
set_parameter(name="top_bias", value=1.0)
solve(type="dc", absolute_error=1.0, relative_error=1e-10, maximum_iterations=30)
print(
    (
        get_contact_charge(
            device=device_name, contact="top", equation="PotentialEquation"
        )
    )
)
print(
    (
        get_contact_charge(
            device=device_name, contact="bot", equation="PotentialEquation"
        )
    )
)
print(
    get_edge_model_values(
        device=device_name, region=region_name, name="PotentialEdgeFlux"
    )
)
