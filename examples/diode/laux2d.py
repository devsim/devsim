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

# The purpose is to verify our triangle element field calculation.
# It is based on Laux's weighting scheme
#@article{Laux:1985,
# author = {Laux, Steven E. and Byrnes, Robert G.},
# title = {Semiconductor device simulation using generalized mobility models},
# journal = {IBM J. Res. Dev.},
# issue_date = {May 1985},
# volume = {29},
# number = {3},
# month = may,
# year = {1985},
# issn = {0018-8646},
# pages = {289--301},
# numpages = {13},
# url = {http://dx.doi.org/10.1147/rd.293.0289},
# doi = {10.1147/rd.293.0289},
# acmid = {1012099},
# publisher = {IBM Corp.},
# address = {Riverton, NJ, USA},
#}

import sys
try:
    import numpy
    import numpy.linalg
except:
    print("numpy is not available with your installation and is not being run")
    sys.exit(-1)


from devsim import *
from laux_common import *


load_devices(file="gmsh_diode2d_dd.msh")
device = "diode2d"
region = "Bulk"

SetDimension(2)
number_test = -1

RunTest(device, region, number_test, "ElectricField", "Potential")

import devsim
devsim.set_parameter(name="V_t", value=0.0259)
devsim.set_parameter(name="mu_n", value=400)
devsim.set_parameter(name="ElectronCharge", value=1.6e-19)
RunTest(device, region, number_test, "ElectronCurrent", "Potential")

