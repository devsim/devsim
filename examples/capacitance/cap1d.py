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
device="MyDevice"
region="MyRegion"

###
### Create a 1D mesh
###
create_1d_mesh (mesh="mesh1")
add_1d_mesh_line (mesh="mesh1", pos=0.0, ps=0.1, tag="contact1")
add_1d_mesh_line (mesh="mesh1", pos=1.0, ps=0.1, tag="contact2")
add_1d_contact   (mesh="mesh1", name="contact1", tag="contact1", material="metal")
add_1d_contact   (mesh="mesh1", name="contact2", tag="contact2", material="metal")
add_1d_region    (mesh="mesh1", material="Si", region=region, tag1="contact1", tag2="contact2")
finalize_mesh (mesh="mesh1")
create_device (mesh="mesh1", device=device)

###
### Set parameters on the region
###
set_parameter(device=device, region=region, name="Permittivity", value=3.9*8.85e-14)

###
### Create the Potential solution variable
###
node_solution(device=device, region=region, name="Potential")

###
### Creates the Potential@n0 and Potential@n1 edge model
###
edge_from_node_model(device=device, region=region, node_model="Potential")

###
### Electric field on each edge, as well as its derivatives with respect to
### the potential at each node
###
edge_model(device=device, region=region, name="ElectricField",
           equation="(Potential@n0 - Potential@n1)*EdgeInverseLength")

edge_model(device=device, region=region, name="ElectricField:Potential@n0",
           equation="EdgeInverseLength")

edge_model(device=device, region=region, name="ElectricField:Potential@n1",
           equation="-EdgeInverseLength")

###
### Model the D Field
###
edge_model(device=device, region=region, name="DField",
           equation="Permittivity*ElectricField")

edge_model(device=device, region=region, name="DField:Potential@n0",
           equation="diff(Permittivity*ElectricField, Potential@n0)")

edge_model(device=device, region=region, name="DField:Potential@n1",
           equation="-DField:Potential@n0")

###
### Create the bulk equation
###
equation(device=device, region=region, name="PotentialEquation", variable_name="Potential",
         edge_model="DField", variable_update="default")


###
### Contact models and equations
###
for c in ("contact1", "contact2"):
    contact_node_model(device=device, contact=c, name="%s_bc" % c,
                       equation="Potential - %s_bias" % c)

    contact_node_model(device=device, contact=c, name="%s_bc:Potential" % c,
                       equation="1")

    contact_equation(device=device, contact=c, name="PotentialEquation",
                     node_model="%s_bc" % c, edge_charge_model="DField")

###
### Set the contact
###
set_parameter(device=device, region=region, name="contact1_bias", value=1.0e-0)
set_parameter(device=device, region=region, name="contact2_bias", value=0.0)

###
### Solve
###
solve(type="dc", absolute_error=1.0, relative_error=1e-10, maximum_iterations=30)

###
### Print the charge on the contacts
###
for c in ("contact1", "contact2"):
    print ("contact: %s charge: %1.5e" % (c, get_contact_charge(device=device, contact=c, equation="PotentialEquation")))

