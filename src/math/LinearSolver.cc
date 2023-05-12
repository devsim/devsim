/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "LinearSolver.hh"
#include "Preconditioner.hh"

#include "OutputStream.hh"
#include "dsTimer.hh"

#include <sstream>

//#include <iostream>
namespace dsMath {
template <typename DoubleType>
LinearSolver<DoubleType>::LinearSolver()
{}

template <typename DoubleType>
LinearSolver<DoubleType>::~LinearSolver()
{}

template <typename DoubleType>
bool LinearSolver<DoubleType>::Solve(Matrix<DoubleType> &m, Preconditioner<DoubleType> &p, std::vector<DoubleType> &x, std::vector<DoubleType> &b)
{
  dsTimer timer("LinearSolve");
  return this->SolveImpl(m, p, x, b);
}

template <typename DoubleType>
bool LinearSolver<DoubleType>::ACSolve(Matrix<DoubleType> &m, Preconditioner<DoubleType> &p, std::vector<std::complex<DoubleType>> &x, std::vector<std::complex<DoubleType>> &b)
{
  dsTimer timer("ACLinearSolve");
  return this->ACSolveImpl(m, p, x, b);
}

template <typename DoubleType>
bool LinearSolver<DoubleType>::NoiseSolve(Matrix<DoubleType> &m, Preconditioner<DoubleType> &p, std::vector<std::complex<DoubleType>> &x, std::vector<std::complex<DoubleType>> &b)
{
  dsTimer timer("ACLinearSolve");
  return this->NoiseSolveImpl(m, p, x, b);
}
}

template class dsMath::LinearSolver<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class dsMath::LinearSolver<float128>;
#endif

