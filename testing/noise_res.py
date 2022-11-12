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
test_common.CreateNoiseMesh(res1.device, res1.region)
devsim.set_parameter(name="botbias", value=0.0)
res1.run_initial_bias(use_circuit_bias=True, net_doping=1e17)


for v in (0.0, 1e-3):
    devsim.circuit_alter(name="V1", value=v)
    devsim.solve(type="dc", absolute_error=1e10, relative_error=1e-7, maximum_iterations=30)
    for contact in res1.contacts:
        test_common.printResistorCurrent(device=res1.device, contact=contact)
devsim.solve(type="dc", absolute_error=1e10, relative_error=1e-7, maximum_iterations=30)

devsim.solve(type="noise", frequency=1e5, output_node="V1.I")
devsim.vector_gradient(device=res1.device, region=res1.region, node_model="V1.I_ElectronContinuityEquation_real", calc_type="avoidzero")
for name in (
    "V1.I_ElectronContinuityEquation_real",
  "V1.I_ElectronContinuityEquation_imag",
  "V1.I_ElectronContinuityEquation_real_gradx",
  "V1.I_ElectronContinuityEquation_imag_gradx",
):
    devsim.print_node_values(device=res1.device, region=res1.region, name=name)

rv="V1.I_ElectronContinuityEquation_real_gradx"
iv="V1.I_ElectronContinuityEquation_imag_gradx"

for name, equation in (
    ("noisesource", "4*ElectronCharge^2 * ThermalVoltage * mu_n * Electrons"),
  ("vfield", "(%(rv)s*%(rv)s+%(iv)s*%(iv)s)" % {'rv' : rv, 'iv' : iv}),
  ("noise", "vec_sum(vfield * noisesource * NodeVolume)"),
):
    devsim.node_model(device=res1.device, region=res1.region, name=name, equation=equation)
    devsim.print_node_values(device=res1.device, region=res1.region, name=name)

