/***
DEVSIM
Copyright 2021 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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
