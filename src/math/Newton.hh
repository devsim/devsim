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

#ifndef DS_NEWTON_HH
#define DS_NEWTON_HH
#include "MatrixEntries.hh"
#include "dsMathTypes.hh"
#include "MathEnum.hh"
#include <cstddef>
#include <vector>
#include <complex>

class Device;
/// This is the outer nonlinear solver
namespace dsMath {
class Matrix;
class LinearSolver;
namespace TimeMethods {
    enum TimeMethod_t {DCONLY, INTEGRATEDC, INTEGRATETR, INTEGRATEBDF1, INTEGRATEBDF2};
}

class Newton {
    public:
        typedef std::vector<size_t> permvec_t;

        /// Newton takes on linear solver
        /// near solver selects Preconditioner
        Newton() : maxiter(DefaultMaxIter), absLimit(DefaultAbsError), relLimit(DefaultRelError), qrelLimit(DefaultQRelError), dimension(0) {}
        ~Newton() {};

        //// INTEGRATE_DC means that we are just gonna Assemble I, Q when done

        bool Solve(LinearSolver &, TimeMethods::TimeMethod_t, double tdelta = 0.0, double a0 = 0.0, double a1 = 0.0, double a2 = 0.0, double b0 = 1.0, double b1 = 0.0, double b2 = 0.0);

        bool ACSolve(LinearSolver &, double);

        bool NoiseSolve(const std::string &, LinearSolver &, double);
        //Newton(LinearSolver &iterator);
        void SetAbsError(double x)
        {
            absLimit = x;
        }
        void SetRelError(double x)
        {
            relLimit = x;
        }
        void SetQRelError(double x)
        {
            qrelLimit = x;
        }
        void SetMaxIter(int x)
        {
            maxiter = x;
        }
    protected:
        template <typename T>
        void LoadIntoMatrix(const RealRowColValueVec &rcv, Matrix &matrix, T scl = 1.0, size_t offset = 0);
        template <typename T>
        void LoadIntoMatrixPermutated(const RealRowColValueVec &rcv, Matrix &matrix, const permvec_t &, T scl = 1.0, size_t offset = 0);
        template <typename T>
        void LoadIntoRHS(const RHSEntryVec &, std::vector<T> &, T scl = 1.0, size_t offset = 0);
        template <typename T>
        void LoadIntoRHSPermutated(const RHSEntryVec &, std::vector<T> &, const permvec_t &, T scl = 1.0, size_t offset = 0);
    private:

        size_t NumberEquationsAndSetDimension();

        void BackupSolutions();
        void RestoreSolutions();

        template <typename T>
        void LoadMatrixAndRHS(Matrix &, std::vector<T> &, permvec_t &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode, T);

        //// TODO: may be more efficient to reuse matrix and scale imaginary elements
        void LoadMatrixAndRHSAC(Matrix &, ComplexDoubleVec_t &, permvec_t &, double);
        void LoadCircuitRHSAC(ComplexDoubleVec_t &);

        void LoadMatrixAndRHSOnCircuit(RealRowColValueVec &, RHSEntryVec &rhs, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

        void AssembleContactsAndInterfaces(RealRowColValueVec &, RHSEntryVec &, permvec_t &, Device &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);
        //// This one can't permutate anything
        void AssembleBulk(RealRowColValueVec &, RHSEntryVec &, Device &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

        void AssembleTclEquations(RealRowColValueVec &, RHSEntryVec &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

        static const size_t DefaultMaxIter;
        static const double DefaultAbsError;
        static const double DefaultRelError;
        static const double DefaultQRelError;

        Newton(const Newton &);

        int    maxiter; /// The maximum number of iterations
        double absLimit;  /// The calculated abs error (maybe come on per device or per region basis)
        double relLimit;  /// The calculated rel error
        double qrelLimit;


        size_t dimension;

        //// Work vectors to be reused
#if 0
        RowColValueVec m;
        RHSEntryVec    v;
#endif

        static const double rhssign;
};
}
#endif
