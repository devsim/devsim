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

// derived classes must register dependencies
InterfaceNodeModel::InterfaceNodeModel(const std::string &nm, const InterfacePtr ip)
    : name(nm),
      myinterface(ip),
      uptodate(false),
      inprocess(false),
      isuniform(false),
      uniform_value(0.0)
{ 
  ip->AddInterfaceNodeModel(this);
  length = GetInterface().GetNodes0().size();
}

void InterfaceNodeModel::CalculateValues() const
{
  FPECheck::ClearFPE();
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

  if (FPECheck::CheckFPE())
  {
    std::ostringstream os;
    const std::string &dname = GetInterface().GetRegion0()->GetDevice()->GetName();
    os << "There was a floating point exception while evaluating the interface node model " << name
    << " on Device: " << dname << " on Interface: " << GetInterface().GetName() << "\n";
    FPECheck::ClearFPE();
    GeometryStream::WriteOut(OutputStream::FATAL, GetInterface(), os.str().c_str());
  }
}

const NodeScalarList &InterfaceNodeModel::GetScalarValues() const
{
  CalculateValues();

  if (isuniform)
  {
    values.clear();
    values.resize(length, uniform_value);
  }

  return values;
}

void InterfaceNodeModel::SetValues(const double &v) const
{
  values.clear();
  isuniform = true;
  uniform_value = v;

  MarkOld();
  uptodate = true;
}

void InterfaceNodeModel::SetValues(const NodeScalarList &nv) const
{
  const_cast<InterfaceNodeModel *>(this)->SetValues(nv);
}

void InterfaceNodeModel::SetValues(const NodeScalarList &nv)
{
  values = nv;

  isuniform = false;
  uniform_value = 0.0;

  MarkOld();
  uptodate = true;
}

void InterfaceNodeModel::SetValues(const InterfaceNodeModel &nm)
{
  if (&nm != this)
  {
    if (nm.IsUniform())
    {
      const double v = nm.GetUniformValue();
      InterfaceNodeModel::SetValues(v);
    }
    else
    {
      const NodeScalarList &v = nm.GetScalarValues();
      InterfaceNodeModel::SetValues(v);
    }
  }
}


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

  return isuniform;
}

double InterfaceNodeModel::GetUniformValue() const
{
  dsAssert(isuniform, "UNEXPECTED");
  return uniform_value;
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

