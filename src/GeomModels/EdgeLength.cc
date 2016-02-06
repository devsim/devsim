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

#include "EdgeLength.hh"
#include "EdgeModel.hh"
#include "Vector.hh"
#include "Region.hh"
#include "Edge.hh"
#include "Node.hh"

EdgeLength::EdgeLength(RegionPtr rp) :
EdgeModel("EdgeLength", rp, EdgeModel::SCALAR)
{
}


void EdgeLength::calcEdgeScalarValues() const
{
    const ConstEdgeList &el = GetRegion().GetEdgeList();
    std::vector<double> ev(el.size());
    for (size_t i = 0; i < ev.size(); ++i)
    {
        ev[i] = calcEdgeLength(el[i]);
    }
    SetValues(ev);
}

double EdgeLength::calcEdgeLength(ConstEdgePtr ep) const
{
    Vector vm = ep->GetNodeList()[0]->Position();
    vm -= ep->GetNodeList()[1]->Position();
    const double val = vm.magnitude();
    return val;
}

void EdgeLength::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

