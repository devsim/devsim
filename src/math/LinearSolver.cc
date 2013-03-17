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

