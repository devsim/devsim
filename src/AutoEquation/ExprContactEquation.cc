/***
DEVSIM
Copyright 2013 Devsim LLC

This file is part of DEVSIM.

DEVSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, version 3.

DEVSIM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with DEVSIM.  If not, see <http://www.gnu.org/licenses/>.
***/

#include "ExprContactEquation.hh"
#include "Region.hh"

#include "ObjectCache.hh"
#include "ModelExprData.hh"

#include "ObjectHolder.hh"

#include <cmath>
#include <ostream>

// TODO:"Element Node Volume Support!!!!"

ExprContactEquation::ExprContactEquation(
            const std::string &eq,// nodemodel
            const std::string &var, // variable
            ContactPtr c,
            RegionPtr  r,
            const std::string &nmi,// nodemodel
            const std::string &emi, // edgemodel
            const std::string &eemi, // elementedgemodel
            const std::string &nmc,// nodemodel
            const std::string &emc, // edgemodel
            const std::string &eemc, // elementedgemodel
            const std::string &nmq,// nodemodel
            const std::string &emq ,// edgemodel
            const std::string &eemq // elementedgemodel
            ) :  ContactEquation(eq, var, c, r),
                 nodemodel_int(nmi),
                 edgemodel_int(emi),
                 elementedgemodel_int(eemi),
                 nodemodel_current(nmc),
                 edgemodel_current(emc),
                 elementedgemodel_current(eemc),
                 nodemodel_charge(nmq),
                 edgemodel_charge(emq),
                 elementedgemodel_charge(eemq)
{
}


void ExprContactEquation::DerivedAssemble(dsMath::RealRowColValueVec &m, RHSEntryVec &v, PermutationMap &p, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
    ModelExprDataCachePtr model_cache = ModelExprDataCachePtr(new ModelExprDataCache()); 
    Region &r = const_cast<Region &>(GetRegion());
    r.SetModelExprDataCache(model_cache);

    const std::string &NodeVolumeModel = GetRegion().GetNodeVolumeModel();
    const std::string &EdgeCoupleModel = GetRegion().GetEdgeCoupleModel();
    const std::string &ElementEdgeCoupleModel = GetRegion().GetElementEdgeCoupleModel();

    if (w == dsMathEnum::PERMUTATIONSONLY)
    {
        if (!nodemodel_int.empty())
        {
            model_cache->clear();
            AssembleNodeEquation(nodemodel_int, m, v, p, w, NodeVolumeModel);
        }
    }
    else if (t == dsMathEnum::DC)
    {
        if (!nodemodel_int.empty())
        {
            model_cache->clear();
            AssembleNodeEquation(nodemodel_int, m, v, p, w, NodeVolumeModel);
        }

        if (!edgemodel_int.empty())
        {
            model_cache->clear();
            AssembleEdgeEquation(edgemodel_int, m, v, w, EdgeCoupleModel);
        }

        if (!elementedgemodel_int.empty())
        {
            model_cache->clear();
            AssembleElementEdgeEquation(elementedgemodel_int, m, v, w, ElementEdgeCoupleModel, 1.0, -1.0);
        }

        const std::string &circuitnode = GetCircuitNode();
        if (!circuitnode.empty())
        {
            if (!nodemodel_current.empty())
            {
                model_cache->clear();
                AssembleNodeEquationOnCircuit(nodemodel_current, m, v, w, NodeVolumeModel);
            }
            if (!edgemodel_current.empty())
            {
                model_cache->clear();
                AssembleEdgeEquationOnCircuit(edgemodel_current, m, v, w, EdgeCoupleModel);
            }
            if (!elementedgemodel_current.empty())
            {
                model_cache->clear();
                AssembleElementEdgeEquationOnCircuit(elementedgemodel_current, m, v, w, ElementEdgeCoupleModel, 1.0, -1.0);
            }
        }
    }
    else if (t == dsMathEnum::TIME)
    {
    // In a circuit equation, we need to account for displacement flux
    /**
    //// Need time domain equivalent for internal models
        if (!nodemodel_int.empty())
        {
            AssembleNodeEquation(nodemodel_int, m, v, p);
        }

        if (!edgemodel_int.empty())
        {
            AssembleEdgeEquation(edgemodel_int, m, v, p);
        }
    */

        //// TODO: We really rely on there being a nodemodel in DCAssemble to set permutation entries
        const std::string &circuitnode = GetCircuitNode();
        if (!circuitnode.empty())
        {
            if (!nodemodel_charge.empty())
            {
                model_cache->clear();
                AssembleNodeEquationOnCircuit(nodemodel_charge, m, v, w, NodeVolumeModel);
            }
            if (!edgemodel_charge.empty())
            {
                model_cache->clear();
                AssembleEdgeEquationOnCircuit(edgemodel_charge, m, v, w, EdgeCoupleModel);
            }
            if (!elementedgemodel_charge.empty())
            {
                model_cache->clear();
                AssembleElementEdgeEquationOnCircuit(elementedgemodel_charge, m, v, w, ElementEdgeCoupleModel, 1.0, -1.0);
            }
        }
    }
}

void ExprContactEquation::calcCurrent()
{
    /// handle cylindrical
    double ch = integrateNodeModelOverNodes(nodemodel_current, GetRegion().GetNodeVolumeModel());
    ch += integrateEdgeModelOverNodes(edgemodel_current, GetRegion().GetEdgeCoupleModel());
    ch += integrateElementEdgeModelOverNodes(elementedgemodel_current, GetRegion().GetElementEdgeCoupleModel(), 1.0, -1.0);
    SetCurrent(ch);
}

// Modeled after the Expr Equation
// Need to separate on a per contact basis
void ExprContactEquation::calcCharge()
{
    double ch = integrateNodeModelOverNodes(nodemodel_charge, GetRegion().GetNodeVolumeModel());
    ch += integrateEdgeModelOverNodes(edgemodel_charge, GetRegion().GetEdgeCoupleModel());
    ch += integrateElementEdgeModelOverNodes(elementedgemodel_charge, GetRegion().GetElementEdgeCoupleModel(), 1.0, -1.0);
    SetCharge(ch);
}

void ExprContactEquation::Serialize(std::ostream &of) const
{
  of << "COMMAND contact_equation "
        "-device \"" << GetDeviceName()
        << "\" -contact \"" << GetContactName()
        << "\" -name \"" << GetName()
        << "\" -variable_name \"" <<  GetVariable()
        << "\" -edge_charge_model \"" << edgemodel_charge
        << "\" -edge_current_model \"" << edgemodel_current
        << "\" -edge_model \"" << edgemodel_int
        << "\" -element_charge_model \"" << elementedgemodel_charge
        << "\" -element_current_model \"" << elementedgemodel_current
        << "\" -element_model \"" << elementedgemodel_int
        << "\" -node_charge_model \"" << nodemodel_charge
        << "\" -node_current_model \"" << nodemodel_current
        << "\" -node_model \"" << nodemodel_int
        << "\" -circuit_node \"" << GetCircuitNode()
        << "\"";
}

void ExprContactEquation::GetCommandOptions_Impl(std::map<std::string, ObjectHolder> &omap) const
{
  omap["device"] = ObjectHolder(GetDeviceName());
  omap["contact"] = ObjectHolder(GetContactName());
  omap["name"] = ObjectHolder(GetName());
  omap["variable_name"] = ObjectHolder( GetVariable());
  omap["edge_charge_model"] = ObjectHolder(edgemodel_charge);
  omap["edge_current_model"] = ObjectHolder(edgemodel_current);
  omap["edge_model"] = ObjectHolder(edgemodel_int);
  omap["element_charge_model"] = ObjectHolder(elementedgemodel_charge);
  omap["element_current_model"] = ObjectHolder(elementedgemodel_current);
  omap["element_model"] = ObjectHolder(elementedgemodel_int);
  omap["node_charge_model"] = ObjectHolder(nodemodel_charge);
  omap["node_current_model"] = ObjectHolder(nodemodel_current);
  omap["node_model"] = ObjectHolder(nodemodel_int);
  omap["circuit_node"] = ObjectHolder(GetCircuitNode());
}


