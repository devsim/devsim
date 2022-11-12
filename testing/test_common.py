# Copyright 2019 DEVSIM LLC
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

def print_ac_circuit_solution():
    print('Circuit AC Solution')
    nodes = devsim.get_circuit_node_list()
    for node in devsim.get_circuit_node_list():
        r = devsim.get_circuit_node_value(solution='ssac_real', node=node)
        i = devsim.get_circuit_node_value(solution='ssac_imag', node=node)
        print("%s\t%1.15e\t%1.15e" % (node, r , i))

def print_circuit_solution():
    print('Circuit Solution')
    nodes = devsim.get_circuit_node_list()
    for node in devsim.get_circuit_node_list():
        r = devsim.get_circuit_node_value(solution='dcop', node=node)
        print("%s\t%1.15e" % (node, r))


def printResistorCurrent(device, contact):
    ecurr=devsim.get_contact_current(device=device, contact=contact, equation="ElectronContinuityEquation")
    hcurr=0.0
    tcurr=ecurr + hcurr
    print("Contact: %s\nElectron %g\nHole %g\nTotal %g" % (contact, ecurr, hcurr, tcurr))

####
#### Meshing
####
def CreateSimpleMesh(device, region, material='Si'):
    '''
      Commonly used for the regressions
      device length: 1
      spacing:       0.1
      contacts:      top (x=0), bot (x=1)
    '''
    devsim.create_1d_mesh  (mesh='dog')
    devsim.add_1d_mesh_line(mesh='dog', pos=0, ps=0.1, tag='top')
    devsim.add_1d_mesh_line(mesh='dog', pos=1, ps=0.1, tag='bot')
    devsim.add_1d_contact  (mesh='dog', name='top', tag='top', material='metal')
    devsim.add_1d_contact  (mesh='dog', name='bot', tag='bot', material='metal')
    devsim.add_1d_region   (mesh='dog', material=material, region=region, tag1='top', tag2='bot')
    devsim.finalize_mesh   (mesh='dog')
    devsim.create_device   (mesh='dog', device=device)

def CreateSimpleMeshWithInterface(device, region0, region1, interface, material1='Si', material2='Si'):
    '''
      Commonly used for the regressions
      device length: 1
      spacing:       0.1
      contacts:      top (x=0), bot (x=1)
      interface:     mid (x=0.5)
    '''
    devsim.create_1d_mesh  (mesh='dog')
    devsim.add_1d_mesh_line(mesh='dog', pos=0, ps=0.1, tag='top')
    devsim.add_1d_mesh_line(mesh='dog', pos=0.5, ps=0.1, tag='mid')
    devsim.add_1d_mesh_line(mesh='dog', pos=1, ps=0.1, tag='bot')
    devsim.add_1d_contact  (mesh='dog', name='top', tag='top', material='metal')
    devsim.add_1d_contact  (mesh='dog', name='bot', tag='bot', material='metal')
    devsim.add_1d_interface(mesh='dog', name=interface, tag='mid')
    devsim.add_1d_region(mesh='dog', material=material1, region=region0, tag1='top', tag2='mid')
    devsim.add_1d_region(mesh='dog', material=material2, region=region1, tag1='mid', tag2='bot')
    devsim.finalize_mesh   (mesh='dog')
    devsim.create_device   (mesh='dog', device=device)

def CreateNoiseMesh(device, region, material='Si'):
    '''
      Commonly used for the regressions
      device length: 1
      spacing:       0.1
      contacts:      top (x=0), bot (x=1)
    '''
    devsim.create_1d_mesh  (mesh='dog')
    devsim.add_1d_mesh_line(mesh='dog', pos=0, ps=5e-7, tag='top')
    devsim.add_1d_mesh_line(mesh='dog', pos=1e-5, ps=5e-7, tag='bot')
    devsim.add_1d_contact  (mesh='dog', name='top', tag='top', material='metal')
    devsim.add_1d_contact  (mesh='dog', name='bot', tag='bot', material='metal')
    devsim.add_1d_region   (mesh='dog', material=material, region=region, tag1='top', tag2='bot')
    devsim.finalize_mesh   (mesh='dog')
    devsim.create_device   (mesh='dog', device=device)

####
#### Constants
####
def SetupResistorConstants(device, region):
    for name, value in (
        ("Permittivity",     11.1*8.85e-14),
      ("ElectronCharge",   1.6e-19),
      ("IntrinsicDensity", 1.0e10),
      ("ThermalVoltage",   0.0259),
      ("mu_n", 400),
      ("mu_p", 200),
    ):
        devsim.set_parameter(device=device, region=region, name=name, value=value)

def SetupInitialResistorSystem(device, region, net_doping=1e16):
    '''
      resistor physics
    '''
    devsim.set_parameter(device=device, region=region, name='net_doping', value=net_doping)
    devsim.node_solution(device=device, region=region, name='Potential')
    devsim.edge_from_node_model(device=device, region=region, node_model='Potential')

    # node models
    for name, equation in (
        ("NetDoping",                 "net_doping"),
      ("IntrinsicElectrons",        "NetDoping"),
      ("IntrinsicCharge",           "-IntrinsicElectrons + NetDoping"),
      ("IntrinsicCharge:Potential", "-IntrinsicElectrons:Potential"),
    ):
        devsim.node_model(device=device, region=region, name=name, equation=equation)

    # edge models
    for name, equation in (
        ("ElectricField",              "(Potential@n0 - Potential@n1)*EdgeInverseLength"),
      ("ElectricField:Potential@n0", "EdgeInverseLength"),
      ("ElectricField:Potential@n1", "-EdgeInverseLength"),
      ("PotentialEdgeFlux",              "Permittivity*ElectricField"),
      ("PotentialEdgeFlux:Potential@n0", "diff(Permittivity*ElectricField, Potential@n0)"),
      ("PotentialEdgeFlux:Potential@n1", "-PotentialEdgeFlux:Potential@n0"),
    ):
        devsim.edge_model(device=device, region=region, name=name, equation=equation)

    ####
    #### PotentialEquation
    ####
    devsim.equation(device=device, region=region, name="PotentialEquation", variable_name="Potential", edge_model="PotentialEdgeFlux", variable_update="log_damp")

def SetupCarrierResistorSystem(device, region):
    '''
      This adds electron continuity
    '''
    devsim.node_solution(device=device, region=region, name='Electrons')
    devsim.edge_from_node_model(device=device, region=region, node_model='Electrons')
    devsim.set_node_values(device=device, region=region, name='Electrons', init_from='IntrinsicElectrons')

    ####
    #### PotentialNodeCharge
    ####
    for name, equation in (
        ("PotentialNodeCharge",           "-ElectronCharge*(-Electrons + NetDoping)"),
      ("PotentialNodeCharge:Electrons", "+ElectronCharge"),
    ):
        devsim.node_model(device=device, region=region, name=name, equation=equation)

    ####
    #### PotentialEquation modified for carriers present
    ####
    devsim.equation(device=device, region=region, name='PotentialEquation', variable_name='Potential', node_model='PotentialNodeCharge',
                    edge_model="PotentialEdgeFlux", variable_update="default")


    ####
    #### vdiff, Bern01, Bern10
    ####
    for name, equation in (
        ("vdiff",               "(Potential@n0 - Potential@n1)/ThermalVoltage"),
      ("vdiff:Potential@n0",  "ThermalVoltage^(-1)"),
      ("vdiff:Potential@n1",  "-ThermalVoltage^(-1)"),
      ("Bern01",              "B(vdiff)"),
      ("Bern01:Potential@n0", "dBdx(vdiff)*vdiff:Potential@n0"),
      ("Bern01:Potential@n1", "dBdx(vdiff)*vdiff:Potential@n1"),
      ("Bern10",              "Bern01 + vdiff"),
      ("Bern10:Potential@n0", "Bern01:Potential@n0 + vdiff:Potential@n0"),
      ("Bern10:Potential@n1", "Bern01:Potential@n1 + vdiff:Potential@n1"),
    ):
        devsim.edge_model(device=device, region=region, name=name, equation=equation)

    ####
    #### Electron Current
    ####
    current_equation="ElectronCharge*mu_n*EdgeInverseLength*ThermalVoltage*(Electrons@n1*Bern10 - Electrons@n0*Bern01)"
    for name, equation in (
        ("ElectronCurrent", current_equation),
      ("ElectronCurrent:Electrons@n0", "simplify(diff(%s, Electrons@n0))" % current_equation),
      ("ElectronCurrent:Electrons@n1", "simplify(diff(%s, Electrons@n1))" % current_equation),
      ("ElectronCurrent:Potential@n0", "simplify(diff(%s, Potential@n0))" % current_equation),
      ("ElectronCurrent:Potential@n1", "simplify(diff(%s, Potential@n1))" % current_equation),
    ):
        devsim.edge_model(device=device, region=region, name=name, equation=equation)

    ####
    #### Time derivative term
    ####
    for name, equation in (
        ("NCharge", "-ElectronCharge * Electrons"),
      ("NCharge:Electrons", "-ElectronCharge"),
    ):
        devsim.node_model(device=device, region=region, name=name, equation=equation)

    ####
    #### Electron Continuity Equation
    ####
    devsim.equation(device=device, region=region, name="ElectronContinuityEquation", variable_name="Electrons",
                    edge_model="ElectronCurrent", time_node_model="NCharge", variable_update="positive")


def SetupInitialResistorContact(device, contact, use_circuit_bias=False, circuit_node=""):
    if circuit_node:
        bias_name = circuit_node
    else:
        bias_name = contact + "bias"
    bias_node_model  = contact + "node_model"

    devsim.contact_node_model(device=device, contact=contact, name=bias_node_model, equation="Potential - %s" % bias_name)
    devsim.contact_node_model(device=device, contact=contact, name="%s:Potential" % bias_node_model, equation="1")
    if use_circuit_bias:
        devsim.contact_node_model(device=device, contact=contact, name="%s:%s" % (bias_node_model, bias_name), equation="-1")
    #edge_model -device $device -region $region -name "contactcharge_edge_top"  -equation $conteq
    if use_circuit_bias:
        devsim.contact_equation(device=device, contact=contact, name="PotentialEquation",
                                node_model=bias_node_model, circuit_node=bias_name)
    else:
        devsim.contact_equation(device=device, contact=contact, name="PotentialEquation",
                                node_model=bias_node_model)

def SetupCarrierResistorContact(device, contact, use_circuit_bias=False, circuit_node=""):
    '''
      Electron continuity equation at contact
    '''
    if circuit_node:
        bias_name=circuit_node
    else:
        bias_name = contact + "bias"
    region=devsim.get_region_list(device=device, contact=contact)[0]
    if 'celec' not in devsim.get_node_model_list(device=device, region=region):
        devsim.node_model(device=device, region=region, name="celec", equation="0.5*(NetDoping+(NetDoping^2 + 4 * IntrinsicDensity^2)^(0.5))")

    for name, equation in (
        ("%snodeelectrons" % contact,           "Electrons - celec"),
      ("%snodeelectrons:Electrons" % contact, "1."),
    ):
        devsim.contact_node_model(device=device, contact=contact, name=name, equation=equation)

    if use_circuit_bias:
        devsim.contact_equation(device=device, contact=contact, name="ElectronContinuityEquation",
                                node_model="%snodeelectrons" % contact, edge_current_model="ElectronCurrent", circuit_node=bias_name)
    else:
        devsim.contact_equation(device=device, contact=contact, name="ElectronContinuityEquation",
                                node_model="%snodeelectrons" % contact, edge_current_model="ElectronCurrent")

def SetupContinuousPotentialAtInterface(device, interface):
    # type continuous means that regular equations in both regions are swapped into the primary region
    devsim.interface_model(device=device, interface=interface, name="continuousPotential", equation="Potential@r0-Potential@r1")
    devsim.interface_model(device=device, interface=interface, name="continuousPotential:Potential@r0", equation= "1")
    devsim.interface_model(device=device, interface=interface, name="continuousPotential:Potential@r1", equation="-1")
    devsim.interface_equation(device=device, interface=interface, name="PotentialEquation", interface_model="continuousPotential", type="continuous")

def SetupContinuousElectronsAtInterface(device, interface):
    # type continuous means that regular equations in both regions are swapped into the primary region
    devsim.interface_model(device=device, interface=interface, name="continuousElectrons", equation="Electrons@r0-Electrons@r1")
    devsim.interface_model(device=device, interface=interface, name="continuousElectrons:Electrons@r0", equation= "1")
    devsim.interface_model(device=device, interface=interface, name="continuousElectrons:Electrons@r1", equation="-1")
    devsim.interface_equation(device=device, interface=interface, name="ElectronContinuityEquation", interface_model="continuousElectrons", type="continuous")

def SetupElectronSRVAtInterface(device, interface):
    '''
      Surface Recombination Velocity At Interface
    '''
    devsim.set_parameter(device=device, name="alpha_n", value=1e-7)
    iexp="(alpha_n@r0)*(Electrons@r0-Electrons@r1)"
    for name, equation in (
        ("srvElectrons", iexp),
      ("srvElectrons2", "srvElectrons"),
      ("srvElectrons:Electrons@r0", "diff(%s,Electrons@r0)" % iexp),
      ("srvElectrons:Electrons@r1", "diff(%s,Electrons@r1)" % iexp),
      ("srvElectrons2:Electrons@r0", "srvElectrons:Electrons@r0"),
      ("srvElectrons2:Electrons@r1", "srvElectrons:Electrons@r1"),
    ):
        devsim.interface_model(device=device, interface=interface, name=name, equation=equation)

    devsim.interface_equation(device=device, interface=interface, name="ElectronContinuityEquation", interface_model="srvElectrons2", type="fluxterm")


