/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "DirectLinearSolver.hh"
#include "Preconditioner.hh"

#include "OutputStream.hh"

#include <sstream>

//#include <iostream>
namespace dsMath {
template <typename DoubleType>
DirectLinearSolver<DoubleType>::DirectLinearSolver()
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
  OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
}
}

template <typename DoubleType>
bool DirectLinearSolver<DoubleType>::SolveImpl(Matrix<DoubleType> &mat, Preconditioner<DoubleType> &pre, std::vector<DoubleType> &sol, std::vector<DoubleType> &rhs)
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

template <typename DoubleType>
bool DirectLinearSolver<DoubleType>::ACSolveImpl(Matrix<DoubleType> &mat, Preconditioner<DoubleType> &pre, ComplexDoubleVec_t<DoubleType> &sol, ComplexDoubleVec_t<DoubleType> &rhs)
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

template <typename DoubleType>
bool DirectLinearSolver<DoubleType>::NoiseSolveImpl(Matrix<DoubleType> &mat, Preconditioner<DoubleType> &pre, ComplexDoubleVec_t<DoubleType> &sol, ComplexDoubleVec_t<DoubleType> &rhs)
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

template class dsMath::DirectLinearSolver<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class dsMath::DirectLinearSolver<float128>;
#endif

