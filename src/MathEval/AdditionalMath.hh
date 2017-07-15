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

#ifndef ADDITIONAL_MATH_HH
#define ADDITIONAL_MATH_HH

template <typename U>
inline U min(U x, U y)
{
  if (x <= y)
  {
    return x;
  }
  else
  {
    return y;
  }
}

template <typename U>
inline U max(U x, U y)
{
  if (x >= y)
  {
    return x;
  }
  else
  {
    return y;
  }
}

template <typename U>
inline U step(U x)
{
    return (x >= 0.0) ? 1.0 : 0.0;
}

template <typename U>
inline U sgn(U x)
{
    return (x >= 0.0) ? 1.0 : -1.0;
}

#endif

