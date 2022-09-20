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
import dio2_element_physics
#####
# dio1
#
# Make doping a step function
# print dat to text file for viewing in grace
# verify currents analytically
# in dio2 add recombination
#

####
#### Meshing
####
def createMesh(device, region):
    devsim.create_2d_mesh  (mesh="dog")
    devsim.add_2d_mesh_line(mesh="dog", dir="x", pos=0     , ps=1e-6)
    devsim.add_2d_mesh_line(mesh="dog", dir="x", pos=0.5e-5, ps=1e-8)
    devsim.add_2d_mesh_line(mesh="dog", dir="x", pos=1e-5  , ps=1e-6)
    devsim.add_2d_mesh_line(mesh="dog", dir="y", pos=0     , ps=1e-6)
    devsim.add_2d_mesh_line(mesh="dog", dir="y", pos=1e-5  , ps=1e-6)

    devsim.add_2d_mesh_line(mesh="dog", dir="x", pos=-1e-8   , ps=1e-8)
    devsim.add_2d_mesh_line(mesh="dog", dir="x", pos=1.001e-5, ps=1e-8)

    devsim.add_2d_region   (mesh="dog", material="Si", region=region)
    devsim.add_2d_region   (mesh="dog", material="Si", region="air1", xl=-1e-8,  xh=0)
    devsim.add_2d_region   (mesh="dog", material="Si", region="air2", xl=1.0e-5, xh=1.001e-5)

    devsim.add_2d_contact  (mesh="dog", name="top", region=region, yl=0.8e-5, yh=1e-5, xl=0, xh=0, bloat=1e-10, material="metal")
    devsim.add_2d_contact  (mesh="dog", name="bot", region=region, xl=1e-5,   xh=1e-5, bloat=1e-10, material="metal")

    devsim.finalize_mesh   (mesh="dog")
    devsim.create_device   (mesh="dog", device=device)



device="MyDevice"
region="MyRegion"

createMesh(device, region)

dio2_element_physics.setMaterialParameters(device, region)

dio2_element_physics.createSolution(device, region, "Potential")

####
#### NetDoping
####
for name, equation in (
    ("Acceptors", "1.0e18*step(0.5e-5-x)"),
  ("Donors",    "1.0e18*step(x-0.5e-5)"),
  ("NetDoping", "Donors-Acceptors"),
):
    devsim.node_model(device=device, region=region, name=name, equation=equation)

dio2_element_physics.createPotentialOnly(device, region)


dio2_element_physics.createPotentialOnlyContact(device, region, "top")
dio2_element_physics.createPotentialOnlyContact(device, region, "bot")

####
#### Initial DC solution
####
devsim.solve(type="dc", absolute_error=1.0, relative_error=1e-12, maximum_iterations=30)

devsim.write_devices(file="dio2_element_2d_potentialonly.flps", type="floops")

####
#### drift diffusion
####
dio2_element_physics.createSolution(device, region, "Electrons")
dio2_element_physics.createSolution(device, region, "Holes")

####
#### create initial guess from dc only solution
####
devsim.set_node_values(device=device, region=region, name="Electrons", init_from="IntrinsicElectrons")
devsim.set_node_values(device=device, region=region, name="Holes",     init_from="IntrinsicHoles")

dio2_element_physics.createDriftDiffusion(device, region)

dio2_element_physics.createDriftDiffusionAtContact(device, region, "top")
dio2_element_physics.createDriftDiffusionAtContact(device, region, "bot")



devsim.set_parameter(device=device, region=region, name="topbias", value=0.0)

#
# This is to test the solution backup
#
#set_parameter -device device -region region -name "topbias" -value 10
#catch {solve -type dc -absolute_error 1e10 -relative_error 1e-10 -maximum_iterations 30} x
#puts x

v=0
while v < 0.51:
    devsim.set_parameter(device=device, region=region, name="topbias", value=v)
    devsim.solve(type="dc", absolute_error=1e10, relative_error=1e-10, maximum_iterations=30)
    dio2_element_physics.printCurrents(device, "top", v)
    dio2_element_physics.printCurrents(device, "bot", 0.0)
    v += 0.1

devsim.write_devices(file="dio2_element_2d_dd.flps", type="floops")
devsim.write_devices(file="dio2_element_2d_dd", type="vtk")

