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

devsim.load_devices( file="mesh1.msh")

for x in devsim.get_device_list():
    for y in devsim.get_region_list(device=x):
        print("%s %s" % (x, y))

device="MyDevice"
region="MyRegion"

devsim.set_parameter( device=device, region=region, name="Permittivity", value=3.9*8.85e-14)
devsim.set_parameter( device=device, region=region, name="ElectricCharge", value=1.6e-19)


devsim.node_solution( device=device, region=region, name="Potential")

devsim.edge_from_node_model(device=device, region=region, node_model="Potential")

devsim.edge_model( device=device, region=region, name="ElectricField",              equation="(Potential@n0 - Potential@n1)*EdgeInverseLength")
devsim.edge_model( device=device, region=region, name="ElectricField:Potential@n0", equation="EdgeInverseLength")
devsim.edge_model( device=device, region=region, name="ElectricField:Potential@n1", equation="-EdgeInverseLength")

devsim.set_parameter( device=device, region=region, name="topbias",    value=1.0e-0)
devsim.set_parameter( device=device, region=region, name="botbias", value=0.0)

devsim.edge_model( device=device, region=region, name="PotentialEdgeFlux", equation="Permittivity*ElectricField")
devsim.edge_model( device=device, region=region, name="PotentialEdgeFlux:Potential@n0", equation="diff(Permittivity*ElectricField, Potential@n0)")
devsim.edge_model( device=device, region=region, name="PotentialEdgeFlux:Potential@n1", equation="-PotentialEdgeFlux:Potential@n0")

devsim.equation( device=device, region=region, name="PotentialEquation", variable_name="Potential", edge_model="PotentialEdgeFlux", variable_update="default")

devsim.node_model( device=device, region=region, name="topnode_model"          , equation="Potential - topbias")
devsim.node_model( device=device, region=region, name="topnode_model:Potential", equation="1")

devsim.node_model( device=device, region=region, name="botnode_model"          , equation="Potential - botbias")
devsim.node_model( device=device, region=region, name="botnode_model:Potential", equation="1")

devsim.contact_equation( device=device, contact="top", name="PotentialEquation", node_model="topnode_model",
                         edge_charge_model="PotentialEdgeFlux")

devsim.contact_equation( device=device, contact="bot", name="PotentialEquation", node_model="botnode_model",
                         edge_charge_model="PotentialEdgeFlux")

devsim.solve(type="dc", absolute_error=1.0, relative_error=1e-10, maximum_iterations=30)

print(devsim.get_contact_charge(device=device, contact="top", equation="PotentialEquation"))
print(devsim.get_contact_charge(device=device, contact="bot", equation="PotentialEquation"))

devsim.print_node_values(device=device, region=region, name="NodeVolume")
devsim.print_edge_values(device=device, region=region, name="ElectricField")
devsim.print_edge_values(device=device, region=region, name="EdgeCouple")


