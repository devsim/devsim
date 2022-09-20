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

import mos_2d_create
from mos_2d_physics import *

import devsim
device=devsim.get_device_list()[0]
oxide_regions   = ("oxide",)
silicon_regions = ("gate", "bulk")
all_regions     = ("gate", "bulk", "oxide")

for i in all_regions:
    createSolution(device, i, "Potential")

for i in silicon_regions:
    setSiliconParameters(device, i)
    createSiliconPotentialOnly(device, i)
for i in oxide_regions:
    setOxideParameters(device, i)
    createOxidePotentialOnly(device, "oxide")

createSiliconPotentialOnlyContact(device, "gate", "gate")
createSiliconPotentialOnlyContact(device, "bulk", "drain")
createSiliconPotentialOnlyContact(device, "bulk", "source")
createSiliconPotentialOnlyContact(device, "bulk", "body")

createSiliconOxideInterface(device, "bulk_oxide")
createSiliconOxideInterface(device, "gate_oxide")

devsim.solve(type="dc", absolute_error=1.0e-13, relative_error=1e-12, maximum_iterations=30)
devsim.solve(type="dc", absolute_error=1.0e-13, relative_error=1e-12, maximum_iterations=30)

createSolution(device, "gate", "Electrons")
createSolution(device, "gate", "Holes")
devsim.set_node_values(device=device, region="gate", name="Electrons", init_from="IntrinsicElectrons")
devsim.set_node_values(device=device, region="gate", name="Holes",     init_from="IntrinsicHoles")
createSiliconDriftDiffusion(device, "gate")
createSiliconDriftDiffusionAtContact(device, "gate", "gate")

createSolution(device, "bulk", "Electrons")
createSolution(device, "bulk", "Holes")
devsim.set_node_values(device=device, region="bulk", name="Electrons", init_from="IntrinsicElectrons")
devsim.set_node_values(device=device, region="bulk", name="Holes",     init_from="IntrinsicHoles")
createSiliconDriftDiffusion(device, "bulk")


createSiliconDriftDiffusionAtContact(device, "bulk", "drain")
createSiliconDriftDiffusionAtContact(device, "bulk", "source")
createSiliconDriftDiffusionAtContact(device, "bulk", "body")

devsim.solve(type="dc", absolute_error=1.0e30, relative_error=1e-5, maximum_iterations=30)

devsim.element_from_edge_model(edge_model="ElectricField", device=device, region="bulk")

devsim.write_devices(file="mos_2d_dd.msh", type="devsim")

with open("mos_2d_params.py", "w", encoding="utf-8") as ofh:
    ofh.write('import devsim\n')
    for p in devsim.get_parameter_list():
        v=repr(devsim.get_parameter(name=p))
        ofh.write('devsim.set_parameter(name="%s", value=%s)\n' % (p, v))
    for i in devsim.get_device_list():
        for p in devsim.get_parameter_list(device=i):
            v=repr(devsim.get_parameter(device=i, name=p))
            ofh.write('devsim.set_parameter(device="%s", name="%s", value=%s)\n' % (i, p, v))

    for i in devsim.get_device_list():
        for j in devsim.get_region_list(device=i):
            for p in devsim.get_parameter_list(device=i, region=j):
                v=repr(devsim.get_parameter(device=i, region=j, name=p))
                ofh.write('devsim.set_parameter(device="%s", region="%s", name="%s", value=%s)\n' % (i, j, p, v))

