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

#include "mcModel.hh"
#include "mcModelCompiler.hh"
#include "dsAssert.hh"

#include <iostream>
namespace Eqo {
EqObjPtr Model::getScale()
{
   return con(1.0);
}

EqObjPtr Model::getUnscaledValue()
{
   return shared_from_this();
}

double Model::getSign()
{
    return 1.0;
}

EqObjPtr Model::getUnsignedValue()
{
   return shared_from_this();
}

EqObjPtr Model::expand()
{
   return shared_from_this();
}

EqObjPtr Model::clone()
{
   return mod(value);
}

EqObjPtr Model::subst(const std::string &str, EqObjPtr eqo)
{
    if (str == this->stringValue())
        return eqo;
    else
        return shared_from_this();
}

Model::Model(std::string var) : EquationObject(MODEL_OBJ), value(var)
{
}

std::string Model::createStringValue()
{
    return value;
}

/// Should make it possible to specify derivative
EqObjPtr Model::Derivative(EqObjPtr foo)
{
    dsAssert(isInModelList(value), "UNEXPECTED");

    if (foo->stringValue() == value)
    {
        return EqObjPtr(new Constant(1.0));
    }

    if (findInModelList(value)->Derivative(foo)->Simplify()->isZero())
    {
        return EqObjPtr(new Constant(0.0));
    }

    std::string bar("d_");
    bar += value;
    bar += "_d_";
    bar += foo->stringValue();

    if (!isInModelList(bar))
    {
       std::cerr << "Creating derivative " << bar << std::endl;

       ModelList.push_back(make_pair(bar, findInModelList(value)->Derivative(foo)->Simplify()));
    }
    else
       std::cerr << "Reusing derivative " << bar << std::endl;

    return mod(bar);
}

EqObjPtr Model::Simplify()
{
   return shared_from_this();
}

/**
 * This function may never be called if variables
 * are turned into Pow's.
 */
EqObjPtr Model::CombineProduct(std::vector<EqObjPtr> y)
{
   if (y.empty())
      return shared_from_this();

   y.push_back(shared_from_this());
   return EqObjPtr(new Product(y));
}

EqObjPtr Model::CombineAdd(std::vector<EqObjPtr> y)
{
   if (y.empty())
      return shared_from_this();

   y.push_back(shared_from_this());
   return EqObjPtr(new Add(y));
}

bool Model::isZero()
{
   return false;
}

bool Model::isOne()
{
   return false;
}

}

