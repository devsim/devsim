/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef EQUATIONOBJECT_H
#define EQUATIONOBJECT_H

#include <memory>
#include "dsAssert.hh"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>

namespace Eqo {
class EquationObject;
typedef std::shared_ptr<EquationObject> EqObjPtr;
inline EqObjPtr con(double x);
/**
 Enumerated type quickly tells what type the derived class is.  May
 be faster than using dynamic_cast.  This is passed in the constructor
 of the derived class to the base class.
*/
enum EqObjType {CONST_OBJ=0, VARIABLE_OBJ, ADD_OBJ, PRODUCT_OBJ, EXPONENT_OBJ, POW_OBJ, LOG_OBJ, MODEL_OBJ, USERFUNC_OBJ, BLOGICAL_OBJ, ULOGICAL_OBJ, IF_OBJ, IFELSE_OBJ};

const char * const EqObjNames[] = {
    "constant",
    "variable",
    "add",
    "product",
    "exp",
    "pow",
    "log",
    "model",
    "userfunc",
    "blogical",
    "ulogical",
    "if",
    "ifelse"
};


typedef std::shared_ptr<EquationObject> EqObjPtr;
std::set<std::string> UniteReferencedType(EqObjType, const std::vector<EqObjPtr> &x);
std::set<std::string> UniteReferencedType(EqObjType, const EqObjPtr &op1, const EqObjPtr &op2);

/**
   This is the abstract base class which provides the interface for all objects.
*/
class EquationObject : public std::enable_shared_from_this<EquationObject>
{
   public:
      typedef std::shared_ptr<EquationObject> EqObjPtr;

      virtual std::vector<EqObjPtr> getArgs() = 0;


      EqObjType getType() {return eqtype;}
      virtual ~EquationObject() {};

      virtual EqObjPtr Derivative(EqObjPtr)=0;
      virtual EqObjPtr Simplify()=0;

      const std::string &stringValue();
      /// Right now for name only
      virtual std::set<std::string> getReferencedType(Eqo::EqObjType)=0;

      // Both of these should be for std::vector<EqObjPtr>
      // Each EquationObject should know how to combine itself
      // with others of the same type
      // to do, create default method for simple types
      virtual EqObjPtr CombineProduct(std::vector<EqObjPtr>) = 0;
      virtual EqObjPtr CombineAdd(std::vector<EqObjPtr>) = 0;

      virtual bool isZero()=0;
      virtual bool isOne()=0;

      virtual EqObjPtr getScale()=0;
      virtual EqObjPtr getUnscaledValue()=0;

      virtual double getSign() = 0;
      virtual EqObjPtr getUnsignedValue() = 0;

      // creates a clone copy of original structure recursively
      // note that constant and variables do not need to clone
      // their values since they are simple data
      virtual EqObjPtr clone() = 0;

      // Substitute the first expression for the
      // non virtual since I want to check each for their string value
      // \todo make non virtual later
      // probably won't work for anything very well, except for strings.
      // If substitution is successful, it always returns a clone to the substitution
      //
      // Note that even though this changes the contents of the equation object, you
      // must always set the result equal to the return value, in case the top object
      // is replaced
// Really need to reconsider subst interface to not be string
      virtual EqObjPtr subst(const std::string &, EqObjPtr) = 0;

      // Expand out all products with additions
      virtual EqObjPtr expand() = 0;

      // csc = 1/sin
      virtual bool hasReciprocal() = 0;
      virtual EqObjPtr getReciprocal() = 0;

      /// TODO: for more powerful user defined functions
      /// hasInverse
      /// hasAnnhilater
      /// hasCombiner
   protected:
      virtual std::string createStringValue()=0;

      EquationObject(EqObjType foo) : eqtype(foo) {}

      std::vector<EqObjPtr> createArgs(EqObjPtr x) {
        std::vector<EqObjPtr> out;
        out.push_back(x);
        return out;
      }

      std::set<std::string> CreateSetIfThisType(EqObjType rt)
      {
        std::set<std::string> ret;
        if (eqtype == rt)
        {
            ret.insert(this->stringValue());
        }
        return ret;
      }

      std::vector<EqObjPtr> createArgs(EqObjPtr x, EqObjPtr y) {
        std::vector<EqObjPtr> out;
        out.reserve(2);
        out.push_back(x);
        out.push_back(y);
        return out;
      }

      std::vector<EqObjPtr> createArgs(EqObjPtr x, EqObjPtr y, EqObjPtr z) {
        std::vector<EqObjPtr> out;
        out.reserve(3);
        out.push_back(x);
        out.push_back(y);
        out.push_back(z);
        return out;
      }

      std::vector<EqObjPtr> createArgs(std::vector<EqObjPtr> &x) {
        return x;
      }

   private:
      EquationObject(const EquationObject &);
      //Cannot return abstract class
      //EquationObject operator=(const EquationObject &);

      EqObjType   eqtype;
      std::string stringValue_;
};

class Constant : public EquationObject {
    public:
        Constant(double);
        ~Constant() {};


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

        bool hasReciprocal()
        {
          return !(this->isZero());
        }

        EqObjPtr getReciprocal()
        {
          return con(1.0/dvalue);
        }











        double getDoubleValue()
        {
          return dvalue;
        }

        std::set<std::string> getReferencedType(Eqo::EqObjType rt) {
            return CreateSetIfThisType(rt);
        }

        friend class Pow;
        friend class Product;

        std::vector<EqObjPtr> getArgs() {
          return createArgs(this->shared_from_this());
        }
    private:
        std::string createStringValue();

        Constant(const Constant &);
        Constant operator=(const Constant &);

        const double dvalue;
};

class Add : public EquationObject {
    public:
        Add(EqObjPtr, EqObjPtr);
        Add(std::vector<EqObjPtr>);

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

        bool hasReciprocal() {return false;}
        EqObjPtr getReciprocal() {dsAssert(0, "UNEXPECTED"); return con(0);}
        std::set<std::string> getReferencedType(Eqo::EqObjType rt)
        {
            return UniteReferencedType(rt, values);
        }

        friend class Product; // expansion of (a + ...)*(b + ...)

        std::vector<EqObjPtr> getArgs() {
          return createArgs(values);
        }

    private:
        std::string createStringValue();

        Add(const Add &);
        Add operator=(const Add &);

        std::vector<EqObjPtr> values;
};

class Variable : public EquationObject {
    public:
        const std::string &getVariableName() {
          return value;
        }

        Variable(std::string);
        ~Variable() {};
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
          return createArgs(shared_from_this());
        }

        bool hasReciprocal() {return false;}
        EqObjPtr getReciprocal() {dsAssert(0, "UNEXPECTED"); return con(0);}
        std::set<std::string> getReferencedType(Eqo::EqObjType rt) {
            return CreateSetIfThisType(rt);
        }

    private:
        std::string createStringValue();

        Variable(const Variable &);
        Variable operator=(const Variable &);

        const std::string value;
};

class Product : public EquationObject {
    public:
        friend class Pow;
        Product(EqObjPtr, EqObjPtr);
        Product(std::vector<EqObjPtr>);

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

        void ProductVecSimplify(std::vector<EqObjPtr> &);

        bool hasReciprocal() {return false;}
        EqObjPtr getReciprocal() {dsAssert(0, "UNEXPECTED"); return con(0);}

        std::set<std::string> getReferencedType(Eqo::EqObjType rt)
        {
            return UniteReferencedType(rt, values);
        }

        std::vector<EqObjPtr> getArgs() {
          return createArgs(values);
        }

    private:
        std::string createStringValue();

        Product(const Product &);
        Product operator=(const Product &);

        std::vector<EqObjPtr> values;
};

class Exponent : public EquationObject {
    public:
        Exponent(EqObjPtr);

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

        bool hasReciprocal() {return false;}
        EqObjPtr getReciprocal() {dsAssert(0, "UNEXPECTED"); return con(0);}

        std::set<std::string> getReferencedType(Eqo::EqObjType rt)
        {
            return value->getReferencedType(rt);
        }

        std::vector<EqObjPtr> getArgs() {
          return createArgs(value);
        }

        friend class Log;
    private:
        std::string createStringValue();

        Exponent(const Exponent &);
        Exponent operator=(const Exponent &);

        const EqObjPtr value;
};

class Log : public EquationObject {
    public:
        Log(EqObjPtr);

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

        bool hasReciprocal() {return false;}
        EqObjPtr getReciprocal() {dsAssert(0, "UNEXPECTED"); return con(0);}
        std::set<std::string> getReferencedType(Eqo::EqObjType rt)
        {
            return value->getReferencedType(rt);
        }

        friend class Exponent;

        std::vector<EqObjPtr> getArgs() {
          return createArgs(value);
        }

    private:
        std::string createStringValue();

        Log(const Log &);
        Log operator=(const Log &);

        const EqObjPtr value;
};

class Pow : public EquationObject {
    public:
        Pow(EqObjPtr, EqObjPtr);

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

        bool hasReciprocal() {return false;}
        EqObjPtr getReciprocal() {dsAssert(0, "UNEXPECTED"); return con(0);}
        std::set<std::string> getReferencedType(Eqo::EqObjType rt)
        {
            return UniteReferencedType(rt, base, exponent);
        }

        friend class Log;
        friend class Product;

        std::vector<EqObjPtr> getArgs() {
          return createArgs(base, exponent);
        }
    private:
        std::string createStringValue();

        Pow(const Pow &);
        Pow operator=(const Pow &);

        const EqObjPtr base;
        const EqObjPtr exponent;
};


/**
 * The following is to define macro variables
 */
extern std::map<std::string, EqObjPtr> variableMap;



/**
 * The following entries are to create tables of available
 * commands for my parser
 */
typedef EqObjPtr (*EqUnaryFuncPtr) (EqObjPtr);
typedef EqObjPtr (*EqBinaryFuncPtr) (EqObjPtr, EqObjPtr);

struct BinaryTblEntry {
    const char *name;
    EqBinaryFuncPtr func;
    const char *desc;
};

struct UnaryTblEntry {
    const char *name;
    EqUnaryFuncPtr func;
    const char *desc;
};

extern UnaryTblEntry UnaryTable[];
extern BinaryTblEntry BinaryTable[];

EqBinaryFuncPtr getBinaryFuncPtr(std::string);
EqUnaryFuncPtr getUnaryFuncPtr(std::string);

/// Print help
EqObjPtr printHelp();

inline EqObjPtr operator* (EqObjPtr x, EqObjPtr y)
{
  EqObjPtr ret;
  if (x->isZero() || y->isZero())
  {
    ret = con(0.0);
  }
  else if (x->isOne())
  {
    ret = y;
  }
  else if (y->isOne())
  {
    ret = x;
  }
  else if (x->getType() == Eqo::PRODUCT_OBJ)
  {
    std::vector<EqObjPtr> x_args = x->getArgs();

    if (y->getType() == Eqo::PRODUCT_OBJ)
    {
      std::vector<EqObjPtr> y_args = y->getArgs();
      for (std::vector<EqObjPtr>::iterator it =  y_args.begin(); it != y_args.end(); ++it)
      {
        x_args.push_back(*it);
      }
    }
    else
    {
      x_args.push_back(y);
    }

    ret = EqObjPtr(new Product(x_args));
  }
  else if (y->getType() == Eqo::PRODUCT_OBJ)
  {
    std::vector<EqObjPtr> y_args = y->getArgs();
    std::vector<EqObjPtr> x_args;
    x_args.reserve(y_args.size() + 1);
    x_args.push_back(x);
    for (std::vector<EqObjPtr>::iterator it =  y_args.begin(); it != y_args.end(); ++it)
    {
      x_args.push_back(*it);
    }
    ret = EqObjPtr(new Product(x_args));
  }
  else
  {
    ret = EqObjPtr(new Product(x, y));
  }

  return ret;
}

EqObjPtr pow (EqObjPtr x, EqObjPtr y);
inline EqObjPtr con(double x);

inline EqObjPtr operator/ (EqObjPtr x, EqObjPtr y)
{
    if (y->hasReciprocal())
    {
        if ((x->getType() == CONST_OBJ) && (y->getType() == CONST_OBJ))
        {
            return EqObjPtr(new Product(x, y->getReciprocal()))->Simplify();
        }
    }
   return EqObjPtr(new Product(x, pow(y, con(-1))));
}

inline EqObjPtr operator+ (EqObjPtr x, EqObjPtr y)
{
  EqObjPtr ret;
  if (x->isZero())
  {
    ret = y;
  }
  else if (y->isZero())
  {
    ret = x;
  }
  else if (x->getType() == Eqo::ADD_OBJ)
  {
    std::vector<EqObjPtr> x_args = x->getArgs();

    if (y->getType() == Eqo::ADD_OBJ)
    {
      std::vector<EqObjPtr> y_args = y->getArgs();
      for (std::vector<EqObjPtr>::iterator it =  y_args.begin(); it != y_args.end(); ++it)
      {
        x_args.push_back(*it);
      }
    }
    else
    {
      x_args.push_back(y);
    }

    ret = EqObjPtr(new Add(x_args));
  }
  else if (y->getType() == Eqo::ADD_OBJ)
  {
    std::vector<EqObjPtr> y_args = y->getArgs();
    std::vector<EqObjPtr> x_args;
    x_args.reserve(y_args.size() + 1);
    x_args.push_back(x);
    for (std::vector<EqObjPtr>::iterator it =  y_args.begin(); it != y_args.end(); ++it)
    {
      x_args.push_back(*it);
    }

    ret = EqObjPtr(new Add(x_args));
  }
  else
  {
    ret = EqObjPtr(new Add(x,y));
  }

  return ret;
}

inline EqObjPtr operator- (EqObjPtr x, EqObjPtr y)
{
   return (x + (con(-1)*y));
}

inline EqObjPtr exp (EqObjPtr x)
{
   return EqObjPtr(new Exponent(x));
}

inline EqObjPtr pow (EqObjPtr x, EqObjPtr y)
{
   return EqObjPtr(new Pow(x, y));
}

inline EqObjPtr log (EqObjPtr x)
{
   return EqObjPtr(new Log(x));
}

inline EqObjPtr diff(EqObjPtr x, EqObjPtr y)
{
   return x->Derivative(y);
}

inline EqObjPtr con(double x)
{
   return EqObjPtr(new Constant(x));
}

inline EqObjPtr var(const char *x)
{
   return EqObjPtr(new Variable(std::string(x)));
}

inline EqObjPtr var(const std::string &x)
{
   return EqObjPtr(new Variable(x));
}

inline EqObjPtr sqrt(EqObjPtr x)
{
    return EqObjPtr(pow(x, con(0.5)));
}

inline EqObjPtr reciprocal_sqrt(EqObjPtr x)
{
    return EqObjPtr(pow(x, con(-0.5)));
}

/// how we print things
inline std::ostream & operator<< (std::ostream &os, EqObjPtr foo)
{
   os << foo->stringValue();
   return os;
}

/// keeps simplifying expression until string value doesn't change
inline EqObjPtr Simplify(EqObjPtr x)
{
   std::string y = x->stringValue();
   EqObjPtr z = x->Simplify();
   while (y != z->stringValue())
   {
      y = z -> stringValue();
      z = z -> Simplify();
   }
   return z;
}

inline EqObjPtr getConstantFactor(EqObjPtr x)
{
   return x->getScale();
}

inline EqObjPtr Expand(EqObjPtr x)
{
   std::string str = x->stringValue();
   EqObjPtr    eq = x->expand();
   while (str != eq->stringValue())
   {
       str = eq->stringValue();
       eq = eq->expand();
   }
   return Simplify(eq);
}

inline EqObjPtr getUnscaledValue(EqObjPtr x)
{
   return x->getUnscaledValue();
}

inline EqObjPtr getUnsignedValue(EqObjPtr x)
{
   return x->getUnsignedValue();
}

inline EqObjPtr getSign(EqObjPtr x)
{
   return EqObjPtr(new Constant(x->getSign()));
}

   /// Sorts in order of constants, variables, then other objects
   void SortEqVector(std::vector<EqObjPtr> &v1);

inline EqObjPtr subst(EqObjPtr a, EqObjPtr b, EqObjPtr c)
{
    return a->subst(b->stringValue(), c);
}

inline EqObjPtr getNegation(EqObjPtr x)
{
    EqObjPtr ret = Eqo::con(-1) * x;
    if (x->getType() == Eqo::CONST_OBJ)
    {
        ret = ret->Simplify();
    }
    return ret;
}

}/* end namespace Eqo */

#endif

