# Copyright 2013 DEVSIM LLC
#
# SPDX-License-Identifier: Apache-2.0

# The purpose is to verify our triangle element field calculation.
# It is based on Laux's weighting scheme
# @article{Laux:1985,
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
# }
import devsim

from devsim import load_devices

from laux_common import SetDimension, RunTest
import laux_common


load_devices(file="gmsh_diode3d_dd.msh")
device = "diode3d"
region = "Bulk"

SetDimension(3)
nee = laux_common.nee
nen = laux_common.nen
dim = laux_common.dim


# There are nee edges per tetrahedron
# number_elements = len(scalar_efield)/laux_common.nee;


number_test = -1

RunTest(device, region, number_test, "ElectricField", "Potential")

devsim.set_parameter(name="V_t", value=0.0259)
devsim.set_parameter(name="mu_n", value=400)
devsim.set_parameter(name="ElectronCharge", value=1.6e-19)
RunTest(device, region, number_test, "ElectronCurrent", "Potential")
