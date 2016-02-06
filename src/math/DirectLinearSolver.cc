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

#include "DirectLinearSolver.hh"
#include "Preconditioner.hh"

#include "OutputStream.hh"

#include <sstream>

//#include <iostream>
namespace dsMath {
DirectLinearSolver::DirectLinearSolver()
{}

namespace {
void WriteOutProblem(bool factored, bool solved)
{
  std::ostringstream os;
  if (!factored)
  {
    os << "Matrix factorization failed\n";
  }
  else if (!solved)
  {
    os << "Matrix solve failed\n";
  }
  OutputStream::WriteOut(OutputStream::INFO, os.str());
}
}

bool DirectLinearSolver::SolveImpl(Matrix &mat, Preconditioner &pre, DoubleVec_t &sol, DoubleVec_t &rhs)
{

  std::string what;
  bool ret = false;
  bool solved = false;
//std::cerr << "Begin LUFactor Matrix\n";

  bool factored = pre.LUFactor(&mat);
//std::cerr << "End LUFactor Matrix\n";
//std::cerr << "Begin LUSolve Matrix\n";

  if (factored)
  {
    solved = pre.LUSolve(sol, rhs);
  }

  ret = factored && solved;

  if (!ret)
  {
    WriteOutProblem(factored, solved);
  }

//std::cerr << "End LUFactor Matrix\n";

  return ret;
}

bool DirectLinearSolver::ACSolveImpl(Matrix &mat, Preconditioner &pre, ComplexDoubleVec_t &sol, ComplexDoubleVec_t &rhs)
{
  bool ret = false;
  bool solved = false;

  bool factored = pre.LUFactor(&mat);

  if (factored)
  {
    solved = pre.LUSolve(sol, rhs);
  }

  ret = factored && solved;

  if (!ret)
  {
    WriteOutProblem(factored, solved);
  }

  return ret;
}

bool DirectLinearSolver::NoiseSolveImpl(Matrix &mat, Preconditioner &pre, ComplexDoubleVec_t &sol, ComplexDoubleVec_t &rhs)
{
  bool ret = false;
  bool solved = false;

  bool factored = pre.LUFactor(&mat);

  if (factored)
  {
    solved = pre.LUSolve(sol, rhs);
  }

  ret = factored && solved;

  if (!ret)
  {
    WriteOutProblem(factored, solved);
  }

  return ret;
}
}

