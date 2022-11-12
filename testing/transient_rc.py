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
import math
from collections import defaultdict

class RCCircuit:
    def __init__(self):
        self.R = 5
        self.C = 5
        self.V = 1.0

    def set_circuit_voltage(self, v):
        self.V = v
        devsim.circuit_alter(name="V1", value=v)

    def create_circuit(self):
        # ssac circuit only
        devsim.circuit_element(name="V1", n1=1, n2=0, value=self.V)
        #lambda = 0.04
        devsim.circuit_element(name="R1", n1=1, n2=2, value=self.R)
        devsim.circuit_element(name="C1", n1=2, n2=0, value=self.C)


    def calculate_expected_voltage_current(self, vprev, tstep):
        '''
        assumes simulation voltage is 0.0
        '''
        v = vprev * math.exp(-tstep/(self.R*self.C))
        i = v/self.R
        return(v, i)



def bdf1(tstep):
    devsim.solve(type="transient_bdf1", absolute_error=1.0, relative_error=1e-14, maximum_iterations=3, tdelta=tstep, charge_error=1e-2)

def bdf1twice(tstep):
    bdf1(0.5*tstep)
    bdf1(0.5*tstep)

def tr(tstep):
    devsim.solve(type="transient_tr", absolute_error=1.0, relative_error=1e-14, maximum_iterations=3, tdelta=tstep, charge_error=1e-2)

def trbdf(tstep):
    gamma = 2 - math.sqrt(2.0)
    devsim.solve(type="transient_tr",   absolute_error=1.0, relative_error=1e-14, maximum_iterations=3, tdelta=tstep, gamma=gamma, charge_error=1e-2)
    devsim.solve(type="transient_bdf2", absolute_error=1.0, relative_error=1e-14, maximum_iterations=3, tdelta=tstep, gamma=gamma, charge_error=1e-2)

def run_simulation(circuit, tstep, tsolve1, tsolve2):
    circuit.set_circuit_voltage(1.0)
    devsim.solve(type="transient_dc", absolute_error=1.0, relative_error=1e-14, maximum_iterations=3)

    vprev = circuit.V
    circuit.set_circuit_voltage(0.0)

    vals = []

    for i in range(10):
        if (i == 0):
            tsolve1(tstep)
        else:
            tsolve2(tstep)
        (vnew, inew) = circuit.calculate_expected_voltage_current(vprev=vprev, tstep=tstep)
        vc = devsim.get_circuit_node_value(node=2, solution='dcop')
        ic = devsim.get_circuit_node_value(node='V1.I', solution='dcop')
        vals.append((vc, vnew, (vc-vnew)/vnew, ic, inew, (ic-inew)/inew))
        vprev = vnew
    return vals

circuit = RCCircuit()
circuit.V=1.0
circuit.R=5
circuit.C=5

circuit.create_circuit()

results = defaultdict(list)
for tstep in (0.1, 0.01, 0.001, 0.0001):
    print("BDF1 Method")
    results['bdf1'].append((tstep, run_simulation(circuit=circuit, tstep=tstep, tsolve1=bdf1twice, tsolve2=bdf1)))

    print("TR Method")
    # the initial timestep should be bdf1 because of the large step in the bias voltage
    results['tr'].append((tstep, run_simulation(circuit=circuit, tstep=tstep, tsolve1=bdf1twice, tsolve2=tr)))

    print("TRBDF Method")
    results['trbdf'].append((tstep, run_simulation(circuit=circuit, tstep=tstep, tsolve1=bdf1twice, tsolve2=trbdf)))

for k, datasets in results.items():
    for tstep, vals in datasets:
        print("method: %s, tstep: %1.5e" % (k, tstep))
        for i in vals:
            print("%1.5e %1.5e %1.5e %1.5e %1.5e %1.5e" % i)
        print()

