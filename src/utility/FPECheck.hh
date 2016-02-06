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
