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

