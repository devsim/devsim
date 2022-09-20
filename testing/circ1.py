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

#### circ1.tcl
####
#### two devices which connected to one another

import devsim
import test_common

devices=("MyDevice1", "MyDevice2")
region="MyRegion"

####
#### Meshing
####
test_common.CreateSimpleMesh(device=devices[0], region=region)
devsim.create_device(mesh="dog", device=devices[1])

for device in devices:
    test_common.SetupResistorConstants(device=device, region="")
    test_common.SetupInitialResistorSystem(device, region)

devsim.add_circuit_node(name="cnode1", variable_update="log_damp")
devsim.circuit_element(name="R1", n1="cnode1", n2=0, value=1e15)

# ground these contacts
devsim.set_parameter(device="MyDevice1", name="topbias", value=0.0)
devsim.set_parameter(device="MyDevice2", name="botbias", value=0.0)

test_common.SetupInitialResistorContact(device="MyDevice1", contact="top", use_circuit_bias=False)
test_common.SetupInitialResistorContact(device="MyDevice1", contact="bot", use_circuit_bias=True, circuit_node="cnode1")

test_common.SetupInitialResistorContact(device="MyDevice2", contact="top", use_circuit_bias=True, circuit_node="cnode1")
test_common.SetupInitialResistorContact(device="MyDevice2", contact="bot", use_circuit_bias=False)

devsim.solve(type="dc", absolute_error=1.0, relative_error=1e-14, maximum_iterations=30)

for device in devices:
    print(device)
    for name in ("Potential", "IntrinsicElectrons"):
        devsim.print_node_values(device=device, region=region, name=name)

for device in devices:
    test_common.SetupCarrierResistorSystem(device=device, region=region)

test_common.SetupCarrierResistorContact(device="MyDevice1", contact="top", use_circuit_bias=False)
test_common.SetupCarrierResistorContact(device="MyDevice1", contact="bot", use_circuit_bias=True, circuit_node="cnode1")

test_common.SetupCarrierResistorContact(device="MyDevice2", contact="top", use_circuit_bias=True, circuit_node="cnode1")
test_common.SetupCarrierResistorContact(device="MyDevice2", contact="bot", use_circuit_bias=False)

for v in (0.0, 0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.10):
    print("topbias %f" % v)
    devsim.set_parameter(device="MyDevice1", name="topbias", value=v)
    devsim.solve(type="dc", absolute_error=1, relative_error=1e-9, maximum_iterations=30)
    test_common.print_circuit_solution()
    for device in devices:
        test_common.printResistorCurrent(device=device, contact="top")
        test_common.printResistorCurrent(device=device, contact="bot")

