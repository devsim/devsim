# Copyright 2013 DEVSIM LLC
#
# SPDX-License-Identifier: Apache-2.0

from devsim import *
#print region_info
device ="d1"
region ="r1"

create_1d_mesh(mesh="dog")
for p, s, t in ((0.0, 0.1, "top"), (1.0, 0.1, "bot")):
    add_1d_mesh_line(mesh ="dog", pos=p, ps = s, tag = t)

for t in ('top', 'bot'):
    add_1d_contact(  mesh ="dog", name=t, tag=t, material="metal")

add_1d_region(   mesh ="dog", material="Si", region = region, tag1 ="top" , tag2 ="bot")
finalize_mesh(mesh="dog")
create_device(mesh="dog", device = device)

#avoid contention with sqlite2 test
create_db(filename="foodb2")
add_db_entry(material="global", parameter="q"  , value=1.60217646e-19, unit="coul", description="Electron Charge")
add_db_entry(material="Si"    , parameter="one", value=1             , unit=""    , description="")

node_model(device=device, region=region, name="test", equation="one * q")

print_node_values(device=device, region=region, name="test")

add_db_entry(material="global", parameter="q", value=2, unit="coul", description="Electron Charge")
print_node_values (device=device, region=region, name="test")
add_db_entry(material="Si", parameter="one", value=3, unit="", description="")
print(("Material: %s" % get_material(device=device, region=region)))
print_node_values (device=device, region=region, name="test")
set_material(device=device, region=region, material="Ox")
add_db_entry(material="Ox", parameter="one", value=4, unit="", description="")
print_node_values (device=device, region=region, name="test")

