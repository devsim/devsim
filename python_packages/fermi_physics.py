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

#material parameter value unit description
constants = (
    ("global", "Permittivity",    8.85418782e-14, "F/cm^2", "Permittivity of Free Space"),
    ("SiO2",   "epsr",            3.9,            "",       "Relative Permittivity"),
    ("global", "ElectronCharge",  1.60217646e-19, "coul",   "Charge of electrons")
    ("Si",     "epsr",           11.7,            "",       "Relative Permittivity"),
    ("Si",     "NC300",           3.23e19,        "#/cm^3", "Conduction band DOS at 300K"),
    ("Si",     "NV300",           1.83e19,        "#/cm^3", "Valence band DOS at 300K"),
    ("Si",     "EG300",           1.12,           "eV",     "Energy gap at 300K"),
    ("Si",     "EG_ALPHA",        0.0,            "",       "Temperature dependence of Energy gap model"),
    ("Si",     "EG_BETA",         0.0,            "",       "Temperature dependence of Energy gap model"),
)

####
#### These will become part of package
####
#### Need relation between EF and Potential

Eta_C = "(EF - EC)/(k * T)"
Eta_V = "(EV - EF)/(k * T)"


#### EG MODEL from Wikipedia
EG(T) = EG(0) - (EG_ALPHA * T^2)/(T + EG_BETA)
ni(T) = (NC * NV)^0.5 * exp(-EG/(2 * k * T))


