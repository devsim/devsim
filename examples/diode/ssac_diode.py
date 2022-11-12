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

#### Small Signal simulation
from devsim import *
from devsim.python_packages.simple_physics import *
import diode_common
import math

#This requires a circuit element to integrated current
circuit_element(name="V1", n1=GetContactBiasName("top"), n2=0, value=0.0, acreal=1.0, acimag=0.0)





device="MyDevice"
region="MyRegion"

diode_common.CreateMesh2(device, region)

diode_common.SetParameters(device=device, region=region)

diode_common.SetNetDoping(device=device, region=region)

diode_common.InitialSolution(device, region, circuit_contacts="top")

# Initial DC solution
solve(type="dc", absolute_error=1.0, relative_error=1e-12, maximum_iterations=30)


diode_common.DriftDiffusionInitialSolution(device, region, circuit_contacts=["top"])

v=0.0
while v < 0.51:
    circuit_alter(name="V1", value=v)
    solve(type="dc", absolute_error=1e10, relative_error=1e-10, maximum_iterations=30)
    #TODO: get out circuit information
#  PrintCurrents(device, "top")
    PrintCurrents(device, "bot")
    solve(type="ac", frequency=1.0)
    cap=get_circuit_node_value(node="V1.I", solution="ssac_imag")/ (-2*math.pi)
    print("capacitance {0} {1}".format(v, cap))
    v += 0.1

for x in get_circuit_node_list():
    for y in get_circuit_solution_list():
        z = get_circuit_node_value(node=x, solution=y)
        print(("{0}\t{1}\t{2}".format(x, y, z)))

