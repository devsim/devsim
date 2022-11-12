/***
DEVSIM
Copyright 2013 DEVSIM LLC

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

#include "AtContactNode.hh"
#include "Contact.hh"
#include "Region.hh"
#include "Device.hh"
#include "Node.hh"
#include "GeometryStream.hh"

template <typename DoubleType>
AtContactNode<DoubleType>::AtContactNode(RegionPtr rp)
    : NodeModel("AtContactNode", rp, NodeModel::DisplayType::SCALAR)
{
    RegisterCallback("@@@ContactChange");
}

template <typename DoubleType>
void AtContactNode<DoubleType>::calcNodeScalarValues() const
{
    const Region &region = GetRegion();

    const std::string &rname = region.GetName();

    //// TODO: a region should probably own this
    const Device::ContactList_t &cl = region.GetDevice()->GetContactList();

    const ConstNodeList &nl = region.GetNodeList();
    std::vector<DoubleType> nv(nl.size());

    Device::ContactList_t::const_iterator it = cl.begin();
    for ( ; it != cl.end(); ++it)
    {
        if ((it->second->GetRegion()->GetName()) != rname)
        {
            continue;
        }

        const ConstNodeList_t &cnodes = it->second->GetNodes();
        for (ConstNodeList_t::const_iterator jt = cnodes.begin(); jt != cnodes.end(); ++jt)
        {

#if 0
            std::ostringstream os;
            os << region.GetName() << " Node  pointer " << *jt << "\n";
            GeometryStream::WriteOut(OutputStream::OutputType::INFO, os.str());
#endif

            const size_t index = (*jt)->GetIndex();
            nv[index] += 1.0;
        }
    }
    SetValues(nv);
}

template <typename DoubleType>
void AtContactNode<DoubleType>::setInitialValues()
{
    DefaultInitializeValues();
}

template <typename DoubleType>
void AtContactNode<DoubleType>::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

template class AtContactNode<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class AtContactNode<float128>;
#endif


