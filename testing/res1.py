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
contacts=('top', 'bot')

def run_initial_bias(use_circuit_bias, net_doping=1e16):
    '''
      use_circuit for topbias
    '''
    test_common.SetupResistorConstants(device, region)
    test_common.SetupInitialResistorSystem(device, region, net_doping)
    test_common.SetupInitialResistorContact(device=device, contact='top', use_circuit_bias=use_circuit_bias)
    test_common.SetupInitialResistorContact(device=device, contact='bot')

    #####
    ##### Initial DC Solution
    #####
    devsim.solve(type='dc', absolute_error=1.0, relative_error=1e-10, maximum_iterations=30)

    for name in ("Potential", "IntrinsicElectrons"):
        devsim.print_node_values(device=device, region=region, name=name)

    test_common.SetupCarrierResistorSystem(device, region)
    test_common.SetupCarrierResistorContact(device=device, contact='top', use_circuit_bias=use_circuit_bias)
    test_common.SetupCarrierResistorContact(device, contact='bot')

if __name__ == '__main__':
    test_common.CreateSimpleMesh(device, region)
    devsim.set_parameter(name="topbias", value=0.0)
    devsim.set_parameter(name="botbias", value=0.0)
    run_initial_bias(False)
    for v in (0.0, 0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.10):
        devsim.set_parameter(name="topbias", value=v)
        devsim.solve(type="dc", absolute_error=1.0, relative_error=1e-10, maximum_iterations=30)

        test_common.printResistorCurrent(device=device, contact='top')
        test_common.printResistorCurrent(device=device, contact='bot')

    devsim.print_node_values(device=device, region=region, name="Electrons")
    devsim.print_node_values(device=device, region=region, name="Potential")
    devsim.print_edge_values(device=device, region=region, name="ElectricField")
    devsim.print_edge_values(device=device, region=region, name="ElectronCurrent")

    l=1.0
    q=1.6e-19
    n=1.0e16
    u=400
    R=l/(q*n*u)
    print(0.1/R)

