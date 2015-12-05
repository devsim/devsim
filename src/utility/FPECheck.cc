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

#include "FPECheck.hh"
#include <signal.h>
#ifdef __linux__
#include <fpu_control.h>
#include <fenv.h>
#endif
#ifdef _WIN32
#include <Float.h>
#endif
#ifdef __APPLE__
#include <fenv.h>
#endif
#include <cmath>
#include <cassert>

FPECheck::FPEFlag_t FPECheck::fpe_raised_ = 0;

#ifndef _WIN32
void fpehandle(int)
{
  assert(0);
}
#endif

void FPECheck::InitializeFPE()
{
    /// Prevent the signal handler from trapping the exception and aborting
    /// This has no effect unless there is an feenableexcept
#ifndef _WIN32
    signal(SIGFPE, fpehandle);
    ////// THIS IS TO CAUSE THE FPE TO TRIGGER A SIGNAL
#if 0
    int x=fegetexcept();
    feenableexcept(x| FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW);
#endif
#endif

#ifdef __linux__
//  http://www.wrcad.com/linux_numerics.txt
/*
    This puts the X86 FPU in 64-bit precision mode.  The default
    under Linux is to use 80-bit mode, which produces subtle
    differences from FreeBSD and other systems, eg,
    (int)(1000*atof("0.3")) is 300 in 64-bit mode, 299 in 80-bit
    mode.
*/
#if 1
    fpu_control_t cw;
    _FPU_GETCW(cw);
    cw &= ~_FPU_EXTENDED;
    cw |= _FPU_DOUBLE;
    _FPU_SETCW(cw);
#endif
#endif

    /// Set the flags we want to catch
    ClearFPE();
}


void FPECheck::ClearFPE()
{
#ifdef _WIN32
    _clearfp();
#else
    feclearexcept(FE_ALL_EXCEPT);
#endif
    fpe_raised_ = FPECheck::getClearedFlag();
}

FPECheck::FPEFlag_t FPECheck::getFPEMask()
{
#ifdef _WIN32
  return (EM_INVALID | EM_ZERODIVIDE | EM_OVERFLOW);
#else
  return (FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW);
#endif
}

FPECheck::FPEFlag_t FPECheck::getFPEFlags()
{
#ifdef _WIN32
  return (_statusfp() & getFPEMask()) | fpe_raised_;
#else
  return fetestexcept(getFPEMask()) | fpe_raised_;
#endif
}

bool FPECheck::CheckFPE()
{
  return getFPEFlags() != 0;
}

bool FPECheck::CheckFPE(FPECheck::FPEFlag_t x)
{
  return (x & getFPEMask()) != 0;
}

bool FPECheck::IsInvalid(FPECheck::FPEFlag_t x)
{
#ifdef _WIN32
  return (x & EM_INVALID) != 0;
#else
  return (x & FE_INVALID) != 0;
#endif
}

bool FPECheck::IsDivByZero(FPECheck::FPEFlag_t x)
{
#ifdef _WIN32
  return (x & EM_ZERODIVIDE) != 0;
#else
  return (x & FE_DIVBYZERO) != 0;
#endif
}

bool FPECheck::IsInexact(FPECheck::FPEFlag_t x)
{
#ifdef _WIN32
  return (x & EM_INEXACT) != 0;
#else
  return (x & FE_INEXACT) != 0;
#endif
}

bool FPECheck::IsOverflow(FPECheck::FPEFlag_t x)
{
#ifdef _WIN32
  return (x & EM_OVERFLOW) != 0;
#else
  return (x & FE_OVERFLOW) != 0;
#endif
}

bool FPECheck::IsUnderflow(FPECheck::FPEFlag_t x)
{
#ifdef _WIN32
  return (x & EM_UNDERFLOW) != 0;
#else
  return (x & FE_UNDERFLOW) != 0;
#endif
}

std::string FPECheck::getFPEString(const FPECheck::FPEFlag_t feFlags)
{
  std::string out;
  if (IsInvalid(feFlags))
  {
    out += "Invalid";
  }

  if (IsDivByZero(feFlags))
  {
    if (!out.empty())
    {
      out += ", ";
    }
    
    out += "Divide-by-zero";
  }

  if (IsInexact(feFlags))
  {
    if (!out.empty())
    {
      out += ", ";
    }
    out += "Inexact";
  }

  if (IsOverflow(feFlags))
  {
    if (!out.empty())
    {
      out += ", ";
    }
    out += "Overflow";
  }

  if (IsUnderflow(feFlags))
  {
    if (!out.empty())
    {
      out += ", ";
    }
    out += "Underflow";
  }

  return out;
}

std::string FPECheck::getFPEString()
{
  const FPECheck::FPEFlag_t feFlags = getFPEFlags();
  return getFPEString(feFlags);
}

FPECheck::FPEFlag_t FPECheck::getClearedFlag()
{
  return 0;
}

FPECheck::FPEFlag_t FPECheck::combineFPEFlags(FPECheck::FPEFlag_t x, FPECheck::FPEFlag_t y)
{
  return x | y;
}

void FPECheck::raiseFPE(FPECheck::FPEFlag_t x)
{
  /// consider making this mutexed
  fpe_raised_ |= x;
}

#ifdef TEST_FPE_CODE
#include <iostream>
int main()
{
  FPECheck::InitializeFPE();

  double a = 1;
  double b = 0;

  FPECheck::ClearFPE();
  double x = a/b;
  if (FPECheck::CheckFPE())
  {
    std::cerr << "There was an FPE" << std::endl;
    std::cerr << FPECheck::getFPEString();
  }

  FPECheck::ClearFPE();
  double y = 3.0*4;
  if (FPECheck::CheckFPE())
  {
    std::cerr << "There was an FPE" << std::endl;
    std::cerr << FPECheck::getFPEString();
  }

  FPECheck::ClearFPE();
  double z = log(0.0001);
  if (FPECheck::CheckFPE())
  {
    std::cerr << "There was an FPE" << std::endl;
    std::cerr << FPECheck::getFPEString();
  }
}
#endif /*TEST_FPE_CODE*/

