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

global device
device = "disk"

import sys
from devsim import *

if len(sys.argv) != 2:
    sys.stderr.write('must specify voltage')
    sys.exit(1)

Ve=float(sys.argv[1])
#Ensure tcl is doing floating point math on the bias
set_parameter(name="top_bias", value=Ve/2.0)
set_parameter(name="bot_bias", value=-Ve/2.0)


create_gmsh_mesh(file="disk_3d.msh", mesh="disk")
add_gmsh_region (mesh="disk", gmsh_name="solution", region="solution", material="ionic_solution")
add_gmsh_region (mesh="disk", gmsh_name="dna", region="dna", material="dna")
add_gmsh_region (mesh="disk", gmsh_name="dielectric", region="dielectric", material="dielectric")
add_gmsh_contact(mesh="disk", gmsh_name="top", region="solution", name="top", material="metal")
add_gmsh_contact(mesh="disk", gmsh_name="bot", region="solution", name="bot", material="metal")
add_gmsh_interface(mesh="disk", gmsh_name="dielectric_solution", region0="dielectric", region1="solution", name="dielectric_solution")
add_gmsh_interface(mesh="disk", gmsh_name="dna_solution",        region0="dna", region1="solution", name="dna_solution")
finalize_mesh(mesh="disk")
create_device(mesh="disk", device=device)

from bioapp1_common import *

set_parameter(device="disk", region="dna", name="charge_density", value=0)
solve(type="dc", relative_error=1e-7, absolute_error=1e11, maximum_iterations=100)
for region in ("dna", "dielectric", "solution"):
    node_solution   (device=device, region=region, name="Potential_zero")
    set_node_values (device=device, region=region, name="Potential_zero", init_from="Potential")

set_parameter(device="disk", region="dna", name="charge_density", value=2e21)
solve(type="dc", relative_error=1e-7, absolute_error=1e11, maximum_iterations=100)
for region in ("dna", "dielectric", "solution"):
    node_model(device=device, region=region, name="LogDeltaPotential",
               equation="log(abs(Potential-Potential_zero) + 1e-10)/log(10)")

write_devices(file="bioapp1_3d_{0}.dat".format(Ve),  type="tecplot")


