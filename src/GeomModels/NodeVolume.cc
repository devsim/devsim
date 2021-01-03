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

#include "NodeVolume.hh"
#include "Region.hh"
#include "Edge.hh"
#include "EdgeModel.hh"
#include "EdgeScalarData.hh"
#include "dsAssert.hh"

template <typename DoubleType>
NodeVolume<DoubleType>::NodeVolume(RegionPtr rp)
    : NodeModel("NodeVolume", rp, NodeModel::DisplayType::SCALAR)
{
    const size_t dimension = rp->GetDimension();

    RegisterCallback("EdgeNodeVolume");
}

template <typename DoubleType>
void NodeVolume<DoubleType>::calcNodeScalarValues() const
{
  const Region &r = GetRegion();
  const size_t dimension = r.GetDimension();

  std::vector<DoubleType> nv(r.GetNumberNodes());

  ConstEdgeModelPtr env = r.GetEdgeModel("EdgeNodeVolume");
  dsAssert(env.get(), "UNEXPECTED");

  EdgeScalarData<DoubleType> evol = EdgeScalarData<DoubleType>(*env);

  for (size_t i = 0; i < nv.size(); ++i)
  {
    DoubleType volume = 0.0;

    const ConstEdgeList &el = r.GetNodeToEdgeList()[i];

    ConstEdgeList::const_iterator it = el.begin();
    const ConstEdgeList::const_iterator itend = el.end();
    for ( ; it != itend; ++it)
    {
      volume += evol[(*it)->GetIndex()];
    }

    nv[i] = volume;
  }

  SetValues(nv);
}

template <typename DoubleType>
void NodeVolume<DoubleType>::setInitialValues()
{
    DefaultInitializeValues();
}

template <typename DoubleType>
void NodeVolume<DoubleType>::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

template class NodeVolume<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class NodeVolume<float128>;
#endif

