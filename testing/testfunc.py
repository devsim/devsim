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
from math import cos
from math import sin
device = "MyDevice"
region = "MyRegion"

####
#### Meshing
####

create_1d_mesh(mesh="dog")
add_1d_mesh_line(mesh="dog", pos=0.0, ps=0.1, tag="top")
add_1d_mesh_line(mesh="dog", pos=1.0, ps=0.1, tag="bot")
add_1d_contact  (mesh="dog", name="top", material="metal", tag="top")
add_1d_contact  (mesh="dog", name="bot", material="metal", tag="bot")
add_1d_region   (mesh="dog", material="Si", region=region, tag1="top", tag2="bot")
finalize_mesh(mesh="dog")
create_device(mesh="dog", device=device)

symdiff(expr="declare(gee(x,y))")
symdiff(expr="declare(sin(x))")
symdiff(expr="define(cos(x), -sin(x))")
symdiff(expr="define(sin(x),  cos(x))")

#proc cos {x} {
#  return [expr cos($x)]
#}

#proc sin {x} {
#  return [expr sin($x)]
#}
#
# This is a degenerate case causing the result object of this procedure to be the same as
# one of our original arguments
def gee(x, y):
    return x

register_function(name="cos", procedure=cos, nargs=1)
register_function(name="sin", procedure=sin, nargs=1)
register_function(name="gee", procedure=gee, nargs=2)
node_model(device=device, region=region, name="gee", equation="gee(x, 1)")
node_model(device=device, region=region, name="foo", equation="sin(3.14159*0.25)")
node_model(device=device, region=region, name="bar", equation="diff(sin(x),x)")
print_node_values(device=device, region=region, name="gee")
print_node_values(device=device, region=region, name="foo")
print_node_values(device=device, region=region, name="bar")


