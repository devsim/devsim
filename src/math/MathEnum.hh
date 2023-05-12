/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef MATH_ENUM
#define MATH_ENUM
namespace dsMathEnum {
enum class WhatToLoad {MATRIXONLY, RHS, MATRIXANDRHS, PERMUTATIONSONLY};
enum class TimeMode   {DC, TIME};

extern const char *WhatToLoadString[];
extern const char *TimeModeString[];

}
#endif

