/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef DS_DIRECT_LINEAR_SOLVER_HH
#define DS_DIRECT_LINEAR_SOLVER_HH
#include "LinearSolver.hh"

namespace dsMath {
// Special case
// x = inv(A) b
template <typename DoubleType>
class DirectLinearSolver : public LinearSolver<DoubleType>
{
   public:
        DirectLinearSolver();
        ~DirectLinearSolver() {};
   protected:
   private:
        bool SolveImpl(Matrix<DoubleType> &, Preconditioner<DoubleType> &, std::vector<DoubleType> &, std::vector<DoubleType> & );
        bool ACSolveImpl(Matrix<DoubleType> &, Preconditioner<DoubleType> &,  std::vector<std::complex<DoubleType>> &, std::vector<std::complex<DoubleType>> & );
        bool NoiseSolveImpl(Matrix<DoubleType> &, Preconditioner<DoubleType> &, std::vector<std::complex<DoubleType>> &, std::vector<std::complex<DoubleType>> & );

        DirectLinearSolver(const DirectLinearSolver &);
        DirectLinearSolver &operator=(const DirectLinearSolver &);
};
}

#endif

