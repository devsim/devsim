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
def cap_mesh(region_name, material_name):
    '''
      Creates a simple cap mesh named cap1 for test purposes with a given region name
      Contacts, top, bot
      length 1.0
      material "Ox"
      mesh can be instantiated as a device
    '''
    create_1d_mesh(mesh="cap1")
    for p, s, t in ((0.0, 0.1, "top"), (1.0, 0.1, "bot")):
        add_1d_mesh_line(mesh = "cap1", pos=p, ps = s, tag = t)

    for t in ('top', 'bot'):
        add_1d_contact(  mesh = "cap1", name=t, tag=t, material="metal")

    add_1d_region(   mesh = "cap1", material=material_name, region=region_name, tag1 ="top" , tag2 = "bot")
    finalize_mesh(mesh="cap1")
    return "cap1"
