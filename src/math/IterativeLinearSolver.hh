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

#ifndef DS_ITERATIVE_LINEAR_SOLVER_HH
#define DS_ITERATIVE_LINEAR_SOLVER_HH
#include "LinearSolver.hh"

namespace dsMath {
class Matrix;
class Preconditioner;
// Special case
// x = inv(A) b
class IterativeLinearSolver : public LinearSolver
{
   public:
        IterativeLinearSolver();
        ~IterativeLinearSolver() {};
   protected:
   private:
        bool SolveImpl(Matrix &, Preconditioner &, DoubleVec_t &, DoubleVec_t & );
        bool ACSolveImpl(Matrix &, Preconditioner &,  ComplexDoubleVec_t &, ComplexDoubleVec_t & );
        bool NoiseSolveImpl(Matrix &, Preconditioner &, ComplexDoubleVec_t &, ComplexDoubleVec_t & );

        IterativeLinearSolver(const IterativeLinearSolver &);
        IterativeLinearSolver &operator=(const IterativeLinearSolver &);

        int restart_;
        int linear_iterations_;
        double relative_tolerance_;
};
}

#endif
