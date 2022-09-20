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

def CreateElementElectronContinuityEquation(device, region, current_model):
    '''
      Uses element current model for equation
    '''
    equation(device=device, region=region, name="ElectronContinuityEquation", variable_name="Electrons",
             time_node_model="NCharge", node_model="ElectronGeneration",
             element_model=current_model, variable_update="positive")

#TODO: expand for circuit
def CreateElementContactElectronContinuityEquation(device, contact, current_model):
    '''
      Uses element current model for equation
    '''
    contact_electrons_name = "{0}nodeelectrons".format(contact)
    contact_equation(device=device, contact=contact, name="ElectronContinuityEquation",
                     node_model=contact_electrons_name,
                     element_current_model=current_model)

#### this version is from the direction of current flow
#### TODO: version from interface normal distance
def CreateNormalElectricFieldFromInterfaceNormal(device, region, interface):
    ### assume the interface normal already exists
    #### Get the electric field on the element
    element_from_edge_model(edge_model="ElectricField", device=device, region=region)
    element_from_edge_model(edge_model="ElectricField", device=device, region=region, derivative="Potential")

    #### Get the normal e field to current flow /// need to figure out sign importance for electron/hole
    Enormal="{0}_normal_x * ElectricField_x + {0}_normal_y * ElectricField_y".format(interface)
    CreateElementModel2d          (device, region, "Enormal", Enormal)
    CreateElementModelDerivative2d(device, region, "Enormal", Enormal, "Potential")


###### make this on a per carrier basis function
###### assume that low field model already exists, but not projected
def CreateNormalElectricFieldFromCurrentFlow(device, region, low_curr):
    dimension=get_dimension(device=device)
    if dimension != 2:
        raise ValueError("Supported in 2d only")

    element_from_edge_model(edge_model=low_curr, device=device, region=region)
    element_from_edge_model(edge_model=low_curr, device=device, region=region, derivative="Potential")
    element_from_edge_model(edge_model=low_curr, device=device, region=region, derivative="Electrons")
    element_from_edge_model(edge_model=low_curr, device=device, region=region, derivative="Holes")

    #### Get the current magnitude on the element
    #### do we need the derivative, since this is only scaling the direction?
    #### Worry about small denominator 1e-300 is about the limit 
    J_lf_mag="pow({0}_x^2 + {0}_y^2 + 1e-300, 0.5)".format(low_curr)
    CreateElementModel2d(device, region, "{0}_mag".format(low_curr), "{0}".format(J_lf_mag))
    for j in ("Electrons", "Holes", "Potential"):
        for i in ("@en0", "@en1", "@en2"):
            ex="({0}_x * {0}_x:{1}{2} + {0}_y * {0}_y:{1}{2})/{0}_mag".format(low_curr, j, i)
            CreateElementModel2d(device, region, "{0}_mag:{1}{2}".format(low_curr, j, i), ex)

    ### This calculates the normalized current in each direction
    for i in  ("x", "y"):
        J_norm="{0}_{1} / {0}_mag".format(low_curr, i)
        CreateElementModel2d(device, region, "{0}_norm_{1}".format(low_curr, i), J_norm)
        CreateElementModelDerivative2d(device, region, "{0}_norm_{1}".format(low_curr, i), J_norm, "Electrons", "Holes", "Potential")

    #### Get the electric field on the element
    element_from_edge_model(edge_model="ElectricField", device=device, region=region)
    element_from_edge_model(edge_model="ElectricField", device=device, region=region, derivative="Potential")

    #### Get the parallel e field to current flow
    Eparallel_J="{0}_norm_x * ElectricField_x + {0}_norm_y * ElectricField_y".format(low_curr)
    CreateElementModel2d          (device, region, "Eparallel_{0}".format(low_curr), Eparallel_J)
    CreateElementModelDerivative2d(device, region, "Eparallel_{0}".format(low_curr), Eparallel_J, "Potential")

    # magnitude e field
    ElectricField_mag="pow(ElectricField_x^2 + ElectricField_y^2 + 1e-300, 0.5)"
    CreateElementModel2d(device, region, "ElectricField_mag", ElectricField_mag)
    #the , turns this into an actual tuple
    for j in ("Potential",):
        for i in ("@en0","@en1", "@en2"):
            ex="(ElectricField_x * ElectricField_x:{0}{1} + ElectricField_y * ElectricField_y:{0}{1})/ElectricField_mag".format(j, i)
            CreateElementModel2d(device, region, "ElectricField_mag:{0}{1}".format(j, i), ex)

    #### Get the normal e field to current flow
    Enormal_J="pow(max(ElectricField_mag^2 - Eparallel_{0}^2,1e-300), 0.5)".format(low_curr)
    CreateElementModel2d(device, region, "Enormal_{0}".format(low_curr), Enormal_J)

    #CreateElementModelDerivative2d $device $region Enormal_{low_curr} {Enormal_J} Electrons Holes Potential
    for j in ("Electrons", "Holes", "Potential"):
        for i in ("@en0", "@en1", "@en2"):
            ex="(ElectricField_mag * ElectricField_mag:{0}{1} - Eparallel_{2} * Eparallel_{2}:{0}{1})/Enormal_{2}".format(j, i, low_curr)
            CreateElementModel2d(device, region, "Enormal_{0}:{1}{2}".format(low_curr, j, i), ex)

def CreateElementElectronCurrent2d(device, region, name, mobility_model):
    Jn = "ElectronCharge*{0}*EdgeInverseLength*V_t*kahan3(Electrons@en1*Bern01,  Electrons@en1*vdiff,  -Electrons@en0*Bern01)".format(mobility_model)
    CreateElementModel2d(device, region, name, Jn)
    for i in ("Electrons", "Holes", "Potential"):
        CreateElementModelDerivative2d(device, region, name, Jn, i)

def CreateElementHoleCurrent2d(device, region, name, mobility_model):
    Jp ="-ElectronCharge*{0}*EdgeInverseLength*V_t*kahan3(Holes@en1*Bern01, -Holes@en0*Bern01, -Holes@en0*vdiff)".format(mobility_model)
    CreateElementModel2d(device, region, name, Jp)
    for i in ("Electrons", "Holes", "Potential"):
        CreateElementModelDerivative2d(device, region, name, Jp, i)

