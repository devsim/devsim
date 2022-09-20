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

devsim.load_devices( file="mesh2.msh")

for x in devsim.get_device_list():
    for y in devsim.get_region_list(device=x):
        print("%s %s" % (x, y))

device="MyDevice"
interface="MySiOx"
regions=("MyOxRegion", "MySiRegion")

devsim.set_parameter( device=device, region="MySiRegion", name="Permittivity", value=11.1*8.85e-14)
devsim.set_parameter( device=device, region="MySiRegion", name="ElectricCharge", value=1.6e-19)

devsim.set_parameter( device=device, region="MyOxRegion", name="Permittivity", value=3.9*8.85e-14)
devsim.set_parameter( device=device, region="MyOxRegion", name="ElectricCharge", value=1.6e-19)

for region in regions:
    devsim.node_solution( device=device, region=region, name="Potential")
    devsim.edge_from_node_model( device=device, region=region, node_model="Potential")

    devsim.edge_model( device=device, region=region, name="ElectricField", equation="(Potential@n0 - Potential@n1)*EdgeInverseLength")
    devsim.edge_model( device=device, region=region, name="ElectricField:Potential@n0", equation="EdgeInverseLength")
    devsim.edge_model( device=device, region=region, name="ElectricField:Potential@n1", equation="-EdgeInverseLength")

    devsim.edge_model( device=device, region=region, name="PotentialEdgeFlux", equation="Permittivity*ElectricField")
    devsim.edge_model( device=device, region=region, name="PotentialEdgeFlux:Potential@n0", equation="diff(Permittivity*ElectricField, Potential@n0)")
    devsim.edge_model( device=device, region=region, name="PotentialEdgeFlux:Potential@n1", equation="-PotentialEdgeFlux:Potential@n0")

    devsim.equation( device=device, region=region, name="PotentialEquation", variable_name="Potential", edge_model="PotentialEdgeFlux", variable_update="default")

devsim.set_parameter( device=device, name="topbias",    value=1.0)
devsim.set_parameter( device=device, name="botbias", value=0.0)



devsim.node_model( device=device, region="MySiRegion", name="topnode_model"          , equation="Potential - topbias")
devsim.node_model( device=device, region="MySiRegion", name="topnode_model:Potential", equation="1")

devsim.node_model( device=device, region="MyOxRegion", name="botnode_model"          , equation="Potential - botbias")
devsim.node_model( device=device, region="MyOxRegion", name="botnode_model:Potential", equation="1")

devsim.contact_equation( device=device, contact="top", name="PotentialEquation",
                         node_model="topnode_model", edge_charge_model="PotentialEdgeFlux")

devsim.contact_equation( device=device, contact="top", name="PotentialEquation",
                         node_model="topnode_model", edge_charge_model="PotentialEdgeFlux")

devsim.contact_equation( device=device, contact="bot", name="PotentialEquation",
                         node_model="botnode_model", edge_charge_model="PotentialEdgeFlux")

# type continuous means that regular equations in both regions are swapped into the primary region
devsim.interface_model( device=device, interface=interface, name="continuousPotential", equation="Potential@r0-Potential@r1")
devsim.interface_model( device=device, interface=interface, name="continuousPotential:Potential@r0", equation= "1")
devsim.interface_model( device=device, interface=interface, name="continuousPotential:Potential@r1", equation="-1")
devsim.interface_equation( device=device, interface=interface, name="PotentialEquation", interface_model="continuousPotential", type="continuous")



def print_charge():
    print(devsim.get_contact_charge( device=device, contact="top", equation="PotentialEquation"))
    print(devsim.get_contact_charge( device=device, contact="bot", equation="PotentialEquation"))
def print_flux():
    devsim.print_edge_values(device=device, region="MySiRegion", name="PotentialEdgeFlux")
    devsim.print_edge_values(device=device, region="MyOxRegion", name="PotentialEdgeFlux")
def set_permittivities(ox, si):
    devsim.set_parameter( device=device, region="MySiRegion", name="Permittivity", value=si*8.85e-14)
    devsim.set_parameter( device=device, region="MyOxRegion", name="Permittivity", value=ox*8.85e-14)

devsim.solve(type="dc", absolute_error=1.0, relative_error=1e-10, maximum_iterations=30)
print_charge()
print_flux()

set_permittivities(si=1.0, ox=1.0)
devsim.solve(type="dc", absolute_error=1.0, relative_error=1e-10, maximum_iterations=30)
print_charge()
print_flux()

# makes sure this global scope does not interfere
devsim.set_parameter( device=device, name="Permittivity", value=11.1*8.85e-14)

set_permittivities(si=11.1, ox=3.9)
devsim.solve(type="dc", absolute_error=1.0, relative_error=1e-10, maximum_iterations=30)
print_charge()
print_flux()

devsim.print_edge_values(device=device, region="MySiRegion", name="ElectricField")
devsim.print_edge_values(device=device, region="MyOxRegion", name="ElectricField")

devsim.print_node_values(device=device, region="MySiRegion", name="Potential")
devsim.print_node_values(device=device, region="MyOxRegion", name="Potential")

