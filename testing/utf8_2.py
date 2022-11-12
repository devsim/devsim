# -*- coding: utf-8 -*-
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

##Title: cap1.py
##Purpose: Simple example for 1D capacitor
from devsim import *

device_info = {'device' : 'MyDevice'}
region_info = device_info
region_info['region'] = "MyRegion"

#print region_info
device = "MyDevice"
region = "MyRegion"

create_1d_mesh(mesh="dog")
for p, s, t in ((0.0, 0.1, "top"), (1.0, 0.1, "bot")):
    add_1d_mesh_line(mesh = "dog", pos=p, ps = s, tag = t)

for t in ('top', 'bot'):
    add_1d_contact(  mesh = "dog", name=t, material="metal", tag=t)

add_1d_region(   mesh = "dog", material="Si", region = region, tag1 ="top" , tag2 = "bot")
finalize_mesh(mesh="dog")
create_device(mesh="dog", device = device)

#for i in get_device_list():
#  print i


set_parameter(name = "Permittivity", value=(3.9*8.85e-14), **region_info)
set_parameter(name = "ElectricCharge", value=1.6e-19, **region_info)

print((get_parameter(name="Permittivity", **region_info)))
print((get_parameter(name="ElectricCharge", **region_info)))

node_solution(name="ψ", **region_info)
edge_from_node_model(node_model="ψ", **region_info)

for n, e in (
    ('ElectricField',              '(ψ@n0 - ψ@n1)*EdgeInverseLength;'),
    ('ElectricField:ψ@n0', 'EdgeInverseLength;'),
    ('ElectricField:ψ@n1', '-EdgeInverseLength;')
) :
    edge_model(name = n, equation = e, **region_info)

set_parameter(name="topbias",    value = 1.0e-0, **region_info)
set_parameter(name="botbias", value=0.0, **region_info)

for n, e in(
    ("ψEdgeFlux",              "Permittivity*ElectricField;"),
    ("ψEdgeFlux:ψ@n0", "diff(Permittivity*ElectricField, ψ@n0);"),
    ("ψEdgeFlux:ψ@n1", "-ψEdgeFlux:ψ@n0;")
) :
    edge_model(name = n, equation = e, **region_info)

equation(name="ψEquation", variable_name="ψ", node_model="", edge_model="ψEdgeFlux", time_node_model="", variable_update="default", **region_info)

conteq="Permittivity*ElectricField;"
node_model(name="topnode_model",           equation = "ψ - topbias;", **region_info)
node_model(name="topnode_model:ψ", equation = "1;", **region_info)
edge_model(name="contactcharge_edge_top",  equation =conteq, **region_info)

node_model(name = "botnode_model",           equation = "ψ - botbias;", **region_info)
node_model(name = "botnode_model:ψ", equation = "1;", **region_info)
edge_model(name = "contactcharge_edge_bottom",  equation =conteq, **region_info)

contact_equation( device=device , contact= "top" , name= "ψEquation"
                  , node_model="topnode_model", edge_model= ""
                  , node_charge_model= "" , edge_charge_model= "contactcharge_edge_top"
                  , node_current_model= ""   , edge_current_model= "")

contact_equation( device=device , contact= "bot" , name= "ψEquation"
                  , node_model="botnode_model", edge_model= ""
                  , node_charge_model= "" , edge_charge_model= "contactcharge_edge_bottom"
                  , node_current_model= ""   , edge_current_model= "")



solve( type="dc", absolute_error= 1.0 , relative_error= 1e-10 , maximum_iterations= 30)

print((get_contact_charge(device=device , contact="top", equation="ψEquation")))
print((get_contact_charge(device=device , contact="bot", equation="ψEquation")))

#write_devices(file='cap1.msh', type='devsim')

