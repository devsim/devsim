/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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
    RegisterCallback("EdgeNodeVolume");
}

template <typename DoubleType>
void NodeVolume<DoubleType>::calcNodeScalarValues() const
{
  const Region &r = GetRegion();

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

