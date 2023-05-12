/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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
    static double ManualCheckAndRaiseFPE(const double &x);

  private:
    void ClearFP();
    FPECheck();
    FPECheck(const FPECheck &);
    FPECheck &operator=(const FPECheck &);

    static FPECheck::FPEFlag_t fpe_raised_;
};
#endif
