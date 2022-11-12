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

import devsim
devsim.load_devices(file="mos_2d_dd.msh")
device=devsim.get_device_list()[0]
from mos_2d_physics import *

setSiliconParameters(device, "gate")
setSiliconParameters(device, "bulk")
setOxideParameters(device, "oxide")

createSiliconPotentialOnly(device, "bulk")
createSiliconPotentialOnly(device, "gate")

createOxidePotentialOnly(device, "oxide")

createSiliconPotentialOnlyContact(device, "gate", "gate")
createSiliconPotentialOnlyContact(device, "bulk", "drain")
createSiliconPotentialOnlyContact(device, "bulk", "source")
createSiliconPotentialOnlyContact(device, "bulk", "body")

createSiliconOxideInterface(device, "bulk_oxide")
createSiliconOxideInterface(device, "gate_oxide")

createSiliconDriftDiffusion(device, "gate")
createSiliconDriftDiffusionAtContact(device, "gate", "gate")

createSiliconDriftDiffusion(device, "bulk")


createSiliconDriftDiffusionAtContact(device, "bulk", "drain")
createSiliconDriftDiffusionAtContact(device, "bulk", "source")
createSiliconDriftDiffusionAtContact(device, "bulk", "body")
devsim.solve(type="dc", absolute_error=1.0e30, relative_error=1e-5, maximum_iterations=30)

