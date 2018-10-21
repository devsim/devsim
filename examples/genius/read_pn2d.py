# Copyright 2013 Devsim LLC
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

mesh_name = "pn2d"
result = create_genius_mesh(file=mesh_name+".cgns", mesh=mesh_name)

contacts = {}
for region_name, region_info in result['mesh_info']['regions'].iteritems():
  add_genius_region(mesh=mesh_name, genius_name=region_name, region=region_name, material=region_info['material'])
  for boundary, is_electrode in region_info['boundary_info'].iteritems():
    if is_electrode:
      if boundary in contacts:
        contacts[boundary].append(region_name)
      else:
        contacts[boundary] = [region_name, ]

for contact, regions in contacts.iteritems():
  if len(regions) == 1:
    add_genius_contact(mesh=mesh_name, genius_name=contact, name=contact, region=regions[0], material='metal')
  else:
    for region in regions:
      add_genius_contact(mesh=mesh_name, genius_name=contact, name=contact+'@'+region, region=region, material='metal')



for boundary_name, regions in result['mesh_info']['boundaries'].iteritems():
  if (len(regions) == 2):
    add_genius_interface(mesh=mesh_name, genius_name=boundary_name, name=boundary_name, region0=regions[0], region1=regions[1])

finalize_mesh(mesh=mesh_name)
create_device(mesh=mesh_name, device=mesh_name)
write_devices(file=mesh_name+'.msh', type='devsim')
write_devices(file=mesh_name+'.dat', type='tecplot')

for i in get_parameter_list(device=mesh_name):
  print("%s %s" % (i, get_parameter(device=mesh_name, name=i)))
for i in get_region_list(device=mesh_name):
  for j in get_parameter_list(device=mesh_name, region=i):
    print("%s %s %s" % (i, j, get_parameter(device=mesh_name, region=i, name=j)))
#  print "%s %s" % (region, region_info)
#print result['mesh_info']['boundaries']
