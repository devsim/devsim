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
