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


#include "EquationObject.hh"
#include "dsAssert.hh"
#include <memory>
#include <string>
#include <sstream>
#include <iostream>
#include <typeinfo>
#include <cmath>
#include <utility>
#include <algorithm>
#include <map>
#include <iomanip>


namespace Eqo {
typedef std::shared_ptr<EquationObject> EqObjPtr;

std::map<std::string, EqObjPtr> variableMap;

UnaryTblEntry UnaryTable[] = {
    {"simplify",    Simplify,         "simplify(obj)         -- Simplify as much as possible"},
    {"exp",         exp,              "exp(obj)              -- Exponentiation with respect to e"},
    {"expand",        Expand,          "expand(obj)          -- Perform expansion of products with sums"},
    {"log",         log,              "log(obj)              -- Natural logarithm"},
    {"unscaledval", getUnscaledValue, "unscaledval(obj)      -- Get value without constant scaling"},
    {"scale",       getConstantFactor,"scale(obj)            -- Get constant factor"},
    {"unsignedval", getUnsignedValue, "unsignedval(obj)      -- Get unsigned value"},
    {"sign",        getSign,          "sign(obj)             -- Get sign as 1 or -1"},
    {nullptr, nullptr, nullptr}
};

BinaryTblEntry BinaryTable[] = {
    {"pow",  pow,  "pow(obj1, obj2)       -- Raise obj1 to the power of obj2"},
    {"diff", diff, "diff(obj1, obj2)      -- Take derivative  of obj1 with respect to obj2"},
    {nullptr, nullptr, nullptr}
};

EqUnaryFuncPtr getUnaryFuncPtr(std::string x) {
    size_t i=0; 
    while (UnaryTable[i].name != nullptr)
    {
        if (x == UnaryTable[i].name)
            return UnaryTable[i].func;
        ++i;
    }
    return nullptr;
}

EqBinaryFuncPtr getBinaryFuncPtr(std::string x) {
    size_t i=0; 
    while (BinaryTable[i].name != nullptr)
    {
        if (x == BinaryTable[i].name)
            return BinaryTable[i].func;
        ++i;
    }
    return nullptr;
}

#if 0
EqObjPtr printHelp() {
    size_t i=0; 
    cerr << "\n";
    cerr << "Functions:\n"
            "--------------------------------------------------\n";
    while (UnaryTable[i].name != nullptr)
    {
        cerr << UnaryTable[i].desc << "\n";
        ++i;
    }
    i=0; 
    while (BinaryTable[i].name != nullptr)
    {
        cerr << BinaryTable[i].desc << "\n";
        ++i;
    }

    // Really need to create a Ternary table
    cerr << "subst(obj1,obj2,obj3)            -- substitute obj3 for obj2 into obj1\n";
    cerr << "declare(name(arg1, arg2, . . .)) -- declare function name taking dummy arguments\n"
            "                                    arg1, arg2, . . . .  Derivatives assumed to be 0\n";
    cerr << "define(name(arg1, arg2, . . .), obj1 , obj2, . . .) -- declare function name taking arguments\n"
            "                                    arg1, arg2, . . . having corresponding derivatives obj1, obj2, . . .\n";


    cerr << "\nAlgebraic Operations: (high to low precedence)\n"
            "--------------------------------------------------\n"
            "()     Parenthesis for setting precedence or enclosing function arguments\n"
            "-+     unary minus/plus\n"
            "^      Exponentiation\n"
            "*/     Multiply/Divide\n"
            "+-     Addition/Subtraction\n"
            "=      Macro assignment\n";
            ;

    cerr << "\nObjects:\n"
            "--------------------------------------------------\n"
            "Objects consist of any combination of functions and operations on the\n"
            "following types of objects:\n"
            "numbers   -- integer of floating point including scientific e notation\n"
            "variables -- string beginning with a letter or number with any combination\n"
            "             of letters, numbers and \"_\" characters\n"
            "macros    -- name given to an expression with the \"=\" operator.\n"
            "$_        -- result of the last successful operation\n";

    cerr << "\nCommands:\n"
            "--------------------------------------------------\n"
            "quit      -- exit program\n"
            "help      -- print help text\n" 
            "copyright -- print copyright\n"; 
    cerr << endl;
    return con(0.0);
}
#endif

namespace {
//// order by CONST_OBJ, the VARIABLE_OBJ, then everything else see enum
struct EqVecCompare : public std::binary_function<EqObjPtr, EqObjPtr, bool>
{
  bool operator()(EqObjPtr x, EqObjPtr y)
  {
    const EqObjType xtype = x->getType();
    const EqObjType ytype = y->getType();

    bool ret = false;

    if (xtype < ytype)
    {
      ret = true;
    }
    else if (xtype == ytype)
    {
      const std::string &xstr = x->stringValue();
      const std::string &ystr = y->stringValue();

      ret = (xstr < ystr);
    }

    return ret;
  }
};

}

/// Sorts in order of constants, variables, then other objects
void SortEqVector(std::vector<EqObjPtr> &v1)
{
  std::sort(v1.begin(), v1.end(), EqVecCompare());
}

const std::string &EquationObject::stringValue()
{
  if (stringValue_.empty())
  {
    stringValue_ = this->createStringValue();
  }

  return stringValue_;
}

EqObjPtr Variable::getScale()
{
   return con(1);
}

EqObjPtr Variable::getUnscaledValue()
{
   return shared_from_this();
}

double Variable::getSign()
{
    return 1.0;
}

EqObjPtr Variable::getUnsignedValue()
{
   return shared_from_this();
}


Variable::Variable(std::string var) : EquationObject(VARIABLE_OBJ), value(var)
{
}

std::string Variable::createStringValue()
{
    return value;
}

EqObjPtr Variable::Derivative(EqObjPtr foo)
{
    if (foo->stringValue() == value)
        return con(1.0);
    else
        return con(0.0);
}

EqObjPtr Variable::Simplify()
{
   return shared_from_this();
}

/**
 * This function may never be called if variables
 * are turned into Pow's.
 */
EqObjPtr Variable::CombineProduct(std::vector<EqObjPtr> y)
{
   if (y.empty())
      return shared_from_this();

   y.push_back(shared_from_this());
   return EqObjPtr(new Product(y));
}

EqObjPtr Variable::CombineAdd(std::vector<EqObjPtr> y)
{
   if (y.empty())
      return EqObjPtr(shared_from_this());

   y.push_back(shared_from_this());
   return EqObjPtr(new Add(y));
}

bool Variable::isZero()
{
   return false;
}

bool Variable::isOne()
{
   return false;
}

EqObjPtr Variable::clone()
{
    EquationObject *n = new Variable(value);
    return EqObjPtr(n);
}

EqObjPtr Variable::subst(const std::string &str, EqObjPtr eqo)
{
    if (str == this->stringValue())
        return eqo;
    else
        return shared_from_this();
}

EqObjPtr Variable::expand()
{
    return shared_from_this();
}

EqObjPtr Constant::getScale()
{
   return shared_from_this();
}

EqObjPtr Constant::getUnscaledValue()
{
   return con(1);
}

double Constant::getSign()
{
    const double sign = (dvalue < 0.0) ? -1.0 : 1.0;
    return sign;
}

EqObjPtr Constant::getUnsignedValue()
{
    const double val = (dvalue < 0.0) ? -dvalue : dvalue;
    return con(val);
}

Constant::Constant(double x) : EquationObject(CONST_OBJ), dvalue(x)
{
}

std::string Constant::createStringValue()
{
    std::ostringstream os;
    if (dvalue != floor(dvalue))
    {
      os << std::scientific << std::setprecision(15);
    }
    if (dvalue < 0.0)
    {
      os << "(" << dvalue << ")";
    }
    else
    {
      os << dvalue;
    }
    return std::string(os.str());
}

EqObjPtr Constant::Derivative(EqObjPtr foo)
{
    EqObjPtr dog = con(0);
    return dog;
}

EqObjPtr Constant::Simplify()
{
   return this->clone();
}

EqObjPtr Constant::CombineProduct(std::vector<EqObjPtr> y)
{
   double x=dvalue;
   size_t len=y.size();
   for (size_t i=0; i < len; ++i)
   {
      Constant *Y = static_cast<Constant *>(y[i].get());
      x *= Y->dvalue;
   }
   return EqObjPtr(new Constant(x));
}

EqObjPtr Constant::CombineAdd(std::vector<EqObjPtr> y)
{
    EqObjPtr ret;
    double               dsum(0.0);
    dsum += this->getDoubleValue();


    size_t len=y.size();
    for (size_t i=0; i < len; ++i)
    {
        Constant *Y = static_cast<Constant *>(y[i].get());

        dsum += Y->getDoubleValue();
    }
    return con(dsum);
}

bool Constant::isZero()
{
   return (this->getDoubleValue()==0.0);
}

bool Constant::isOne()
{
   return (this->getDoubleValue()==1.0);
}

EqObjPtr Constant::clone()
{
    return con(dvalue);
}

EqObjPtr Constant::subst(const std::string &str, EqObjPtr eqo)
{
    if (str == this->stringValue())
        return eqo;
    else
        return shared_from_this();
}

EqObjPtr Constant::expand()
{
    return shared_from_this();
}


Product::Product(EqObjPtr one, EqObjPtr two) : EquationObject(PRODUCT_OBJ)
{

   values.resize(2);
   values[0]=one;
   values[1]=two;
}

Product::Product(std::vector<EqObjPtr> one) : EquationObject(PRODUCT_OBJ)
{
   values = one;
   SortEqVector(values);
}

std::string Product::createStringValue()
{
    const size_t len = values.size();
    std::ostringstream os;

    os << "(";
    if (len > 0)
    {
        const std::string &s = values[0]->stringValue();
        size_t beg = 1;
        if (len > 1 && s == "(-1)")
        {
            os << "-" << values[1];
            beg = 2;
        }
        else
        {
            os << s;
        }

        for (size_t i=beg; i < len; ++i)
            os << " * " << values[i];
    }
    os << ")";

#if 0
    if (this->getSign() > 0.0)
    {
        os << "(";
        if (len > 0)
            os << values[0];
        for (size_t i=1; i < len; ++i)
            os << " * " << values[i];
        os << ")";
    }
    else
    {
        os << "-" << this->getUnsignedValue()->stringValue();
    }
#endif

    return os.str();
}

EqObjPtr Product::Derivative(EqObjPtr foo)
{
//   cout << "in  :" << EqObjPtr(new Product(values)) << " " << foo << endl;

   const size_t len=values.size();
   std::vector<EqObjPtr> vec_add;
   vec_add.reserve(len);

   // Apply chain rule for each term
   for (size_t i = 0; i < len; ++i)
   {
      std::vector<EqObjPtr> vec_prod;
      vec_prod.reserve(len);

      EqObjPtr dptr = values[i]->Derivative(foo);
      if (dptr->isZero())
         continue;

      if (!dptr->isOne())
         vec_prod.push_back(dptr);

      for (size_t j = 0; j < len; ++j)
      {
         if (i == j)
            continue;
         vec_prod.push_back(values[j]);
      }
      if (vec_prod.empty())
         continue;
      else if (vec_prod.size()==1)
         vec_add.push_back(vec_prod[0]);
      else
         vec_add.push_back(EqObjPtr(new Product(vec_prod)));
   }

   EqObjPtr out;
   if (vec_add.empty())
      out = con(0);
   else if (vec_add.size()==1)
      out = vec_add[0];
   else
      out = EqObjPtr(new Add(vec_add));

//   cout << "out  :" << out << endl;
   return out;
}

    // Performs the various simplification rules.
void Product::ProductVecSimplify(std::vector<EqObjPtr> &tvals)
{
   // When each section is done swap it back into tvals
   {
      // Combine Products together
      const size_t len = tvals.size();
      std::vector<bool> isDone(len, false);
      std::vector<EqObjPtr> tmp;
      tmp.reserve(2*tvals.size());

      // Convert variable's to pow's for combination
      std::vector<EqObjPtr> tmp2;
      tmp2.reserve(len);
      for (size_t i = 0; i < len; ++i)
      {
         if (tvals[i]->getType()==VARIABLE_OBJ)
            tmp2.push_back(pow(tvals[i],con(1)));
         else
            tmp2.push_back(tvals[i]);
      }
      tvals.swap(tmp2);

      // identifies objects of the same type and tries to combine them
      // Calls the CombineProduct for each type
      for (size_t i = 0; i < len; ++i)
      {
         if (isDone[i])
            continue;

         isDone[i] = true;

         EqObjPtr tptr = tvals[i];
         std::vector<EqObjPtr> tmpvec;

         for (size_t j = i + 1; j < len; ++j)
         {
            if (isDone[j])
               continue;

            EqObjPtr uptr = tvals[j];

            // Combine elements of the same type
            if ((tptr.get()->getType())==(uptr.get()->getType()))
            {
               tmpvec.push_back(uptr);
               isDone[j] = true;
               continue;
            }
         }
         if (tmpvec.empty())
            tmp.push_back(tptr);
         else
            tmp.push_back(tptr->CombineProduct(tmpvec));
      }
      swap(tmp,tvals); // swap back;
   }

   // Remove Nested products
   {
      const size_t len = tvals.size();
      std::vector<EqObjPtr> tmp;
      tmp.reserve(2*len);

      for (size_t i = 0; i < len; ++i) 
      {
         Product *foo = dynamic_cast<Product *>(tvals[i].get());
         if (foo == nullptr)
         {
            tmp.push_back(tvals[i]);
         }
         else
         {
            size_t sublen = foo->values.size();
            for (size_t j=0; j < sublen; ++j)
            {
               tmp.push_back(foo->values[j]);
            }
         }
      }
      swap(tmp,tvals); // swap back;
   }

   {  
      const size_t len = tvals.size();
      std::vector<EqObjPtr> tmp;
      // break down all factors and remove ones
      tmp.reserve(len);
      bool hasOne = false;
      for (size_t i = 0; i < len; ++i) 
      {
         if (!tvals[i]->isOne())
            tmp.push_back(tvals[i]->Simplify());
         else
            hasOne = true;
      }
      if (tmp.empty() && hasOne)
         tmp.push_back(con(1));

      swap(tmp,tvals); // swap back;
   }

}

EqObjPtr Product::Simplify()
{
   if (this->isZero())
      return con(0);

   const size_t tlen = values.size();

   if (tlen==1)
      return values[0]->Simplify();

   std::vector<EqObjPtr> tvals;
   tvals.reserve (tlen);
   for (size_t i=0; i < tlen; ++i)
   {
      tvals.push_back(values[i]->Simplify());
   }

   ProductVecSimplify(tvals);

    EqObjPtr out;
    if (tvals.size()==1)
        out = tvals[0];
    else
        out = EqObjPtr(new Product(tvals));

    // now try method where everything is a pow (Catch (1-x)/(1-x) = (1-x)^1*(1-x)^(-1) = 1)
    // start with already simplified std::vector
    if (tvals.size() > 1)
    {
        std::vector<EqObjPtr> tmp;
        tmp.reserve(tvals.size());
        for (size_t i = 0; i < tvals.size(); ++i)
        {
            if (tvals[i]->getType() != POW_OBJ)
                tmp.push_back(pow(tvals[i], con(1)));
            else
                tmp.push_back(tvals[i]);
        }
        SortEqVector(tmp);
        ProductVecSimplify(tmp);

        EqObjPtr teq;
        if (tmp.size() == 1)
            teq = tmp[0];
        else
            teq = EqObjPtr(new Product(tmp));

        // If the result is longer, don't use it
        if (teq->stringValue().size() < out->stringValue().size())
            out = teq;
    }

    {
        /// now if we are a pow, expand out until a list of 2 factors
        /// x^3 = x^2 * x
        /// Need to catch when exponent ends up zero
        std::vector<EqObjPtr> tmp;
        tmp.reserve(tvals.size());
        for (size_t i = 0; i < tvals.size(); ++i)
        {
            if (tvals[i]->getType() == POW_OBJ)
            {
                Pow *foo = static_cast<Pow *>(tvals[i].get());
                EqObjPtr e = foo->exponent;
                EqObjPtr b = foo->base;

                if (e->isOne())
                {
                    tmp.push_back(b);
                    
                }
                else {
                    if (b->getType() == PRODUCT_OBJ)
                    {
                        Product *bar = static_cast<Product *>(b.get());
                        /// See if we already have a helper function for adding product values to list
                        std::vector<EqObjPtr> xx = bar->values;
                        for (size_t j=0; j < xx.size(); ++j)
                        {
                            tmp.push_back(xx[j]);
                        }
                        tmp.push_back(pow(b,(e - con(1))));

                    }
                    else
                    {
                        tmp.push_back(b);
                        tmp.push_back(pow(b,(e - con(1))));
                    }
                }

            }
            else
            {
                tmp.push_back(tvals[i]->clone());
            }
        }

        size_t j = 0;
        /// Keep reducing while it makes sense
        do
        {
            j = tmp.size();
            ProductVecSimplify(tmp);
        } while (tmp.size() < j);

        EqObjPtr teq;
        if (tmp.size() == 1)
            teq = tmp[0];
        else
            teq = EqObjPtr(new Product(tmp));
        if (teq->stringValue().size() < out->stringValue().size())
            out = teq;

    }

   return out;
}

EqObjPtr Product::CombineProduct(std::vector<EqObjPtr> y)
{
   std::vector<EqObjPtr> tmp = values;
   const size_t len = y.size();
   for (size_t i=0; i < len; ++i)
   {
      Product *Y = dynamic_cast<Product *>(y[i].get());
      const size_t len2 = (Y->values).size();
      for (size_t j=0; j < len2; ++j)
         tmp.push_back(Y->values[j]);
   }

   return EqObjPtr(new Product(tmp));
}

EqObjPtr Product::CombineAdd(std::vector<EqObjPtr> y)
{
   if (y.empty())
      return (EqObjPtr(new Product(values)));

   y.push_back(EqObjPtr(new Product(values)));

   return (EqObjPtr(new Add(y)));
}

bool Product::isZero()
{
   size_t len = values.size();
   for (size_t i=0; i<len; ++i)
   {
      if (values[i]->isZero())
         return true;
   }

   return false;
}

bool Product::isOne()
{
   if (values.empty())
      return false;

   const size_t len = values.size();

   for (size_t i=0; i<len; ++i)
   {
      if (!values[i]->isOne())
         return false;
   }

   return true;
}

/// factors out constants scales
//  /todo would be to factor for arbitrary type
EqObjPtr Product::getScale()
{
   dsAssert(!values.empty(), "UNEXPECTED");
   const size_t len = values.size();
   std::vector<EqObjPtr> tmp;
   tmp.reserve(len+1);
   for (size_t i=0; i < len; ++i)
   {
      if (values[i]->getType()==CONST_OBJ)
         tmp.push_back(values[i]);
   }

   if (tmp.empty())
      return con(1);

   return EqObjPtr(new Product(tmp))->Simplify();
}

EqObjPtr Product::getUnscaledValue()
{
   const size_t len = values.size();
   std::vector<EqObjPtr> tmp;
   tmp.reserve(len+1);
   for (size_t i=0; i < len; ++i)
   {
      if (values[i]->getType()!=CONST_OBJ)
         tmp.push_back(values[i]);
   }

   if (tmp.empty())
      return con(1);
   else if (tmp.size()==1)
      return EqObjPtr(tmp[0]);

   // Is this a clone or another reference to the original underlying object
   return EqObjPtr(new Product(tmp));
}

double Product::getSign()
{
    // do I already have one
    int    foundConst = 0;
    double foundSign = 1.0;
    for (size_t i = 0; i < values.size(); ++i)
    {
        if (values[i]->getType() == CONST_OBJ)
        {
            foundSign = values[i]->getSign();
            ++foundConst;
        }

        // only have a negative sign if only one const obj exists
        if (foundConst > 1)
        {
            foundSign = 1.0;
            break;
        }
    }
    return foundSign;
#if 0
    double sign = 1.0;
    EqObjPtr val = this->getScale();
    Constant *foo = dynamic_cast<Constant *>(val.get());
    dsAssert(foo, "UNEXPECTED");
    if (foo)
    {
        sign = (foo->value < 0.0) ? -1.0 : 1.0;
    }
    return sign;
#endif
}

EqObjPtr Product::getUnsignedValue()
{
    EqObjPtr ret;
    const double sign = this->getSign();
    if (sign >= 0.0)
    {
        // return a new smart pointer with references to smart pointers to these values
        ret = shared_from_this();
    }
    else
    {
        EqObjPtr unscaled = this->getUnscaledValue();
        if (unscaled->getType() == CONST_OBJ)
        {
            Constant *c = dynamic_cast<Constant *>(unscaled.get());
            dsAssert(c != 0, "UNEXPECTED"); // this must be true
            // We expect the value to be signed
                if (c->dvalue < 0.0)
                {
                    ret = con(-c->getDoubleValue());
                }
                else
                {
                    ret = unscaled->clone();
                }
        }
        else // This must be a product
        {
            EqObjPtr scaled = this->getScale();
            Constant *s = dynamic_cast<Constant *>(scaled.get());
            dsAssert(s != 0, "UNEXPECTED"); // this must be true
            const double val = s->getDoubleValue();

            dsAssert(val < 0.0, "UNEXPECTED");

            EqObjPtr negval;
                negval = con(-(s->dvalue));


            if (val == -1.0)
            {
                ret = unscaled;
            }
            else
            {
                // Want to prevent something like -8*(a*x^2)
//              ret = con(-val) * unscaled;
                if (unscaled->getType() == PRODUCT_OBJ)
                {
                    Product *Y = static_cast<Product *>(unscaled.get());
                    typedef std::vector<EqObjPtr> EqVec;
                    EqVec &uvals = Y->values;
                    EqVec newvals;
                    newvals.reserve(uvals.size()+1);
                    newvals.push_back(negval);
                    for (size_t i = 0; i < uvals.size(); ++i)
                    {
                        // Is the clone necessary, well just in case
                        newvals.push_back(uvals[i]->clone());
                    }
                    EquationObject *eq = new Product(newvals);
                    ret = EqObjPtr(eq);
                }
                else
                {
                    ret = negval * unscaled;
                }
            }
        }
    }

    return ret;
}

EqObjPtr Product::clone()
{
    const size_t len = values.size();
    dsAssert(len != 0, "UNEXPECTED");
    std::vector<EqObjPtr> tmp(len);
    for (size_t i = 0; i < len; ++i)
    {
        tmp[i] = values[i]->clone();
    }
    EquationObject *n = new Product(tmp);
    return EqObjPtr(n);
}

EqObjPtr Product::subst(const std::string &str, EqObjPtr eqo)
{
    if (str == this->stringValue())
        return eqo;

    //// TODO: back port this to the original adiff
    else if (str == this->getUnscaledValue()->stringValue())
    {
      return this->getScale() * eqo;
    }
    else
    {
        std::vector<EqObjPtr> new_values(values.size());
        for (size_t i=0; i < values.size(); ++i)
        {
            new_values[i] = values[i]->subst(str, eqo);
        }
        return EqObjPtr(new Product(new_values));
    }
}

// expand out all products with additions
EqObjPtr Product::expand()
{
    const size_t len = values.size();
    dsAssert(len != 0, "UNEXPECTED");
    std::vector<EqObjPtr> tmp;
    tmp.reserve(len);
    std::vector<EqObjPtr> adds;
    adds.reserve(len);
    for (size_t i = 0; i < len; ++i)
    {
        if (values[i]->getType() == ADD_OBJ)
            adds.push_back(values[i]->expand());
        else
            tmp.push_back(values[i]->expand());
            
    }
    EqObjPtr scale;
    if (tmp.size() == 1)
    {
        scale = tmp[0];
    }
    else if (tmp.size() > 1)
    {
        EquationObject *n = new Product(tmp); 
        scale = EqObjPtr(n);
    }

    EqObjPtr out;
    if (adds.empty())
    {
        dsAssert(!tmp.empty(), "UNEXPECTED");
        out = scale;
    }
    else
    {
        if (tmp.empty())
            scale = con(1);
        std::vector<EqObjPtr> pout;
        // break out into separate function
        // do the expansion here
        // pout is the result of the multiplication of scale
        // with each object within an add
        for (size_t i = 0; i < adds.size(); ++i)
        {
            // use static cast since we know this is an add
            Add *Y = static_cast<Add *>(adds[i].get());
            // get reference
            std::vector<EqObjPtr> &v = Y->values;
            if (scale->isOne())
            {
                pout = v; // copy
            }
            else
            {
                for (size_t j = 0; j < v.size(); ++j)
                {
                    pout.push_back(scale * v[j]);
                }
            }
            EquationObject *foo = new Add(pout);
            scale = EqObjPtr(foo);
            pout.clear(); // reuse for the next pass
        }
        out = scale;
    }

    return out;
}

Add::Add(EqObjPtr one, EqObjPtr two) : EquationObject(ADD_OBJ)
{
   values.resize(2);
   values[0]=one;
   values[1]=two;
}

Add::Add(std::vector<EqObjPtr> one) : EquationObject(ADD_OBJ)
{
   values = one;
   SortEqVector(values);
}

std::string Add::createStringValue()
{
   const size_t len = values.size();
   std::ostringstream os;
   os << "(";
   if (len > 0)
   {
        if (values[0]->getSign() > 0.0)
            os << values[0];
        else
            os << "-" << values[0]->getUnsignedValue();
   }
   for (size_t i=1; i < len; ++i)
   {
        if (values[i]->getSign() > 0.0)
            os << " + " << values[i];
        else
            os << " - " << values[i]->getUnsignedValue();
   }
   os << ")";
    return os.str();
}

EqObjPtr Add::Derivative(EqObjPtr foo)
{
//   cout << EqObjPtr(new Add(values)) << "\t" << foo << endl;
   const size_t len=values.size();
   std::vector<EqObjPtr> vec;
   vec.reserve(len);

   for (size_t i = 0; i < len; ++i)
   {
      EqObjPtr dptr = values[i]->Derivative(foo);
      if (!dptr->isZero())
         vec.push_back(dptr);
   }

   EqObjPtr out;
   if (vec.empty())
      out = con(0);
   else if (vec.size()==1)
      out = vec[0];
   else
      out = EqObjPtr(new Add(vec));

//   cout << out << endl;
   return out;
}

EqObjPtr Add::Simplify()
{
   const size_t tlen = values.size();

   if (tlen==1)
      return values[0]->Simplify();

   std::vector<EqObjPtr> tvals;
   tvals.reserve (tlen);
   for (size_t i=0; i < tlen; ++i)
   {
      tvals.push_back(values[i]->Simplify());
   }

   // When each section is done swap it back into tvals

   {
      // Combine elements which match each other's types
      const size_t len = tvals.size();
      std::vector<bool> isDone(len, false);
      std::vector<EqObjPtr> tmp;
      tmp.reserve(2*tvals.size());

      for (size_t i = 0; i < len; ++i)
      {
         if (isDone[i])
            continue;

         isDone[i] = true;

         EqObjPtr tptr = tvals[i];
         std::vector<EqObjPtr> tmpvec;

         for (size_t j = i + 1; j < len; ++j)
         {
            if (isDone[j])
               continue;

            EqObjPtr uptr = tvals[j];

            if ((tptr.get()->getType())==(uptr.get()->getType()))
            {
               tmpvec.push_back(uptr);
               isDone[j] = true;
               continue;
            }
         }
         if (tmpvec.empty())
            tmp.push_back(tptr);
         else
            tmp.push_back(tptr->CombineAdd(tmpvec));
      }
      swap(tmp,tvals); // swap back;
   }


   {
      // Combine all additions, including ((x+y)+z)
      const size_t len = tvals.size();
      std::vector<EqObjPtr> tmp;
      tmp.reserve(2*len);

      for (size_t i = 0; i < len; ++i) 
      {
         Add *foo = dynamic_cast<Add *>(tvals[i].get());
         if (foo == nullptr)
         {
            tmp.push_back(tvals[i]);
         }
         else
         {
            size_t sublen = foo->values.size();
            for (size_t j=0; j < sublen; ++j)
            {
               tmp.push_back(foo->values[j]);
            }
         }
      }
      swap(tmp,tvals); // swap back;
   }

   {
      // Remove zero entries
      const size_t len = tvals.size();
      std::vector<EqObjPtr> tmp;
      tmp.reserve(len);
      for (size_t i = 0; i < len; ++i) 
      {
         EqObjPtr tptr = tvals[i] -> Simplify();
         if (!tptr->isZero())
            tmp.push_back(tptr);
      }
      swap(tmp,tvals); // swap back;
   }

   // Adds terms with different factors
   {
      size_t len=tvals.size();
      std::vector <EqObjPtr> tmp;
      tmp.reserve(len);

      std::vector<bool> isDone(len,false);
      for (size_t i=0; i < len; ++i)
      {
         if (isDone[i])
            continue;
         isDone[i]=true;

         EqObjPtr tptr = tvals[i];

         if (tptr->isZero())
            continue;

         const std::string name = tptr->getUnscaledValue()->stringValue();
//       cout << name << endl;

         std::vector<EqObjPtr> factvec;
         factvec.push_back(tptr->getScale());

         for (size_t j=(i+1); j < len; ++j)
         {
            if (isDone[j])
               continue;

            EqObjPtr uptr = tvals[j];

            if (uptr->isZero())
            {
               isDone[j] = true;
               continue;
            }

            if (name == uptr->getUnscaledValue()->stringValue())
            {
               isDone[j] = true;
               factvec.push_back(uptr->getScale());
            }
         }


         EqObjPtr fact = EqObjPtr(new Add(factvec))->Simplify();

         if (fact->isOne())
            tmp.push_back(tptr->getUnscaledValue());
         else
            tmp.push_back((fact*tptr->getUnscaledValue())->Simplify());
      }
      swap(tmp,tvals); // swap back;
   }

   if (tvals.empty())
      return con(0);
   else if (tvals.size()==1)
      return tvals[0];

   return EqObjPtr(new Add(tvals));
}

EqObjPtr Add::CombineProduct(std::vector<EqObjPtr> y)
{
   if (y.empty())
      return shared_from_this();

   y.push_back(shared_from_this());
   return (EqObjPtr(new Product(y)));
}

EqObjPtr Add::CombineAdd(std::vector<EqObjPtr> y)
{
   std::vector<EqObjPtr> tmp = values;
   const size_t len = y.size();
   for (size_t i=0; i < len; ++i)
   {
      Add *Y = dynamic_cast<Add *>(y[i].get());
      const size_t len2 = (Y->values).size();
      for (size_t j=0; j < len2; ++j)
         tmp.push_back(Y->values[j]);
   }

   return EqObjPtr(new Add(tmp));
}

EqObjPtr Add::clone()
{
    const size_t len = values.size();
    dsAssert(len != 0, "UNEXPECTED");
    std::vector<EqObjPtr> tmp(len);
    for (size_t i = 0; i < len; ++i)
    {
        tmp[i] = values[i]->clone();
    }
    EquationObject *n = new Add(tmp);
    return EqObjPtr(n);
}

EqObjPtr Add::subst(const std::string &str, EqObjPtr eqo)
{
    if (str == this->stringValue())
        return eqo;
    else
    {
        std::vector<EqObjPtr> new_values(values.size());
        for (size_t i=0; i < values.size(); ++i)
        {
            new_values[i] = values[i]->subst(str, eqo);
        }
        return EqObjPtr(new Add(new_values));
    }
}

bool Add::isZero()
{
   return false;
}

bool Add::isOne()
{
   return false;
}

EqObjPtr Add::getScale()
{
   return con(1.0);
}

EqObjPtr Add::getUnscaledValue()
{
   return shared_from_this();
}

double Add::getSign()
{
    return 1.0;
}

EqObjPtr Add::getUnsignedValue()
{
   return shared_from_this();
}

EqObjPtr Add::expand()
{
    const size_t len = values.size();
    dsAssert(len != 0, "UNEXPECTED");
    std::vector<EqObjPtr> tmp(len);
    for (size_t i = 0; i < len; ++i)
    {
        tmp[i] = values[i]->expand();
    }
    EquationObject *n = new Add(tmp);
    return EqObjPtr(n);
}


Exponent::Exponent(EqObjPtr var) : EquationObject(EXPONENT_OBJ), value(var)
{
}

std::string Exponent::createStringValue()
{
    std::ostringstream os;
    os << "exp(" << value << ")";
    return std::string(os.str());
}

EqObjPtr Exponent::Derivative(EqObjPtr foo)
{
  if (value->getType() == CONST_OBJ)
  {
    return con(0.0);
  }
  else
  {
     return value->Derivative(foo)*exp(value);
  }
}

EqObjPtr Exponent::Simplify()
{
   if (value->isZero())
      return con(1);

   if (value->getType() == LOG_OBJ)
   {
      Log *Y = dynamic_cast<Log *>(value.get());
      return (Y->value);
   }

   return exp(value->Simplify());
}

/**
 * exp(y)*exp(x)=exp(x+y)
 */
EqObjPtr Exponent::CombineProduct(std::vector<EqObjPtr> y)
{
   std::vector<EqObjPtr> tmp;
   tmp.push_back(value);
   const size_t len = y.size();
   for (size_t i=0; i < len; ++i)
   {
      Exponent *Y = dynamic_cast<Exponent *>(y[i].get());
      dsAssert(Y!=nullptr, "UNEXPECTED");
      tmp.push_back(Y->value);
   }
   return EqObjPtr(new Exponent(EqObjPtr(new Add(tmp))));
}

EqObjPtr Exponent::CombineAdd(std::vector<EqObjPtr> y)
{
   if (y.empty())
      return shared_from_this();

   y.push_back(EqObjPtr(new Exponent(value)));
   return EqObjPtr(new Add(y));
}

bool Exponent::isZero()
{
   return false;
}

bool Exponent::isOne()
{
   return false;
}

EqObjPtr Exponent::getScale()
{
   return con(1.0);
}

EqObjPtr Exponent::getUnscaledValue()
{
   return shared_from_this();
}

double Exponent::getSign()
{
    return 1.0;
}

EqObjPtr Exponent::getUnsignedValue()
{
   return shared_from_this();
}

EqObjPtr Exponent::clone()
{
    EquationObject *n = new Exponent(value->clone());
    return EqObjPtr(n);
}

EqObjPtr Exponent::subst(const std::string &str, EqObjPtr eqo)
{
    if (str == this->stringValue())
        return eqo;
    else
        return exp(value->subst(str, eqo));
}

EqObjPtr Exponent::expand()
{
    return exp(value->expand());
}

Log::Log(EqObjPtr var) : EquationObject(LOG_OBJ), value(var)
{
}

std::string Log::createStringValue()
{
    std::ostringstream os;
    os << "log(" << value << ")";
    return std::string(os.str());
}

EqObjPtr Log::Derivative(EqObjPtr foo)
{
  EqObjPtr ret;
  if (value->getType() == CONST_OBJ)
  {
    ret = con(0.0);
  }
  else
  {
    ret = value->Derivative(foo)*pow(value,EqObjPtr(new Constant(-1.0)));
  }

  return ret;
}

EqObjPtr Log::Simplify()
{
   if (value->isOne())
      return con(0);

   ///log(exp(x))) = x;
   if (value->getType() == EXPONENT_OBJ)
   {
      Exponent *Y = dynamic_cast<Exponent *>(value.get());
      return (Y->value);
   }

   ///log(exp(x))) = x;
   if (value->getType() == EXPONENT_OBJ)
   {
      Exponent *Y = dynamic_cast<Exponent *>(value.get());
      return (Y->value);
   }

   ///log(pow(x,y))) = y*log(x);
   if (value->getType() == POW_OBJ)
   {
      Pow *Y = dynamic_cast<Pow *>(value.get());
      return (Y->exponent * log(Y->base));
   }

   return log(value->Simplify());
}

EqObjPtr Log::CombineProduct(std::vector<EqObjPtr> y)
{
   if (y.empty())
     return shared_from_this();

   y.push_back(shared_from_this());
   return EqObjPtr(new Product(y));
}

/**
 * now just return original std::vector
 * in the future, log(x)+log(y) = log(x*y)
 */
EqObjPtr Log::CombineAdd(std::vector<EqObjPtr> y)
{
   if (y.empty())
      return shared_from_this();

   y.push_back(shared_from_this());
   return EqObjPtr(new Add(y));
}

bool Log::isZero()
{
   return false;
}

bool Log::isOne()
{
   return false;
}

EqObjPtr Log::getScale()
{
   return con(1);
}

EqObjPtr Log::getUnscaledValue()
{
   return shared_from_this();
}

double Log::getSign()
{
    return 1.0;
}

EqObjPtr Log::getUnsignedValue()
{
   return shared_from_this();
}

EqObjPtr Log::clone()
{
    EquationObject *n = new Log(value->clone());
    return EqObjPtr(n);
}

EqObjPtr Log::subst(const std::string &str, EqObjPtr eqo)
{
    if (str == this->stringValue())
        return eqo;
    else
        return log(value->subst(str, eqo));
}

EqObjPtr Log::expand()
{
    return log(value->expand());
}

Pow::Pow(EqObjPtr b, EqObjPtr e) : EquationObject(POW_OBJ),
   base(b), exponent(e)
{
}

std::string Pow::createStringValue()
{
    std::ostringstream os;
    // need to control the choice between printing behavior
    os << "pow(" << base << "," << exponent << ")";
//    os << "(" << base << ")^(" << exponent << ")";
//    os << "(" << base << "^" << exponent << ")";
    return std::string(os.str());
}

EqObjPtr Pow::Derivative(EqObjPtr foo)
{
  EqObjPtr ret;
  if (exponent->getType() == CONST_OBJ)
  {
    double ex =static_cast<Constant *>(exponent.get())->getDoubleValue();
    //// x^0 is 1 diff(2^3, x) is 0
    //// it doesn't matter if we consider 0^0 == 0 or 0^0 == 1
    //// since the derivative is the same
    if ((ex == 0.0) || (base->getType() == CONST_OBJ))
    {
      ret = con(0.0);
    }
    /// diff(y^1, x) is diff(y,x)
    else if (ex == 1.0)
    {
      ret = diff(base, foo);
    }
    /// diff(y^2,x) is 2 * y * diff(y,x)
    else if (ex == 2.0)
    {
      ret = con(2.0) * base * diff(base, foo);
    }
    //// diff(y^3, x) is 3*y^2
    else
    {
      ret = con(ex) * pow(base, con(ex-1.0))*diff(base, foo);
    }
  }
  else if (base->getType() == CONST_OBJ)
  {
    //diff(0^y, x) == 0 no matter what
    //// 1^x is 1
    if (base->isZero() || base->isOne())
    {
      ret = con(0.0);
    }
    /// We already handled the case of the exponent being constant above
    else
    {
      ret = log(base) * shared_from_this() * diff(exponent, foo);
    }
  }
  else
  {
     ret = exponent*pow(base,exponent - con(1)) * diff(base, foo) + log(base)*(this->clone())*diff(exponent,foo);
//     ret = diff((log(base)*exponent),foo)*pow(base,exponent);
  }
  return ret;
}

/**
 * if exponent is 0, return 1.0
 * if exponent is 1.0, return base
 */
EqObjPtr Pow::Simplify()
{
   EqObjPtr b  = base->Simplify();
   EqObjPtr expo = exponent->Simplify();

   /// pow(x,0)=1
   if (expo->isZero())
      return con(1);

   /// pow(x,1)=x
   if (expo->isOne())
      return base;

   /// pow(1,x)=1
   if (b->isOne())
      return con(1);

   /// pow(0,x)=0
   if (b->isZero())
      return con(0.0);

   /// pow(m,n)
   if ((b->getType() == CONST_OBJ)
         && (expo->getType() == CONST_OBJ))
   {
      Constant *x=static_cast<Constant *>(b.get());
      Constant *y=static_cast<Constant *>(expo.get());
      return con(std::pow(x->getDoubleValue(),y->getDoubleValue()));
   }

   /// may want to do static cast
   /// pow(pow(x,y),z) = pow(x,(y*z))
   if (b->getType() == POW_OBJ)
   {
      Pow *Y = static_cast<Pow *>(b.get());
//      cout << Y-> base << " " <<  (Y->exponent * exponent) << endl;
      return pow(Y->base, (Y->exponent * expo)->Simplify());
   }
   return pow(b, expo);
}

/**
 * If the same base is shared, add the exponents
 *
 * In the future, if the same exponent is shared
 * combine the bases as a product
 */
EqObjPtr Pow::CombineProduct(std::vector<EqObjPtr> y)
{
   y.push_back(pow(base,exponent));
   const size_t len = y.size();
   std::vector<bool> isDone(len,false);
   std::vector <EqObjPtr> out;

   // If the same base is shared, add the exponents
   for (size_t i=0; i < len; ++i)
   {
      if (isDone[i])
         continue;
      isDone[i]=true;
      Pow *Y1 = static_cast<Pow *>(y[i].get());
      std::vector <EqObjPtr> tmp_exp;
      tmp_exp.push_back(Y1->exponent);

      for (size_t j=i+1; j < len; ++j)
      {
         if (isDone[j])
            continue;

         Pow *Y2 = static_cast<Pow *>(y[j].get());
         if ((Y1->base)->stringValue() == (Y2->base)->stringValue())
         {
            tmp_exp.push_back(Y2->exponent);
            isDone[j]=true;
         }
      }
      if (tmp_exp.size() == 1)
         out.push_back(pow(Y1->base, tmp_exp[0]));
      else
         out.push_back(pow(Y1->base, EqObjPtr(new Add(tmp_exp))));
   }

   if (out.size()==1)
      return out[0];

   return EqObjPtr(new Product(out));
}

EqObjPtr Pow::CombineAdd(std::vector<EqObjPtr> y)
{
   y.push_back(pow(base,exponent));
   return EqObjPtr(new Add(y));
}

bool Pow::isZero()
{
   return false;
}

bool Pow::isOne()
{
   return false;
}

EqObjPtr Pow::getScale()
{
   return con(1);
}

EqObjPtr Pow::getUnscaledValue()
{
   return pow(base,exponent);
}

double Pow::getSign()
{
    return 1.0;
}

EqObjPtr Pow::getUnsignedValue()
{
   return shared_from_this();
}

EqObjPtr Pow::clone()
{
    EquationObject *n = new Pow(base->clone(), exponent->clone());
    return EqObjPtr(n);
}

EqObjPtr Pow::subst(const std::string &str, EqObjPtr eqo)
{
    if (str == this->stringValue())
        return eqo;
    else
    {
        EqObjPtr new_base = base->subst(str, eqo);
        EqObjPtr new_exponent = exponent->subst(str, eqo);
        return EqObjPtr(pow(new_base, new_exponent));
    }
}

// TODO: refacter dangerous code using doubles
// this is very flaky code
// need to make sure that integer exponents are handled properly
// make sure to clone the object before expansion
EqObjPtr Pow::expand()
{
    EqObjPtr expanded_base = base->expand();
    EqObjPtr expanded_exponent = exponent->expand();

    EqObjPtr out = pow(expanded_base, expanded_exponent);
    // want to factor out order greater than 1
//    cout << exponent->getType() << endl;
    if (expanded_exponent->getType() == CONST_OBJ)
    {
        Constant *Y = static_cast<Constant *>(expanded_exponent.get());
        double dval = Y->dvalue;
        bool negative = (dval < 0.0);
        if (dval < 0.0)
        {
            negative = true;
            dval = -dval;
        }

        std::ostringstream os;
        os << dval;
        std::string tstr = os.str();
        // look for non floating point numbers
        // arbitrarily restrict exponents less than 100
        // warning magic number
//      cout << "DEBUG tstr" << tstr << endl;
        if ((dval < 100) && (tstr.find('.') == std::string::npos))
        {
            const int ex = atoi(tstr.c_str());
            if (ex > 1)
            {
                // assume already expanded above
                EqObjPtr newbase = expanded_base->clone();
                out = newbase;
                for (int i = 1; i < ex; ++i)
                {
                    out = out*newbase;
                }
                out = out->expand();
                if (negative)
                    out = pow(out,con(-1));
            }
        }

    }

    return out;
}

std::set<std::string> UniteReferencedType(EqObjType rt, const EqObjPtr &op1, const EqObjPtr &op2)
{
   std::set<std::string> out;

   if (op1->getType() == rt)
   {
         out.insert(op1->stringValue());
   }
   else
   {
      std::set<std::string> rm= op1->getReferencedType(rt);
      std::set<std::string>::iterator it=rm.begin();
      std::set<std::string>::iterator end=rm.end();
      for ( ; it != end; ++it)
            out.insert(*it);
   }

   if (op2->getType() == rt)
   {
         out.insert(op2->stringValue());
   }
   else
   {
      std::set<std::string> rm= op2->getReferencedType(rt);
      std::set<std::string>::iterator it=rm.begin();
      std::set<std::string>::iterator end=rm.end();
      for ( ; it != end; ++it)
            out.insert(*it);
   }
   return out;
}

std::set<std::string> UniteReferencedType(EqObjType rt, const std::vector<EqObjPtr> &x)
{
   std::set<std::string> out;
   for (size_t i=0; i < x.size(); ++i)
   {
      if (x[i]->getType() == rt)
      {
            out.insert(x[i]->stringValue());
      }
      else
      {
         std::set<std::string> rm= x[i]->getReferencedType(rt);
         std::set<std::string>::iterator it=rm.begin();
         std::set<std::string>::iterator end=rm.end();
         for ( ; it != end; ++it)
         {
            out.insert(*it);
         }
      }
   }
   return out;
}

} // always close the namespace
