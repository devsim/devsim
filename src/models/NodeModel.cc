/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "NodeModel.hh"
#include "Device.hh"
#include "Region.hh"
#include "Node.hh"
#include "Contact.hh"
#include "dsAssert.hh"
#include "FPECheck.hh"
#include "GeometryStream.hh"

#include <algorithm>

const char *NodeModel::DisplayTypeString[] = {
  "nodisplay",
  "scalar",
  nullptr
};

bool NodeModel::IsZero() const
{
  CalculateValues();
  return model_data.IsUniform() && model_data.IsZero();
}

bool NodeModel::IsOne() const
{
  CalculateValues();
  return model_data.IsUniform() && model_data.IsOne();
}

NodeModel::~NodeModel() {
#if 0
    myregion->UnregisterCallback(name);
#endif
}

// derived classes must register dependencies
NodeModel::NodeModel(const std::string &nm, const RegionPtr rp, NodeModel::DisplayType dt, const ContactPtr cp)
    : name(nm),
      myregion(rp),
      mycontact(cp),
      uptodate(false),
      inprocess(false),
      displayType(dt),
      model_data(rp->GetNumberNodes())
{
  myself = rp->AddNodeModel(this);
}

const std::string &NodeModel::GetName() const
{
  return name;
}

std::string NodeModel::GetRealName() const
{
  return name + "_Real";
}

std::string NodeModel::GetImagName() const
{
  return name + "_Imag";
}

void NodeModel::InitializeValues()
{
  setInitialValues();
}

void NodeModel::DefaultInitializeValues()
{
    // default initialization is to 0.0
  model_data.clear();
}

void NodeModel::CalculateValues() const
{
  FPECheck::ClearFPE();
  if (!uptodate)
  {
    inprocess = true;
    try
    {
        this->calcNodeScalarValues();
    }
    catch (...)
    {
        inprocess = false;
        throw;
    }
    uptodate = true;
    inprocess = false;
  }

  if (FPECheck::CheckFPE())
  {
    std::ostringstream os;
    os << "There was a floating point exception of type \"" << FPECheck::getFPEString() << "\"  while evaluating the node model " << name
    << " on Device: " << GetRegion().GetDevice()->GetName() << " on Region: " << GetRegion().GetName() << "\n";
    FPECheck::ClearFPE();
    GeometryStream::WriteOut(OutputStream::OutputType::FATAL, GetRegion(), os.str().c_str());
  }
}

//
// If not uptodate, then recalculate the model
template <typename DoubleType>
const NodeScalarList<DoubleType> & NodeModel::GetScalarValues() const
{
  CalculateValues();

  model_data.expand_uniform();

  return model_data.GetValues<DoubleType>();
}

// Note this is logically const and internal to the program
// be careful this is duplicated as a non-const below
template <typename DoubleType>
void NodeModel::SetValues(const NodeScalarList<DoubleType> &nv) const
{
  const_cast<NodeModel *>(this)->SetValues(nv);
}

// be careful this is duplicated as a const above
template <typename DoubleType>
void NodeModel::SetValues(const NodeScalarList<DoubleType> &nv)
{
  if (mycontact)
  {
    GetContactIndexes(); // safety
    model_data.set_indexes(atcontact, nv);
  }
  else
  {
    model_data.set_values(nv);
  }

  MarkOld();
  uptodate = true;
}

template <typename DoubleType>
void NodeModel::SetValues(const DoubleType &v) const
{
  const_cast<NodeModel *>(this)->SetValues(v);
}

template <typename DoubleType>
void NodeModel::SetValues(const DoubleType &v)
{
  if (mycontact)
  {
    GetContactIndexes(); // safety
    model_data.set_indexes(atcontact, v);
  }
  else
  {
    model_data.SetUniformValue<DoubleType>(v);
  }

  MarkOld();
  uptodate = true;
}

const std::vector<size_t> & NodeModel::GetContactIndexes() const
{
  if ((mycontact) && (atcontact.empty()))
  {
    const ConstNodeList_t &cnodes = mycontact->GetNodes();
    for (ConstNodeList_t::const_iterator it = cnodes.begin(); it != cnodes.end(); ++it)
    {
      atcontact.push_back((*it)->GetIndex());
    }
    std::sort(atcontact.begin(), atcontact.end());
  }

  return atcontact;
}

/*
  if uniform desired then use SetNodeValues above
*/
template <typename DoubleType>
void NodeModel::SetNodeValue(size_t index, DoubleType value)
{
  if (index >= model_data.GetLength())
  {
    return;
  }

  //// Set values
  //// unsets uniformity
  GetScalarValues<DoubleType>();

  if (mycontact)
  {
    GetContactIndexes(); // safety
    model_data.set_indexes(atcontact, value);
  }
  else
  {
    model_data.SetValue(index, value);
  }

  MarkOld();
  uptodate = true;

}

void NodeModel::MarkOld()
{
  uptodate = false;
  myregion->SignalCallbacks(name);
}

void NodeModel::MarkOld() const
{
  const_cast<NodeModel *>(this)->MarkOld();
}

void NodeModel::RegisterCallback(const std::string &nm)
{
  myregion->RegisterCallback(name, nm);
}

bool NodeModel::IsUniform() const
{
  //// We cannot know if we are uniform unless we are uptodate!
  if (!uptodate)
  {
    CalculateValues();
  }

  return model_data.IsUniform();
}

template <typename DoubleType>
const DoubleType &NodeModel::GetUniformValue() const
{
  return model_data.GetUniformValue<DoubleType>();
}

void NodeModel::SerializeBuiltIn(std::ostream &of) const
{
  of << "BUILTIN";
}

void NodeModel::DevsimSerialize(std::ostream &of) const
{
  of << "begin_node_model \"" << this->GetName() << "\"\n";
  this->Serialize(of);
  of << "\nend_node_model\n\n";
}

void NodeModel::SetContact(const ContactPtr cp)
{
  mycontact = cp;
  atcontact.clear();
  uptodate = false;
}

const std::string &NodeModel::GetDeviceName() const
{
  return GetRegion().GetDevice()->GetName();
}

const std::string &NodeModel::GetRegionName() const
{
  return GetRegion().GetName();
}

const std::string NodeModel::GetContactName() const

{
  std::string ret;
  if (mycontact)
  {
  ret = mycontact->GetName();
  }
  return ret;
}

#define DBLTYPE double
#include "NodeModelInstantiate.cc"

#ifdef DEVSIM_EXTENDED_PRECISION
#undef  DBLTYPE
#define DBLTYPE float128
#include "Float128.hh"
#include "NodeModelInstantiate.cc"
#endif

