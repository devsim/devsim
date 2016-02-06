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

#ifndef DS_DIRECT_LINEAR_SOLVER_HH
#define DS_DIRECT_LINEAR_SOLVER_HH
#include "LinearSolver.hh"

namespace dsMath {
class Matrix;
class Preconditioner;
// Special case
// x = inv(A) b
class DirectLinearSolver : public LinearSolver
{
   public:
        DirectLinearSolver();
        ~DirectLinearSolver() {};
   protected:
   private:
        bool SolveImpl(Matrix &, Preconditioner &, DoubleVec_t &, DoubleVec_t & );
        bool ACSolveImpl(Matrix &, Preconditioner &,  ComplexDoubleVec_t &, ComplexDoubleVec_t & );
        bool NoiseSolveImpl(Matrix &, Preconditioner &, ComplexDoubleVec_t &, ComplexDoubleVec_t & );

        DirectLinearSolver(const DirectLinearSolver &);
        DirectLinearSolver &operator=(const DirectLinearSolver &);
};
}

#endif
