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

from cap_mesh import *

region_name = "MyRegion"
device_name = "MyDevice"
material_name = "Oxide"

mesh_name = cap_mesh(region_name, material_name)
create_device(mesh=mesh_name, device=device_name)

from devsim.python_packages.simple_physics import *

SetOxideParameters(device_name, region_name, 300)
CreateOxidePotentialOnly(device_name, region_name)
CreateOxideContact(device_name, region_name, "top")
CreateOxideContact(device_name, region_name, "bot")

set_parameter(name="bot_bias", value=0.0)
#for i in (0.0, 1.0):
set_parameter(name="top_bias", value=1.0)
solve( type="dc", absolute_error= 1.0 , relative_error= 1e-10 , maximum_iterations=30)
print((get_contact_charge(device=device_name, contact="top", equation="PotentialEquation")))
print((get_contact_charge(device=device_name, contact="bot", equation="PotentialEquation")))
print(get_edge_model_values(device=device_name, region=region_name, name="PotentialEdgeFlux"))

