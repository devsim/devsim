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
import test_common
device="MyDevice"
region="MyRegion"

# basic linear circuit solved by itself
devsim.circuit_element(name="V1", n1=1,        n2=0,        value=1.0)
devsim.circuit_element(name="R1", n1=1,        n2="cnode1", value=5.0)
devsim.circuit_element(name="R2", n1="cnode1", n2=0,        value=5.0)

devsim.solve(type="dc", absolute_error=1.0, relative_error=1e-14, maximum_iterations=3)
test_common.print_circuit_solution()

devsim.circuit_alter(name="V1", value=2.0)
devsim.solve(type="dc", absolute_error=1.0, relative_error=1e-14, maximum_iterations=30)
test_common.print_circuit_solution()

test_common.CreateSimpleMesh(device, region)

devsim.node_model(device=device, region=region, name="erf", equation="erf(x);")
devsim.print_node_values(device=device, region=region, name="erf")
devsim.node_model(device=device, region=region, name="d_erf_dx", equation="diff(erf(x),x);")
devsim.print_node_values(device=device, region=region, name="d_erf_dx")
devsim.node_model(device=device, region=region, name="erfc", equation="erfc(x);")
devsim.print_node_values(device=device, region=region, name="erfc")
devsim.node_model(device=device, region=region, name="d_erfc_dx", equation="diff(erfc(x),x);")
devsim.print_node_values(device=device, region=region, name="d_erfc_dx")

devsim.node_model(device=device, region=region, name="d_abs_dx", equation="diff(abs(x),x);")
devsim.print_node_values(device=device, region=region, name="d_abs_dx")

devsim.set_parameter(name="param", value=-1.0)
devsim.node_model(device=device, region=region, name="deptest", equation="param;")
devsim.print_node_values(device=device, region=region, name="deptest")
devsim.set_parameter(name="param", value=1.0)
devsim.print_node_values(device=device, region=region, name="deptest")

devsim.node_model(device=device, region=region, name="cdeptest", equation="cnode1;")
devsim.print_node_values(device=device, region=region, name="cdeptest")
devsim.set_circuit_node_value(node="cnode1", value=-1.0)
devsim.print_node_values(device=device, region=region, name="cdeptest")

