/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "ExprContactEquation.hh"
#include "Region.hh"

#include "ObjectCache.hh"
#include "ModelExprData.hh"

#include "ObjectHolder.hh"

#include <cmath>
#include <ostream>

template <typename DoubleType>
ExprContactEquation<DoubleType>::ExprContactEquation(
            const std::string &eq,// nodemodel
            ContactPtr c,
            RegionPtr  r,
            const std::string &nmi,// nodemodel
            const std::string &emi, // edgemodel
            const std::string &evmi, // edgevolumemodel
            const std::string &eemi, // elementedgemodel
            const std::string &eevmi0, // volumemodel
            const std::string &eevmi1, // volumemodel
            const std::string &nmc,// nodemodel
            const std::string &emc, // edgemodel
            const std::string &eemc, // elementedgemodel
            const std::string &nmq,// nodemodel
            const std::string &emq ,// edgemodel
            const std::string &eemq // elementedgemodel
            ) :  ContactEquation<DoubleType>(eq, c, r),
                 nodemodel_int(nmi),
                 edgemodel_int(emi),
                 edgevolumemodel_int(evmi),
                 elementedgemodel_int(eemi),
                 volume_node0_model_int_(eevmi0),
                 volume_node1_model_int_(eevmi1),
                 nodemodel_current(nmc),
                 edgemodel_current(emc),
                 elementedgemodel_current(eemc),
                 nodemodel_charge(nmq),
                 edgemodel_charge(emq),
                 elementedgemodel_charge(eemq)
{
}


template <typename DoubleType>
void ExprContactEquation<DoubleType>::DerivedAssemble(dsMath::RealRowColValueVec<DoubleType> &m, dsMath::RHSEntryVec<DoubleType> &v, PermutationMap &p, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t)
{
    ModelExprDataCachePtr<DoubleType> model_cache = ModelExprDataCachePtr<DoubleType>(new ModelExprDataCache<DoubleType>());
    Region &r = const_cast<Region &>(ContactEquation<DoubleType>::GetRegion());
    r.SetModelExprDataCache(model_cache);

    const std::string &NodeVolumeModel = ContactEquation<DoubleType>::GetRegion().GetNodeVolumeModel();
    const std::string &EdgeCoupleModel = ContactEquation<DoubleType>::GetRegion().GetEdgeCoupleModel();
    const std::string &ElementEdgeCoupleModel = ContactEquation<DoubleType>::GetRegion().GetElementEdgeCoupleModel();

    if (w == dsMathEnum::WhatToLoad::PERMUTATIONSONLY)
    {
        if (!nodemodel_int.empty())
        {
            model_cache->clear();
            ContactEquation<DoubleType>::AssembleNodeEquation(nodemodel_int, m, v, p, w, NodeVolumeModel);
        }
    }
    else if (t == dsMathEnum::TimeMode::DC)
    {
        if (!nodemodel_int.empty())
        {
            model_cache->clear();
            ContactEquation<DoubleType>::AssembleNodeEquation(nodemodel_int, m, v, p, w, NodeVolumeModel);
        }

        if (!edgemodel_int.empty())
        {
            model_cache->clear();
            ContactEquation<DoubleType>::AssembleEdgeEquation(edgemodel_int, m, v, w, EdgeCoupleModel, 1.0, -1.0);
        }

        if (!edgevolumemodel_int.empty())
        {
            model_cache->clear();

            const std::string &node0model = ContactEquation<DoubleType>::GetRegion().GetEdgeNode0VolumeModel();
            const std::string &node1model = ContactEquation<DoubleType>::GetRegion().GetEdgeNode1VolumeModel();

            if (node0model == node1model)
            {
                ContactEquation<DoubleType>::AssembleEdgeEquation(edgevolumemodel_int, m, v, w, node0model, 1.0, 1.0);
            }
            else
            {
                ContactEquation<DoubleType>::AssembleEdgeEquation(edgevolumemodel_int, m, v, w, node0model, 1.0, 0.0);
                ContactEquation<DoubleType>::AssembleEdgeEquation(edgevolumemodel_int, m, v, w, node1model, 0.0, 1.0);
            }
        }

        if (!elementedgemodel_int.empty())
        {
            model_cache->clear();
            ContactEquation<DoubleType>::AssembleElementEdgeEquation(elementedgemodel_int, m, v, w, ElementEdgeCoupleModel, 1.0, -1.0);
        }

        if (!(volume_node0_model_int_.empty() && volume_node1_model_int_.empty()))
        {
            model_cache->clear();

            const std::string &node0model = ContactEquation<DoubleType>::GetRegion().GetElementNode0VolumeModel();
            const std::string &node1model = ContactEquation<DoubleType>::GetRegion().GetElementNode1VolumeModel();

            if ((node0model == node1model) && (volume_node0_model_int_ == volume_node1_model_int_))
            {
              ContactEquation<DoubleType>::AssembleElementEdgeEquation(volume_node0_model_int_, m, v, w, node0model, 1.0, 1.0);
            }
            else
            {
              ContactEquation<DoubleType>::AssembleElementEdgeEquation(volume_node0_model_int_, m, v, w, node0model, 1.0, 0.0);
              ContactEquation<DoubleType>::AssembleElementEdgeEquation(volume_node1_model_int_, m, v, w, node1model, 0.0, 1.0);
            }
        }

        const std::string &circuitnode = ContactEquation<DoubleType>::GetCircuitNode();
        if (!circuitnode.empty())
        {
            if (!nodemodel_current.empty())
            {
                model_cache->clear();
                ContactEquation<DoubleType>::AssembleNodeEquationOnCircuit(nodemodel_current, m, v, w, NodeVolumeModel);
            }
            if (!edgemodel_current.empty())
            {
                model_cache->clear();
                ContactEquation<DoubleType>::AssembleEdgeEquationOnCircuit(edgemodel_current, m, v, w, EdgeCoupleModel, 1.0, -1.0);
            }
            if (!elementedgemodel_current.empty())
            {
                model_cache->clear();
                ContactEquation<DoubleType>::AssembleElementEdgeEquationOnCircuit(elementedgemodel_current, m, v, w, ElementEdgeCoupleModel, 1.0, -1.0);
            }
        }
    }
    else if (t == dsMathEnum::TimeMode::TIME)
    {
    // In a circuit equation, we need to account for displacement flux
    /**
    //// Need time domain equivalent for internal models
        if (!nodemodel_int.empty())
        {
            ContactEquation<DoubleType>::AssembleNodeEquation(nodemodel_int, m, v, p);
        }

        if (!edgemodel_int.empty())
        {
            ContactEquation<DoubleType>::AssembleEdgeEquation(edgemodel_int, m, v, p);
        }
    */

        //// TODO: We really rely on there being a nodemodel in DCAssemble to set permutation entries
        const std::string &circuitnode = ContactEquation<DoubleType>::GetCircuitNode();
        if (!circuitnode.empty())
        {
            if (!nodemodel_charge.empty())
            {
                model_cache->clear();
                ContactEquation<DoubleType>::AssembleNodeEquationOnCircuit(nodemodel_charge, m, v, w, NodeVolumeModel);
            }
            if (!edgemodel_charge.empty())
            {
                model_cache->clear();
                ContactEquation<DoubleType>::AssembleEdgeEquationOnCircuit(edgemodel_charge, m, v, w, EdgeCoupleModel, 1.0, -1.0);
            }
            if (!elementedgemodel_charge.empty())
            {
                model_cache->clear();
                ContactEquation<DoubleType>::AssembleElementEdgeEquationOnCircuit(elementedgemodel_charge, m, v, w, ElementEdgeCoupleModel, 1.0, -1.0);
            }
        }
    }
}

template <typename DoubleType>
void ExprContactEquation<DoubleType>::calcCurrent()
{
    /// handle cylindrical
    DoubleType ch = ContactEquation<DoubleType>::integrateNodeModelOverNodes(nodemodel_current, ContactEquation<DoubleType>::GetRegion().GetNodeVolumeModel());
    ch += ContactEquation<DoubleType>::integrateEdgeModelOverNodes(edgemodel_current, ContactEquation<DoubleType>::GetRegion().GetEdgeCoupleModel(), 1.0, -1.0);
    ch += ContactEquation<DoubleType>::integrateElementEdgeModelOverNodes(elementedgemodel_current, ContactEquation<DoubleType>::GetRegion().GetElementEdgeCoupleModel(), 1.0, -1.0);
    ContactEquation<DoubleType>::SetCurrent(ch);
}

// Modeled after the Expr Equation
// Need to separate on a per contact basis
template <typename DoubleType>
void ExprContactEquation<DoubleType>::calcCharge()
{
    DoubleType ch = ContactEquation<DoubleType>::integrateNodeModelOverNodes(nodemodel_charge, ContactEquation<DoubleType>::GetRegion().GetNodeVolumeModel());
    ch += ContactEquation<DoubleType>::integrateEdgeModelOverNodes(edgemodel_charge, ContactEquation<DoubleType>::GetRegion().GetEdgeCoupleModel(), 1.0, -1.0);
    ch += ContactEquation<DoubleType>::integrateElementEdgeModelOverNodes(elementedgemodel_charge, ContactEquation<DoubleType>::GetRegion().GetElementEdgeCoupleModel(), 1.0, -1.0);
    ContactEquation<DoubleType>::SetCharge(ch);
}

template <typename DoubleType>
void ExprContactEquation<DoubleType>::Serialize(std::ostream &of) const
{
  of << "COMMAND contact_equation "
        "-device \"" << ContactEquation<DoubleType>::GetDeviceName()
        << "\" -contact \"" << ContactEquation<DoubleType>::GetContactName()
        << "\" -name \"" << ContactEquation<DoubleType>::GetName()
        << "\" -edge_charge_model \"" << edgemodel_charge
        << "\" -edge_current_model \"" << edgemodel_current
        << "\" -edge_model \"" << edgemodel_int
        << "\" -edge_volume_model \"" << edgevolumemodel_int
        << "\" -element_charge_model \"" << elementedgemodel_charge
        << "\" -element_current_model \"" << elementedgemodel_current
        << "\" -element_model \"" << elementedgemodel_int
        << "\" -volume_node0_model \"" << volume_node0_model_int_
        << "\" -volume_node1_model \"" << volume_node1_model_int_
        << "\" -node_charge_model \"" << nodemodel_charge
        << "\" -node_current_model \"" << nodemodel_current
        << "\" -node_model \"" << nodemodel_int
        << "\" -circuit_node \"" << ContactEquation<DoubleType>::GetCircuitNode()
        << "\"";
}

template <typename DoubleType>
void ExprContactEquation<DoubleType>::GetCommandOptions_Impl(std::map<std::string, ObjectHolder> &omap) const
{
  omap["device"] = ObjectHolder(ContactEquation<DoubleType>::GetDeviceName());
  omap["contact"] = ObjectHolder(ContactEquation<DoubleType>::GetContactName());
  omap["name"] = ObjectHolder(ContactEquation<DoubleType>::GetName());
  omap["edge_charge_model"] = ObjectHolder(edgemodel_charge);
  omap["edge_current_model"] = ObjectHolder(edgemodel_current);
  omap["edge_model"] = ObjectHolder(edgemodel_int);
  omap["edge_volume_model"] = ObjectHolder(edgevolumemodel_int);
  omap["element_charge_model"] = ObjectHolder(elementedgemodel_charge);
  omap["element_current_model"] = ObjectHolder(elementedgemodel_current);
  omap["element_model"] = ObjectHolder(elementedgemodel_int);
  omap["volume_node0_model"] = ObjectHolder(volume_node0_model_int_);
  omap["volume_node1_model"] = ObjectHolder(volume_node1_model_int_);
  omap["node_charge_model"] = ObjectHolder(nodemodel_charge);
  omap["node_current_model"] = ObjectHolder(nodemodel_current);
  omap["node_model"] = ObjectHolder(nodemodel_int);
  omap["circuit_node"] = ObjectHolder(ContactEquation<DoubleType>::GetCircuitNode());
}

template class ExprContactEquation<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class ExprContactEquation<float128>;
#endif

