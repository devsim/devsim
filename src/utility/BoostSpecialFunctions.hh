/***
DEVSIM
Copyright 2021 DEVSIM LLC

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
#ifndef BOOST_SPECIAL_FUNCTIONS_HH
#define BOOST_SPECIAL_FUNCTIONS_HH
//https://www.boost.org/doc/libs/1_52_0/libs/math/doc/sf_and_dist/html/math_toolkit/policy/pol_ref/error_handling_policies.html
#include <boost/math/special_functions/erf.hpp>
namespace my_policy {
using namespace boost::math::policies;
typedef policy<
  domain_error<errno_on_error>,
  pole_error<errno_on_error>,
  overflow_error<errno_on_error>,
  evaluation_error<errno_on_error>
> use_errno;
};
#endif
