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

#ifndef MATH_EVAL_HH
#define MATH_EVAL_HH

#include "MathWrapper.hh"

#include <vector>
#include <string>
#include <map>

class ObjectHolder;

class MathEval {
// first is function name
// second is args
// third is error string
  public:
    double EvaluateMathFunc(const std::string &, std::vector<double> &, std::string &) const ;
    void   EvaluateMathFunc(const std::string &, std::vector<double> &, const std::vector<const std::vector<double> *> &, std::string &, std::vector<double> &, size_t vlen) const;

    void   EvaluateTclMathFunc(const std::string &, std::vector<double> &, const std::vector<const std::vector<double> *> &, std::string &, std::vector<double> &) const;

    static MathEval &GetInstance();
    static void DestroyInstance();

    void AddTclMath(const std::string &, size_t);
    void RemoveTclMath(const std::string &);

  private:
    MathEval(); 
    ~MathEval();
    MathEval &operator=(const MathEval &);
    MathEval(const MathEval &);

    static MathEval *instance_;
    //// TODO: real pointer, not smart pointer (delete in destructor for this class
    std::map<std::string, Eqomfp::MathWrapperPtr> FuncPtrMap_;
//    std::map<std::string, binaryfuncptr>     binaryFuncPtrMap_;
//    std::map<std::string, ternaryfuncptr>    ternaryFuncPtrMap_;
//    std::map<std::string, quaternaryfuncptr> quaternaryFuncPtrMap_;

    //// This has priority
    typedef std::map<std::string, size_t> tclMathFuncMap_t;
    tclMathFuncMap_t                      tclMathFuncMap_;
    mutable std::vector<ObjectHolder>     tclObjVector;

#if 0
    unaryfuncptr   GetUnaryFuncPtr(const std::string &x) const;
    binaryfuncptr  GetBinaryFuncPtr(const std::string &x) const;
    ternaryfuncptr GetTernaryFuncPtr(const std::string &x) const;
    quaternaryfuncptr GetQuaternaryFuncPtr(const std::string &x) const;
#endif

    void InitializeBuiltInMathFunc();
};
#endif
