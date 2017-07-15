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

#ifndef MISC_MATH_FUNC_HH
#define MISC_MATH_FUNC_HH
template <typename DoubleType>
DoubleType derfdx(DoubleType);

template <typename DoubleType>
DoubleType derfcdx(DoubleType);

template <typename DoubleType>
DoubleType vec_sum(DoubleType);

template <typename DoubleType>
DoubleType vec_max(DoubleType);

template <typename DoubleType>
DoubleType vec_min(DoubleType);

template <typename DoubleType>
DoubleType dot2dfunc(DoubleType, DoubleType, DoubleType, DoubleType);

#endif
