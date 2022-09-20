# Copyright 2013 DEVSIM LLC
#
# Licensed under the Apache License, Version 2.0 (the "License")
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

###
### element edge based example
###

def printCurrents(device, contact, bias):
    ecurr=devsim.get_contact_current(contact=contact, equation="ElectronContinuityEquation", device=device)
    hcurr=devsim.get_contact_current(contact=contact, equation="HoleContinuityEquation", device=device)
    tcurr = ecurr + hcurr
    print("%s %g %g %g %g" % (contact, bias, ecurr, hcurr, tcurr))

####
#### Constants
####
def setMaterialParameters(device, region):
    q=1.6e-19
    k=1.3806503e-23
    eps=8.85e-14
    T=300
    for name, value in (
        ("Permittivity", 11.1*eps),
      ("ElectronCharge", q),
      ("n_i", 1.0e10),
      ("kT", eps * T),
      ("V_t", k*T/q),
      ("mu_n", 400),
      ("mu_p", 200),
    ):
        devsim.set_parameter(device=device, region=region, name=name, value=value)

def createSolution(device, region, name):
    devsim.node_solution(device=device, region=region, name=name)
    devsim.edge_from_node_model(device=device, region=region, node_model=name)

def createPotentialOnly(device, region):
    for name, equation in (
        ("IntrinsicElectrons",                     "n_i*exp(Potential/V_t)"),
      ("IntrinsicElectrons:Potential",           "diff(n_i*exp(Potential/V_t), Potential)"),
      ("IntrinsicHoles",                         "n_i^2/IntrinsicElectrons"),
      ("IntrinsicHoles:Potential",               "diff(n_i^2/IntrinsicElectrons, Potential)"),
      ("IntrinsicCharge",                        "IntrinsicHoles-IntrinsicElectrons + NetDoping"),
      ("IntrinsicCharge:Potential",              "diff(IntrinsicHoles-IntrinsicElectrons, Potential)"),
      ("PotentialIntrinsicNodeCharge",           "-ElectronCharge*IntrinsicCharge"),
      ("PotentialIntrinsicNodeCharge:Potential", "diff(-ElectronCharge*IntrinsicCharge, Potential)"),
    ):
        devsim.node_model(device=device, region=region, name=name, equation=equation)

    for name, equation in (
        ("EField",              "(Potential@n0-Potential@n1)*EdgeInverseLength"),
      ("EField:Potential@n0", "EdgeInverseLength"),
      ("EField:Potential@n1", "-EField:Potential@n0"),
    ):
        devsim.edge_model(device=device, region=region, name=name, equation=equation)

    devsim.element_from_edge_model(edge_model="EField", device=device, region=region)
    devsim.element_from_edge_model(edge_model="EField", derivative="Potential", device=device, region=region)

    foo="dot2d(EField_x, EField_y, unitx, unity)"
    for name, equation in (
        ("ElectricField", foo),
      ("ElectricField:Potential@en0", "diff(%s, Potential@en0)" % foo),
      ("ElectricField:Potential@en1", "diff(%s, Potential@en1)" % foo),
      ("ElectricField:Potential@en2", "diff(%s, Potential@en2)" % foo),
      ("PotentialEdgeFlux", "Permittivity*ElectricField"),
      ("PotentialEdgeFlux:Potential@en0", "diff(Permittivity*ElectricField,Potential@en0)"),
      ("PotentialEdgeFlux:Potential@en1", "diff(Permittivity*ElectricField,Potential@en1)"),
      ("PotentialEdgeFlux:Potential@en2", "diff(Permittivity*ElectricField,Potential@en2)"),
    ):
        devsim.element_model(device=device, region=region, name=name, equation=equation)

    devsim.equation(device=device, region=region, name="PotentialEquation", variable_name="Potential",
                    node_model="PotentialIntrinsicNodeCharge", element_model="PotentialEdgeFlux", variable_update="log_damp")

def createPotentialOnlyContact(device, region, contact):
    format_dict = {"contact" : contact}
    biasname = contact + "bias"
    devsim.set_parameter(device=device, region=region, name=biasname, value=0.0)

    for name, equation in (
        ("celec_%(contact)s", "1e-10 + 0.5*(NetDoping+(NetDoping^2 + 4 * n_i^2)^(0.5))+1e-10"),
      ("chole_%(contact)s", "1e-10 + 0.5*(-NetDoping+(NetDoping^2 + 4 * n_i^2)^(0.5))+1e-10"),
      ("%(contact)snodemodel", '''ifelse(NetDoping > 0,
      Potential-%(contact)sbias-V_t*log(celec_%(contact)s/n_i),
      Potential-%(contact)sbias+V_t*log(chole_%(contact)s/n_i))'''),
      ("%(contact)snodemodel:Potential", "1"),
    ):
        name_sub     = name % format_dict
        equation_sub = equation % format_dict
        devsim.contact_node_model(device=device, contact=contact, name=name_sub, equation=equation_sub)


    devsim.contact_equation(device=device, contact=contact, name="PotentialEquation",
                            node_model="%snodemodel" % contact)

def createDriftDiffusion(device, region):
    for name, equation in (
        ("PotentialNodeCharge"          , "-ElectronCharge*(Holes -Electrons + NetDoping)"),
      ("PotentialNodeCharge:Electrons", "+ElectronCharge"),
      ("PotentialNodeCharge:Holes"    , "-ElectronCharge"),
    ):
        devsim.node_model(device=device, region=region, name=name, equation=equation)

    devsim.equation(device=device, region=region, name="PotentialEquation", variable_name="Potential", node_model="PotentialNodeCharge",
                    element_model="PotentialEdgeFlux", variable_update="log_damp")

    for name, equation in (
        ("vdiff"              , "(Potential@n0 - Potential@n1)/V_t"),
      ("vdiff:Potential@n0" , "V_t^(-1)"),
      ("vdiff:Potential@n1" , "-V_t^(-1)"),
      ("Bern01"             , "B(vdiff)"),
      ("Bern01:Potential@n0", "dBdx(vdiff)*vdiff:Potential@n0"),
      ("Bern01:Potential@n1", "dBdx(vdiff)*vdiff:Potential@n1"),
      ("Bern10"             , "Bern01 + vdiff"),
      ("Bern10:Potential@n0", "Bern01:Potential@n0 + vdiff:Potential@n0"),
      ("Bern10:Potential@n1", "Bern01:Potential@n1 + vdiff:Potential@n1"),
    ):
        devsim.edge_model(device=device, region=region, name=name, equation=equation)

    Jn      ="ElectronCharge*mu_n*EdgeInverseLength*V_t*(Electrons@n1*Bern10 - Electrons@n0*Bern01)"
    dJndn0  ="simplify(diff( %s, Electrons@n0))" % Jn
    dJndn1  ="simplify(diff( %s, Electrons@n1))" % Jn
    dJndpot0="simplify(diff( %s, Potential@n0))" % Jn
    dJndpot1="simplify(diff( %s, Potential@n1))" % Jn
    for name, equation in (
        ("ElectronCurrent"             , Jn),
      ("ElectronCurrent:Electrons@n0", dJndn0),
      ("ElectronCurrent:Electrons@n1", dJndn1),
      ("ElectronCurrent:Potential@n0", dJndpot0),
      ("ElectronCurrent:Potential@n1", dJndpot1),
    ):
        devsim.edge_model(device=device, region=region, name=name, equation=equation)

    Jp      ="-ElectronCharge*mu_p*EdgeInverseLength*V_t*(Holes@n1*Bern01 - Holes@n0*Bern10)"
    dJpdp0  ="simplify(diff(%s, Holes@n0))" % Jp
    dJpdp1  ="simplify(diff(%s, Holes@n1))" % Jp
    dJpdpot0="simplify(diff(%s, Potential@n0))" % Jp
    dJpdpot1="simplify(diff(%s, Potential@n1))" % Jp

    for name, equation in (
        ("HoleCurrent"             , Jp),
      ("HoleCurrent:Holes@n0"    , dJpdp0),
      ("HoleCurrent:Holes@n1"    , dJpdp1),
      ("HoleCurrent:Potential@n0", dJpdpot0),
      ("HoleCurrent:Potential@n1", dJpdpot1),
    ):
        devsim.edge_model(device=device, region=region, name=name, equation=equation)

    NCharge="-ElectronCharge * Electrons"
    dNChargedn="-ElectronCharge"
    for name, equation in (
        ("NCharge",           NCharge),
      ("NCharge:Electrons", dNChargedn),
    ):
        devsim.node_model(device=device, region=region, name=name, equation=equation)

    PCharge="-ElectronCharge * Holes"
    dPChargedp="-ElectronCharge"
    for name, equation in (
        ("PCharge", PCharge),
      ("PCharge:Holes", dPChargedp),
    ):
        devsim.node_model(device=device, region=region, name=name, equation=equation)


    ni=devsim.get_parameter(device=device, region=region, name="n_i")
    for name, value in (
        ("n1", ni),
      ("p1", ni),
      ("taun", 1e-5),
      ("taup", 1e-5),
    ):
        devsim.set_parameter(device=device, region=region, name=name, value=value)

    USRH="-ElectronCharge*(Electrons*Holes - n_i^2)/(taup*(Electrons + n1) + taun*(Holes + p1))"
    dUSRHdn="simplify(diff($USRH, Electrons))"
    dUSRHdp="simplify(diff($USRH, Holes))"
    for name, equation in (
        ("USRH", USRH),
      ("USRH:Electrons", dUSRHdn),
      ("USRH:Holes", dUSRHdp),
    ):
        devsim.node_model(device=device, region=region, name=name, equation=equation)

    devsim.equation(device=device, region=region, name="ElectronContinuityEquation", variable_name="Electrons",
                    edge_model="ElectronCurrent", variable_update="positive",
                    time_node_model="NCharge", node_model="USRH")

    devsim.equation(device=device, region=region, name="HoleContinuityEquation", variable_name="Holes",
                    edge_model="HoleCurrent", variable_update="positive",
                    time_node_model="PCharge", node_model="USRH")

def createDriftDiffusionAtContact(device, region, contact):
    format_dict = {'contact' : contact}
    for name, equation in (
        ("%(contact)snodeelectrons"          , "ifelse(NetDoping > 0, Electrons - celec_%(contact)s, Electrons - n_i^2/chole_%(contact)s)"),
      ("%(contact)snodeholes"              , "ifelse(NetDoping < 0, Holes - chole_%(contact)s, Holes - n_i^2/celec_%(contact)s)"),
      ("%(contact)snodeelectrons:Electrons", "1.0"),
      ("%(contact)snodeholes:Holes"        , "1.0"),
    ):
        name_sub = name % format_dict
        equation_sub = equation % format_dict
        devsim.contact_node_model(device=device, contact=contact, name=name_sub, equation=equation_sub)

        devsim.contact_equation(device=device, contact=contact, name="ElectronContinuityEquation",
                                node_model="%snodeelectrons" % contact,
                                edge_current_model="ElectronCurrent")

        devsim.contact_equation(device=device, contact=contact, name="HoleContinuityEquation",
                                node_model="%snodeholes" % contact,
                                edge_current_model="HoleCurrent")

