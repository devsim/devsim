/***
DEVSIM
Copyright 2021 Devsim LLC

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
#ifndef BOOST_MATH_SPECIAL_HH
#define BOOST_MATH_SPECIAL_HH
#include <boost/math/special_functions/erf.hpp>
namespace my_policy {
using namespace boost::math::policies;
typedef policy<
  domain_error<errno_on_error>,
  pole_error<errno_on_error>,
  overflow_error<errno_on_error>,
  evaluation_error<errno_on_error>
> my_policy;
};
#endif
