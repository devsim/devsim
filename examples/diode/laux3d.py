# Copyright 2013 Devsim LLC
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
# number = {dim},
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
import laux_common


load_devices(file="gmsh_diode3d_dd.msh")
device = "diode3d"
region = "Bulk"

SetDimension(3)
nee = laux_common.nee
nen = laux_common.nen
dim = laux_common.dim



# There are nee edges per tetrahedron
#number_elements = len(scalar_efield)/laux_common.nee;


number_test = -1

RunTest(device, region, number_test)



#row = 0
#if True:
##for row in range(10):
#  col = 4
#  sl1 = slice(nee*row,nee*(row+1))
#  sl2 = slice(dim*col,dim*(col+1))
#  print output[(sl1, sl2)]
#  print output_compare[(sl1,sl2)]
#  print output[(sl1, sl2)] - output_compare[(sl1,sl2)]
#

#print "VERIFY node_index@n0 == node_index@en0"
## creates node_index@n0, node_index@n1
#element_model(device=device, region=region, name="scalar_edge_index", equation="edge_index")
#edge_from_node_model(node_model="node_index", device=device, region=region)
#scalar_edge_index = get_element_model_values(device=device, region=region, name="scalar_edge_index")
#scalar_edge_index = [int(x) for x in scalar_edge_index]
#scalar_node_index0 = get_edge_model_values(device=device, region=region, name="node_index@n0")
#scalar_node_index0 = [int(x) for x in scalar_node_index0]
#scalar_node_index1 = get_edge_model_values(device=device, region=region, name="node_index@n1")
#scalar_node_index1 = [int(x) for x in scalar_node_index1]
#eedge = scalar_edge_index[sl1]
#print eedge
##print scalar_edge_index[sl1]
#print [scalar_node_index0[x] for x in eedge]
#print [scalar_node_index1[x] for x in eedge]
#print node_indexes[0][sl1]
#print node_indexes[1][sl1]

