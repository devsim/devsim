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
class Matrix;
class Preconditioner;
/// This is the linear solver (inside the newton loop)
class LinearSolver {
    public:
       virtual ~LinearSolver() = 0;

       bool Solve(Matrix &, Preconditioner &, DoubleVec_t &, DoubleVec_t & );
       bool ACSolve(Matrix &, Preconditioner &, ComplexDoubleVec_t &, ComplexDoubleVec_t & );
       bool NoiseSolve(Matrix &, Preconditioner &, ComplexDoubleVec_t &, ComplexDoubleVec_t & );

    protected:
        LinearSolver();
    private:
       virtual bool SolveImpl(Matrix &, Preconditioner &, DoubleVec_t &, DoubleVec_t & )=0;
       virtual bool ACSolveImpl(Matrix &, Preconditioner &, ComplexDoubleVec_t &, ComplexDoubleVec_t & )=0;
       virtual bool NoiseSolveImpl(Matrix &, Preconditioner &, ComplexDoubleVec_t &, ComplexDoubleVec_t & )=0;

        LinearSolver(const LinearSolver &);
        LinearSolver &operator=(const LinearSolver &);

};
}

#endif
