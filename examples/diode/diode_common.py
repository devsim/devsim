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
from devsim.python_packages.simple_physics import *
#####
# dio1
#
# Make doping a step function
# print dat to text file for viewing in grace
# verify currents analytically
# in dio2 add recombination
#

def CreateMesh(device, region):
    '''
      Meshing
    '''
    create_1d_mesh(mesh="dio")
    add_1d_mesh_line(mesh="dio", pos=0, ps=1e-7, tag="top")
    add_1d_mesh_line(mesh="dio", pos=0.5e-5, ps=1e-9, tag="mid")
    add_1d_mesh_line(mesh="dio", pos=1e-5, ps=1e-7, tag="bot")
    add_1d_contact  (mesh="dio", name="top", tag="top", material="metal")
    add_1d_contact  (mesh="dio", name="bot", tag="bot", material="metal")
    add_1d_region   (mesh="dio", material="Si", region=region, tag1="top", tag2="bot")
    finalize_mesh(mesh="dio")
    create_device(mesh="dio", device=device)

# this is the mesh for the ssac device
# TODO: use CreateMesh and update regressions
def CreateMesh2(device, region):
    create_1d_mesh(mesh="dio")
    add_1d_mesh_line(mesh="dio", pos=0, ps=1e-7, tag="top")
    add_1d_mesh_line(mesh="dio", pos=0.5e-5, ps=1e-8, tag="mid")
    add_1d_mesh_line(mesh="dio", pos=1e-5, ps=1e-7, tag="bot")
    add_1d_contact(mesh="dio", name="top", tag="top", material="metal")
    add_1d_contact(mesh="dio", name="bot", tag="bot", material="metal")
    add_1d_region(mesh="dio", material="Si", region=region, tag1="top", tag2="bot")
    finalize_mesh(mesh="dio")
    create_device(mesh="dio", device=device)

def Create2DMesh(device, region):
    create_2d_mesh(mesh="dio")
    add_2d_mesh_line(mesh="dio", dir="x", pos=0,      ps=1e-6)
    add_2d_mesh_line(mesh="dio", dir="x", pos=0.5e-5, ps=1e-8)
    add_2d_mesh_line(mesh="dio", dir="x", pos=1e-5,   ps=1e-6)
    add_2d_mesh_line(mesh="dio", dir="y", pos=0,      ps=1e-6)
    add_2d_mesh_line(mesh="dio", dir="y", pos=1e-5,   ps=1e-6)

    add_2d_mesh_line(mesh="dio", dir="x", pos=-1e-8,    ps=1e-8)
    add_2d_mesh_line(mesh="dio", dir="x", pos=1.001e-5, ps=1e-8)

    add_2d_region(mesh="dio", material="Si", region=region)
    add_2d_region(mesh="dio", material="Si", region="air1", xl=-1e-8,  xh=0)
    add_2d_region(mesh="dio", material="Si", region="air2", xl=1.0e-5, xh=1.001e-5)

    add_2d_contact(mesh="dio", name="top", material="metal", region=region, yl=0.8e-5, yh=1e-5, xl=0, xh=0, bloat=1e-10)
    add_2d_contact(mesh="dio", name="bot", material="metal", region=region, xl=1e-5,   xh=1e-5, bloat=1e-10)

    finalize_mesh(mesh="dio")
    create_device(mesh="dio", device=device)

def Create2DGmshMesh(device, region):
    #this reads in the gmsh format
    create_gmsh_mesh (mesh="diode2d", file="gmsh_diode2d.msh")
    add_gmsh_region  (mesh="diode2d", gmsh_name="Bulk",    region=region, material="Silicon")
    add_gmsh_contact (mesh="diode2d", gmsh_name="Base",    region=region, material="metal", name="top")
    add_gmsh_contact (mesh="diode2d", gmsh_name="Emitter", region=region, material="metal", name="bot")
    finalize_mesh    (mesh="diode2d")
    create_device    (mesh="diode2d", device=device)

def Create3DGmshMesh(device, region):
    #this reads in the gmsh format
    create_gmsh_mesh (mesh="diode3d", file="gmsh_diode3d.msh")
    add_gmsh_region  (mesh="diode3d", gmsh_name="Bulk",    region=region, material="Silicon")
    add_gmsh_contact (mesh="diode3d", gmsh_name="Base",    region=region, material="metal", name="top")
    add_gmsh_contact (mesh="diode3d", gmsh_name="Emitter", region=region, material="metal", name="bot")
    finalize_mesh    (mesh="diode3d")
    create_device    (mesh="diode3d", device=device)


def SetParameters(device, region):
    '''
      Set parameters for 300 K
    '''
    SetSiliconParameters(device, region, 300)


def SetNetDoping(device, region):
    '''
      NetDoping
    '''
    CreateNodeModel(device, region, "Acceptors", "1.0e18*step(0.5e-5-x)")
    CreateNodeModel(device, region, "Donors",    "1.0e18*step(x-0.5e-5)")
    CreateNodeModel(device, region, "NetDoping", "Donors-Acceptors")

def InitialSolution(device, region, circuit_contacts=None):
    # Create Potential, Potential@n0, Potential@n1
    CreateSolution(device, region, "Potential")

    # Create potential only physical models
    CreateSiliconPotentialOnly(device, region)


    # Set up the contacts applying a bias
    for i in get_contact_list(device=device):
        if circuit_contacts and i in circuit_contacts:
            CreateSiliconPotentialOnlyContact(device, region, i, True)
        else:
            ###print "FIX THIS"
            ### it is more correct for the bias to be 0, and it looks like there is side effects
            set_parameter(device=device, name=GetContactBiasName(i), value=0.0)
            CreateSiliconPotentialOnlyContact(device, region, i)


def DriftDiffusionInitialSolution(device, region, circuit_contacts=None):
    ####
    #### drift diffusion solution variables
    ####
    CreateSolution(device, region, "Electrons")
    CreateSolution(device, region, "Holes")

    ####
    #### create initial guess from dc only solution
    ####
    set_node_values(device=device, region=region, name="Electrons", init_from="IntrinsicElectrons")
    set_node_values(device=device, region=region, name="Holes",     init_from="IntrinsicHoles")

    ###
    ### Set up equations
    ###
    CreateSiliconDriftDiffusion(device, region)
    for i in get_contact_list(device=device):
        if circuit_contacts and i in circuit_contacts:
            CreateSiliconDriftDiffusionAtContact(device, region, i, True)
        else:
            CreateSiliconDriftDiffusionAtContact(device, region, i)





#####
##### Ramp the bias to 0.5 Volts
#####
#v = 0.0
#while v < 0.51:
#  set_parameter(device=device, name=GetContactBiasName("top"), value=v)
#  solve(type="dc", absolute_error=1e10, relative_error=1e-10, maximum_iterations=30)
#  PrintCurrents(device, "top")
#  PrintCurrents(device, "bot")
#  v += 0.1
#
#write_devices(file="diode_1d.dat", type="tecplot")
##import matplotlib
##import matplotlib.pyplot
##x=get_node_model_values(device=device, region=region, name="x")
##ymax = 10
##ymin = 10
##fields = ("Electrons", "Holes", "Donors", "Acceptors")
##for i in fields:
##    y=get_node_model_values(device=device, region=region, name=i)
##    if (max(y) > ymax):
##      ymax = max(y)
##    matplotlib.pyplot.semilogy(x, y)
##matplotlib.pyplot.xlabel('x (cm)')
##matplotlib.pyplot.ylabel('Density (#/cm^3)')
##matplotlib.pyplot.legend(fields)
##ymax *= 10
##matplotlib.pyplot.axis([min(x), max(x), ymin, ymax])
##matplotlib.pyplot.savefig("diode_1d_density.eps")
##
##matplotlib.pyplot.clf()
##edge_average_model(device=device, region=region, node_model="x", edge_model="xmid")
##xmid=get_edge_model_values(device=device, region=region, name="xmid")
##efields = ("ElectronCurrent", "HoleCurrent", )
##y=get_edge_model_values(device=device, region=region, name="ElectronCurrent")
##ymin=min(y)
##ymax=max(y)
##for i in efields:
##  y=get_edge_model_values(device=device, region=region, name=i)
##  if min(y) < ymin:
##    ymin = min(y)
##  elif max(y) > ymax:
##    ymax = max(y)
##  matplotlib.pyplot.plot(xmid, y)
##matplotlib.pyplot.xlabel('x (cm)')
##matplotlib.pyplot.ylabel('J (A/cm^2)')
##matplotlib.pyplot.legend(efields)
##matplotlib.pyplot.axis([min(x), max(x), 0.5*ymin, 2*ymax])
##matplotlib.pyplot.savefig("diode_1d_current.eps")
##print ymin
##print ymax
