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

device="MyDevice"
region="MyRegion"

xmin=0.0
xmax=1.0
ymin=0.0
### not that ymid my not exactly line up with a 0.5 gathered through summation, so use closes bloat with triangle rule to claim ownership
ymid1=0.5
ymid2=0.6
ymax=1.0

devsim.create_2d_mesh(   mesh="dog")
devsim.add_2d_mesh_line(mesh="dog", dir="y", pos=-0.001, ps=0.001)
devsim.add_2d_mesh_line(mesh="dog", dir="x", pos=xmin,   ps=0.1)
devsim.add_2d_mesh_line(mesh="dog", dir="x", pos=xmax,   ps=0.1)
devsim.add_2d_mesh_line(mesh="dog", dir="y", pos=ymin,   ps=0.1)
devsim.add_2d_mesh_line(mesh="dog", dir="y", pos=ymax,   ps=0.1)
#in order to capture interfaces
devsim.add_2d_mesh_line(mesh="dog", dir="y", pos=+1.001, ps=0.001)
# color triangles not nodes or edges
# error if region doesn't have any triangles
# last add_region has highest precedence
# allow use of names or tags
# check my parser to verify that adding is possible (via dynamic_cast)
# note this region won't have equation numbers
devsim.add_2d_region( mesh="dog", material="gas", region="gas1", yl=-.001, yh=0.0)
devsim.add_2d_region( mesh="dog", material="gas", region="gas2", yl=1.0, yh=1.001)
devsim.add_2d_region( mesh="dog", material="Oxide", region="r0", xl=xmin, xh=xmax, yl=ymid1, yh=ymin)
devsim.add_2d_region( mesh="dog", material="Silicon", region="r1", xl=xmin, xh=xmax, yl=ymid2, yh=ymid1)
devsim.add_2d_region( mesh="dog", material="Silicon", region="r2", xl=xmin, xh=xmax, yl=ymid2, yh=ymax)
#warn when leftover triangles are not encapsulated by region
# not specifying bounding box means all intersecting nodes
#bounding box is optional
devsim.add_2d_interface( mesh="dog", name="i0", region0="r0", region1="r1")
devsim.add_2d_interface( mesh="dog", name="i1", region0="r1", region1="r2", xl=0, xh=1, yl=ymid2, yh=ymid2, bloat=1.0e-10)
devsim.add_2d_contact( mesh="dog", name="top", region="r0", yl=ymin, yh=ymin, bloat=1.0e-10, material="metal")
devsim.add_2d_contact( mesh="dog", name="bot", region="r2", yl=ymax, yh=ymax, bloat=1.0e-10, material="metal")
#contact can only apply interface
devsim.finalize_mesh( mesh="dog")
devsim.create_device(mesh="dog", device=device)

devsim.node_model(device=device, name="NetDoping", region="gas1", equation="1e18")
devsim.node_model(device=device, name="NetDoping", region="r0",   equation="1e18")
devsim.node_model(device=device, name="NetDoping", region="r1",   equation="-1e15")
devsim.node_model(device=device, name="NetDoping", region="r2",   equation="-1e15")
devsim.node_model(device=device, name="NetDoping", region="gas2", equation="-1e15")

devsim.interface_normal_model(device=device, region="r0", interface="i0")
devsim.interface_normal_model(device=device, region="r1", interface="i0")
devsim.interface_normal_model(device=device, region="r1", interface="i1")
devsim.interface_normal_model(device=device, region="r2", interface="i1")

devsim.set_parameter(name="raxis_zero", value=0.0)
devsim.set_parameter(name="raxis_variable", value="x")

for region in ("r0", "r1", "r2"):
    devsim.cylindrical_node_volume(  device=device, region=region)
    devsim.cylindrical_edge_couple(  device=device, region=region)
    devsim.cylindrical_surface_area( device=device, region=region)

devsim.write_devices(file="mesh2d.flps", type="floops")
devsim.write_devices(file="mesh2d.msh",  type="devsim")
devsim.write_devices(file="mesh2d.dat",  type="tecplot")

