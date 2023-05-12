/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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

const char *InterfaceExprEquationEnum::EquationTypeString[] =
{
  "unknown",
  "continuous",
  "fluxterm",
  "hybrid"
};

template <typename DoubleType>
InterfaceExprEquation<DoubleType>::InterfaceExprEquation(
    const std::string &eqname,
    const std::string &eqname0,
    const std::string &eqname1,
    InterfacePtr ip,
    const std::string &inmodel,
    InterfaceExprEquationEnum::EquationType et
    ) : InterfaceEquation<DoubleType>(eqname, eqname0, eqname1, ip), interface_node_model_(inmodel), equation_type_(et)
{
    equation_type_ = et;
}

/// TODO: Super strong candidate for refactoring
/// So very inefficient due to going through whole process for each node
/// Same goes for contact equation
/// TODO: Put stuff into new function so a TimeAssemble can be implemented in terms of this
template <typename DoubleType>
void InterfaceExprEquation<DoubleType>::DerivedAssemble(dsMath::RealRowColValueVec<DoubleType> &m, dsMath::RHSEntryVec<DoubleType> &v, PermutationMap &p, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
    ModelExprDataCachePtr<DoubleType> model_cache0 = ModelExprDataCachePtr<DoubleType>(new ModelExprDataCache<DoubleType>());
    Region &r0 = *const_cast<Region *>(InterfaceEquation<DoubleType>::GetInterface().GetRegion0());
    r0.SetModelExprDataCache(model_cache0);

    ModelExprDataCachePtr<DoubleType> model_cache1 = ModelExprDataCachePtr<DoubleType>(new ModelExprDataCache<DoubleType>());
    Region &r1 = *const_cast<Region *>(InterfaceEquation<DoubleType>::GetInterface().GetRegion1());
    r1.SetModelExprDataCache(model_cache1);

    InterfaceModelExprDataCachePtr<DoubleType> interface_model_cache = InterfaceModelExprDataCachePtr<DoubleType>(new InterfaceModelExprDataCache<DoubleType>());
    Interface &interface = const_cast<Interface &>(InterfaceEquation<DoubleType>::GetInterface());
    interface.SetInterfaceModelExprDataCache(interface_model_cache);

    const std::string &SurfaceAreaModel = InterfaceEquation<DoubleType>::GetInterface().GetSurfaceAreaModel();
    if (t == dsMathEnum::TimeMode::DC)
    {
        if (!interface_node_model_.empty())
        {
            //// Type 1 is where we permutate the two equations on both sides together
            //// A new equation is specified for the left over equation
            if (equation_type_ == InterfaceExprEquationEnum::CONTINUOUS)
            {
                InterfaceEquation<DoubleType>::NodeVolumeType1Assemble(interface_node_model_, m, v, p, w, SurfaceAreaModel);
            }
            else if (equation_type_ == InterfaceExprEquationEnum::FLUXTERM)
            {
                InterfaceEquation<DoubleType>::NodeVolumeType2Assemble(interface_node_model_, m, v, p, w, SurfaceAreaModel);
            }
            else if (equation_type_ == InterfaceExprEquationEnum::HYBRID)
            {
                InterfaceEquation<DoubleType>::NodeVolumeType3Assemble(interface_node_model_, m, v, p, w, SurfaceAreaModel);
            }
            else
            {
                dsAssert(0, "UNEXPECTED");
            }
        }
    }
    else if (t == dsMathEnum::TimeMode::TIME)
    {
    }
    else
    {
        dsAssert(0, "UNEXPECTED");
    }
}

template <typename DoubleType>
void InterfaceExprEquation<DoubleType>::Serialize(std::ostream &of) const
{
  const auto &nm  = InterfaceEquation<DoubleType>::GetName();
  const auto &nm0 = InterfaceEquation<DoubleType>::GetName0();
  const auto &nm1 = InterfaceEquation<DoubleType>::GetName1();

  of << "COMMAND interface_equation -device \"" << InterfaceEquation<DoubleType>::GetInterface().GetDeviceName() << "\" -interface \"" << InterfaceEquation<DoubleType>::GetInterface().GetName() << "\" -name \"" << nm;
  of << "\" -name0 \"" << nm0;
  of << "\" -name1 \"" << nm1;
  of << "\" -interface_model \"" << interface_node_model_
    << "\" -type \"" << InterfaceExprEquationEnum::EquationTypeString[equation_type_]
    << "\"";
}

template <typename DoubleType>
void InterfaceExprEquation<DoubleType>::GetCommandOptions_Impl(std::map<std::string, ObjectHolder> &omap) const
{
  omap["device"] = ObjectHolder(InterfaceEquation<DoubleType>::GetInterface().GetDeviceName());
  omap["interface"] = ObjectHolder(InterfaceEquation<DoubleType>::GetInterface().GetName());
  omap["name"]  = ObjectHolder(InterfaceEquation<DoubleType>::GetName());
  omap["name0"] = ObjectHolder(InterfaceEquation<DoubleType>::GetName0());
  omap["name1"] = ObjectHolder(InterfaceEquation<DoubleType>::GetName1());
  omap["interface_model"] = ObjectHolder(interface_node_model_);
  omap["type"] = ObjectHolder(InterfaceExprEquationEnum::EquationTypeString[equation_type_]);
}

template class InterfaceExprEquation<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class InterfaceExprEquation<float128>;
#endif

