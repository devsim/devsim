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

#ifndef DS_NEWTON_HH
#define DS_NEWTON_HH
#include "MatrixEntries.hh"
#include "dsMathTypes.hh"
#include "MathEnum.hh"
#include <cstddef>
#include <vector>
#include <complex>
#include <map>

class PermutationEntry;

class ObjectHolder;
typedef std::map<std::string, ObjectHolder> ObjectHolderMap_t;

class Device;
/// This is the outer nonlinear solver
namespace dsMath {
template <typename DoubleType>
class Matrix;

template <typename DoubleType>
class LinearSolver;

namespace TimeMethods {
    enum class TimeMethod_t {DCONLY, INTEGRATEDC, INTEGRATETR, INTEGRATEBDF1, INTEGRATEBDF2};

    template <typename DoubleType>
    struct TimeParams {
      TimeParams(TimeMethod_t tm, DoubleType ts, DoubleType g) : method(tm), tstep(ts), gamma(g), tdelta(0.0), a0(0.0), a1(0.0), a2(0.0), b0(1.0), b1(0.0), b2(0.0) {
      }

    // strictly dc
    bool IsDCOnly() const {
      return (method == TimeMethod_t::DCONLY);
    }
    // could be dc transient too.
    bool IsDCMethod() const {
      return (method == TimeMethod_t::DCONLY) || (method == TimeMethod_t::INTEGRATEDC);
    }

    // can be dc transient too
    bool IsTransient() const {
      return (method != TimeMethod_t::DCONLY);
    }

    bool IsIntegration() const {
      return (method == TimeMethod_t::INTEGRATEBDF1) || (method == TimeMethod_t::INTEGRATETR) || (method == TimeMethod_t::INTEGRATEBDF2);
    }
      TimeMethod_t method;
      DoubleType tstep;
      DoubleType gamma;
      DoubleType tdelta;
      DoubleType a0;
      DoubleType a1;
      DoubleType a2;
      DoubleType b0;
      DoubleType b1;
      DoubleType b2;
    };

template <typename DoubleType>
struct DCOnly : public TimeParams<DoubleType>
{
  DCOnly() : TimeParams<DoubleType>(TimeMethod_t::DCONLY, 0.0, 0.0)
  {
    TimeParams<DoubleType>::b0 = 1.0;
  }
};

template <typename DoubleType>
struct TransientDC : public TimeParams<DoubleType>
{
  TransientDC() : TimeParams<DoubleType>(TimeMethod_t::INTEGRATEDC, 0.0, 0.0)
  {
    TimeParams<DoubleType>::b0 = 1.0;
  }
};

template <typename DoubleType>
struct BDF1 : public TimeParams<DoubleType>
{
  BDF1(DoubleType tstep, DoubleType gamma) : TimeParams<DoubleType>(TimeMethod_t::INTEGRATEBDF1, tstep, gamma)
  {
    tdelta = gamma * tstep;
    const DoubleType tf = 1.0 / tdelta;
    a0 = tf;
    a1 = -tf;
    b0 = 1.0;
  }
  using TimeParams<DoubleType>::tdelta;
  using TimeParams<DoubleType>::a0;
  using TimeParams<DoubleType>::a1;
  using TimeParams<DoubleType>::b0;
};

template <typename DoubleType>
struct BDF2 : public TimeParams<DoubleType>
{
  BDF2(DoubleType tstep, DoubleType gamma) : TimeParams<DoubleType>(TimeMethod_t::INTEGRATEBDF2, tstep, gamma)
  {
    //// td for first order projection
    tdelta = (1.0 - gamma) * tstep;
    a0 = (2.0 - gamma) / tdelta;
    a1 = (-1.0) / (gamma * tdelta);
    a2 = (1.0 - gamma) / (gamma * tstep);
    b0 = 1.0;
  }
  using TimeParams<DoubleType>::tdelta;
  using TimeParams<DoubleType>::a0;
  using TimeParams<DoubleType>::a1;
  using TimeParams<DoubleType>::a2;
  using TimeParams<DoubleType>::b0;
};

template <typename DoubleType>
struct TR : public TimeParams<DoubleType>
{
  TR(DoubleType tstep, DoubleType gamma) : TimeParams<DoubleType>(TimeMethod_t::INTEGRATETR, tstep, gamma)
  {
    tdelta = gamma * tstep;
    const DoubleType tf = 2.0 / tdelta;
    a0 = tf;
    a1 = -tf;
    b0 = 1.0;
    b1 = -1.0;
  }
  using TimeParams<DoubleType>::tdelta;
  using TimeParams<DoubleType>::a0;
  using TimeParams<DoubleType>::a1;
  using TimeParams<DoubleType>::b0;
  using TimeParams<DoubleType>::b1;
};

}

template <typename DoubleType>
class Newton {
    public:
        typedef std::vector<PermutationEntry> permvec_t;

        /// Newton takes on linear solver
        /// near solver selects Preconditioner
        Newton() : maxiter(DefaultMaxIter), absLimit(DefaultAbsError), relLimit(DefaultRelError), qrelLimit(DefaultQRelError), dimension(0) {}
        ~Newton() {};

        //// INTEGRATE_DC means that we are just gonna Assemble I, Q when done

        bool Solve(LinearSolver<DoubleType> &, const TimeMethods::TimeParams<DoubleType> &, ObjectHolderMap_t *ohm);

        bool ACSolve(LinearSolver<DoubleType> &, DoubleType);

        bool NoiseSolve(const std::string &, LinearSolver<DoubleType> &, DoubleType);
        //Newton(LinearSolver<DoubleType> &iterator);
        void SetAbsError(DoubleType x)
        {
            absLimit = x;
        }
        void SetRelError(DoubleType x)
        {
            relLimit = x;
        }
        void SetQRelError(DoubleType x)
        {
            qrelLimit = x;
        }
        void SetMaxIter(size_t x)
        {
            maxiter = x;
        }
    protected:
        template <typename T>
        void LoadIntoMatrix(const RealRowColValueVec<DoubleType> &rcv, Matrix<DoubleType> &matrix, T scl = 1.0, size_t offset = 0);
        template <typename T>
        void LoadIntoMatrixPermutated(const RealRowColValueVec<DoubleType> &rcv, Matrix<DoubleType> &matrix, const permvec_t &, T scl = 1.0, size_t offset = 0);
        template <typename T>
        void LoadIntoRHS(const RHSEntryVec<DoubleType> &, std::vector<T> &, T scl = 1.0, size_t offset = 0);
        template <typename T>
        void LoadIntoRHSPermutated(const RHSEntryVec<DoubleType> &, std::vector<T> &, const permvec_t &, T scl = 1.0, size_t offset = 0);
    private:
        void InitializeTransientAssemble(const TimeMethods::TimeParams<DoubleType> &, size_t, std::vector<DoubleType> &);
        bool CheckTransientProjection(const TimeMethods::TimeParams<DoubleType> &, const std::vector<DoubleType> &);
        void UpdateTransientCurrent(const TimeMethods::TimeParams<DoubleType> &, size_t, const std::vector<DoubleType> &, std::vector<DoubleType> &);

        void PrintDeviceErrors(const Device &device, ObjectHolderMap_t *);
        void PrintCircuitErrors(ObjectHolderMap_t *);
        void PrintNumberEquations(size_t, ObjectHolderMap_t *);
        void PrintIteration(size_t, ObjectHolderMap_t *);

        size_t NumberEquationsAndSetDimension();

        void BackupSolutions();
        void RestoreSolutions();

        template <typename T>
        void LoadMatrixAndRHS(Matrix<DoubleType> &, std::vector<T> &, permvec_t &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode, T);

        //// TODO: may be more efficient to reuse matrix and scale imaginary elements
        void LoadMatrixAndRHSAC(Matrix<DoubleType> &, std::vector<std::complex<DoubleType>> &, permvec_t &, DoubleType);
        void LoadCircuitRHSAC(std::vector<std::complex<DoubleType>> &);

        void LoadMatrixAndRHSOnCircuit(RealRowColValueVec<DoubleType> &, RHSEntryVec<DoubleType> &rhs, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

        void AssembleContactsAndInterfaces(RealRowColValueVec<DoubleType> &, RHSEntryVec<DoubleType> &, permvec_t &, Device &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);
        //// This one can't permutate anything
        void AssembleBulk(RealRowColValueVec<DoubleType> &, RHSEntryVec<DoubleType> &, Device &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

        void AssembleTclEquations(RealRowColValueVec<DoubleType> &, RHSEntryVec<DoubleType> &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

        static const size_t DefaultMaxIter;
        static const DoubleType DefaultAbsError;
        static const DoubleType DefaultRelError;
        static const DoubleType DefaultQRelError;

        Newton(const Newton &);

        size_t maxiter; /// The maximum number of iterations
        DoubleType absLimit;  /// The calculated abs error (maybe come on per device or per region basis)
        DoubleType relLimit;  /// The calculated rel error
        DoubleType qrelLimit;


        size_t dimension;

        static const DoubleType rhssign;
};
}
#endif

