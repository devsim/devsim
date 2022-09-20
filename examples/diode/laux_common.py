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
    sys.exit(0)


from devsim import *

dim = None
nee = None
nen = None
directions = ('x', 'y', 'z')

def SetDimension(dimension):
    global dim
    global nee
    global nen
    if dimension == 2:
        dim = 2 # dimension
        nee = 3 # number of edges per element
        nen = 3 # number of nodes per element
    elif dimension == 3:
        dim = 3
        nee = 6
        nen = 4


# nen edges make up the dense matrix calculation for 1 node
# the element corresponds to 1 of n tetrahedron
# node indexes are the element edge quantities
def GetElementData(element_index, node_indexes):
    element_data = {}
    nodes = []

    # get nodes for each element edge 0, 1, 2, 3
    edge_node_list = []
    for j in range(nee):
        index = nee*element_index + j
        enodes = []
        for i in node_indexes:
            enodes.append(i[index])
        edge_node_list.append(enodes)
    element_data['edge_node_list'] = edge_node_list

    # get the all of the node indexes on the element
    # also available using get_element_node_list
    nodes = sorted(edge_node_list[0])
    element_data['node_indexes'] = nodes

    vmap = {}
    for i in range(nen):
        vmap[nodes[i]] = i
    element_data['node_index_positions'] = vmap

    # n0 and n1 indexes for each element edge head and tail
    node_to_edge_indexes = []
    n0_indexes = node_indexes[0]
    n1_indexes = node_indexes[1]
    for ni in nodes:
        edge_indexes = []
        for j in range(nee):
            ei = nee*element_index + j
            if (n0_indexes[ei] == ni) or (n1_indexes[ei] == ni):
                edge_indexes.append(j)
        node_to_edge_indexes.append(edge_indexes)
    # how each node index connects to which element edge
    element_data['node_to_edge_indexes'] = node_to_edge_indexes

    # store the element index
    element_data['element_index'] = element_index
    return element_data

def GetNodeMatrices(element_data, unit_vectors):
    # create the matrix associated with a given node
    matrices = []
    ei = element_data['element_index']
    for i in range(nen):
        edge_indexes = element_data['node_to_edge_indexes'][i]
        M = numpy.zeros((dim,dim))
        for j in range(dim):
            edge_index = nee * ei + edge_indexes[j]
            for k in range(dim):
                M[j][k] = unit_vectors[k][edge_index]
        matrices.append(M)
    return matrices

def CalculateEField(element_data, scalar_efield):
    # calculate the vector field with respect to each scalar edge quantity
    # calculated for each of the nen nodes on the element
    vector_efields = []
    ei = element_data['element_index']
    matrices = element_data['matrices']
    for i in range(nen):
        edge_indexes = element_data['node_to_edge_indexes'][i]
        B = numpy.zeros((dim,1))
        for j in range(dim):
            edge_index = nee * ei + edge_indexes[j]
            B[j] = scalar_efield[edge_index]
        ans = numpy.linalg.solve(matrices[i], B)
        vector_efields.append(ans)
    return vector_efields

def CalculateEFieldDerivatives(element_data, scalar_efield_derivatives):
    # handle the case for each node derivative
    # scalar fields are w.r.t. to n0 and n1 on each edge
    vector_efield_derivatives = []
    ei = element_data['element_index']
    matrices = element_data['matrices']
    # These are the derivatives for all of the nodes of the whole element
    node_indexes = element_data['node_indexes']
    for i in range(nen):
        edge_indexes = element_data['node_to_edge_indexes'][i]
        B = numpy.zeros((dim,nen))
        for j in range(dim):
            edge_index = edge_indexes[j]
            input_edge_index = nee * ei + edge_index
            edge_node_list = element_data['edge_node_list'][edge_index]
            # these are the derivative nodes corresponding
            for k in range(nen):
                # this is the node we are taking the derivative with respect to
                # it is over the entire element
                nk = node_indexes[k]
                # are we in the head or tail node
                val = 0.0
                if nk == edge_node_list[0]:
                    val = scalar_efield_derivatives[0][input_edge_index]
                elif nk == edge_node_list[1]:
                    val = scalar_efield_derivatives[1][input_edge_index]
                # each column corresponds to a different derivative node
                B[j][k] = val
        ans = numpy.linalg.solve(matrices[i], B)
        vector_efield_derivatives.append(ans)
    return vector_efield_derivatives

def CalculateFinalValuesPairs(element_data, vector_efields, output0, output1):
    ei = element_data['element_index']
    node_indexes = element_data['node_indexes']
    edge_node_list = element_data['edge_node_list']
    node_index_positions = element_data['node_index_positions']

    outputs = [output0, output1]

    for edge_index in range(nee):
        enodes = edge_node_list[edge_index]
        output_edge_index = nee * ei + edge_index
        for i, ii in enumerate(enodes[0:2]):
            # make sure node indexes are in proper range
            val = numpy.transpose(vector_efields[node_index_positions[ii]][:,0])
            outputs[i][output_edge_index, 0:dim] = val

def CalculateFinalDerivativesPairs(element_data, vector_efield_derivatives, output0, output1):
    ei = element_data['element_index']
    node_indexes = element_data['node_indexes']
    edge_node_list = element_data['edge_node_list']
    node_index_positions = element_data['node_index_positions']

    outputs = [output0, output1]

    for edge_index in range(nee): # for each element edge
        enodes = edge_node_list[edge_index]
        output_edge_index = nee * ei + edge_index
        for i, ii in enumerate(enodes[0:2]):
            for k, kk in enumerate(enodes): # for the derivative nodes
                val = numpy.transpose(vector_efield_derivatives[node_index_positions[ii]][:,node_index_positions[kk]])
                row_stride = slice(dim*(k+1),dim*(k+2))
                outputs[i][output_edge_index, row_stride] = val

#    for i in range(nen):
#        edge_indexes = node_to_edge_indexes_map[i]
#        for j in range(dim):
#            edge_index = edge_indexes[j]
#            output_edge_index = nee * ei + edge_index
#            for k in range(nen):
#                # this are the derivatives corresponding to the ordered list of nodes on the entire element
#                nk = node_indexes[k]
#                #val = 0.5 * numpy.transpose(vector_efield_derivatives[i][:][k])
#                val = 0.5 * numpy.transpose(vector_efield_derivatives[i][:,k])
#                for kk in range(nen):
#                    nkk = edge_node_list[edge_index][kk]
#                    if nk == nkk:
#                        row_stride = slice(dim*(kk+1),dim*(kk+2))
#                        #print row_stride
#                        output[output_edge_index, row_stride] += val



def CalculateFinalValues3D(element_data, vector_efields, output):
    # this is the weighted value from each node on the edge
    ei = element_data['element_index']
    #node_indexes = element_data['node_indexes']
    #edge_node_list = element_data['edge_node_list']
    node_to_edge_indexes_map = element_data['node_to_edge_indexes']
    for i in range(nen):
        edge_indexes = node_to_edge_indexes_map[i]
        #print len(edge_indexes)
        #raise RuntimeError("STOP")
        val = 0.5 * numpy.transpose(vector_efields[i][:,0])
        for j in range(dim):
            edge_index = edge_indexes[j]
            #if (abs(edge_index) > 5):
            #  raise RuntimeError("PROBLEM")
            output_edge_index = nee * ei + edge_index
            output[output_edge_index,0:dim] += val

def CalculateFinalDerivatives3D(element_data, vector_efield_derivatives, output):
    # this is the weighted value from each node on the edge
    # now for the derivatives
    ei = element_data['element_index']
    node_indexes = element_data['node_indexes']
    edge_node_list = element_data['edge_node_list']
    node_to_edge_indexes_map = element_data['node_to_edge_indexes']
    for i in range(nen):
        edge_indexes = node_to_edge_indexes_map[i]
        for j in range(dim):
            edge_index = edge_indexes[j]
            output_edge_index = nee * ei + edge_index
            for k in range(nen):
                # this are the derivatives corresponding to the ordered list of nodes on the entire element
                nk = node_indexes[k]
                #val = 0.5 * numpy.transpose(vector_efield_derivatives[i][:][k])
                val = 0.5 * numpy.transpose(vector_efield_derivatives[i][:,k])
                for kk in range(nen):
                    nkk = edge_node_list[edge_index][kk]
                    if nk == nkk:
                        row_stride = slice(dim*(kk+1),dim*(kk+2))
                        #print row_stride
                        output[output_edge_index, row_stride] += val

def CalculateFinalValues2D(element_data, vector_efields, ecouple, output):
    ei = element_data['element_index']
    # this is special weighting based on edge away from edge of interest
    wts  = numpy.zeros((nee,)) # this handles the weights summed into our edge
    outs = numpy.zeros((nee,dim))
    # iterate over our element nodes
    node_to_edge_indexes = element_data['node_to_edge_indexes']
    for i in range(nen):
        edge_indexes = node_to_edge_indexes[i]
        for j in edge_indexes:
            for k in edge_indexes:
                if j == k:
                    continue
                wt = ecouple[nee * ei + k]
                wts[j] += wt
                outs[j] += wt * vector_efields[i][:,0]
    for i in range(nee):
        outs[i] /= wts[i]
        output[nee * ei + i,0:dim] = numpy.transpose(outs[i])

def CalculateFinalDerivatives2D(element_data, vector_efield_derivatives, ecouple, output):
    ei = element_data['element_index']
    # this is special weighting based on edge away from edge of interest
    wts  = numpy.zeros((nee,)) # this handles the weights summed into our edge
    outs = numpy.zeros((nee,nen*dim))
    # iterate over our element nodes
    node_indexes = element_data['node_indexes']
    node_to_edge_indexes = element_data['node_to_edge_indexes']
    edge_node_list = element_data['edge_node_list']
    for i in range(nen):
        edge_indexes = node_to_edge_indexes[i]
        for j in edge_indexes:
            for k in edge_indexes:
                if j == k:
                    continue
                wt = ecouple[nee * ei + k]
                wts[j] += wt
                for l in range(nen):
                    row_stride = slice(dim*l,dim*(l+1))
                    outs[j,row_stride] += wt * vector_efield_derivatives[i][:,l]
                    #outs[j,] += wt * vector_efields[i][:,0]
                #val = 0.5 * numpy.transpose(vector_efield_derivatives[i][:,k])
    for i in range(nee):
        outs[i] /= wts[i]
        for k in range(nen):
            nk = node_indexes[k]
            for kk in range(nen):
                nkk = edge_node_list[i][kk]
                if nk == nkk:
                    row_stride1 = slice(dim*(kk+1),dim*(kk+2))
                    row_stride2 = slice(dim*(k),dim*(k+1))
                    #print row_stride1
                    #print row_stride2
                    output[nee * ei + i,row_stride1] = numpy.transpose(outs[i, row_stride2])
                    break


def GetScalarField(device, region, name, mname):
    element_model(device=device, region=region, name=name, equation=mname)
    return numpy.array(get_element_model_values(device=device, region=region, name=name))

def GetScalarFieldDerivatives(device, region, name, mname, vname):
    ret = []
    for i in range(2):
        oname = name + str(i)
        iname = "%s:%s@n%d" % (mname, vname, i)
        element_model(device=device, region=region, name=oname, equation=iname)
        ret.append(get_element_model_values(device=device, region=region, name=oname))
    return ret


def GetNodeIndexes(device, region):
    ret = []
    element_from_node_model(node_model="node_index", device=device, region=region)
    for i in range(nen):
        tmp = get_element_model_values(device=device, region=region, name="node_index@en%d" % i)
        tmp = [int(x) for x in tmp]
        ret.append(tmp)
    return ret


def GetUnitVectors(device, region):
    ret = []
    for i in range(dim):
        mname = "s%s" % directions[i]
        element_model(device=device, region=region, name=mname, equation="unit%s" % directions[i])
        ret.append(get_element_model_values(device=device, region=region, name=mname))
    return ret

def SetupOutputCompare(device, region, model, variable, output):
    k = 0
    for i in range(dim):
        mname = model + "_" + directions[i]
        output[:,k] = numpy.array(get_element_model_values(device=device, region=region, name=mname))
        print("%d %s" % (k, mname))
        k += 1
    for j in range(nen):
        for i in range(dim):
            mname = model + "_" + directions[i]
            dname = mname + ":" + variable + "@en%d" % j
            output[:,k] = numpy.array(get_element_model_values(device=device, region=region, name=dname))
            print("%d %s" % (k, dname))
            k += 1

def DoCompare(output, output_compare, number_test):
    test2 = output[0:nee*number_test] - output_compare[0:nee*number_test]
    print(numpy.linalg.norm(test2, ord=numpy.inf ))
    for row in range(number_test):
        for col in range(nen+1):
            sl1 = slice(nee*row,nee*(row+1))
            sl2 = slice(dim*col,dim*(col+1))
            norm = numpy.linalg.norm(output[(sl1, sl2)]-output_compare[(sl1,sl2)])
            if norm > 1e-4:
                print("%d %d %g" % (row, col, norm))

    # use for debugging
    if True:
        #for row in range(10):
        row = 0
        col = 0
        sl1 = slice(nee*row,nee*(row+1))
        sl2 = slice(dim*col,dim*(col+1))
        print(output[(sl1, sl2)])
        print(output_compare[(sl1,sl2)])
        print(output[(sl1, sl2)] - output_compare[(sl1,sl2)])

def RunTest(device, region, number_test, scalar_field, derivative_variable):
    scalar_efield = GetScalarField(device, region, "scalar_efield", scalar_field)
    scalar_efield_derivatives = GetScalarFieldDerivatives(device, region, "scalar_efield_n", scalar_field, derivative_variable)
    node_indexes = GetNodeIndexes(device, region)
    unit_vectors = GetUnitVectors(device, region)

    number_elements = len(scalar_efield)//nee

    if number_test < 1:
        number_test = number_elements


    output_dims = (nee*number_elements, dim*(nen+1))
    output = numpy.zeros(output_dims)
    output0 = numpy.zeros(output_dims)
    output1 = numpy.zeros(output_dims)


    ecouple = None
    if dim == 2:
        ecouple = get_element_model_values(device=device, region=region, name="ElementEdgeCouple")

    for ei in range(number_test):
        edata = GetElementData(ei, node_indexes)

        edata['matrices'] = GetNodeMatrices(edata, unit_vectors)

        vector_efields = CalculateEField(edata, scalar_efield)

        vector_efield_derivatives = CalculateEFieldDerivatives(edata, scalar_efield_derivatives)

        if dim == 2:
            CalculateFinalValues2D(edata, vector_efields, ecouple, output)
            CalculateFinalDerivatives2D(edata, vector_efield_derivatives, ecouple, output)
        elif dim == 3:
            CalculateFinalValues3D(edata, vector_efields, output)
            CalculateFinalDerivatives3D(edata, vector_efield_derivatives, output)
        else:
            raise RuntimeError("Bad Dimension %d" % dim)

        CalculateFinalValuesPairs(edata, vector_efields, output0, output1)
        CalculateFinalDerivativesPairs(edata, vector_efield_derivatives, output0, output1)

    # For the averaged field
    element_from_edge_model(device=device, region=region, edge_model=scalar_field)
    element_from_edge_model(device=device, region=region, edge_model=scalar_field, derivative=derivative_variable)
    output_compare = numpy.zeros(output_dims)
    SetupOutputCompare(device, region, scalar_field, derivative_variable, output_compare)
    DoCompare(output, output_compare, number_test)

    # For the node based fields
    element_pair_from_edge_model(device=device, region=region, edge_model=scalar_field)
    element_pair_from_edge_model(device=device, region=region, edge_model=scalar_field, derivative=derivative_variable)
    for i, v in enumerate((output0, output1)):
        output_compare = numpy.zeros(output_dims)
        n = scalar_field + ("_node%d" % i)
        SetupOutputCompare(device, region, n, derivative_variable, output_compare)
        DoCompare(v, output_compare, number_test)

