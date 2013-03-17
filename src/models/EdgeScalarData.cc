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

#include "EdgeScalarData.hh"
#include "Region.hh"
#include "Edge.hh"
#include "Node.hh"
#include "EdgeModel.hh"
#include "ScalarData.cc"

//// Manual Template Instantiation
template class ScalarData<EdgeModel>;

template class
ScalarData<EdgeModel>& ScalarData<EdgeModel>::op_equal<ScalarDataHelper::times_equal>(const double &, const ScalarDataHelper::times_equal &);

template class
ScalarData<EdgeModel>& ScalarData<EdgeModel>::op_equal<ScalarDataHelper::times_equal>(const ScalarData<EdgeModel> &, const ScalarDataHelper::times_equal &);

template class
ScalarData<EdgeModel>& ScalarData<EdgeModel>::op_equal<ScalarDataHelper::plus_equal>(const double &, const ScalarDataHelper::plus_equal &);

template class
ScalarData<EdgeModel>& ScalarData<EdgeModel>::op_equal<ScalarDataHelper::plus_equal>(const ScalarData<EdgeModel> &, const ScalarDataHelper::plus_equal &);


///
/// Creates Edge model for node value on both sides of edge
/// Can be made more optimal by using vector on region which provides mapping from node index to edge index
void createEdgeModelsFromNodeModel(const NodeScalarList &nm, const Region &reg, EdgeScalarList &edge0, EdgeScalarList &edge1)
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
