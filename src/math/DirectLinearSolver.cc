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

