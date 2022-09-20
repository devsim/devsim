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

#set_parameter -name threads_available -value 1
#set_parameter -name threads_task_size -value 1024
import gmsh_mos2d_create
from devsim import *
from devsim.python_packages.simple_physics import *
from devsim.python_packages.ramp import *
from devsim.python_packages.Klaassen import *
from devsim.python_packages.mos_physics import *

# TODO: write out mesh, and then read back in as separate test
device = "mos2d"
silicon_regions=("gate", "bulk")
oxide_regions=("oxide",)
regions = ("gate", "bulk", "oxide")
interfaces = ("bulk_oxide", "gate_oxide")

for i in regions:
    CreateSolution(device, i, "Potential")

for i in silicon_regions:
    SetSiliconParameters(device, i, 300)
    CreateSiliconPotentialOnly(device, i)

for i in oxide_regions:
    SetOxideParameters(device, i, 300)
    CreateOxidePotentialOnly(device, i, "log_damp")

### Set up contacts
contacts = get_contact_list(device=device)
for i in contacts:
    tmp = get_region_list(device=device, contact=i)
    r = tmp[0]
    print("%s %s" % (r, i))
    CreateSiliconPotentialOnlyContact(device, r, i)
    set_parameter(device=device, name=GetContactBiasName(i), value=0.0)

for i in interfaces:
    CreateSiliconOxideInterface(device, i)

#for d in get_device_list():
#  for gn in get_parameter_list():
#    print("{0} {1}").format(gn, get_parameter(device=d, name=gn))
#  for gn in get_parameter_list(device=d):
#    print("{0} {1} {2}").format(d, gn, get_parameter(device=d, name=gn))
#  for r in get_region_list(device=d):
#    for gn in get_parameter_list(device=d, region=r):
#      print("{0} {1} {2} {3}").format(d, r, gn, get_parameter(device=d, region=r, name=gn))
#write_devices(file="foo.msh", type="devsim")
solve(type="dc", absolute_error=1.0e-13, relative_error=1e-12, maximum_iterations=30)
solve(type="dc", absolute_error=1.0e-13, relative_error=1e-12, maximum_iterations=30)
#
##write_devices -file gmsh_mos2d_potentialonly.flps -type floops
write_devices(file="gmsh_mos2d_potentialonly", type="vtk")

for i in silicon_regions:
    CreateSolution(device, i, "Electrons")
    CreateSolution(device, i, "Holes")
    set_node_values(device=device, region=i, name="Electrons", init_from="IntrinsicElectrons")
    set_node_values(device=device, region=i, name="Holes",     init_from="IntrinsicHoles")

    Set_Mobility_Parameters(device, i)
    Klaassen_Mobility(device, i)
    #use bulk Klaassen mobility
    CreateSiliconDriftDiffusion(device, i, "mu_bulk_e", "mu_bulk_h")

for c in contacts:
    tmp = get_region_list(device=device, contact=c)
    r = tmp[0]
    CreateSiliconDriftDiffusionAtContact(device, r, c)

for r in silicon_regions:
    node_model(device=device, region=r, name="logElectrons", equation="log(Electrons)/log(10)")
    CreateNormalElectricFieldFromCurrentFlow(device, r, "ElectronCurrent")
    CreateNormalElectricFieldFromCurrentFlow(device, r, "HoleCurrent")
    Philips_Surface_Mobility(device, r, "Enormal_ElectronCurrent", "Enormal_HoleCurrent")
    #Philips_VelocitySaturation $device $region mu_vsat_e mu_bulk_e Eparallel_ElectronCurrent vsat_e
    Philips_VelocitySaturation(device, r, "mu_vsat_e", "mu_e_0", "Eparallel_ElectronCurrent", "vsat_e")
    CreateElementModel2d(device, r, "mu_ratio", "mu_vsat_e/mu_bulk_e")
    CreateElementModel2d(device, r, "mu_surf_ratio", "mu_e_0/mu_bulk_e")
    CreateElementModel2d(device, r, "epar_ratio", "abs(Eparallel_ElectronCurrent/ElectricField_mag)")
    #createElementElectronCurrent2d $device $region ElementElectronCurrent mu_n
    #createElementElectronCurrent2d $device $region ElementElectronCurrent mu_bulk_e
    CreateElementElectronCurrent2d(device, r, "ElementElectronCurrent", "mu_vsat_e")
    # element_from_edge_model -edge_model ElectricField   -device $device -region $i
    CreateElementModel2d(device, r, "magElementElectronCurrent", "log(abs(ElementElectronCurrent)+1e-10)/log(10)")
    vector_element_model(device=device, region=r, element_model="ElementElectronCurrent")
    # we aren't going to worry about holes for now.
    #createNormalElectricFieldFromCurrentFlow $device $region HoleCurrent
    CreateElementElectronContinuityEquation(device, r, "ElementElectronCurrent")

for contact in ("body", "drain", "source"):
    CreateElementContactElectronContinuityEquation(device, contact, "ElementElectronCurrent")

#write_devices(file="debug.msh", type="devsim")
solve(type="dc", absolute_error=1.0e30, relative_error=1e-10, maximum_iterations=100)
write_devices(file="gmsh_mos2d_dd_kla_zero.dat", type="tecplot")
write_devices(file="gmsh_mos2d_dd_kla_zero", type="vtk")


drainbias=get_parameter(device=device, name=GetContactBiasName("drain"))
gatebias=get_parameter(device=device, name=GetContactBiasName("gate"))

rampbias(device, "gate",  0.5, 0.5, 0.001, 100, 1e-8, 1e30, printAllCurrents)
rampbias(device, "drain", 0.5, 0.1, 0.001, 100, 1e-8, 1e30, printAllCurrents)

write_devices(file="gmsh_mos2d_dd_kla.dat", type="tecplot")
write_devices(file="gmsh_mos2d_dd_kla", type="vtk")

