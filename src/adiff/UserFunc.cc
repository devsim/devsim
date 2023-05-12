/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "UserFunc.hh"
#include "UserFunc.hh"

#include "dsAssert.hh"

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

namespace Eqo {

std::map<std::string, UserDiffInfoVec> UserFuncMap;

// Used for implicitly declared functions
void CreateUserFunc(std::string nm, size_t nar)
{
    dsAssert(UserFuncMap.count(nm) == 0, "UNEXPECTED");
    UserFuncMap[nm].resize(nar);
}

void SetUserFuncDiffs(std::string nm, UserDiffInfoVec &dif)
{
    // cannot set diffs if they are already set
    // should actually print error
    if (UserFuncMap.count(nm))
    {
        // \todo error handling
        // must be the same exact size
        dsAssert(dif.size() == UserFuncMap[nm].size(), "UNEXPECTED");
    }
    dsAssert(dif.size() > 0, "UNEXPECTED");

    UserFuncMap[nm] = dif; // assume that I own this data
#if 0
    UserDiffInfoVec f;
    for (size_t i = 0; i < dif.size(); ++i)
    {
        // clone may not be necessary
        f.push_back(std::make_pair(dif[i].first, dif[i].second->clone()));
    }
    UserFuncMap[nm] = f;
#endif
}

UserFunc::UserFunc(std::string nm, std::vector<EqObjPtr> &a) : EquationObject(USERFUNC_OBJ), name(nm)
{
    if (UserFuncMap.count(nm))
    {
        // \todo error handling
        dsAssert(a.size() == UserFuncMap[nm].size(), "UNEXPECTED");
    }
    else
    {
        UserFuncMap[nm].resize(a.size());
    }

    args = a; // assume that I own this data
}

// This is messed up
std::string UserFunc::createStringValue()
{
   const size_t len = args.size();

   // \todo need to set special error handling

   dsAssert(len > 0, "UNEXPECTED");
   std::ostringstream os;
   os << name << "(";
   for (size_t i=0; i < len; ++i)
   {
       if (i != 0)
            os << ", ";
       os << args[i];
   }
   os << ")";
    return os.str();
}

// I'm substituting in for variable names
EqObjPtr UserFunc::subst(const std::string &str, EqObjPtr eqo)
{
    if (str == this->stringValue())
        return eqo;
    else
    {
        std::vector<EqObjPtr> new_args(args.size());
        for (size_t i = 0; i < args.size(); ++i)
        {
            new_args[i] = args[i]->subst(str, eqo);
        }
        return EqObjPtr(new UserFunc(name, new_args));
    }
}

//// unfinished
// \todo optimize
EqObjPtr UserFunc::Derivative(EqObjPtr foo)
{
    // for now assume defined
    dsAssert(UserFuncMap.count(name) != 0, "UNEXPECTED");
    dsAssert(args.size() == UserFuncMap[name].size(), "UNEXPECTED");

    std::vector<EqObjPtr> der;
    for (size_t i = 0; i < args.size(); ++i)
    {
        Eqo::EqObjPtr y = args[i]->Derivative(foo);
        // clean up and use const references
        EqObjPtr x;
        if (UserFuncMap[name][i].second)
        {
            x = UserFuncMap[name][i].second;
        }
        else
        {
            x = con(0);
        }
        // substitute in all of our arguments
        for (size_t j = 0; j < args.size(); ++j)
        {
            const std::string &var = UserFuncMap[name][j].first;
            x = x->subst(var, args[j]);
        }
        // Bring this test out of the for loop
        if (y->isZero()) // don't want redundant ddx = 1
        {
            x = con(0); // chain rule
        }
        else if (!y->isOne())
        {
            x = x * y;
        }
        der.push_back(x);
    }
    dsAssert(der.size() != 0, "UNEXPECTED");
    if (der.size() == 1)
        return der[0];
    else
        return EqObjPtr(new Add(der));
}

EqObjPtr UserFunc::Simplify()
{
    std::vector<EqObjPtr> vals;
    vals.reserve(args.size());
    for (size_t i = 0; i < args.size(); ++i)
    {
        vals.push_back(args[i]->Simplify());
    }
    return (EqObjPtr(new UserFunc(name, vals)));
}

bool UserFunc::isZero()
{
    return false;
}

bool UserFunc::isOne()
{
    return false;
}

EqObjPtr UserFunc::clone()
{
    const size_t len = args.size();
    dsAssert(len != 0, "UNEXPECTED");
    std::vector<EqObjPtr> tmp(len);
    for (size_t i = 0; i < len; ++i)
    {
        tmp[i] = args[i]->clone();
    }
    EquationObject *n = new UserFunc(name, tmp);
    return EqObjPtr(n);
}

EqObjPtr UserFunc::getScale()
{
   return EqObjPtr(new Constant(1.0));
}

EqObjPtr UserFunc::getUnscaledValue()
{
   return EqObjPtr(new UserFunc(name, args));
}

double UserFunc::getSign()
{
    return 1.0;
}

EqObjPtr UserFunc::getUnsignedValue()
{
   return EqObjPtr(new UserFunc(name, args));
}

EqObjPtr UserFunc::CombineAdd(std::vector<EqObjPtr> y)
{
   if (y.empty())
      return (EqObjPtr(new UserFunc(name, args)));

   y.push_back(EqObjPtr(new UserFunc(name, args)));

   if (y.size() == 1)
       return y[0];
   else
       return (EqObjPtr(new Add(y)));
}

EqObjPtr UserFunc::CombineProduct(std::vector<EqObjPtr> y)
{
   if (y.empty())
      return EqObjPtr(new UserFunc(name, args));

   y.push_back(EqObjPtr(new UserFunc(name, args)));

   if (y.size() == 1)
       return y[0];
   else
       return (EqObjPtr(new Product(y)));
}

EqObjPtr UserFunc::expand()
{
    const size_t len = args.size();

    std::vector<EqObjPtr> tmp(len);
    for (size_t i = 0; i < len; ++i)
    {
        tmp[i] = args[i]->expand();
    }
    EquationObject *n = new UserFunc(name, tmp);
    return EqObjPtr(n);
}

std::set<std::string> UserFunc::getReferencedType(Eqo::EqObjType rt)
{
    return UniteReferencedType(rt, args);
}

}// end namespace

