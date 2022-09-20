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
debug = False
def CreateSolution(device, region, name):
    '''
      Creates solution variables
      As well as their entries on each edge
    '''
    node_solution(name=name, device=device, region=region)
    edge_from_node_model(node_model=name, device=device, region=region)

def CreateNodeModel(device, region, model, expression):
    '''
      Creates a node model
    '''
    result=node_model(device=device, region=region, name=model, equation=expression)
    if debug:
        print(("NODEMODEL {d} {r} {m} \"{re}\"".format(d=device, r=region, m=model, re=result)))

def CreateNodeModelDerivative(device, region, model, expression, *vars):
    '''
      Create a node model derivative
    '''
    for v in vars:
        CreateNodeModel(device, region,
                        "{m}:{v}".format(m=model, v=v),
                        "simplify(diff({e},{v}))".format(e=expression, v=v))


def CreateContactNodeModel(device, contact, model, expression):
    '''
      Creates a contact node model
    '''
    result=contact_node_model(device=device, contact=contact, name=model, equation=expression)
    if debug:
        print(("CONTACTNODEMODEL {d} {c} {m} \"{re}\"".format(d=device, c=contact, m=model, re=result)))


def CreateContactNodeModelDerivative(device, contact, model, expression, variable):
    '''
      Creates a contact node model derivative
    '''
    CreateContactNodeModel(device, contact,
                           "{m}:{v}".format(m=model, v=variable),
                           "simplify(diff({e}, {v}))".format(e=expression, v=variable))

def CreateEdgeModel (device, region, model, expression):
    '''
      Creates an edge model
    '''
    result=edge_model(device=device, region=region, name=model, equation=expression)
    if debug:
        print("EDGEMODEL {d} {r} {m} \"{re}\"".format(d=device, r=region, m=model, re=result));

def CreateEdgeModelDerivatives(device, region, model, expression, variable):
    '''
      Creates edge model derivatives
    '''
    CreateEdgeModel(device, region,
                    "{m}:{v}@n0".format(m=model, v=variable),
                    "simplify(diff({e}, {v}@n0))".format(e=expression, v=variable))
    CreateEdgeModel(device, region,
                    "{m}:{v}@n1".format(m=model, v=variable),
                    "simplify(diff({e}, {v}@n1))".format(e=expression, v=variable))

def CreateContactEdgeModel(device, contact, model, expression):
    '''
      Creates a contact edge model
    '''
    result=contact_edge_model(device=device, contact=contact, name=model, equation=expression)
    if debug:
        print(("CONTACTEDGEMODEL {d} {c} {m} \"{re}\"".format(d=device, c=contact, m=model, re=result)))

def CreateContactEdgeModelDerivative(device, contact, model, expression, variable):
    '''
      Creates contact edge model derivatives with respect to variable on node
    '''
    CreateContactEdgeModel(device, contact, "{m}:{v}".format(m=model, v=variable), "simplify(diff({e}, {v}))".format(e=expression, v=variable))

def CreateInterfaceModel(device, interface, model, expression):
    '''
      Creates a interface node model
    '''
    result=interface_model(device=device, interface=interface, name=model, equation=expression)
    if debug:
        print(("INTERFACEMODEL {d} {i} {m} \"{re}\"".format(d=device, i=interface, m=model, re=result)))

#def CreateInterfaceModelDerivative(device, interface, model, expression, variable):
#  '''
#    Creates interface edge model derivatives with respect to variable on node
#  '''
#  CreateInterfaceModel(device, interface, "{m}:{v}".format(m=model, v=variable), "simplify(diff({e}, {v}))".format(e=expression, v=variable))

def CreateContinuousInterfaceModel(device, interface, variable):
    mname = "continuous{0}".format(variable)
    meq = "{0}@r0 - {0}@r1".format(variable)
    mname0 = "{0}:{1}@r0".format(mname, variable)
    mname1 = "{0}:{1}@r1".format(mname, variable)
    CreateInterfaceModel(device, interface, mname, meq)
    CreateInterfaceModel(device, interface, mname0,  "1")
    CreateInterfaceModel(device, interface, mname1, "-1")
    return mname


def InEdgeModelList(device, region, model):
    '''
      Checks to see if this edge model is available on device and region
    '''
    return model in get_edge_model_list(device=device, region=region)

def InNodeModelList(device, region, model):
    '''
      Checks to see if this node model is available on device and region
    '''
    return model in get_node_model_list(device=device, region=region)

#### Make sure that the model exists, as well as it's node model
def EnsureEdgeFromNodeModelExists(device, region, nodemodel):
    '''
      Checks if the edge models exists
    '''
    if not InNodeModelList(device, region, nodemodel):
        raise "{} must exist"

    emlist = get_edge_model_list(device=device, region=region)
    emtest = ("{0}@n0".format(nodemodel) and "{0}@n1".format(nodemodel))
    if not emtest:
        if debug:
            print("INFO: Creating ${0}@n0 and ${0}@n1".format(nodemodel))
        edge_from_node_model(device=device, region=region, node_model=nodemodel)

def CreateElementModel2d(device, region, model, expression):
    result=element_model(device=device, region=region, name=model, equation=expression)
    if debug:
        print(("ELEMENTMODEL {d} {r} {m} \"{re}\"".format(d=device, r=region, m=model, re=result)))


def CreateElementModelDerivative2d(device, region, model_name, expression, *args):
    if len(args) == 0:
        raise ValueError("Must specify a list of variable names")
    for i in args:
        for j in ("@en0", "@en1", "@en2"):
            CreateElementModel2d(device, region, "{0}:{1}{2}".format(model_name, i, j), "diff({0}, {1}{2})".format(expression, i, j))

### edge_model is the name of the edge model to be created
def CreateGeometricMean(device, region, nmodel, emodel):
    edge_average_model(device=device, region=region, edge_model=emodel, node_model=nmodel, average_type="geometric")

def CreateGeometricMeanDerivative(device, region, nmodel, emodel, *args):
    if len(args) == 0:
        raise ValueError("Must specify a list of variable names")
    for i in args:
        edge_average_model(device=device, region=region, edge_model=emodel, node_model=nmodel,
                           derivative=i, average_type="geometric")

