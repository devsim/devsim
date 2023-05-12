/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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

const char * const datatypename[] = {
    "nodedata",
    "double",
    "invalid"
};

/// Undefined behavior if we have a double or invalid data
template <typename DoubleType>
ScalarValuesType<DoubleType> InterfaceModelExprData<DoubleType>::GetScalarValues() const
{
  ScalarValuesType<DoubleType> ret;

  if (type == datatype::NODEDATA)
  {
    if (nsd->IsUniform())
    {
      ret = ScalarValuesType<DoubleType>(nsd->GetUniformValue(), nsd->GetLength());
    }
    else
    {
      ret = ScalarValuesType<DoubleType>(nsd->GetScalarList());
    }
  }
  else
  {
    dsAssert(false, "UNEXPECTED");
  }
  return ret;
}

//// ERROR need to make full copy of vector data
template <typename DoubleType>
InterfaceModelExprData<DoubleType>::InterfaceModelExprData(const InterfaceModelExprData<DoubleType> &x) : nsd(x.nsd), val(x.val), type(x.type)
{
}

template <typename DoubleType>
InterfaceModelExprData<DoubleType>::InterfaceModelExprData(const InterfaceNodeScalarData<DoubleType> &x) : val(0.0), type(datatype::NODEDATA) {
    InterfaceNodeScalarData<DoubleType> *foo = new InterfaceNodeScalarData<DoubleType>(x);
    nsd = std::shared_ptr<InterfaceNodeScalarData<DoubleType>>(foo);
};

template <typename DoubleType>
InterfaceModelExprData<DoubleType> &InterfaceModelExprData<DoubleType>::operator=(const InterfaceModelExprData<DoubleType> &x)
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

template <typename DoubleType>
void InterfaceModelExprData<DoubleType>::makeUnique()
{
  if (nsd && (!nsd.unique()))
  {
    nsd = nsd_ptr(new InterfaceNodeScalarData<DoubleType>(*nsd));
  }
}


template<typename DoubleType> template<typename T>
InterfaceModelExprData<DoubleType> &InterfaceModelExprData<DoubleType>::op_equal(const InterfaceModelExprData<DoubleType> &other, const T &func)
{
  makeUnique();

  /// We were converted automatically above
  if (type == datatype::NODEDATA)
  {
    if (other.type == datatype::NODEDATA)
    {
      nsd->op_equal_data(*other.nsd, func);
    }
    else if (other.type == datatype::DOUBLE)
    {
      nsd->op_equal_scalar(other.val, func);
    }
    else
    {
      type = datatype::INVALID;
    }
  }
  else if (type == datatype::DOUBLE)
  {
    if (other.type == datatype::NODEDATA)
    {
      InterfaceNodeScalarData<DoubleType> *x = new InterfaceNodeScalarData<DoubleType>(val, other.nsd->GetLength());
      nsd = nsd_ptr(x);
      nsd->op_equal_data(*other.nsd, func);
      type = datatype::NODEDATA;
    }
    else if (other.type == datatype::DOUBLE)
    {
      func(val, other.val);
    }
    else
    {
      type = datatype::INVALID;
    }
  }

  return *this;
}

//// yes += is a copy and past job of this
//// Make sure they are changed together
//// In the future, we will just use function pointers so that the work
//// isn't duplicated
template <typename DoubleType>
InterfaceModelExprData<DoubleType> &InterfaceModelExprData<DoubleType>::operator*=(const InterfaceModelExprData<DoubleType> &other)
{
  this->op_equal(other, ScalarDataHelper::times_equal<DoubleType>());
  return *this;
}

//// yes *= is a copy and past job of this
//// Make sure they are changed together
template <typename DoubleType>
InterfaceModelExprData<DoubleType> &InterfaceModelExprData<DoubleType>::operator+=(const InterfaceModelExprData<DoubleType> &other)
{
  this->op_equal(other, ScalarDataHelper::plus_equal<DoubleType>());
  return *this;
}

//// Manual Template Instantiation
template class InterfaceModelExprData<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class InterfaceModelExprData<float128>;
#endif
}

