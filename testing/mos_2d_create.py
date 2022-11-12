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
device="mymos"

device_width=   1.0e-4
gate_width=     1.0e-5
diffusion_width=0.4

air_thickness=      1e-7
oxide_thickness=    1e-5
gate_thickness=     1e-5
device_thickness=   1e-4
diffusion_thickness=1e-5

x_diffusion_decay=  1e-20
y_diffusion_decay=  1e-10

bulk_doping=        -1e15
body_doping=        -1e19
drain_doping=       1e19
source_doping=      1e19
gate_doping=        1e20

y_channel_spacing=    1e-8
y_diffusion_spacing=  1e-6
y_gate_top_spacing=   1e-8
y_gate_mid_spacing=   (gate_thickness * 0.25)
y_gate_bot_spacing=   1e-8
y_oxide_mid_spacing=  (oxide_thickness * 0.25)
x_channel_spacing=  1e-6
x_diffusion_spacing=1e-5
max_y_spacing=      1e-4
max_x_spacing=      1e-2
y_bulk_mid_spacing=(device_thickness * 0.25)
y_bulk_bottom_spacing=1e-8

x_bulk_left=  0.0
x_bulk_right= (x_bulk_left + device_width)
x_center=     (0.5 * (x_bulk_left + x_bulk_right))
x_gate_left=  (x_center - 0.5 * (gate_width))
x_gate_right= (x_center + 0.5 * (gate_width))
x_device_left=(x_bulk_left - air_thickness)
x_device_right=(x_bulk_right + air_thickness)

y_bulk_top=     0.0
y_oxide_top=    (y_bulk_top - oxide_thickness)
y_oxide_mid=    (0.5 * (y_oxide_top + y_bulk_top))
y_gate_top=     (y_oxide_top - gate_thickness)
y_gate_mid=     (0.5 * (y_gate_top + y_oxide_top))
y_device_top=   (y_gate_top - air_thickness)
y_bulk_bottom=  (y_bulk_top + device_thickness)
y_bulk_mid=     (0.5 * (y_bulk_top + y_bulk_bottom))
y_device_bottom=(y_bulk_bottom + air_thickness)
y_diffusion=    (y_bulk_top + diffusion_thickness)

devsim.create_2d_mesh(  mesh="mos")
devsim.add_2d_mesh_line(mesh="mos", dir="y", pos=y_device_top, ps=max_y_spacing)
devsim.add_2d_mesh_line(mesh="mos", dir="y", pos=y_gate_top, ps=y_gate_top_spacing)
devsim.add_2d_mesh_line(mesh="mos", dir="y", pos=y_gate_mid, ps=y_gate_mid_spacing)
devsim.add_2d_mesh_line(mesh="mos", dir="y", pos=y_oxide_top, ps=y_gate_bot_spacing, ns=y_oxide_mid_spacing)
devsim.add_2d_mesh_line(mesh="mos", dir="y", pos=y_oxide_mid, ps=y_oxide_mid_spacing)
devsim.add_2d_mesh_line(mesh="mos", dir="y", pos=y_bulk_top, ns=y_oxide_mid_spacing, ps=y_channel_spacing)
devsim.add_2d_mesh_line(mesh="mos", dir="y", pos=y_diffusion, ps=y_diffusion_spacing)
devsim.add_2d_mesh_line(mesh="mos", dir="y", pos=y_bulk_mid, ps=y_bulk_mid_spacing)
devsim.add_2d_mesh_line(mesh="mos", dir="y", pos=y_bulk_bottom, ns=y_bulk_bottom_spacing, ps=max_y_spacing)
devsim.add_2d_mesh_line(mesh="mos", dir="y", pos=y_device_bottom, ps=max_y_spacing)

devsim.add_2d_mesh_line(mesh="mos", dir="x", pos=x_center, ps=x_channel_spacing)
devsim.add_2d_mesh_line(mesh="mos", dir="x", pos=x_gate_left, ps=x_channel_spacing)
devsim.add_2d_mesh_line(mesh="mos", dir="x", pos=x_bulk_left, ps=x_diffusion_spacing)
devsim.add_2d_mesh_line(mesh="mos", dir="x", pos=x_gate_right, ps=x_channel_spacing)
devsim.add_2d_mesh_line(mesh="mos", dir="x", pos=x_bulk_right, ps=x_diffusion_spacing)
devsim.add_2d_mesh_line(mesh="mos", dir="x", pos=x_device_right, ps=max_x_spacing)
devsim.add_2d_mesh_line(mesh="mos", dir="x", pos=x_device_left, ps=max_x_spacing)

devsim.add_2d_region(mesh="mos", material="Air"    , region="air")
devsim.add_2d_region(mesh="mos", material="Silicon", region="bulk", xl=x_bulk_left, xh=x_bulk_right, yl=y_bulk_bottom, yh=y_bulk_top)
devsim.add_2d_region(mesh="mos", material="Silicon", region="gate", xl=x_gate_left, xh=x_gate_right, yl=y_oxide_top, yh=y_gate_top)
devsim.add_2d_region(mesh="mos", material="Oxide"  , region="oxide", xl=x_gate_left, xh=x_gate_right, yl=y_bulk_top, yh=y_oxide_top)

devsim.add_2d_contact(mesh="mos", name="gate", region="gate", yl=y_gate_top, yh=y_gate_top, material="metal")
devsim.add_2d_contact(mesh="mos", name="body", region="bulk", yl=y_bulk_bottom, yh=y_bulk_bottom, material="metal")
devsim.add_2d_contact(mesh="mos", name="source", region="bulk", yl=y_bulk_top, yh=y_bulk_top, xl=x_device_left, xh=x_gate_left, material="metal")
devsim.add_2d_contact(mesh="mos", name="drain" , region="bulk", yl=y_bulk_top, yh=y_bulk_top, xl=x_gate_right, xh=x_device_right, material="metal")

devsim.add_2d_interface(mesh="mos", name="gate_oxide", region0="gate", region1="oxide")
devsim.add_2d_interface(mesh="mos", name="bulk_oxide", region0="bulk", region1="oxide")
devsim.finalize_mesh(mesh="mos")
devsim.create_device(mesh="mos", device=device)


format_dict= {
    'gate_doping' : gate_doping,
  'source_doping' : source_doping,
  'drain_doping' : drain_doping,
  'body_doping' : body_doping,
  'bulk_doping' : bulk_doping,
  'x_gate_left' : x_gate_left,
  'x_gate_right' : x_gate_right,
  'x_diffusion_decay' : x_diffusion_decay,
  'y_diffusion' : y_diffusion,
  'y_diffusion_decay' : y_diffusion_decay,
  'y_bulk_bottom' : y_bulk_bottom,
}

devsim.node_model(name="NetDoping"   , device=device, region="gate", equation="%(gate_doping)s" % format_dict)
devsim.node_model(name="DrainDoping" , device=device, region="bulk", equation="0.25*%(drain_doping)s*erfc((x-%(x_gate_left)s)/%(x_diffusion_decay)s)*erfc((y-%(y_diffusion)s)/%(y_diffusion_decay)s)" % format_dict)
devsim.node_model(name="SourceDoping", device=device, region="bulk", equation="0.25*%(source_doping)s*erfc(-(x-%(x_gate_right)s)/%(x_diffusion_decay)s)*erfc((y-%(y_diffusion)s)/%(y_diffusion_decay)s)" % format_dict)
devsim.node_model(name="BodyDoping", device=device, region="bulk", equation="0.5*%(body_doping)s*erfc(-(y-%(y_bulk_bottom)s)/%(y_diffusion_decay)s)" % format_dict)
devsim.node_model(name="NetDoping"   , device=device, region="bulk", equation="DrainDoping + SourceDoping + %(bulk_doping)s + BodyDoping" % format_dict)

devsim.write_devices( file="mos_2d", type="vtk")
devsim.write_devices( file="mos_2d.flps", type="floops")

