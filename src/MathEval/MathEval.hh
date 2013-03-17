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
