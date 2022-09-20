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

#ifndef LOGICALFUNC_HH
#define LOGICALFUNC_HH
#include "EquationObject.hh"

/**
 * Here we will define two types of functions for logicals
 * 1.  Unary !
 * 2.  Binary != == < > <= >= && ||
 *
 * Derivatives are assumed to be zero
 */
namespace Eqo {
class UnaryLogical : public EquationObject
{
    public:
        UnaryLogical(const std::string &/*unaryOp*/, EqObjPtr /*arg*/);
        ~UnaryLogical() {};

        EqObjPtr Derivative(EqObjPtr);
        EqObjPtr Simplify();

        EqObjPtr CombineProduct(std::vector<EqObjPtr>);
        EqObjPtr CombineAdd(std::vector<EqObjPtr>);

        bool isZero();
        bool isOne();

        EqObjPtr getScale();
        EqObjPtr getUnscaledValue();

        double getSign();
        EqObjPtr getUnsignedValue();

        EqObjPtr clone();
        EqObjPtr subst(const std::string &, EqObjPtr);

        EqObjPtr expand();

        std::vector<EqObjPtr> getArgs() {
          return createArgs(arg);
        }

        const std::string &getOperator()
        {
          return unaryOperator;
        }

        bool hasReciprocal() {return false;}
        EqObjPtr getReciprocal() {dsAssert(0, "UNEXPECTED"); return con(0);}

        std::set<std::string> getReferencedType(Eqo::EqObjType rt) {
            return arg->getReferencedType(rt);
        }

    private:
        std::string createStringValue();

        UnaryLogical(const UnaryLogical &);
        UnaryLogical operator=(const UnaryLogical &);

        const std::string unaryOperator;
        const EqObjPtr arg;
};

class BinaryLogical : public EquationObject
{
    public:
        BinaryLogical(const std::string & /*op*/, EqObjPtr /*left*/, EqObjPtr /*right*/);
        ~BinaryLogical() {};

        EqObjPtr Derivative(EqObjPtr);
        EqObjPtr Simplify();

        EqObjPtr CombineProduct(std::vector<EqObjPtr>);
        EqObjPtr CombineAdd(std::vector<EqObjPtr>);

        bool isZero();
        bool isOne();

        EqObjPtr getScale();
        EqObjPtr getUnscaledValue();

        double getSign();
        EqObjPtr getUnsignedValue();

        EqObjPtr clone();
        EqObjPtr subst(const std::string &, EqObjPtr);

        EqObjPtr expand();

        std::vector<EqObjPtr> getArgs() {
          return createArgs(arg1, arg2);
        }

        const std::string &getOperator()
        {
          return binaryOperator;
        }

        bool hasReciprocal() {return false;}
        EqObjPtr getReciprocal() {dsAssert(0, "UNEXPECTED"); return con(0);}

        std::set<std::string> getReferencedType(Eqo::EqObjType rt) {
            return UniteReferencedType(rt, arg1, arg2);
        }

    private:
        std::string createStringValue();

        BinaryLogical(const BinaryLogical &);
        BinaryLogical operator=(const BinaryLogical &);

        const std::string binaryOperator;
        EqObjPtr arg1;
        EqObjPtr arg2;
};
}

#endif
