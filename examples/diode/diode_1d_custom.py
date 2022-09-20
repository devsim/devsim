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

from devsim import *
import devsim.python_packages.simple_physics as simple_physics
import diode_common
#####
# dio1
#
# Make doping a step function
# print dat to text file for viewing in grace
# verify currents analytically
# in dio2 add recombination
#

device="MyDevice"
region="MyRegion"

diode_common.CreateMesh(device=device, region=region)

diode_common.SetParameters(device=device, region=region)
set_parameter(device=device, region=region, name="taun", value=1e-8)
set_parameter(device=device, region=region, name="taup", value=1e-8)

diode_common.SetNetDoping(device=device, region=region)

print_node_values(device=device, region=region, name="NetDoping")

diode_common.InitialSolution(device, region)

# Initial DC solution
solve(type="dc", absolute_error=1.0, relative_error=1e-10, maximum_iterations=30)

diode_common.DriftDiffusionInitialSolution(device, region)
###
### Drift diffusion simulation at equilibrium
###
solve(type="dc", absolute_error=1e10, relative_error=1e-10, maximum_iterations=30)


class ContactBC:
    def __init__(self, device, region, contact, variable):
        self.device = device
        self.region = region
        self.contact = contact
        self.variable = variable
        self.contact_nodes = None
        self.node_volume = None
        self.contact_bias_name = None

    def initialize(self):
        self.equation_numbers=get_equation_numbers(device=self.device, region=self.region, variable=self.variable)
        contact_nodes = []
        for e in get_element_node_list(device=self.device, region=self.region, contact=self.contact):
            contact_nodes.extend(e)
        self.contact_nodes = sorted(set(contact_nodes))
        self.node_volume = get_node_model_values(device=self.device, region=self.region, name=self.variable)
        self.contact_bias_name = simple_physics.GetContactBiasName(self.contact)

    def assemble(self, what, timemode):
        '''
          This assumes that constant bias conditions
        '''
        if not self.contact_nodes:
            self.initialize()

        rcv=[]
        rv=[]

        if timemode != "DC":
            return [rcv, rv]

        if  what != "MATRIXONLY":
            bias = get_parameter(device=self.device, name=self.contact_bias_name)
            var = get_node_model_values(device=self.device, region=self.region, name=self.variable)
            bias_offset = get_node_model_values(device=self.device, region=self.region, name="bias_offset")
            for c in self.contact_nodes:
                r = self.equation_numbers[c]
                v = (var[c] - bias + bias_offset[c]) * self.node_volume[c]
                rv.extend([r, v])

        if  what != "RHS":
            # this is the derivative
            for c in self.contact_nodes:
                r = self.equation_numbers[c]
                v = self.node_volume[c]
                rcv.extend([r, r, v])

        # always return False for boundary conditions
        return rcv, rv, False

    def getAssembleFunc(self):
        return lambda what, timemode: self.assemble(what, timemode)

# how return function operating on one class
f=ContactBC(device=device, region=region, contact="top", variable="Potential")
custom_equation(name="test1", procedure=f.getAssembleFunc())

### deactivate the existing contact model
node_model(device=device, region=region, name=simple_physics.GetContactNodeModelName("top"), equation="0")
node_model(device=device, region=region, name=simple_physics.GetContactNodeModelName("top")+":Potential", equation="0")

### need the bias offset based on the doping
node_model(device=device, region=region, name="bias_offset", equation="ifelse(NetDoping > 0, \
    -V_t*log({0}/n_i), \
    V_t*log({1}/n_i))".format(simple_physics.celec_model, simple_physics.chole_model))


####
#### Ramp the bias to 0.5 Volts
####
v = 0.0
while v < 0.51:
    set_parameter(device=device, name=simple_physics.GetContactBiasName("top"), value=v)
    solve(type="dc", absolute_error=1e10, relative_error=1e-10, maximum_iterations=30)
    simple_physics.PrintCurrents(device, "top")
    simple_physics.PrintCurrents(device, "bot")
    v += 0.1

write_devices(file="diode_1d_custom.dat", type="tecplot")

