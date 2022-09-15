
#ifndef FLOAT128_HH
#define FLOAT128_HH
#ifndef DEVSIM_EXTENDED_PRECISION
#error "File included when DEVSIM_EXTENDED_PRECISION not enabled"
#endif
#include <boost/multiprecision/float128.hpp>
using namespace boost::multiprecision;
#endif
