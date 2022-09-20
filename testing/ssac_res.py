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

# solid state resistor ssac
import devsim
import res1
import test_common

devsim.circuit_element(name="V1", n1="topbias", n2=0, acreal=1.0)
test_common.CreateSimpleMesh(res1.device, res1.region)
devsim.set_parameter(name="botbias", value=0.0)
res1.run_initial_bias(use_circuit_bias=True)

for v in (0.0, 0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.10):
    devsim.circuit_alter(name="V1", value=v)
    devsim.solve(type="dc", absolute_error=1.0, relative_error=1e-10, maximum_iterations=30)
    for contact in res1.contacts:
        test_common.printResistorCurrent(device=res1.device, contact=contact)

for f in (0, 1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9, 1e10, 1e11, 1e12):
    devsim.solve(type="ac", frequency=f)
    test_common.print_ac_circuit_solution()

for x in devsim.get_circuit_node_list():
    for y in devsim.get_circuit_solution_list():
        z=devsim.get_circuit_node_value(node=x, solution=y)
        print("%s %s %1.15e" % (x,y,z))

