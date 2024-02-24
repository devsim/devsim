/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#if !defined(USE_ITERATIVE_SOLVER)
#error "Do not include if USE_ITERATIVE_SOLVER is not set"
#endif

#ifndef DS_ITERATIVE_LINEAR_SOLVER_HH
#define DS_ITERATIVE_LINEAR_SOLVER_HH
#include "LinearSolver.hh"

namespace dsMath {
// Special case
// x = inv(A) b
template <typename DoubleType>
class IterativeLinearSolver : public LinearSolver<DoubleType>
{
   public:
        IterativeLinearSolver();
        ~IterativeLinearSolver() {};
   protected:
   private:
        bool SolveImpl(Matrix<DoubleType> &, Preconditioner<DoubleType> &, DoubleVec_t<DoubleType> &, DoubleVec_t<DoubleType> & );
        bool ACSolveImpl(Matrix<DoubleType> &, Preconditioner<DoubleType> &,  ComplexDoubleVec_t<DoubleType> &, ComplexDoubleVec_t<DoubleType> & );
        bool NoiseSolveImpl(Matrix<DoubleType> &, Preconditioner<DoubleType> &, ComplexDoubleVec_t<DoubleType> &, ComplexDoubleVec_t<DoubleType> & );

        IterativeLinearSolver(const IterativeLinearSolver &);
        IterativeLinearSolver &operator=(const IterativeLinearSolver &);

        int restart_;
        int linear_iterations_;
        DoubleType relative_tolerance_;
};
}
#endif

