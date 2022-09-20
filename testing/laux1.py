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

def calculateValues(scalar_efield, eecouple, sx, sy):
    '''
    calculates values, using the exact algorithm used internally to the software
    '''
    row0 = numpy.array([0, 0, 1])
    row1 = numpy.array([1, 2, 2])
    ans = numpy.zeros((3,1))
    M = numpy.zeros((2,2))
    B = numpy.zeros((2,1))
    e_this = [0] * 2
    e_opp  = [0] * 2
    efx = [0] *len(scalar_efield)
    efy = [0] *len(scalar_efield)
    for ei in range(len(scalar_efield)//3):
        for i in range(3):
            e_base = 3*ei + i
            k = 0
            for j in range(3):
                if i == j:
                    continue
                e_this[k] = 3*ei + j
                k = k + 1

            e_opp[0] = e_this[1]
            e_opp[1] = e_this[0]

            M[0][0] = sx[e_base]
            M[0][1] = sy[e_base]
            B[0] = scalar_efield[e_base]

            ex = 0
            ey = 0
            den = 0
            for j in range(2):
                M[1][0] = sx[e_this[j]]
                M[1][1] = sy[e_this[j]]
                B[1] = scalar_efield[e_this[j]]
                ans = numpy.linalg.solve(M, B)
                ex = ex + ans[0]*eecouple[e_this[j]]
                ey = ey + ans[1]*eecouple[e_this[j]]
                den = den + eecouple[e_this[j]]
            efx[e_base] = ex/den
            efy[e_base] = ey/den
    return (efx, efy)


load_devices(file="trimesh3.msh")
device = "MyDevice"
region = "MyRegion"
# trying to verify Laux current waiting implementation
# these are the scalar values
element_model(device=device, region=region, name="scalar_efield", equation="ElectricField")
element_model(device=device, region=region, name="sx", equation="unitx")
element_model(device=device, region=region, name="sy", equation="unity")

scalar_efield = get_element_model_values(device=device, region=region, name="scalar_efield")
sx = get_element_model_values(device=device, region=region, name="sx")
sy = get_element_model_values(device=device, region=region, name="sy")

element_from_edge_model(device=device, region=region, edge_model="ElectricField")
efieldx = get_element_model_values(device=device, region=region, name="ElectricField_x")
efieldy = get_element_model_values(device=device, region=region, name="ElectricField_y")
eecouple = get_element_model_values(device=device, region=region, name="ElementEdgeCouple")



(ex, ey) = calculateValues(scalar_efield, eecouple, sx, sy)
for i in range(len(ex)):
    print("%g\t%g\t%g\t%g" %( ex[i], ey[i], efieldx[i], efieldy[i]))

#
# now verify derivatives
#
edge_from_node_model(node_model="node_index", device=device, region=region)
edge_nodes = []
potential_diff = []
nedge=0
for i in range(2):
    tmp  = get_edge_model_values(device=device, region=region, name="node_index@n%d" % i)
    if not edge_nodes:
        nedge =len(tmp)
        edge_nodes = [0]*nedge
        potential_diff = [0]*nedge
        for j in range(nedge):
            edge_nodes[j] = [0]*2
            potential_diff[j] = [0]*2
    for j in range(nedge):
        edge_nodes[j][i] = tmp[j]
    tmp  = get_edge_model_values(device=device, region=region, name="ElectricField:Potential@n%d" % i)
    for j in range(nedge):
        potential_diff[j][i] = tmp[j]




element_model(device=device, region=region, name="scalar_edge_index", equation="edge_index")
scalar_edge_index = get_element_model_values(device=device, region=region, name="scalar_edge_index")
scalar_edge_index = [int(x) for x in scalar_edge_index]
element_from_node_model(node_model="node_index", device=device, region=region)

nelem = len(efieldx)
element_node_indexes = [0]*nelem
ddata_x = [0]*nelem
ddata_y = [0]*nelem
cdata_x = [0]*nelem
cdata_y = [0]*nelem
for i in range(nelem):
    element_node_indexes[i] = [0]*3
    ddata_x[i] = [0]*3
    ddata_y[i] = [0]*3
    cdata_x[i] = [0]*3
    cdata_y[i] = [0]*3
for i in range(3):
    tmp  = get_element_model_values(device=device, region=region, name="node_index@en%d" % i)
    for j in range(nelem):
        element_node_indexes[j][i] = int(tmp[j])

element_from_edge_model(device=device, region=region, edge_model="ElectricField", derivative="Potential")
for i in range(3):
    tmp  = get_element_model_values(device=device, region=region, name="ElectricField_x:Potential@en%d" % i)
    for j in range(nelem):
        cdata_x[j][i] = tmp[j]
for i in range(3):
    tmp  = get_element_model_values(device=device, region=region, name="ElectricField_y:Potential@en%d" % i)
    for j in range(nelem):
        cdata_y[j][i] = tmp[j]

diff_field = [0]*nelem
dest_map = [0]*nelem # maps global node of element to this particular edge
for dnode in range(3): #this is the node we are taking the derivative with respect to
                       # it is the global node index and will be figure out shortly
    for i in range(nelem//3):
        bi = 3*i  # this is the base element edge(0)
        dindex = element_node_indexes[bi][dnode]# this is the node we are taking the derivative with respect to
        for j in range(3): # this is the edge of interest
            ei = bi + j # this is the edge we are looking at
            sei = scalar_edge_index[ei]
            if dindex in edge_nodes[sei]:
                pos = edge_nodes[sei].index(dindex)
                diff_field[ei] = potential_diff[sei][pos]
            else:
                diff_field[ei] = 0.0
            dest_map[ei] = element_node_indexes[ei].index(dindex)
    (ex, ey) = calculateValues(diff_field, eecouple, sx, sy)
    for i in range(nelem):
        ddata_x[i][dest_map[i]] = ex[i]
        ddata_y[i][dest_map[i]] = ey[i]
        # these are the node indexes w.r.t. the first edge on the triangle
        #ddata_x[i][dnode] = ex[i]
        #ddata_y[i][dnode] = ey[i]

#print element_node_indexes

for i in range(3):
    print("index %d" % i)
    for j in range(nelem):
        print("%g\t%g\t%g\t%g" %( ddata_x[j][i], ddata_y[j][i], cdata_x[j][i], cdata_y[j][i]))

#print element_node_indexes
#print potential_diff
#print get_edge_model_values(device=device, region=region, name="ElectricField:Potential@n0")
#print get_edge_model_values(device=device, region=region, name="ElectricField:Potential@n1")
