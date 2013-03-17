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

#ifndef FPE_CHECK_HH
#define FPE_CHECK_HH
#include <string>
class FPECheck
{
  public:
    typedef  int FPEFlag_t;
    static void InitializeFPE();
    static void ClearFPE();
    static bool CheckFPE();
    static bool CheckFPE(FPEFlag_t);
    static std::string getFPEString();

    static std::string getFPEString(FPECheck::FPEFlag_t);
    static FPECheck::FPEFlag_t   getFPEFlags();
    static FPECheck::FPEFlag_t   combineFPEFlags(FPECheck::FPEFlag_t, FPECheck::FPEFlag_t);

    static bool IsUnderflow(FPECheck::FPEFlag_t);
    static bool IsOverflow(FPECheck::FPEFlag_t);
    static bool IsInexact(FPECheck::FPEFlag_t);
    static bool IsDivByZero(FPECheck::FPEFlag_t);
    static bool IsInvalid(FPECheck::FPEFlag_t);

    static FPECheck::FPEFlag_t getFPEMask();

    static FPECheck::FPEFlag_t getClearedFlag();

    static void raiseFPE(FPECheck::FPEFlag_t);

  private:
    void ClearFP();
    FPECheck();
    FPECheck(const FPECheck &);
    FPECheck &operator=(const FPECheck &);

    static FPECheck::FPEFlag_t fpe_raised_;
};
#endif
