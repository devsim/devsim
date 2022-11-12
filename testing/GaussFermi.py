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

#Purpose: Fermi integral and inverse

import devsim
import test_common

device="MyDevice"
region="MyRegion"

test_common.CreateSimpleMesh(device, region)



devsim.node_model(device=device, region=region, name="zeta1", equation="x*70 - 70;")
devsim.node_model(device=device, region=region, name="g1", equation="gfi(zeta1, s);")
devsim.node_model(device=device, region=region, name="zeta2", equation="igfi(g1, s);")


devsim.node_model(device=device, region=region, name="dgfidx1", equation="diff(gfi(zeta1, s), zeta1);")
devsim.node_model(device=device, region=region, name="digfidx1", equation="diff(igfi(g1, s), g1);")
devsim.node_model(device=device, region=region, name="dgfidx2", equation="1/digfidx1;")

for s in (2,4,6,8):
    devsim.set_parameter(name="s", value=s)

    devsim.print_node_values(device=device, region=region, name="zeta1")
    devsim.print_node_values(device=device, region=region, name="g1")
    devsim.print_node_values(device=device, region=region, name="zeta2")

    devsim.print_node_values(device=device, region=region, name="dgfidx1")
    devsim.print_node_values(device=device, region=region, name="digfidx1")
    devsim.print_node_values(device=device, region=region, name="dgfidx2")
