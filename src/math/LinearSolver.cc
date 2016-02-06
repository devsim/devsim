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

#include "LinearSolver.hh"
#include "Preconditioner.hh"

#include "OutputStream.hh"
#include "dsTimer.hh"

#include <sstream>

//#include <iostream>
namespace dsMath {
LinearSolver::LinearSolver()
{}

LinearSolver::~LinearSolver()
{}

bool LinearSolver::Solve(Matrix &m, Preconditioner &p, DoubleVec_t &x, DoubleVec_t &b)
{
  dsTimer timer("LinearSolve");
  return this->SolveImpl(m, p, x, b);
}

bool LinearSolver::ACSolve(Matrix &m, Preconditioner &p, ComplexDoubleVec_t &x, ComplexDoubleVec_t &b)
{
  dsTimer timer("ACLinearSolve");
  return this->ACSolveImpl(m, p, x, b);
}

bool LinearSolver::NoiseSolve(Matrix &m, Preconditioner &p, ComplexDoubleVec_t &x, ComplexDoubleVec_t &b)
{
  dsTimer timer("ACLinearSolve");
  return this->NoiseSolveImpl(m, p, x, b);
}
}

