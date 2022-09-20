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
import test_common

devsim.circuit_element(name="V1", n1="topbias", n2=0, acreal=1.0)

device="MyDevice"
region="MyRegion"

####
#### Meshing
####
devsim.create_2d_mesh(mesh="dog")
devsim.add_2d_mesh_line(mesh="dog", dir="x", pos=0, ps=1e-6)
devsim.add_2d_mesh_line(mesh="dog", dir="x", pos=1e-5, ps=1e-6)

devsim.add_2d_mesh_line(mesh="dog", dir="y", pos=0, ps=1e-6)
devsim.add_2d_mesh_line(mesh="dog", dir="y", pos=1e-5, ps=1e-6)

devsim.add_2d_mesh_line(mesh="dog", dir="x", pos=-1e-8, ps=1e-8)
devsim.add_2d_mesh_line(mesh="dog", dir="x", pos=1.001e-5, ps=1e-8)

devsim.add_2d_region   (mesh="dog", material="Si", region=region)
devsim.add_2d_region   (mesh="dog", material="Si", region="air1", xl=-1e-8,  xh=0)
devsim.add_2d_region   (mesh="dog", material="Si", region="air2", xl=1.0e-5, xh=1.001e-5)

devsim.add_2d_contact  (mesh="dog", name="top", region=region, xl=0,    xh=0,    bloat=1e-10, material="metal")
devsim.add_2d_contact  (mesh="dog", name="bot", region=region, xl=1e-5, xh=1e-5, bloat=1e-10, material="metal")

devsim.finalize_mesh(mesh="dog")
devsim.create_device(mesh="dog", device=device)

test_common.SetupResistorConstants(device, region)
test_common.SetupInitialResistorSystem(device, region, net_doping=1e17)

test_common.SetupInitialResistorContact(device=device, contact="top", use_circuit_bias=True, circuit_node="topbias")
test_common.SetupInitialResistorContact(device=device, contact="bot", use_circuit_bias=False)
devsim.set_parameter(name="botbias", value=0.0)

devsim.solve(type='dc', absolute_error=1.0, relative_error=1e-10, maximum_iterations=30)

test_common.SetupCarrierResistorSystem(device=device, region=region)
test_common.SetupCarrierResistorContact(device=device, contact="top", use_circuit_bias=True, circuit_node="topbias")
test_common.SetupCarrierResistorContact(device=device, contact="bot", use_circuit_bias=False)

for v in (0.0, 1e-3):
    devsim.circuit_alter(name="V1", value=v)
    devsim.solve(type='dc', absolute_error=1.0e10, relative_error=1e-7, maximum_iterations=30)
    test_common.printResistorCurrent(device=device, contact="top")
    test_common.printResistorCurrent(device=device, contact="bot")

for i in range(4):
    devsim.solve(type='dc', absolute_error=1.0e10, relative_error=1e-7, maximum_iterations=30)

devsim.solve(type="noise", frequency=1e5, output_node="V1.I")

rvx="V1.I_ElectronContinuityEquation_real_gradx"
ivx="V1.I_ElectronContinuityEquation_imag_gradx"
rvy="V1.I_ElectronContinuityEquation_real_grady"
ivy="V1.I_ElectronContinuityEquation_imag_grady"

for name, equation in (
    ("noisesource", "4*ElectronCharge^2 * ThermalVoltage * mu_n * Electrons"),
  ("vfield",      "(%(rvx)s*%(rvx)s+%(ivx)s*%(ivx)s) + (%(rvy)s*%(rvy)s+%(ivy)s*%(ivy)s)" % {"rvx" : rvx, "rvy" : rvy, "ivx" : ivx, "ivy" : ivy}),
  ("noise",       "vfield * noisesource * NodeVolume"),
):
    devsim.node_model(device=device, region=region, name=name, equation=equation)

print(sum(devsim.get_node_model_values(device=device, region=region, name="noise")))

devsim.write_devices(file="noise_res_2d.flps", type="floops")

