/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "InterfaceNodeModel.hh"
#include "Interface.hh"
#include "Node.hh"
#include "Device.hh"
#include "Region.hh"
#include "dsAssert.hh"
#include "FPECheck.hh"
#include "GeometryStream.hh"



InterfaceNodeModel::~InterfaceNodeModel() {
#if 0
  myinterface->UnregisterCallback(name);
#endif
}

bool InterfaceNodeModel::IsZero() const
{
  CalculateValues();
  return model_data.IsUniform() && model_data.IsZero();
}

bool InterfaceNodeModel::IsOne() const
{
  CalculateValues();
  return model_data.IsUniform() && model_data.IsOne();
}

// derived classes must register dependencies
InterfaceNodeModel::InterfaceNodeModel(const std::string &nm, const InterfacePtr ip)
    : name(nm),
      myinterface(ip),
      model_data(ip->GetNodes0().size())
{
  ip->AddInterfaceNodeModel(this);
}

void InterfaceNodeModel::CalculateValues() const
{
  FPECheck::ClearFPE();

  // TODO: fix this so the values are actually cached
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
  inprocess = false;
#if 0
  // this is the code before we tried to recover from FATAL model evaluation errors
  this->calcNodeScalarValues();
  if (!uptodate)
  {
    inprocess = true;
    this->calcNodeScalarValues();
    uptodate = true;
    inprocess = false;
  }
  else
  {
  }
#endif

  if (FPECheck::CheckFPE())
  {
    std::ostringstream os;
    const std::string &dname = GetInterface().GetRegion0()->GetDevice()->GetName();
    os << "There was a floating point exception while evaluating the interface node model " << name
    << " on Device: " << dname << " on Interface: " << GetInterface().GetName() << "\n";
    FPECheck::ClearFPE();
    GeometryStream::WriteOut(OutputStream::OutputType::FATAL, GetInterface(), os.str().c_str());
  }
}

template <typename DoubleType>
const NodeScalarList<DoubleType> &InterfaceNodeModel::GetScalarValues() const
{
  CalculateValues();

  model_data.expand_uniform();

  return model_data.GetValues<DoubleType>();
}

template <typename DoubleType>
void InterfaceNodeModel::SetValues(const DoubleType &v) const
{
  model_data.SetUniformValue<DoubleType>(v);

  MarkOld();
  uptodate = true;
}

template <typename DoubleType>
void InterfaceNodeModel::SetValues(const NodeScalarList<DoubleType> &nv) const
{
  const_cast<InterfaceNodeModel *>(this)->SetValues(nv);
}

template <typename DoubleType>
void InterfaceNodeModel::SetValues(const NodeScalarList<DoubleType> &nv)
{
  model_data.set_values(nv);

  MarkOld();
  uptodate = true;
}

#if 0
template <typename DoubleType>
void InterfaceNodeModel::SetValues(const InterfaceNodeModel &nm)
{
  if (&nm != this)
  {
    if (nm.IsUniform())
    {
      const DoubleType v = nm.GetUniformValue<double>();
      InterfaceNodeModel::SetValues(v);
    }
    else
    {
      const NodeScalarList<DoubleType> &v = nm.GetScalarValues<double>();
      InterfaceNodeModel::SetValues(v);
    }
  }
}
#endif


void InterfaceNodeModel::MarkOld()
{
    uptodate = false;
    myinterface->SignalCallbacks(name);
}

void InterfaceNodeModel::MarkOld() const
{
  const_cast<InterfaceNodeModel *>(this)->MarkOld();
}

void InterfaceNodeModel::RegisterCallback(const std::string &nm)
{
    myinterface->RegisterCallback(name, nm);
}

bool InterfaceNodeModel::IsUniform() const
{
  //// We cannot know if we are uniform unless we are uptodate!
  if (!uptodate)
  {
    CalculateValues();
  }

  return model_data.IsUniform();
}

template <typename DoubleType>
const DoubleType &InterfaceNodeModel::GetUniformValue() const
{
  return model_data.GetUniformValue<DoubleType>();
}

void InterfaceNodeModel::DevsimSerialize(std::ostream &of) const
{
  of << "begin_interface_node_model \"" << this->GetName() << "\"\n";
  this->Serialize(of);
  of << "\nend_interface_node_model\n\n";
}

const std::string &InterfaceNodeModel::GetDeviceName() const
{
  return myinterface->GetRegion0()->GetDevice()->GetName();
}

const std::string &InterfaceNodeModel::GetInterfaceName() const
{
  return myinterface->GetName();
}

#define DBLTYPE double
#include "InterfaceNodeModelInstantiate.cc"

#ifdef DEVSIM_EXTENDED_PRECISION
#undef  DBLTYPE
#define DBLTYPE float128
#include "Float128.hh"
#include "InterfaceNodeModelInstantiate.cc"
#endif

