/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef MATHWRAPPER_HH
#define MATHWRAPPER_HH
#include <memory>
#include <vector>
#include <string>

namespace Eqomfp {
template <typename DoubleType>
class MathWrapper;

template <typename DoubleType>
using MathWrapperPtr = std::shared_ptr<MathWrapper<DoubleType>>;

template <typename DoubleType>
using unaryfuncptr = DoubleType (*)(DoubleType);
template <typename DoubleType>
using binaryfuncptr = DoubleType (*)(DoubleType, DoubleType);
template <typename DoubleType>
using ternaryfuncptr = DoubleType (*)(DoubleType, DoubleType, DoubleType);
template <typename DoubleType>
using quaternaryfuncptr = DoubleType (*)(DoubleType, DoubleType, DoubleType, DoubleType);


template <typename DoubleType>
class MathWrapper {
  public:
    MathWrapper(const std::string &name, const size_t narg) : name_(name), nargs_(narg) {};

    void Evaluate(const std::vector<DoubleType> &/*dvals*/, const std::vector<const std::vector<DoubleType> *> &/*vvals*/, std::string &/*error*/, std::vector<DoubleType> &/*result*/, size_t /*vbeg*/, size_t /*vend*/) const;
    DoubleType Evaluate(const std::vector<DoubleType> &/*vals*/, std::string &/*error*/) const;

    virtual ~MathWrapper() = 0;
    size_t GetNumberArguments() const
    {
      return nargs_;
    }

  protected:

    virtual void DerivedEvaluate(const std::vector<DoubleType> &/*dvals*/, const std::vector<const std::vector<DoubleType> *> &/*vvals*/, std::vector<DoubleType> &/*result*/, size_t /*vbeg*/, size_t /*vend*/) const = 0;
    virtual DoubleType DerivedEvaluate(const std::vector<DoubleType> &/*vals*/) const = 0;

  private:
    MathWrapper();
    MathWrapper(const MathWrapper &);
    MathWrapper &operator=(const MathWrapper &);

    std::string  name_;
    const size_t nargs_;
};


//// 1
template <typename DoubleType>
class MathWrapper1 : public MathWrapper<DoubleType> {
  public:
    MathWrapper1(const std::string &name, unaryfuncptr<DoubleType> fptr) : MathWrapper<DoubleType>(name, 1), funcptr_(fptr) {};
    ~MathWrapper1() {}

  protected:

    void DerivedEvaluate(const std::vector<DoubleType> &/*dvals*/, const std::vector<const std::vector<DoubleType> *> &/*vvals*/, std::vector<DoubleType> &/*result*/, size_t /*vbeg*/, size_t /*vend*/) const;
    DoubleType DerivedEvaluate(const std::vector<DoubleType> &/*vals*/) const;

  private:
    unaryfuncptr<DoubleType> funcptr_;
};

//// 2
template <typename DoubleType>
class MathWrapper2 : public MathWrapper<DoubleType> {
  public:
    MathWrapper2(const std::string &name, binaryfuncptr<DoubleType> fptr) : MathWrapper<DoubleType>(name, 2), funcptr_(fptr) {};
    ~MathWrapper2() {}

  protected:

    void DerivedEvaluate(const std::vector<DoubleType> &/*dvals*/, const std::vector<const std::vector<DoubleType> *> &/*vvals*/, std::vector<DoubleType> &/*result*/, size_t /*vbeg*/, size_t /*vend*/) const;
    DoubleType DerivedEvaluate(const std::vector<DoubleType> &/*vals*/) const;

  private:
    binaryfuncptr<DoubleType> funcptr_;
};

//// 3
template <typename DoubleType>
class MathWrapper3 : public MathWrapper<DoubleType> {
  public:
    MathWrapper3(const std::string &name, ternaryfuncptr<DoubleType> fptr) : MathWrapper<DoubleType>(name, 3), funcptr_(fptr) {};
    ~MathWrapper3() {}

  protected:

    void DerivedEvaluate(const std::vector<DoubleType> &/*dvals*/, const std::vector<const std::vector<DoubleType> *> &/*vvals*/, std::vector<DoubleType> &/*result*/, size_t /*vbeg*/, size_t /*vend*/) const;
    DoubleType DerivedEvaluate(const std::vector<DoubleType> &/*vals*/) const;

  private:
    ternaryfuncptr<DoubleType> funcptr_;
};

//// 4
template <typename DoubleType>
class MathWrapper4 : public MathWrapper<DoubleType> {
  public:
    MathWrapper4(const std::string &name, quaternaryfuncptr<DoubleType> fptr) : MathWrapper<DoubleType>(name, 4), funcptr_(fptr) {};
    ~MathWrapper4() {}

  protected:

    void DerivedEvaluate(const std::vector<DoubleType> &/*dvals*/, const std::vector<const std::vector<DoubleType> *> &/*vvals*/, std::vector<DoubleType> &/*result*/, size_t /*vbeg*/, size_t /*vend*/) const;
    DoubleType DerivedEvaluate(const std::vector<DoubleType> &/*vals*/) const;

  private:
    quaternaryfuncptr<DoubleType> funcptr_;
};

template <typename DoubleType>
class PowWrapper : public MathWrapper<DoubleType> {
  public:
    PowWrapper(const std::string &name) : MathWrapper<DoubleType>(name, 2) {};
    ~PowWrapper() {}

  protected:

    void DerivedEvaluate(const std::vector<DoubleType> &/*dvals*/, const std::vector<const std::vector<DoubleType> *> &/*vvals*/, std::vector<DoubleType> &/*result*/, size_t /*vbeg*/, size_t /*vend*/) const;
    DoubleType DerivedEvaluate(const std::vector<DoubleType> &/*vals*/) const;

  private:
};
}
#endif

