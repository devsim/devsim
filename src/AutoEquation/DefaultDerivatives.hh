/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef DEFAULT_DERIVATIVES_HH
#define DEFAULT_DERIVATIVES_HH
#include <memory>

namespace Eqo {
class EquationObject;
typedef std::shared_ptr<EquationObject> EqObjPtr;
}
namespace dsHelper {
Eqo::EqObjPtr CreateDefaultDerivatives();
}
#endif
