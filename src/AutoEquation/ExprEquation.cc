/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "ExprEquation.hh"
#include "Region.hh"
#include "EdgeModel.hh"
#include "EdgeScalarData.hh"

#include "NodeScalarData.hh"
#include "dsAssert.hh"

#include "ObjectCache.hh"
#include "ModelExprData.hh"

#include "ObjectHolder.hh"

#include <vector>
#include <string>

template <typename DoubleType>
ExprEquation<DoubleType>::ExprEquation(
    const std::string &eqname,
    RegionPtr rp,
    const std::string &var,
    const std::string &nmodel,
    const std::string &emodel,
    const std::string &evmodel,
    const std::string &eemodel,
    const std::string &eev0model,
    const std::string &eev1model,
    const std::string &tdnmodel,
    EquationEnum::UpdateType ut
    ) : Equation<DoubleType>(eqname, rp, var, ut), node_model_(nmodel), edge_model_(emodel), edge_volume_model_(evmodel), element_model_(eemodel), volume_node0_model_(eev0model), volume_node1_model_(eev1model), time_node_model_(tdnmodel)
{
}

template <typename DoubleType>
void ExprEquation<DoubleType>::DerivedAssemble(dsMath::RealRowColValueVec<DoubleType> &m, dsMath::RHSEntryVec<DoubleType> &v, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
    ModelExprDataCachePtr<DoubleType> model_cache = ModelExprDataCachePtr<DoubleType>(new ModelExprDataCache<DoubleType>());
    Region &r = const_cast<Region &>(Equation<DoubleType>::GetRegion());
    r.SetModelExprDataCache(model_cache);

    if (t == dsMathEnum::TimeMode::DC)
    {
        if (!edge_model_.empty())
        {
            model_cache->clear();
            Equation<DoubleType>::EdgeCoupleAssemble(edge_model_, m, v, w);
            if (!edge_volume_model_.empty())
            {
              Equation<DoubleType>::EdgeNodeVolumeAssemble(edge_volume_model_, m, v, w);
            }
        }

        if (!node_model_.empty())
        {
            model_cache->clear();
            Equation<DoubleType>::NodeVolumeAssemble(node_model_, m, v, w);
        }

        if (!element_model_.empty())
        {
            model_cache->clear();
            Equation<DoubleType>::ElementEdgeCoupleAssemble(element_model_, m, v, w);
        }

        if (!volume_node0_model_.empty() || !volume_node1_model_.empty())
        {
            model_cache->clear();
            Equation<DoubleType>::ElementNodeVolumeAssemble(volume_node0_model_, volume_node1_model_, m, v, w);
        }

    }
    else if (t == dsMathEnum::TimeMode::TIME)
    {
        if (!time_node_model_.empty())
        {
            model_cache->clear();
            Equation<DoubleType>::NodeVolumeAssemble(time_node_model_, m, v, w);
        }
    }
    else
    {
        dsAssert(0, "UNEXPECTED");
    }
}

template <typename DoubleType>
void ExprEquation<DoubleType>::UpdateValues(NodeModel &nm, const dsMath::DoubleVec_t<DoubleType> &rhs)
{
    Equation<DoubleType>::DefaultUpdate(nm, rhs);
}

template <typename DoubleType>
void ExprEquation<DoubleType>::ACUpdateValues(NodeModel &nm, const dsMath::ComplexDoubleVec_t<DoubleType> &rhs)
{
    Equation<DoubleType>::DefaultACUpdate(nm, rhs);
}

template <typename DoubleType>
void ExprEquation<DoubleType>::NoiseUpdateValues(const std::string &nm, const std::vector<PermutationEntry> &permvec, const dsMath::ComplexDoubleVec_t<DoubleType> &rhs)
{
    Equation<DoubleType>::DefaultNoiseUpdate(nm, permvec, rhs);
}

template <typename DoubleType>
void ExprEquation<DoubleType>::Serialize(std::ostream &of) const
{
  of << "COMMAND equation -device \"" << Equation<DoubleType>::GetRegion().GetDeviceName() << "\" -region \"" << Equation<DoubleType>::GetRegion().GetName() << "\" -name \"" << Equation<DoubleType>::GetName() << "\" -variable_name \"" << Equation<DoubleType>::GetVariable()
    << "\" -node_model \"" << node_model_
    << "\" -edge_model \"" << edge_model_
    << "\" -edge_volume_model \"" << edge_volume_model_
    << "\" -element_model \"" << element_model_
    << "\" -volume_node0_model \"" << volume_node0_model_
    << "\" -volume_node1_model \"" << volume_node1_model_
    << "\" -time_node_model \"" << time_node_model_ << "\"";
}

template <typename DoubleType>
void ExprEquation<DoubleType>::GetCommandOptions_Impl(std::map<std::string, ObjectHolder> &omap) const
{
  omap["device"] = ObjectHolder(Equation<DoubleType>::GetRegion().GetDeviceName());
  omap["region"] = ObjectHolder(Equation<DoubleType>::GetRegion().GetName());
  omap["name"]   = ObjectHolder(Equation<DoubleType>::GetName());
  omap["variable_name"] = ObjectHolder(Equation<DoubleType>::GetVariable());
  omap["node_model"] = ObjectHolder(node_model_);
  omap["edge_model"] = ObjectHolder(edge_model_);
  omap["edge_volume_model"] = ObjectHolder(edge_volume_model_);
  omap["element_model"] = ObjectHolder(element_model_);
  omap["volume_node0_model"] = ObjectHolder(volume_node0_model_);
  omap["volume_node1_model"] = ObjectHolder(volume_node1_model_);
  omap["time_node_model"] = ObjectHolder(time_node_model_);
}

template class ExprEquation<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class ExprEquation<float128>;
#endif

