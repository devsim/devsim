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

from .simple_dd import *
from devsim import *
#TODO: make this a class so that paramters can be changed
contactcharge_node="contactcharge_node"
contactcharge_edge="contactcharge_edge"
ece_name="ElectronContinuityEquation"
hce_name="HoleContinuityEquation"
celec_model = "(1e-10 + 0.5*abs(NetDoping+(NetDoping^2 + 4 * n_i^2)^(0.5)))"
chole_model = "(1e-10 + 0.5*abs(-NetDoping+(NetDoping^2 + 4 * n_i^2)^(0.5)))"

q      = 1.6e-19 # coul
k      = 1.3806503e-23 # J/K
eps_0  = 8.85e-14 # F/cm^2
#T      = 300 # K
eps_si = 11.1
eps_ox = 3.9
# TODO: make this temperature dependent
n_i    = 1.0e10 # #/cm^3
# constant in our approximation
mu_n   = 400
mu_p   = 200


def GetContactBiasName(contact):
    return "{0}_bias".format(contact)

def GetContactNodeModelName(contact):
    return "{0}nodemodel".format(contact)

def PrintCurrents(device, contact):
    '''
       print out contact currents
    '''
    # TODO add charge
    contact_bias_name = GetContactBiasName(contact)
    electron_current= get_contact_current(device=device, contact=contact, equation=ece_name)
    hole_current    = get_contact_current(device=device, contact=contact, equation=hce_name)
    total_current   = electron_current + hole_current                                        
    voltage         = get_parameter(device=device, name=GetContactBiasName(contact))
    print("{0}\t{1}\t{2}\t{3}\t{4}".format(contact, voltage, electron_current, hole_current, total_current))

#in the future, worry about workfunction
def CreateOxideContact(device, region, contact):
    conteq="Permittivity*ElectricField"
    contact_bias_name  = GetContactBiasName(contact)
    contact_model_name = GetContactNodeModelName(contact)
    eq = "Potential - {0}".format(contact_bias_name)
    CreateContactNodeModel(device, contact, contact_model_name, eq)
    CreateContactNodeModelDerivative(device, contact, contact_model_name, eq, "Potential")

    #TODO: make everyone use dfield
    if not InEdgeModelList(device, region, contactcharge_edge):
        CreateEdgeModel(device, region, contactcharge_edge, "Permittivity*ElectricField")
        CreateEdgeModelDerivatives(device, region, contactcharge_edge, "Permittivity*ElectricField", "Potential")

    contact_equation(device=device, contact=contact, name="PotentialEquation",
                     node_model=contact_model_name, edge_charge_model= contactcharge_edge)




#####
##### Constants
#####
def SetOxideParameters(device, region, T):
    '''
      Sets physical parameters
    '''
    set_parameter(device=device, region=region, name="Permittivity",   value=eps_ox * eps_0)
    set_parameter(device=device, region=region, name="ElectronCharge", value=q)

def SetSiliconParameters(device, region, T):
    '''
      Sets physical parameters assuming constants
    '''
    #### TODO: make T a free parameter and T dependent parameters as models
    set_parameter(device=device, region=region, name="Permittivity",   value=eps_si * eps_0)
    set_parameter(device=device, region=region, name="ElectronCharge", value=q)
    set_parameter(device=device, region=region, name="n_i",            value=n_i)
    set_parameter(device=device, region=region, name="T",              value=T)
    set_parameter(device=device, region=region, name="kT",             value=k * T)
    set_parameter(device=device, region=region, name="V_t",            value=k*T/q)
    set_parameter(device=device, region=region, name="mu_n",           value=mu_n)
    set_parameter(device=device, region=region, name="mu_p",           value=mu_p)
    #default SRH parameters
    set_parameter(device=device, region=region, name="n1", value=n_i)
    set_parameter(device=device, region=region, name="p1", value=n_i)
    set_parameter(device=device, region=region, name="taun", value=1e-5)
    set_parameter(device=device, region=region, name="taup", value=1e-5)

def CreateSiliconPotentialOnly(device, region):
    '''
      Creates the physical models for a Silicon region
    '''
    if not InNodeModelList(device, region, "Potential"):
        print("Creating Node Solution Potential")
        CreateSolution(device, region, "Potential")
    elec_i = "n_i*exp(Potential/V_t)"
    hole_i = "n_i^2/IntrinsicElectrons"
    charge_i = "kahan3(IntrinsicHoles, -IntrinsicElectrons, NetDoping)"
    pcharge_i = "-ElectronCharge * IntrinsicCharge"

    # require NetDoping
    for i in (
        ("IntrinsicElectrons", elec_i),
         ("IntrinsicHoles", hole_i),
         ("IntrinsicCharge", charge_i),
         ("PotentialIntrinsicCharge", pcharge_i)
    ):
        n = i[0]
        e = i[1]
        CreateNodeModel(device, region, n, e)
        CreateNodeModelDerivative(device, region, n, e, "Potential")

    ### TODO: Edge Average Model
    for i in (
        ("ElectricField",     "(Potential@n0-Potential@n1)*EdgeInverseLength"),
      ("PotentialEdgeFlux", "Permittivity * ElectricField")
    ):
        n = i[0]
        e = i[1]
        CreateEdgeModel(device, region, n, e)
        CreateEdgeModelDerivatives(device, region, n, e, "Potential")

    equation(device=device, region=region, name="PotentialEquation", variable_name="Potential",
             node_model="PotentialIntrinsicCharge", edge_model="PotentialEdgeFlux", variable_update="log_damp")

def CreateSiliconPotentialOnlyContact(device, region, contact, is_circuit=False):
    '''
      Creates the potential equation at the contact
      if is_circuit is true, than use node given by GetContactBiasName
    '''
    # Means of determining contact charge
    # Same for all contacts
    if not InNodeModelList(device, region, "contactcharge_node"):
        CreateNodeModel(device, region, "contactcharge_node", "ElectronCharge*IntrinsicCharge")
    #### TODO: This is the same as D-Field
    if not InEdgeModelList(device, region, "contactcharge_edge"):
        CreateEdgeModel(device, region, "contactcharge_edge", "Permittivity*ElectricField")
        CreateEdgeModelDerivatives(device, region, "contactcharge_edge", "Permittivity*ElectricField", "Potential")

#  set_parameter(device=device, region=region, name=GetContactBiasName(contact), value=0.0)

    contact_model = "Potential -{0} + ifelse(NetDoping > 0, \
    -V_t*log({1}/n_i), \
    V_t*log({2}/n_i))".format(GetContactBiasName(contact), celec_model, chole_model)

    contact_model_name = GetContactNodeModelName(contact)
    CreateContactNodeModel(device, contact, contact_model_name, contact_model)
    # Simplify it too complicated
    CreateContactNodeModel(device, contact, "{0}:{1}".format(contact_model_name,"Potential"), "1")
    if is_circuit:
        CreateContactNodeModel(device, contact, "{0}:{1}".format(contact_model_name,GetContactBiasName(contact)), "-1")

    if is_circuit:
        contact_equation(device=device, contact=contact, name="PotentialEquation",
                         node_model=contact_model_name, edge_model="",
                         node_charge_model="contactcharge_node", edge_charge_model="contactcharge_edge",
                         node_current_model="", edge_current_model="", circuit_node=GetContactBiasName(contact))
    else:
        contact_equation(device=device, contact=contact, name="PotentialEquation",
                         node_model=contact_model_name, edge_model="",
                         node_charge_model="contactcharge_node", edge_charge_model="contactcharge_edge",
                         node_current_model="", edge_current_model="")

def CreateSRH(device, region):
    USRH="(Electrons*Holes - n_i^2)/(taup*(Electrons + n1) + taun*(Holes + p1))"
    Gn = "-ElectronCharge * USRH"
    Gp = "+ElectronCharge * USRH"
    CreateNodeModel(device, region, "USRH", USRH)
    CreateNodeModel(device, region, "ElectronGeneration", Gn)
    CreateNodeModel(device, region, "HoleGeneration", Gp)
    for i in ("Electrons", "Holes"):
        CreateNodeModelDerivative(device, region, "USRH", USRH, i)
        CreateNodeModelDerivative(device, region, "ElectronGeneration", Gn, i)
        CreateNodeModelDerivative(device, region, "HoleGeneration", Gp, i)

def CreateECE(device, region, mu_n):
    CreateElectronCurrent(device, region, mu_n)

    NCharge = "-ElectronCharge * Electrons"
    CreateNodeModel(device, region, "NCharge", NCharge)
    CreateNodeModelDerivative(device, region, "NCharge", NCharge, "Electrons")

    equation(device=device, region=region, name="ElectronContinuityEquation", variable_name="Electrons",
             time_node_model = "NCharge",
             edge_model="ElectronCurrent", variable_update="positive", node_model="ElectronGeneration")

def CreateHCE(device, region, mu_p):
    CreateHoleCurrent(device, region, mu_p)
    PCharge = "ElectronCharge * Holes"
    CreateNodeModel(device, region, "PCharge", PCharge)
    CreateNodeModelDerivative(device, region, "PCharge", PCharge, "Holes")

    equation(device=device, region=region, name="HoleContinuityEquation", variable_name="Holes",
             time_node_model = "PCharge",
             edge_model="HoleCurrent", variable_update="positive", node_model="HoleGeneration")

def CreatePE(device, region):
    pne = "-ElectronCharge*kahan3(Holes, -Electrons, NetDoping)"
    CreateNodeModel(device, region, "PotentialNodeCharge", pne)
    CreateNodeModelDerivative(device, region, "PotentialNodeCharge", pne, "Electrons")
    CreateNodeModelDerivative(device, region, "PotentialNodeCharge", pne, "Holes")

    equation(device=device, region=region, name="PotentialEquation", variable_name="Potential",
             node_model="PotentialNodeCharge", edge_model="PotentialEdgeFlux",
             time_node_model="", variable_update="log_damp")


def CreateSiliconDriftDiffusion(device, region, mu_n="mu_n", mu_p="mu_p"):
    CreatePE(device, region)
    CreateBernoulli(device, region)
    CreateSRH(device, region)
    CreateECE(device, region, mu_n)
    CreateHCE(device, region, mu_p)


def CreateSiliconDriftDiffusionAtContact(device, region, contact, is_circuit=False): 
    '''
      Restrict electrons and holes to their equilibrium values
      Integrates current into circuit
    '''
    contact_electrons_model = "Electrons - ifelse(NetDoping > 0, {0}, n_i^2/{1})".format(celec_model, chole_model)
    contact_holes_model = "Holes - ifelse(NetDoping < 0, +{1}, +n_i^2/{0})".format(celec_model, chole_model)
    contact_electrons_name = "{0}nodeelectrons".format(contact)
    contact_holes_name = "{0}nodeholes".format(contact)

    CreateContactNodeModel(device, contact, contact_electrons_name, contact_electrons_model)
    #TODO: The simplification of the ifelse statement is time consuming
#  CreateContactNodeModelDerivative(device, contact, contact_electrons_name, contact_electrons_model, "Electrons")
    CreateContactNodeModel(device, contact, "{0}:{1}".format(contact_electrons_name, "Electrons"), "1")

    CreateContactNodeModel(device, contact, contact_holes_name, contact_holes_model)
    CreateContactNodeModel(device, contact, "{0}:{1}".format(contact_holes_name, "Holes"), "1")

    #TODO: keyword args
    if is_circuit:
        contact_equation(device=device, contact=contact, name="ElectronContinuityEquation",
                         node_model=contact_electrons_name,
                         edge_current_model="ElectronCurrent", circuit_node=GetContactBiasName(contact))

        contact_equation(device=device, contact=contact, name="HoleContinuityEquation",
                         node_model=contact_holes_name,
                         edge_current_model="HoleCurrent", circuit_node=GetContactBiasName(contact))

    else:
        contact_equation(device=device, contact=contact, name="ElectronContinuityEquation",
                         node_model=contact_electrons_name,
                         edge_current_model="ElectronCurrent")

        contact_equation(device=device, contact=contact, name="HoleContinuityEquation",
                         node_model=contact_holes_name,
                         edge_current_model="HoleCurrent")


def CreateOxidePotentialOnly(device, region, update_type="default"):
    '''
      Create electric field model in oxide
      Creates Potential solution variable if not available
    '''
    if not InNodeModelList(device, region, "Potential"):
        print("Creating Node Solution Potential")
        CreateSolution(device, region, "Potential")

    efield="(Potential@n0 - Potential@n1)*EdgeInverseLength"
    # this needs to remove derivatives w.r.t. independents
    CreateEdgeModel(device, region, "ElectricField", efield)
    CreateEdgeModelDerivatives(device, region, "ElectricField", efield, "Potential")
    dfield="Permittivity*ElectricField"
    CreateEdgeModel(device, region, "PotentialEdgeFlux", dfield)
    CreateEdgeModelDerivatives(device, region, "PotentialEdgeFlux", dfield, "Potential")
    equation(device=device, region=region, name="PotentialEquation", variable_name="Potential",
             edge_model="PotentialEdgeFlux", variable_update=update_type)


def CreateSiliconOxideInterface(device, interface):
    '''
      continuous potential at interface
    '''
    model_name = CreateContinuousInterfaceModel(device, interface, "Potential")
    interface_equation(device=device, interface=interface, name="PotentialEquation", interface_model=model_name, type="continuous")

#
##TODO: similar model for silicon/silicon interface
## should use quasi-fermi potential
def CreateSiliconSiliconInterface(device, interface):
    '''
      Enforces potential, electron, and hole continuity across the interface
    '''
    CreateSiliconOxideInterface(device, interface)
    ename = CreateContinuousInterfaceModel(device, interface, "Electrons")
    interface_equation(device=device, interface=interface, name="ElectronContinuityEquation", interface_model=ename, type="continuous")
    hname = CreateContinuousInterfaceModel(device, interface, "Holes")
    interface_equation(device=device, interface=interface, name="HoleContinuityEquation", interface_model=hname, type="continuous")

