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
from devsim.python_packages.model_create import *

def Set_Mobility_Parameters(device, region):
    #As
    set_parameter(device=device, region=region, name="mu_min_e",  value=52.2)
    set_parameter(device=device, region=region, name="mu_max_e",  value=1417)
    set_parameter(device=device, region=region, name="theta1_e",     value=2.285)
    set_parameter(device=device, region=region, name="m_e",      value=1.0)
    set_parameter(device=device, region=region, name="f_BH",     value=3.828)
    set_parameter(device=device, region=region, name="f_CW",     value=2.459)
    set_parameter(device=device, region=region, name="c_D",      value=0.21)
    set_parameter(device=device, region=region, name="Nref_D",   value=4.0e20)
    set_parameter(device=device, region=region, name="Nref_1_e", value=9.68e16)
    set_parameter(device=device, region=region, name="alpha_1_e",    value=0.68)

    #B
    set_parameter(device=device, region=region, name="mu_min_h",  value=44.9)
    set_parameter(device=device, region=region, name="mu_max_h",  value=470.5)
    set_parameter(device=device, region=region, name="theta1_h",     value=2.247)
    set_parameter(device=device, region=region, name="m_h",      value=1.258)
    set_parameter(device=device, region=region, name="c_A",      value=0.50)
    set_parameter(device=device, region=region, name="Nref_A",   value=7.2e20)
    set_parameter(device=device, region=region, name="Nref_1_h", value=2.2e17)
    set_parameter(device=device, region=region, name="alpha_1_h",    value=0.719)


    #F_Pe F_Ph equations
    set_parameter(device=device, region=region, name="r1", value=0.7643)
    set_parameter(device=device, region=region, name="r2", value=2.2999)
    set_parameter(device=device, region=region, name="r3", value=6.5502)
    set_parameter(device=device, region=region, name="r4", value=2.3670)
    set_parameter(device=device, region=region, name="r5", value=-0.01552)
    set_parameter(device=device, region=region, name="r6", value=0.6478)

    #G_Pe G_Ph equations
    set_parameter(device=device, region=region, name="s1", value=0.892333)
    set_parameter(device=device, region=region, name="s2", value=0.41372)
    set_parameter(device=device, region=region, name="s3", value=0.19778)
    set_parameter(device=device, region=region, name="s4", value=0.28227)
    set_parameter(device=device, region=region, name="s5", value=0.005978)
    set_parameter(device=device, region=region, name="s6", value=1.80618)
    set_parameter(device=device, region=region, name="s7", value=0.72169)

    # velocity saturation
    set_parameter(device=device, region=region, name="vsat_e", value=1.0e7)
    set_parameter(device=device, region=region, name="vsat_h", value=1.0e7)

    #Lucent Mobility
    set_parameter(device=device, region=region, name="alpha_e", value=6.85e-21)
    set_parameter(device=device, region=region, name="alpha_h", value=7.82e-21)
    set_parameter(device=device, region=region, name="A_e", value=2.58)
    set_parameter(device=device, region=region, name="A_h", value=2.18)
    set_parameter(device=device, region=region, name="B_e", value=3.61e7)
    set_parameter(device=device, region=region, name="B_h", value=1.51e7)
    set_parameter(device=device, region=region, name="C_e", value=1.70e4)
    set_parameter(device=device, region=region, name="C_h", value=4.18e3)
    set_parameter(device=device, region=region, name="kappa_e", value=1.7)
    set_parameter(device=device, region=region, name="kappa_h", value=0.9)
    set_parameter(device=device, region=region, name="tau_e", value=0.0233)
    set_parameter(device=device, region=region, name="tau_h", value=0.0119)
    set_parameter(device=device, region=region, name="eta_e", value=0.0767)
    set_parameter(device=device, region=region, name="eta_h", value=0.123)
    set_parameter(device=device, region=region, name="delta_e", value=3.58e18)
    set_parameter(device=device, region=region, name="delta_h", value=4.10e15)

#mobility_h="(Electrons@n0*Electrons@n1)^(0.5)"
#mobility_e="(Holes@n0*Holes@n1)^(0.5)"

#assumption is we will substitute As or P as appropriate
#remember the derivatives as well
### This is the Klaassen mobility, need to adapt to Darwish
def Klaassen_Mobility(device, region):
    # require Electrons, Holes, Donors, Acceptors already exist
    mu_L_e="(mu_max_e * (300 / T)^theta1_e)"
    mu_L_h="(mu_max_h * (300 / T)^theta1_h)"
    CreateNodeModel(device, region, "mu_L_e", mu_L_e)
    CreateNodeModel(device, region, "mu_L_h", mu_L_h)

    mu_e_N="(mu_max_e * mu_max_e / (mu_max_e - mu_min_e) * (T/300)^(3*alpha_1_e - 1.5))"
    mu_h_N="(mu_max_h * mu_max_h / (mu_max_h - mu_min_h) * (T/300)^(3*alpha_1_h - 1.5))"
    CreateNodeModel(device, region, "mu_e_N", mu_e_N)
    CreateNodeModel(device, region, "mu_h_N", mu_h_N)

    mu_e_c="(mu_min_e * mu_max_e / (mu_max_e - mu_min_e)) * (300/T)^(0.5)"
    mu_h_c="(mu_min_h * mu_max_h / (mu_max_h - mu_min_h)) * (300/T)^(0.5)"
    CreateNodeModel(device, region, "mu_e_c", mu_e_c)
    CreateNodeModel(device, region, "mu_h_c", mu_h_c)

    PBH_e="(1.36e20/(Electrons + Holes) * (m_e) * (T/300)^2)"
    PBH_h="(1.36e20/(Electrons + Holes) * (m_h) * (T/300)^2)"
    CreateNodeModel (device,  region, "PBH_e", PBH_e)
    CreateNodeModelDerivative(device, region, "PBH_e", PBH_e, "Electrons", "Holes")
    CreateNodeModel (device,  region, "PBH_h", PBH_h)
    CreateNodeModelDerivative(device, region, "PBH_h", PBH_h, "Electrons", "Holes")

    Z_D="(1 + 1 / (c_D + (Nref_D / Donors)^2))"
    Z_A="(1 + 1 / (c_A + (Nref_A / Acceptors)^2))"
    CreateNodeModel (device, region, "Z_D", Z_D)
    CreateNodeModel (device, region, "Z_A", Z_A)

    N_D="(Z_D * Donors)"
    N_A="(Z_A * Acceptors)"
    CreateNodeModel (device,  region, "N_D", N_D)
    CreateNodeModel (device,  region, "N_A", N_A)

    N_e_sc="(N_D + N_A + Holes)"
    N_h_sc="(N_A + N_D + Electrons)"
    CreateNodeModel (device,  region, "N_e_sc", N_e_sc)
    CreateNodeModelDerivative(device, region, "N_e_sc", N_e_sc, "Electrons", "Holes")
    CreateNodeModel (device,  region, "N_h_sc", N_h_sc)
    CreateNodeModelDerivative(device, region, "N_h_sc", N_h_sc, "Electrons", "Holes")

    PCW_e="(3.97e13 * (1/(Z_D^3 * N_e_sc) * (T/300)^3)^(2/3))"
    PCW_h="(3.97e13 * (1/(Z_A^3 * N_h_sc) * (T/300)^3)^(2/3))"

    CreateNodeModel (device,  region, "PCW_e", PCW_e)
    CreateNodeModelDerivative(device, region, "PCW_e", PCW_e, "Electrons", "Holes")
    CreateNodeModel (device,  region, "PCW_h", PCW_h)
    CreateNodeModelDerivative(device, region, "PCW_h", PCW_h, "Electrons", "Holes")

    Pe="(1/(f_CW / PCW_e + f_BH/PBH_e))"
    Ph="(1/(f_CW / PCW_h + f_BH/PBH_h))"
    CreateNodeModel (device,  region, "Pe", Pe)
    CreateNodeModelDerivative(device, region, "Pe", Pe, "Electrons", "Holes")
    CreateNodeModel (device,  region, "Ph", Ph)
    CreateNodeModelDerivative(device, region, "Ph", Ph, "Electrons", "Holes")

    G_Pe="(1 - s1 / (s2 + (1.0/m_e * T/300)^s4 * Pe)^s3 + s5/((m_e * 300/T)^s7*Pe)^s6)"
    G_Ph="(1 - s1 / (s2 + (1.0/m_h * T/300)^s4 * Ph)^s3 + s5/((m_h * 300/T)^s7*Ph)^s6)"

    CreateNodeModel (device,  region, "G_Pe", G_Pe)
    CreateNodeModelDerivative(device, region, "G_Pe", G_Pe, "Electrons", "Holes")
    CreateNodeModel (device,  region, "G_Ph", G_Ph)
    CreateNodeModelDerivative(device, region, "G_Ph", G_Ph, "Electrons", "Holes")

    F_Pe="((r1 * Pe^r6 + r2 + r3 * m_e/m_h)/(Pe^r6 + r4 + r5 * m_e/m_h))"
    F_Ph="((r1 * Ph^r6 + r2 + r3 * m_h/m_e)/(Ph^r6 + r4 + r5 * m_h/m_e))"

    CreateNodeModel (device,  region, "F_Pe", F_Pe)
    CreateNodeModelDerivative(device, region, "F_Pe", F_Pe, "Electrons", "Holes")
    CreateNodeModel (device,  region, "F_Ph", F_Ph)
    CreateNodeModelDerivative(device, region, "F_Ph", F_Ph, "Electrons", "Holes")

    N_e_sc_eff="(N_D + G_Pe * N_A + Holes / F_Pe)"
    N_h_sc_eff="(N_A + G_Ph * N_D + Electrons / F_Ph)"

    CreateNodeModel (device,  region, "N_e_sc_eff", N_e_sc_eff)
    CreateNodeModelDerivative(device, region, "N_e_sc_eff", N_e_sc_eff, "Electrons", "Holes")
    CreateNodeModel (device,  region, "N_h_sc_eff", N_h_sc_eff)
    CreateNodeModelDerivative(device, region, "N_h_sc_eff", N_h_sc_eff, "Electrons", "Holes")

    mu_e_D_A_h="mu_e_N * N_e_sc/N_e_sc_eff * (Nref_1_e / N_e_sc)^alpha_1_e + mu_e_c * ((Electrons + Holes)/N_e_sc_eff)"
    mu_h_D_A_e="mu_h_N * N_h_sc/N_h_sc_eff * (Nref_1_h / N_h_sc)^alpha_1_h + mu_h_c * ((Electrons + Holes)/N_h_sc_eff)"

    CreateNodeModel          (device, region, "mu_e_D_A_h", mu_e_D_A_h)
    CreateNodeModelDerivative(device, region, "mu_e_D_A_h", mu_e_D_A_h, "Electrons", "Holes")

    CreateNodeModel          (device, region, "mu_h_D_A_e", mu_h_D_A_e)
    CreateNodeModelDerivative(device, region, "mu_h_D_A_e", mu_h_D_A_e, "Electrons", "Holes")

    mu_bulk_e_Node="mu_e_D_A_h * mu_L_e / (mu_e_D_A_h + mu_L_e)" 
    CreateNodeModel          (device, region, "mu_bulk_e_Node", mu_bulk_e_Node)
    CreateNodeModelDerivative(device, region, "mu_bulk_e_Node", mu_bulk_e_Node, "Electrons", "Holes")

    mu_bulk_h_Node="mu_h_D_A_e * mu_L_h / (mu_h_D_A_e + mu_L_h)" 
    CreateNodeModel          (device, region, "mu_bulk_h_Node", mu_bulk_h_Node)
    CreateNodeModelDerivative(device, region, "mu_bulk_h_Node", mu_bulk_h_Node, "Electrons", "Holes")

    CreateGeometricMean          (device, region, "mu_bulk_e_Node", "mu_bulk_e")
    CreateGeometricMeanDerivative(device, region, "mu_bulk_e_Node", "mu_bulk_e", "Electrons", "Holes")

    CreateGeometricMean          (device, region, "mu_bulk_h_Node", "mu_bulk_h")
    CreateGeometricMeanDerivative(device, region, "mu_bulk_h_Node", "mu_bulk_h", "Electrons", "Holes")

# mu_bulk is the original bulk model on the edge
# mu_sat  is the saturation velocity model name
# vsat is the name of the parameter for velocity saturation
# eparallel is the name of the parallel efield model
def Philips_VelocitySaturation(device, region, mu_sat, mu_bulk, eparallel, vsat):
    mu="2*({mu_bulk}) / (1 + (1 + 4 * (max(0, ({mu_bulk}) * {eparallel})/{vsat})^2)^0.5)".format(mu_bulk=mu_bulk, eparallel=eparallel, vsat=vsat)
    CreateElementModel2d           (device,  region, mu_sat, mu)
    CreateElementModelDerivative2d (device,  region, mu_sat, mu, "Potential", "Electrons", "Holes")

#### Later on need model for temperature on edge
#### Assumption is Enormal is a magnitude
def Philips_Surface_Mobility(device, region, enormal_e, enormal_h):
    #### for now, we assume that temperature is an edge quantity and not variable dependent
    T_prime_e="(T/300)^kappa_e"
    T_prime_h="(T/300)^kappa_h"
    CreateEdgeModel(device, region, "T_prime_e", T_prime_e)
    CreateEdgeModel(device, region, "T_prime_h", T_prime_h)
    # no variables, only parameters
    CreateNodeModel    (device, region, "NI_node", "(N_D + N_A)")
    CreateGeometricMean(device, region, "NI_node", "NI")

    #CreateGeometricMean(device, region, "Electrons", edgeElectrons)
    #CreateGeometricMeanDerivative(device, region, "Electrons", edgeElectrons Electrons)
    #CreateGeometricMean(device, region, "Holes",     edgeHoles)
    #CreateGeometricMeanDerivative(device, region, "Holes",     edgeHoles Holes)

    #### Need to prevent ridiculus mobilities for small Enormal
    mu_ac_e="B_e /max({0},1e2) + (C_e * NI^tau_e * max({0},1e2)^(-1/3)) / T_prime_e".format(enormal_e)
    mu_ac_h="B_h /max({0},1e2) + (C_h * NI^tau_h * max({0},1e2)^(-1/3)) / T_prime_h".format(enormal_h)
    CreateElementModel2d          (device, region, "mu_ac_e", mu_ac_e)
    CreateElementModelDerivative2d(device, region, "mu_ac_e", mu_ac_e, "Potential")
    CreateElementModel2d          (device, region, "mu_ac_h", mu_ac_h)
    CreateElementModelDerivative2d(device, region, "mu_ac_h", mu_ac_h, "Potential")

    #gamma_e="A_e + alpha_e * (edgeElectrons + edgeHoles) * NI^(-eta_e)"
    #gamma_h="A_h + alpha_h * (edgeElectrons + edgeHoles) * NI^(-eta_h)"
    #CreateElementModel2d          (device, region, "gamma_e", gamma_e)
    #CreateElementModelDerivative2d(device, region, "gamma_e", gamma_e Potential Electrons Holes)
    #CreateElementModel2d          (device, region, "gamma_h", gamma_h)
    #CreateElementModelDerivative2d(device, region, "gamma_h", gamma_h Potential Electrons Holes)

    # Hopefully a less problematic formulation
    gamma_e="A_e + alpha_e * (Electrons + Holes) * NI_node^(-eta_e)"
    gamma_h="A_h + alpha_h * (Electrons + Holes) * NI_node^(-eta_h)"
    CreateNodeModel              (device, region, "gamma_e_Node", gamma_e)
    CreateNodeModelDerivative    (device, region, "gamma_e_Node", gamma_e, "Electrons", "Holes")
    CreateGeometricMean          (device, region, "gamma_e_Node", "gamma_e")
    CreateGeometricMeanDerivative(device, region, "gamma_e_Node", "gamma_e", "Electrons", "Holes")
    CreateNodeModel              (device, region, "gamma_h_Node", gamma_h)
    CreateNodeModelDerivative    (device, region, "gamma_h_Node", gamma_h, "Electrons", "Holes")
    CreateGeometricMean          (device, region, "gamma_h_Node", "gamma_h")
    CreateGeometricMeanDerivative(device, region, "gamma_h_Node", "gamma_h", "Electrons", "Holes")

    #### Need to prevent ridiculus mobilities for small Enormal
    mu_sr_e="delta_e *(max({0},1e2))^(-gamma_e)".format(enormal_e)
    mu_sr_h="delta_h *(max({0},1e2))^(-gamma_h)".format(enormal_h)
    #mu_sr_e="1e8"
    #mu_sr_h="1e8"
    CreateElementModel2d          (device, region, "mu_sr_e", mu_sr_e)
    CreateElementModelDerivative2d(device, region, "mu_sr_e", mu_sr_e, "Potential", "Electrons", "Holes")
    CreateElementModel2d          (device, region, "mu_sr_h", mu_sr_h)
    CreateElementModelDerivative2d(device, region, "mu_sr_h", mu_sr_h, "Potential", "Electrons", "Holes")

    mu_e_0="mu_bulk_e * mu_ac_e * mu_sr_e / (mu_bulk_e*mu_ac_e + mu_bulk_e*mu_sr_e + mu_ac_e*mu_sr_e)"
    mu_h_0="mu_bulk_h * mu_ac_h * mu_sr_h / (mu_bulk_h*mu_ac_h + mu_bulk_h*mu_sr_h + mu_ac_h*mu_sr_h)"
    CreateElementModel2d          (device, region, "mu_e_0", mu_e_0)
    CreateElementModel2d          (device, region, "mu_h_0", mu_h_0)

    #### complicated derivation here
    for k in ("e", "h"):
        for i in ("Potential", "Electrons", "Holes"):
            for j in ("@en0", "@en1", "@en2"):
                ex="mu_{k}_0^2 * (mu_bulk_{k}^(-2)*mu_bulk_{k}:{i}{j} + mu_ac_{k}^(-2)*mu_ac_{k}:{i}{j} + mu_sr_{k}^(-2) * mu_sr_{k}:{i}{j})".format(i=i, j=j, k=k)
                CreateElementModel2d(device, region, "mu_{k}_0:{i}{j}".format(i=i, j=j, k=k), ex)

#CreateElementModelDerivative2d(device, region, "mu_e_0", mu_sr_e Potential Electrons Holes)
#CreateElementModelDerivative2d(device, region, "mu_h_0", mu_sr_h Potential Electrons Holes)

#### do we need to consider what happens if the j dot e in wrong direction
#### or do we take care of this before
#mu_e_ph="2*mu_e_0 / ((1 + (1 + 4 * (mu_e_0 * eparallel_e)^2))^(0.5))"
#mu_h_ph="2*mu_h_0 / ((1 + (1 + 4 * (mu_h_0 * eparallel_h)^2))^(0.5))"


### do geometric mean so that
#emobility = sqrt(mobility@n0 * mobility@n1)
#emobility:mobility@n0 = emobility/mobility@n0
#emobility:mobility@n1 = emobility/mobility@n1
# The @n0 means the quantity mobility:Electrons @n0, not the derivative of an node quantity by an edge quantitiy
#emobility:Electrons@n0 = emobility:mobility@n0 * mobility:Electrons@n0
#emobility:Electrons@n1 = emobility:mobility@n1 * mobility:Electrons@n1
