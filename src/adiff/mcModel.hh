/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef MC_MODEL_HH
#define MC_MODEL_HH
#include "EquationObject.hh"
namespace Eqo {
class Model : public EquationObject {
    public:

        Model(std::string);
        ~Model() {};
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
          return createArgs(this->clone());
        }

        bool hasReciprocal() {return false;}
        EqObjPtr getReciprocal() {dsAssert(0, "UNEXPECTED"); return con(0);}

        std::set<std::string> getReferencedType(Eqo::EqObjType rt) {
            return CreateSetIfThisType(rt);
        }

    private:
        std::string createStringValue();

        Model(const Model &);
        Model operator=(const Model &);

        //Models are stored elsewhere
        const std::string value;
};

inline EqObjPtr mod(const std::string &x)
{
   return EqObjPtr(new Model(std::string(x)));
}
}
#endif
