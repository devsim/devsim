/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef MATH_EVAL_HH
#define MATH_EVAL_HH

#include "MathWrapper.hh"

#include <vector>
#include <string>
#include <map>

class ObjectHolder;

template <typename DoubleType>
class MathEval {
// first is function name
// second is args
// third is error string
  public:
    DoubleType EvaluateMathFunc(const std::string &, std::vector<DoubleType> &, std::string &) const ;
    void   EvaluateMathFunc(const std::string &, std::vector<DoubleType> &, const std::vector<const std::vector<DoubleType> *> &, std::string &, std::vector<DoubleType> &, size_t vlen) const;

    void   EvaluateTclMathFunc(const std::string &, std::vector<DoubleType> &, const std::vector<const std::vector<DoubleType> *> &, std::string &, std::vector<DoubleType> &) const;

    static MathEval &GetInstance();
    static void DestroyInstance();

    bool AddTclMath(const std::string &, ObjectHolder, size_t, std::string & /*error_string*/);
    void RemoveTclMath(const std::string &);

  private:
    MathEval();
    ~MathEval();
    MathEval &operator=(const MathEval &);
    MathEval(const MathEval &);

    static MathEval *instance_;
    std::map<std::string, Eqomfp::MathWrapperPtr<DoubleType>> FuncPtrMap_;

    typedef std::map<std::string, std::pair<ObjectHolder, size_t> > tclMathFuncMap_t;
    tclMathFuncMap_t                      tclMathFuncMap_;

    void InitializeBuiltInMathFunc();
};
#endif

