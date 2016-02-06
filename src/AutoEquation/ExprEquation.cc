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

ExprEquation::ExprEquation(
    const std::string &eqname,
    RegionPtr rp,
    const std::string &var,
    const std::string &nmodel,
    const std::string &emodel,
    const std::string &evmodel,
    const std::string &eemodel,
    const std::string &eevmodel,
    const std::string &tdnmodel,
    Equation::UpdateType ut
    ) : Equation(eqname, rp, var, ut), node_model_(nmodel), edge_model_(emodel), edge_volume_model_(evmodel), element_model_(eemodel), volume_model_(eevmodel), time_node_model_(tdnmodel)
{
}

void ExprEquation::DerivedAssemble(dsMath::RealRowColValueVec &m, dsMath::RHSEntryVec &v, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
    ModelExprDataCachePtr model_cache = ModelExprDataCachePtr(new ModelExprDataCache()); 
    Region &r = const_cast<Region &>(GetRegion());
    r.SetModelExprDataCache(model_cache);

    if (t == dsMathEnum::DC)
    {
        if (!edge_model_.empty())
        {
            model_cache->clear();
            Equation::EdgeCoupleAssemble(edge_model_, m, v, w);
            if (!edge_volume_model_.empty())
            {
              Equation::EdgeNodeVolumeAssemble(edge_volume_model_, m, v, w);
            }
        }

        if (!node_model_.empty())
        {
            model_cache->clear();
            Equation::NodeVolumeAssemble(node_model_, m, v, w);
        }

        if (!element_model_.empty())
        {
            model_cache->clear();
            Equation::ElementEdgeCoupleAssemble(element_model_, m, v, w);
        }

        if (!volume_model_.empty())
        {
            model_cache->clear();
            Equation::ElementNodeVolumeAssemble(volume_model_, m, v, w);
        }

    }
    else if (t == dsMathEnum::TIME)
    {
        if (!time_node_model_.empty())
        {
            model_cache->clear();
            Equation::NodeVolumeAssemble(time_node_model_, m, v, w);
        }
    }
    else
    {
        dsAssert(0, "UNEXPECTED");
    }
}

void ExprEquation::UpdateValues(NodeModel &nm, const std::vector<double> &rhs)
{
    DefaultUpdate(nm, rhs);
}

void ExprEquation::ACUpdateValues(NodeModel &nm, const std::vector<std::complex<double> > &rhs)
{
    DefaultACUpdate(nm, rhs);
}

void ExprEquation::NoiseUpdateValues(const std::string &nm, const std::vector<size_t> &permvec, const std::vector<std::complex<double> > &rhs)
{
    DefaultNoiseUpdate(nm, permvec, rhs);
}

void ExprEquation::Serialize(std::ostream &of) const
{
  of << "COMMAND equation -device \"" << GetRegion().GetDeviceName() << "\" -region \"" << GetRegion().GetName() << "\" -name \"" << GetName() << "\" -variable_name \"" << GetVariable()
    << "\" -node_model \"" << node_model_
    << "\" -edge_model \"" << edge_model_
    << "\" -edge_volume_model \"" << edge_volume_model_
    << "\" -element_model \"" << element_model_
    << "\" -volume_model \"" << volume_model_
    << "\" -time_node_model \"" << time_node_model_ << "\"";
}

void ExprEquation::GetCommandOptions_Impl(std::map<std::string, ObjectHolder> &omap) const
{
  omap["device"] = ObjectHolder(GetRegion().GetDeviceName());
  omap["region"] = ObjectHolder(GetRegion().GetName());
  omap["name"]   = ObjectHolder(GetName());
  omap["variable_name"] = ObjectHolder(GetVariable());
  omap["node_model"] = ObjectHolder(node_model_);
  omap["edge_model"] = ObjectHolder(edge_model_);
  omap["edge_volume_model"] = ObjectHolder(edge_volume_model_);
  omap["element_model"] = ObjectHolder(element_model_);
  omap["volume_model"] = ObjectHolder(volume_model_);
  omap["time_node_model"] = ObjectHolder(time_node_model_);
}


