/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "EdgeScalarData.hh"
#include "Region.hh"
#include "Edge.hh"
#include "Node.hh"
#include "EdgeModel.hh"
#include "ScalarData.cc"

///
/// Creates Edge model for node value on both sides of edge
/// Can be made more optimal by using vector on region which provides mapping from node index to edge index
template <typename DoubleType>
void createEdgeModelsFromNodeModel(const NodeScalarList<DoubleType> &nm, const Region &reg, EdgeScalarList<DoubleType> &edge0, EdgeScalarList<DoubleType> &edge1)
{
    const ConstEdgeList &el = reg.GetEdgeList();
    // Just in case size was never initialized
    edge0.resize(el.size());
    edge1.resize(el.size());

    for (size_t i = 0; i < edge0.size(); ++i)
    {
        const std::vector<ConstNodePtr> &nlist = el[i]->GetNodeList();
        edge0[i] = nm[nlist[0]->GetIndex()];
        edge1[i] = nm[nlist[1]->GetIndex()];
    }
}

template void createEdgeModelsFromNodeModel(const NodeScalarList<double> &nm, const Region &reg, EdgeScalarList<double> &edge0, EdgeScalarList<double> &edge1);
#ifdef DEVSIM_EXTENDED_PRECISION
template void createEdgeModelsFromNodeModel(const NodeScalarList<float128> &nm, const Region &reg, EdgeScalarList<float128> &edge0, EdgeScalarList<float128> &edge1);
#endif

#define SCCLASSNAME EdgeScalarData
#define SCMODELTYPE EdgeModel
#define SCDBLTYPE   double
#include "ScalarDataInstantiate.cc"

#ifdef DEVSIM_EXTENDED_PRECISION
#undef  SCDBLTYPE
#define SCDBLTYPE float128
#include "Float128.hh"
#include "ScalarDataInstantiate.cc"
#endif

