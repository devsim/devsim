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
device="twowire"

create_gmsh_mesh(file="twowire.msh", mesh="twowire")
add_gmsh_region(mesh="twowire", gmsh_name="air", region="air", material="air")
add_gmsh_region(mesh="twowire", gmsh_name="left", region="left", material="iron")
add_gmsh_region(mesh="twowire", gmsh_name="right", region="right", material="iron")
add_gmsh_contact(mesh="twowire", gmsh_name="bottom", region="air", material="metal", name="bottom")
add_gmsh_interface(mesh="twowire", gmsh_name="air_left_interface",  region0="air", region1="left" , name="air_left")
add_gmsh_interface(mesh="twowire", gmsh_name="air_right_interface", region0="air", region1="right", name="air_right")
finalize_mesh(mesh="twowire")
create_device(mesh="twowire", device=device)

set_parameter(device=device, region="air",   name="mu", value=1)
set_parameter(device=device, region="left",  name="mu", value=1)
set_parameter(device=device, region="right", name="mu", value=1)

set_parameter(device=device, region="air",   name="jz", value=0)
set_parameter(device=device, region="left",  name="jz", value=1)
set_parameter(device=device, region="right", name="jz", value=-1)

for region in ("air", "left", "right"):
    node_solution(device=device, region=region, name="Az")
    edge_from_node_model(device=device, region=region, node_model="Az")

    edge_model(device=device, region=region, name="delAz",
               equation="(Az@n1 - Az@n0) * EdgeInverseLength")

    edge_model( device=device, region=region, name="delAz:Az@n1",
                equation="EdgeInverseLength")

    edge_model( device=device, region=region, name="delAz:Az@n0",
                equation="-EdgeInverseLength")

    node_model( device=device, region=region, name="Jz",
                equation="mu * jz")

    equation(device=device, region=region, name="Az_Equation",
             variable_name="Az", edge_model="delAz", node_model="Jz")


    vector_gradient(device=device, region=region, node_model="Az", calc_type="default")

    node_model( device=device, region=region, name="Bx", equation="Az_grady")
    node_model( device=device, region=region, name="By", equation="-Az_gradx")

#### interfaces
for interface in ("air_left", "air_right"):
    interface_model( device=device, interface=interface, name="continuousAz",       equation="Az@r0 - Az@r1")
    interface_model( device=device, interface=interface, name="continuousAz:Az@r0", equation="1.0")
    interface_model( device=device, interface=interface, name="continuousAz:Az@r1", equation="-1.0")

    interface_equation(device=device, interface=interface,  name="Az_Equation",
                       interface_model="continuousAz", type="continuous")


#### contact
contact_node_model(device=device, contact="bottom", name="Az_zero",    equation="Az - 0")
contact_node_model(device=device, contact="bottom", name="Az_zero:Az", equation="1.0")
contact_equation(device=device, contact="bottom", name="Az_Equation",
                 node_model="Az_zero")

#write_devices(file="debug.msh")

solve(relative_error=1e-10, absolute_error=1, type="dc")

write_devices(file="gmsh_{0}_out.msh".format(device))
write_devices(file="gmsh_{0}_out.dat".format(device), type="tecplot")

print("ElementNodeVolume {0:e}".format(sum(get_element_model_values(device=device, region="air", name="ElementNodeVolume"))))
print("NodeVolume {0:e}".format(sum(get_node_model_values(device=device, region="air", name="NodeVolume"))))
print("ElementNodeVolume {0:e}".format(sum(get_element_model_values(device=device, region="left", name="ElementNodeVolume"))))
print("NodeVolume {0:e}".format(sum(get_node_model_values(device=device, region="left", name="NodeVolume"))))
print("ElementNodeVolume {0:e}".format(sum(get_element_model_values(device=device, region="right", name="ElementNodeVolume"))))
print("NodeVolume {0:e}".format(sum(get_node_model_values(device=device, region="right", name="NodeVolume"))))

