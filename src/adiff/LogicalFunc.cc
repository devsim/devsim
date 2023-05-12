/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "LogicalFunc.hh"
#include <string>
#include <sstream>

namespace Eqo {
/**
 * this will most likely always take "!"
 */
UnaryLogical::UnaryLogical(const std::string &op, EqObjPtr x) : EquationObject(ULOGICAL_OBJ), unaryOperator(op), arg(x)
{
}

std::string UnaryLogical::createStringValue()
{
    std::ostringstream os;
    os << "("
         << unaryOperator
         << arg->stringValue() << ")";
    return os.str();
}

/**
 * Logical operators should always return 0
 */
EqObjPtr UnaryLogical::Derivative(EqObjPtr)
{
    return con(0.0);
}

/**
* a !! should cancel things out
* work this out later
* this should be invariant in a logical
*/
EqObjPtr UnaryLogical::Simplify()
{
    return shared_from_this();
}

/// let it just be what it was
EqObjPtr UnaryLogical::CombineProduct(std::vector<EqObjPtr> y)
{
   if (y.empty())
      return (shared_from_this());

   y.push_back(shared_from_this());
   return EqObjPtr(new Product(y));
}

EqObjPtr UnaryLogical::CombineAdd(std::vector<EqObjPtr> y)
{
   if (y.empty())
      return (shared_from_this());

   y.push_back(shared_from_this());
   return EqObjPtr(new Add(y));
}

EqObjPtr UnaryLogical::clone()
{
    return EqObjPtr(new UnaryLogical(unaryOperator, arg->clone()));
}

bool UnaryLogical::isZero()
{
    return false;
}

bool UnaryLogical::isOne()
{
    return false;
}

EqObjPtr UnaryLogical::getUnscaledValue()
{
   return shared_from_this();
}

double UnaryLogical::getSign()
{
    return 1.0;
}

EqObjPtr UnaryLogical::getUnsignedValue()
{
   return shared_from_this();
}

EqObjPtr UnaryLogical::expand()
{
   return shared_from_this();
}

EqObjPtr UnaryLogical::getScale()
{
   return con(1.0);
}

EqObjPtr UnaryLogical::subst(const std::string &str, EqObjPtr eqo)
{
  if (str == this->stringValue())
  {
    return eqo->clone();
  }
  else
  {
    return EqObjPtr(new UnaryLogical(unaryOperator, arg->subst(str, eqo)));
  }
}

BinaryLogical::BinaryLogical(const std::string &op, EqObjPtr x, EqObjPtr y) : EquationObject(BLOGICAL_OBJ), binaryOperator(op), arg1(x), arg2(y)
{
}

std::string BinaryLogical::createStringValue()
{
    std::ostringstream os;
    os << "(" << arg1->stringValue()
         << " " << binaryOperator << " "
         << arg2->stringValue() << ")";
    return os.str();
}

/**
 * Logical operators should always return 0
 */
EqObjPtr BinaryLogical::Derivative(EqObjPtr)
{
    return con(0.0);
}

/**
* a !! should cancel things out
* work this out later
* this should be invariant in a logical
*/
EqObjPtr BinaryLogical::Simplify()
{
    return shared_from_this();
}

/// let it just be what it was
EqObjPtr BinaryLogical::CombineProduct(std::vector<EqObjPtr> y)
{
   if (y.empty())
      return (shared_from_this());

   y.push_back(shared_from_this());
   return EqObjPtr(new Product(y));
}

EqObjPtr BinaryLogical::CombineAdd(std::vector<EqObjPtr> y)
{
   if (y.empty())
      return (shared_from_this());

   y.push_back(shared_from_this());
   return EqObjPtr(new Add(y));
}

EqObjPtr BinaryLogical::clone()
{
    return EqObjPtr(new BinaryLogical(binaryOperator, arg1->clone(), arg2->clone()));
}

bool BinaryLogical::isZero()
{
    return false;
}

bool BinaryLogical::isOne()
{
    return false;
}

EqObjPtr BinaryLogical::getUnscaledValue()
{
   return shared_from_this();
}

double BinaryLogical::getSign()
{
    return 1.0;
}

EqObjPtr BinaryLogical::getUnsignedValue()
{
   return shared_from_this();
}

EqObjPtr BinaryLogical::expand()
{
   return shared_from_this();
}

EqObjPtr BinaryLogical::getScale()
{
   return con(1.0);
}

EqObjPtr BinaryLogical::subst(const std::string &str, EqObjPtr eqo)
{
  if (str == this->stringValue())
  {
    return eqo->clone();
  }
  else
  {
    return EqObjPtr(new BinaryLogical(binaryOperator, arg1->subst(str,eqo), arg2->subst(str,eqo)));
  }
}

}

