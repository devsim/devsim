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

#include "EdgeIndex.hh"
#include "EdgeModel.hh"
#include "Vector.hh"
#include "Region.hh"
#include "Edge.hh"
#include "Node.hh"

EdgeIndex::EdgeIndex(RegionPtr rp) :
EdgeModel("edge_index", rp, EdgeModel::SCALAR)
{
}


void EdgeIndex::calcEdgeScalarValues() const
{
    const ConstEdgeList &el = GetRegion().GetEdgeList();
    std::vector<double> ev(el.size());
    for (size_t i = 0; i < ev.size(); ++i)
    {
        ev[i] = calcEdgeIndex(el[i]);
    }
    SetValues(ev);
}

double EdgeIndex::calcEdgeIndex(ConstEdgePtr ep) const
{
    const double val = ep->GetIndex();
    return val;
}

void EdgeIndex::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

