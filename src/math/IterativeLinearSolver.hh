/***
DEVSIM
Copyright 2013 DEVSIM LLC

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
        bool SolveImpl(Matrix<DoubleType> &, Preconditioner<DoubleType> &, std::vector<DoubleType> &, std::vector<DoubleType> & );
        bool ACSolveImpl(Matrix<DoubleType> &, Preconditioner<DoubleType> &,  std::vector<std::complex<DoubleType>> &, std::vector<std::complex<DoubleType>> & );
        bool NoiseSolveImpl(Matrix<DoubleType> &, Preconditioner<DoubleType> &, std::vector<std::complex<DoubleType>> &, std::vector<std::complex<DoubleType>> & );

        IterativeLinearSolver(const IterativeLinearSolver &);
        IterativeLinearSolver &operator=(const IterativeLinearSolver &);

        int restart_;
        int linear_iterations_;
        DoubleType relative_tolerance_;
};
}
#endif

