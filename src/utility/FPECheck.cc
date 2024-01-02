/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "FPECheck.hh"
#include <signal.h>
#if defined(__linux__) && !defined(__ANDROID__) && !defined(__aarch64__)
#include <fpu_control.h>
#endif
#include <cfenv>
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

#if defined(__linux__) && !defined(__ANDROID__) && !defined(__aarch64__)
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
    feclearexcept(FE_ALL_EXCEPT);
    fpe_raised_ = FPECheck::getClearedFlag();
}

FPECheck::FPEFlag_t FPECheck::getFPEMask()
{
  return (FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW);
}

FPECheck::FPEFlag_t FPECheck::getFPEFlags()
{
  return fetestexcept(getFPEMask()) | fpe_raised_;
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
  return (x & FE_INVALID) != 0;
}

bool FPECheck::IsDivByZero(FPECheck::FPEFlag_t x)
{
  return (x & FE_DIVBYZERO) != 0;
}

bool FPECheck::IsInexact(FPECheck::FPEFlag_t x)
{
  return (x & FE_INEXACT) != 0;
}

bool FPECheck::IsOverflow(FPECheck::FPEFlag_t x)
{
  return (x & FE_OVERFLOW) != 0;
}

bool FPECheck::IsUnderflow(FPECheck::FPEFlag_t x)
{
  return (x & FE_UNDERFLOW) != 0;
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
  feraiseexcept(x);
  fpe_raised_ |= x;
}

double FPECheck::ManualCheckAndRaiseFPE(const double &x)
{
  if (std::isinf(x))
  {
    FPECheck::raiseFPE(FE_OVERFLOW);
  }
  else if (std::isnan(x))
  {
    FPECheck::raiseFPE(FE_INVALID);
  }
  return x;
}

#ifdef TEST_FPE_CODE
#include <iostream>
int main()
{
  FPECheck::InitializeFPE();

  double a = 1e308;
  double b = 1e-308;

  FPECheck::ClearFPE();
  double x = a;
  x /= b;
  std::cerr << x << std::endl;
  if (FPECheck::CheckFPE())
  {
    std::cerr << "There was an FPE" << std::endl;
    std::cerr << FPECheck::getFPEString() << "\n";
  }

  FPECheck::ClearFPE();
  double y = 3.0*4;
  if (FPECheck::CheckFPE())
  {
    std::cerr << "There was an FPE" << std::endl;
    std::cerr << FPECheck::getFPEString() << "\n";
  }

  FPECheck::ClearFPE();
  double z = log(-1);
  if (FPECheck::CheckFPE())
  {
    std::cerr << "There was an FPE" << std::endl;
    std::cerr << FPECheck::getFPEString() << "\n";
  }
}
#endif /*TEST_FPE_CODE*/

