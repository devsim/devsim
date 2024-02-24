/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef DS_LINEAR_SOLVER_HH
#define DS_LINEAR_SOLVER_HH
#include "dsMathTypes.hh"

// This is the base class for all nonlinear solver algorithms
// Actually there really need to only 1 class since the iterative solvers
// are abstractions of the direct solver
//
// The responsibilities are
//
// 0. Create Preconditioner
// 1. Factor Preconditioner
// 2. Minimize r=A inv(P) y - b
// 3. return x = inv(P) y

namespace dsMath {
template <typename DoubleType>
class Matrix;
template <typename DoubleType>
class Preconditioner;
/// This is the linear solver (inside the newton loop)
template <typename DoubleType>
class LinearSolver {
    public:
       virtual ~LinearSolver() = 0;

       bool Solve(Matrix<DoubleType> &, Preconditioner<DoubleType> &, DoubleVec_t<DoubleType> &, DoubleVec_t<DoubleType> & );
       bool ACSolve(Matrix<DoubleType> &, Preconditioner<DoubleType> &, ComplexDoubleVec_t<DoubleType> &, ComplexDoubleVec_t<DoubleType> & );
       bool NoiseSolve(Matrix<DoubleType> &, Preconditioner<DoubleType> &, ComplexDoubleVec_t<DoubleType> &, ComplexDoubleVec_t<DoubleType> & );

    protected:
        LinearSolver();
    private:
       virtual bool SolveImpl(Matrix<DoubleType> &, Preconditioner<DoubleType> &, DoubleVec_t<DoubleType> &, DoubleVec_t<DoubleType> & )=0;
       virtual bool ACSolveImpl(Matrix<DoubleType> &, Preconditioner<DoubleType> &, ComplexDoubleVec_t<DoubleType> &, ComplexDoubleVec_t<DoubleType> & )=0;
       virtual bool NoiseSolveImpl(Matrix<DoubleType> &, Preconditioner<DoubleType> &, ComplexDoubleVec_t<DoubleType> &, ComplexDoubleVec_t<DoubleType> & )=0;

       LinearSolver(const LinearSolver &);
       LinearSolver &operator=(const LinearSolver &);

};
}

#endif

