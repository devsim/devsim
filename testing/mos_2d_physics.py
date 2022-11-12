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

def printCurrents(device, contact, bias):
    ecurr=devsim.get_contact_current(contact=contact, equation="ElectronContinuityEquation", device=device)
    hcurr=devsim.get_contact_current(contact=contact, equation="HoleContinuityEquation", device=device)
    tcurr = ecurr + hcurr
    print("%s %g %g %g %g" % (contact, bias, ecurr, hcurr, tcurr))

####
#### Constants
####
def setOxideParameters(device, region):
    q=1.6e-19
    k=1.3806503e-23
    eps=8.85e-14
    T=300
    devsim.set_parameter(device=device, region=region, name="Permittivity",   value=3.9*eps)
    devsim.set_parameter(device=device, region=region, name="ElectronCharge", value=q)

def setSiliconParameters(device, region):
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

def createSiliconPotentialOnly(device, region):
    ie = devsim.node_model(device=device, region=region, name="IntrinsicElectrons", equation="n_i*exp(Potential/V_t)")
    res = devsim.node_model(device=device, region=region, name="IntrinsicElectrons:Potential", equation="diff(%s, Potential)" % ie)
    for name, equation in (
        ("IntrinsicHoles",                         "n_i^2/IntrinsicElectrons"),
      ("IntrinsicHoles:Potential",               "diff(n_i^2/IntrinsicElectrons, Potential)"),
      ("IntrinsicCharge",                        "IntrinsicHoles-IntrinsicElectrons + NetDoping"),
      ("IntrinsicCharge:Potential",              "diff(IntrinsicHoles-IntrinsicElectrons, Potential)"),
      ("PotentialIntrinsicNodeCharge",           "-ElectronCharge*IntrinsicCharge"),
      ("PotentialIntrinsicNodeCharge:Potential", "diff(-ElectronCharge*IntrinsicCharge, Potential)"),
    ):
        devsim.node_model(device=device, region=region, name=name, equation=equation)

    for name, equation in (
        ("ElectricField",                  "(Potential@n0-Potential@n1)*EdgeInverseLength"),
      ("ElectricField:Potential@n0",     "EdgeInverseLength"),
      ("ElectricField:Potential@n1",     "-ElectricField:Potential@n0"),
      ("PotentialEdgeFlux",              "Permittivity*ElectricField"),
      ("PotentialEdgeFlux:Potential@n0", "diff(Permittivity*ElectricField,Potential@n0)"),
      ("PotentialEdgeFlux:Potential@n1", "-PotentialEdgeFlux:Potential@n0"),
    ):
        devsim.edge_model(device=device, region=region, name=name, equation=equation)

    devsim.equation(device=device, region=region, name="PotentialEquation", variable_name="Potential",
                    node_model="PotentialIntrinsicNodeCharge", edge_model="PotentialEdgeFlux", variable_update="log_damp")

def createSiliconPotentialOnlyContact(device, region, contact):
    bias_name="%sbias" % contact
    format_dict = { "contact" : contact}
    devsim.set_parameter(device=device, region=region, name=bias_name, value=0.0)
    for name, equation in (
        ("celec_%(contact)s", "1e-10 + 0.5*abs(NetDoping+(NetDoping^2 + 4 * n_i^2)^(0.5))"),
      ("chole_%(contact)s", "1e-10 + 0.5*abs(-NetDoping+(NetDoping^2 + 4 * n_i^2)^(0.5))"),
      ("%(contact)snodemodel", '''
        ifelse(NetDoping > 0,
          Potential-%(contact)sbias-V_t*log(celec_%(contact)s/n_i),
          Potential-%(contact)sbias+V_t*log(chole_%(contact)s/n_i))'''),
      ("%(contact)snodemodel:Potential", "1"),
    ):
        name_sub = name % format_dict
        equation_sub = equation % format_dict
        devsim.contact_node_model(device=device, contact=contact, name=name_sub, equation=equation_sub)

    devsim.contact_equation(device=device, contact=contact, name="PotentialEquation",
                            node_model="%snodemodel" % contact)

def createSiliconDriftDiffusion(device, region):
    for name, equation in (
        ("PotentialNodeCharge",           "-ElectronCharge*(Holes -Electrons + NetDoping)"),
      ("PotentialNodeCharge:Electrons", "+ElectronCharge"),
      ("PotentialNodeCharge:Holes",     "-ElectronCharge"),
    ):
        devsim.node_model(device=device, region=region, name=name, equation=equation)

    devsim.equation(device=device, region=region, name="PotentialEquation", variable_name="Potential", node_model="PotentialNodeCharge",
                    edge_model="PotentialEdgeFlux", variable_update="log_damp")

    createBernoulli(device, region)
    createElectronCurrent(device, region)
    createHoleCurrent(device, region)

    NCharge="-ElectronCharge * Electrons"
    dNChargedn="-ElectronCharge"
    devsim.node_model(device=device, region=region, name="NCharge", equation=NCharge)
    devsim.node_model(device=device, region=region, name="NCharge:Electrons", equation=dNChargedn)

    PCharge="-ElectronCharge * Holes"
    dPChargedp="-ElectronCharge"
    devsim.node_model(device=device, region=region, name="PCharge", equation=PCharge)
    devsim.node_model(device=device, region=region, name="PCharge:Holes", equation=dPChargedp)

    ni=devsim.get_parameter(device=device, region=region, name="n_i")
    devsim.set_parameter(device=device, region=region, name="n1", value=ni)
    devsim.set_parameter(device=device, region=region, name="p1", value=ni)
    devsim.set_parameter(device=device, region=region, name="taun", value=1e-5)
    devsim.set_parameter(device=device, region=region, name="taup", value=1e-5)

    USRH="-ElectronCharge*(Electrons*Holes - n_i^2)/(taup*(Electrons + n1) + taun*(Holes + p1))"
    dUSRHdn="simplify(diff(%s, Electrons))" % USRH
    dUSRHdp="simplify(diff(%s, Holes))" % USRH
    devsim.node_model(device=device, region=region , name="USRH", equation=USRH)
    devsim.node_model(device=device, region=region , name="USRH:Electrons", equation=dUSRHdn)
    devsim.node_model(device=device, region=region , name="USRH:Holes", equation=dUSRHdp)


    devsim.equation(device=device, region=region, name="ElectronContinuityEquation", variable_name="Electrons",
                    edge_model="ElectronCurrent", variable_update="positive",
                    time_node_model="NCharge", node_model="USRH")

    devsim.equation(device=device, region=region, name="HoleContinuityEquation", variable_name="Holes",
                    edge_model="HoleCurrent", variable_update="positive",
                    time_node_model="PCharge", node_model="USRH")

def createSiliconDriftDiffusionAtContact(device, region, contact):
    format_dict = { "contact" : contact }
    for name, equation in (
        ("%(contact)snodeelectrons",           "ifelse(NetDoping > 0, Electrons - celec_%(contact)s, Electrons - n_i^2/chole_%(contact)s)"),
      ("%(contact)snodeholes",               "ifelse(NetDoping < 0, Holes - chole_%(contact)s, Holes - n_i^2/celec_%(contact)s)"),
      ("%(contact)snodeelectrons:Electrons", "1.0"),
      ("%(contact)snodeholes:Holes",         "1.0"),
    ):
        name_sub = name % format_dict
        equation_sub = equation % format_dict
        devsim.contact_node_model(device=device, contact=contact, name=name_sub, equation=equation_sub)

    devsim.contact_equation(device=device, contact=contact, name="ElectronContinuityEquation",
                            node_model="%snodeelectrons" % contact, edge_current_model="ElectronCurrent")

    devsim.contact_equation(device=device, contact=contact, name="HoleContinuityEquation",
                            node_model="%snodeholes" % contact, edge_current_model="HoleCurrent")


def createOxidePotentialOnly(device, region):
    for name, equation in (
        ("ElectricField",                   "(Potential@n0 - Potential@n1)*EdgeInverseLength"),
      ("ElectricField:Potential@n0",      "EdgeInverseLength"),
      ("ElectricField:Potential@n1",      "-EdgeInverseLength"),
      ("PotentialEdgeFlux",               "Permittivity*ElectricField"),
      ("PotentialEdgeFlux:Potential@n0",  "diff(Permittivity*ElectricField, Potential@n0)"),
      ("PotentialEdgeFlux:Potential@n1",  "-PotentialEdgeFlux:Potential@n0"),
    ):
        devsim.edge_model(device=device, region=region, name=name, equation=equation)

    devsim.equation(device=device, region=region, name="PotentialEquation", variable_name="Potential",
                    edge_model="PotentialEdgeFlux", variable_update="log_damp")

def createSiliconOxideInterface(device, interface):
    for name, equation in (
        ("continuousPotential",              "Potential@r0-Potential@r1"),
      ("continuousPotential:Potential@r0", "1"),
      ("continuousPotential:Potential@r1", "-1"),
    ):
        devsim.interface_model(device=device, interface=interface, name=name, equation=equation)

    devsim.interface_equation(device=device, interface=interface, name="PotentialEquation", interface_model="continuousPotential", type="continuous")

def createSiliconSiliconInterface(device, interface):
    for variable in ("Potential", "Electrons", "Holes"):
        format_dict = { "var", variable }
        for name, equation in (
            ("continuous%(var)s", "%(var)s@r0-%(var)s@r1"),
          ("continuous%(var)s:%(var)s@r0", "1"),
          ("continuous%(var)s:%(var)s@r1", "-1"),
        ):
            name_sub = name % format_dict
            equation_sub = equation % format_dict
            devsim.interface_model(device=device, interface=interface, name=name_sub, equation=equation_sub)
        eqname = "%sEquation" % variable
        ieqname = "continuous%s" % variable
        devsim.interface_equation(device=device, interface=interface, name=eqname,
                                  interface_model=ieqname, type="continuous")

def createBernoulli(device, region):
    #### test for requisite models here
    vdiffstr="(Potential@n0 - Potential@n1)/V_t"
    for name, equation in (
        ("vdiff", vdiffstr),
      ("vdiff:Potential@n0",  "V_t^(-1)"),
      ("vdiff:Potential@n1",  "-vdiff:Potential@n0"),
      ("Bern01",              "B(vdiff)"),
      ("Bern01:Potential@n0", "dBdx(vdiff) * vdiff:Potential@n0"),
      ("Bern01:Potential@n1", "-Bern01:Potential@n0"),
      ("Bern10",              "Bern01 + vdiff"),
      ("Bern10:Potential@n0", "Bern01:Potential@n0 + vdiff:Potential@n0"),
      ("Bern10:Potential@n1", "Bern01:Potential@n1 + vdiff:Potential@n1"),
    ):
        devsim.edge_model(device=device, region=region, name=name, equation=equation)

def createElectronCurrent(device, region):
    Jn="ElectronCharge*mu_n*EdgeInverseLength*V_t*(Electrons@n1*Bern10 - Electrons@n0*Bern01)"
    devsim.edge_model(device=device, region=region, name="ElectronCurrent", equation=Jn)
    for variable in ("Electrons", "Potential"):
        der = "simplify(diff(%s, %s))" % (Jn, variable)
        devsim.edge_model(device=device, region=region, name="ElectronCurrent", equation=der)

def createHoleCurrent (device, region):
    Jp="-ElectronCharge*mu_p*EdgeInverseLength*V_t*(Holes@n1*Bern01 - Holes@n0*Bern10)"
    devsim.edge_model(device=device, region=region, name="HoleCurrent", equation=Jp)
    for variable in ("Holes", "Potential"):
        der = "simplify(diff(%s, %s))" % (Jp, variable)
        devsim.edge_model(device=device, region=region, name="HoleCurrent", equation=der)

