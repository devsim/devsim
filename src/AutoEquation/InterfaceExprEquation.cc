/***
DEVSIM
Copyright 2013 Devsim LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
***/

#include "InterfaceExprEquation.hh"
#include "Interface.hh"
#include "Region.hh"
#include "Permutation.hh"
#include "NodeScalarData.hh"
#include "InterfaceNodeModel.hh"
#include "MatrixEntries.hh"
#include "dsAssert.hh"

#include "ObjectCache.hh"
#include "ModelExprData.hh"
#include "InterfaceModelExprData.hh"

#include "ObjectHolder.hh"

#include <vector>
#include <string>

const char *InterfaceExprEquation::EquationTypeString[] =
{
  "unknown",
  "continuous",
  "fluxterm"
};

InterfaceExprEquation::InterfaceExprEquation(
    const std::string &eqname,
    InterfacePtr ip,
    const std::string &var,
    const std::string &inmodel,
    EquationType et
    ) : InterfaceEquation(eqname, var, ip), interface_node_model_(inmodel), equation_type_(et)
{
    equation_type_ = et;
}

/// TODO: Super strong candidate for refactoring
/// So very inefficient due to going through whole process for each node
/// Same goes for contact equation
/// TODO: Put stuff into new function so a TimeAssemble can be implemented in terms of this
void InterfaceExprEquation::DerivedAssemble(dsMath::RealRowColValueVec &m, dsMath::RHSEntryVec &v, PermutationMap &p, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
    ModelExprDataCachePtr model_cache0 = ModelExprDataCachePtr(new ModelExprDataCache()); 
    Region &r0 = *const_cast<Region *>(GetInterface().GetRegion0());
    r0.SetModelExprDataCache(model_cache0);

    ModelExprDataCachePtr model_cache1 = ModelExprDataCachePtr(new ModelExprDataCache()); 
    Region &r1 = *const_cast<Region *>(GetInterface().GetRegion1());
    r1.SetModelExprDataCache(model_cache1);

    InterfaceModelExprDataCachePtr interface_model_cache = InterfaceModelExprDataCachePtr(new InterfaceModelExprDataCache()); 
    Interface &interface = const_cast<Interface &>(GetInterface());
    interface.SetInterfaceModelExprDataCache(interface_model_cache);

    const std::string &SurfaceAreaModel = GetInterface().GetSurfaceAreaModel();
    if (t == dsMathEnum::DC)
    {
        if (!interface_node_model_.empty())
        {
            //// Type 1 is where we permutate the two equations on both sides together
            //// A new equation is specified for the left over equation
            if (equation_type_ == CONTINUOUS)
            {
                NodeVolumeType1Assemble(interface_node_model_, m, v, p, w, SurfaceAreaModel);
            }
            else if (equation_type_ == FLUXTERM)
            {
                NodeVolumeType2Assemble(interface_node_model_, m, v, p, w, SurfaceAreaModel);
            }
            else
            {
                dsAssert(0, "UNEXPECTED");
            }
        }
    }
    else if (t == dsMathEnum::TIME)
    {
    }
    else
    {
        dsAssert(0, "UNEXPECTED");
    }
}

void InterfaceExprEquation::Serialize(std::ostream &of) const
{
  of << "COMMAND interface_equation -device \"" << GetInterface().GetDeviceName() << "\" -interface \"" << GetInterface().GetName() << "\" -name \"" << GetName() << "\" -variable_name \"" << GetVariable()
    << "\" -interface_model \"" << interface_node_model_
    << "\" -type \"" << EquationTypeString[equation_type_]
    << "\"";
}
    
void InterfaceExprEquation::GetCommandOptions_Impl(std::map<std::string, ObjectHolder> &omap) const
{
  omap["device"] = ObjectHolder(GetInterface().GetDeviceName());
  omap["interface"] = ObjectHolder(GetInterface().GetName());
  omap["name"] = ObjectHolder(GetName());
  omap["variable_name"] = ObjectHolder(GetVariable());
  omap["interface_model"] = ObjectHolder(interface_node_model_);
  omap["type"] = ObjectHolder(EquationTypeString[equation_type_]);
}

