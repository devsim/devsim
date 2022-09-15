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
template <typename DoubleType> class Matrix;
template <typename DoubleType> class Preconditioner;
/// This is the linear solver (inside the newton loop)
template <typename DoubleType> class LinearSolver {
public:
  virtual ~LinearSolver() = 0;

  bool Solve(Matrix<DoubleType> &, Preconditioner<DoubleType> &,
             std::vector<DoubleType> &, std::vector<DoubleType> &);
  bool ACSolve(Matrix<DoubleType> &, Preconditioner<DoubleType> &,
               std::vector<std::complex<DoubleType>> &,
               std::vector<std::complex<DoubleType>> &);
  bool NoiseSolve(Matrix<DoubleType> &, Preconditioner<DoubleType> &,
                  std::vector<std::complex<DoubleType>> &,
                  std::vector<std::complex<DoubleType>> &);

protected:
  LinearSolver();

private:
  virtual bool SolveImpl(Matrix<DoubleType> &, Preconditioner<DoubleType> &,
                         std::vector<DoubleType> &,
                         std::vector<DoubleType> &) = 0;
  virtual bool ACSolveImpl(Matrix<DoubleType> &, Preconditioner<DoubleType> &,
                           std::vector<std::complex<DoubleType>> &,
                           std::vector<std::complex<DoubleType>> &) = 0;
  virtual bool NoiseSolveImpl(Matrix<DoubleType> &,
                              Preconditioner<DoubleType> &,
                              std::vector<std::complex<DoubleType>> &,
                              std::vector<std::complex<DoubleType>> &) = 0;

  LinearSolver(const LinearSolver &);
  LinearSolver &operator=(const LinearSolver &);
};
} // namespace dsMath

#endif
