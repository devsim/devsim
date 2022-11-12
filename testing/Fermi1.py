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

devsim.set_parameter(name="Nc", value=1e22)
devsim.node_model(device=device, region=region, name="Electrons", equation="2e15*(1 - x) + 1e10;")
devsim.node_model(device=device, region=region, name="r", equation="Electrons/Nc;")
devsim.node_model(device=device, region=region, name="Eta", equation="InvFermi(r);")
devsim.node_model(device=device, region=region, name="r2", equation="Fermi(Eta);")

devsim.node_model(device=device, region=region, name="Eta:r", equation="diff(InvFermi(r),r);")
devsim.node_model(device=device, region=region, name="r2:Eta", equation="diff(Fermi(Eta),Eta);")
devsim.node_model(device=device, region=region, name="r3:Eta", equation="dFermidx(Eta);")
devsim.node_model(device=device, region=region, name="r4:Eta", equation="1.0/dInvFermidx(r2);")

devsim.print_node_values(device=device, region=region, name="Electrons")
devsim.print_node_values(device=device, region=region, name="r")
devsim.print_node_values(device=device, region=region, name="Eta")
devsim.print_node_values(device=device, region=region, name="r2")
devsim.print_node_values(device=device, region=region, name="Eta:r")
devsim.print_node_values(device=device, region=region, name="r2:Eta")
devsim.print_node_values(device=device, region=region, name="r3:Eta")
devsim.print_node_values(device=device, region=region, name="r4:Eta")

devsim.set_parameter(name="Nc", value=3.23e19)
devsim.set_parameter(name="Nv", value=1.83e19)
devsim.set_parameter(name="Eg", value=1.12)
devsim.set_parameter(name="Vt", value=0.0259)
devsim.node_model(device=device, region=region, name="ni_f", equation="(Nc*Nv)^0.5 * Fermi(-Eg/(2*Vt))")
devsim.node_model(device=device, region=region, name="ni_b", equation="(Nc*Nv)^0.5 * exp(-Eg/(2*Vt))")
devsim.print_node_values(device=device, region=region, name="ni_f")
devsim.print_node_values(device=device, region=region, name="ni_b")

