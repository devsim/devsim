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

#ifndef MATHWRAPPER_HH
#define MATHWRAPPER_HH
#include "dsmemory.hh"
#include <vector>
#include <string>

namespace Eqomfp {
class MathWrapper;
typedef std::tr1::shared_ptr<MathWrapper> MathWrapperPtr;

typedef double (*unaryfuncptr)(double);
typedef double (*binaryfuncptr)(double, double);
typedef double (*ternaryfuncptr)(double, double, double);
typedef double (*quaternaryfuncptr)(double, double, double, double);

class MathWrapper {
  public:
    MathWrapper(const std::string &name, const size_t narg) : name_(name), nargs_(narg) {};

    void Evaluate(const std::vector<double> &/*dvals*/, const std::vector<const std::vector<double> *> &/*vvals*/, std::string &/*error*/, std::vector<double> &/*result*/, size_t /*vbeg*/, size_t /*vend*/) const;
    double Evaluate(const std::vector<double> &/*vals*/, std::string &/*error*/) const;

    virtual ~MathWrapper() = 0;
    size_t GetNumberArguments() const
    {
      return nargs_;
    }

  protected:  

    virtual void DerivedEvaluate(const std::vector<double> &/*dvals*/, const std::vector<const std::vector<double> *> &/*vvals*/, std::vector<double> &/*result*/, size_t /*vbeg*/, size_t /*vend*/) const = 0;
    virtual double DerivedEvaluate(const std::vector<double> &/*vals*/) const = 0;

  private:
    MathWrapper();
    MathWrapper(const MathWrapper &);
    MathWrapper &operator=(const MathWrapper &);

    std::string  name_;
    const size_t nargs_;
};


//// 1
class MathWrapper1 : public MathWrapper {
  public:
    MathWrapper1(const std::string &name, unaryfuncptr fptr) : MathWrapper(name, 1), funcptr_(fptr) {};
    ~MathWrapper1() {}

  protected:

    void DerivedEvaluate(const std::vector<double> &/*dvals*/, const std::vector<const std::vector<double> *> &/*vvals*/, std::vector<double> &/*result*/, size_t /*vbeg*/, size_t /*vend*/) const;
    double DerivedEvaluate(const std::vector<double> &/*vals*/) const;

  private:
    unaryfuncptr funcptr_;
};

//// 2
class MathWrapper2 : public MathWrapper {
  public:
    MathWrapper2(const std::string &name, binaryfuncptr fptr) : MathWrapper(name, 2), funcptr_(fptr) {};
    ~MathWrapper2() {}

  protected:

    void DerivedEvaluate(const std::vector<double> &/*dvals*/, const std::vector<const std::vector<double> *> &/*vvals*/, std::vector<double> &/*result*/, size_t /*vbeg*/, size_t /*vend*/) const;
    double DerivedEvaluate(const std::vector<double> &/*vals*/) const;

  private:
    binaryfuncptr funcptr_;
};

//// 3
class MathWrapper3 : public MathWrapper {
  public:
    MathWrapper3(const std::string &name, ternaryfuncptr fptr) : MathWrapper(name, 3), funcptr_(fptr) {};
    ~MathWrapper3() {}

  protected:

    void DerivedEvaluate(const std::vector<double> &/*dvals*/, const std::vector<const std::vector<double> *> &/*vvals*/, std::vector<double> &/*result*/, size_t /*vbeg*/, size_t /*vend*/) const;
    double DerivedEvaluate(const std::vector<double> &/*vals*/) const;

  private:
    ternaryfuncptr funcptr_;
};

//// 4
class MathWrapper4 : public MathWrapper {
  public:
    MathWrapper4(const std::string &name, quaternaryfuncptr fptr) : MathWrapper(name, 4), funcptr_(fptr) {};
    ~MathWrapper4() {}

  protected:

    void DerivedEvaluate(const std::vector<double> &/*dvals*/, const std::vector<const std::vector<double> *> &/*vvals*/, std::vector<double> &/*result*/, size_t /*vbeg*/, size_t /*vend*/) const;
    double DerivedEvaluate(const std::vector<double> &/*vals*/) const;

  private:
    quaternaryfuncptr funcptr_;
};

class PowWrapper : public MathWrapper {
  public:
    PowWrapper(const std::string &name) : MathWrapper(name, 2) {};
    ~PowWrapper() {}

  protected:

    void DerivedEvaluate(const std::vector<double> &/*dvals*/, const std::vector<const std::vector<double> *> &/*vvals*/, std::vector<double> &/*result*/, size_t /*vbeg*/, size_t /*vend*/) const;
    double DerivedEvaluate(const std::vector<double> &/*vals*/) const;

  private:
};
}
#endif

