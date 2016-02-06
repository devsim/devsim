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

#include "InterfaceModelExprData.hh"
#include "InterfaceModelExprEval.hh"
#include "InterfaceNodeScalarData.hh"
#include "dsAssert.hh"
#include "Interface.hh"
#include "InterfaceNodeModel.hh"
#include "NodeModel.hh"
#include "Node.hh"
#include "Region.hh"

#include <sstream>


namespace IMEE {

const char * const InterfaceModelExprData::datatypename[] = {
    "nodedata",
    "double",
    "invalid"
};

/// Undefined behavior if we have a double or invalid data
InterfaceModelExprData::ScalarValuesType InterfaceModelExprData::GetScalarValues() const
{
  ScalarValuesType ret;

  if (type == NODEDATA)
  {
    if (nsd->IsUniform())
    {
      ret = ScalarValuesType(nsd->GetUniformValue(), nsd->GetLength());
    }
    else
    {
      ret = ScalarValuesType(nsd->GetScalarList());
    }
  }
  else
  {
    dsAssert(false, "UNEXPECTED");
  }
  return ret;
}

//// ERROR need to make full copy of vector data
InterfaceModelExprData::InterfaceModelExprData(const InterfaceModelExprData &x) : nsd(x.nsd), val(x.val), type(x.type)
{
}

InterfaceModelExprData::InterfaceModelExprData(const InterfaceNodeScalarData &x) : val(0.0), type(NODEDATA) {
    InterfaceNodeScalarData *foo = new InterfaceNodeScalarData(x);
    nsd = std::shared_ptr<InterfaceNodeScalarData>(foo);
};

InterfaceModelExprData &InterfaceModelExprData::operator=(const InterfaceModelExprData &x)
{
    // Since this class is not derived, we don't have to copy the base part
    if (&x != this)
    {
        nsd  = x.nsd;
        val  = x.val;
        type = x.type;
    }
    return *this;
}


void InterfaceModelExprData::makeUnique()
{
  if (nsd && (!nsd.unique()))
  {
    nsd = nsd_ptr(new InterfaceNodeScalarData(*nsd));
  }
}


template<typename T>
InterfaceModelExprData &InterfaceModelExprData::op_equal(const InterfaceModelExprData &other, const T &func)
{
  makeUnique();

  /// We were converted automatically above
  if (type == NODEDATA)
  {
    if (other.type == NODEDATA)
    {
      nsd->op_equal(*other.nsd, func);
    }
    else if (other.type == DOUBLE)
    {
      nsd->op_equal(other.val, func);
    }
    else
    {
      type = INVALID;
    }
  }
  else if (type == DOUBLE)
  {
    if (other.type == NODEDATA)
    {
      InterfaceNodeScalarData *x = new InterfaceNodeScalarData(val, other.nsd->GetLength()); 
      nsd = nsd_ptr(x);
      nsd->op_equal(*other.nsd, func);
      type = NODEDATA;
    }
    else if (other.type == DOUBLE)
    {
      func(val, other.val);
    }
    else
    {
      type = INVALID;
    }
  }

  return *this;
}

//// yes += is a copy and past job of this
//// Make sure they are changed together
//// In the future, we will just use function pointers so that the work
//// isn't duplicated
InterfaceModelExprData &InterfaceModelExprData::operator*=(const InterfaceModelExprData &other)
{
  this->op_equal(other, ScalarDataHelper::times_equal());
  return *this;
}

//// yes *= is a copy and past job of this
//// Make sure they are changed together
InterfaceModelExprData &InterfaceModelExprData::operator+=(const InterfaceModelExprData &other)
{
  this->op_equal(other, ScalarDataHelper::plus_equal());
  return *this;
}


}

