/***
DEVSIM
Copyright 2013 Devsim LLC

This file is part of DEVSIM.

DEVSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, version 3.

DEVSIM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with DEVSIM.  If not, see <http://www.gnu.org/licenses/>.
***/

#include "DefaultDerivatives.hh"
#include "parseexpr.hh"
#include "OutputStream.hh"
#include "dsAssert.hh"
#include <sstream>

namespace dsHelper {
Eqo::EqObjPtr CreateDefaultDerivatives()
{
    EvalExpr::error_t terrors;
    const std::string expr(
        "declare(dBdx(x)); "
        "define(B(x),dBdx(x)); "
        "declare(sgn(x)); "
        "declare(step(x)); " 
        "define(abs(x), sgn(x)); "
//// from online wolfram.com
//// TODO: define pi as a parameter somewhere in the program
        "declare(derfdx(x)); "
        "declare(derfcdx(x)); "
        "define(erf(x), derfdx(x)); "
        "define(erfc(x), derfcdx(x));"
        "declare(dFermidx(x)); "
        "declare(dInvFermidx(x)); "
        "define(Fermi(x), dFermidx(x)); "
        "define(InvFermi(x), dInvFermidx(x)); "
        "define(vec_sum(x),1);"
        "declare(vec_max(x));"
        "declare(vec_min(x));"
        "declare(dot2d(ax,ay,bx,by));"
        "define(dot2d(ax, ay, bx, by), bx, by, ax, ay);"
//        "define(min(x, y), step(y - x), step(x - y));"
//        "define(max(x, y), step(x - y), step(y - x));"
        "define(max(x, y), x >= y, x < y);"
        "define(min(x, y), x <= y, x > y);"
//        "define(parallel(x,y), dparallel_dx(x, y), dparallel_dx(y, x));"
        "define(kahan3(a, b, c), 1, 1, 1);"
        "define(kahan4(a, b, c, d), 1, 1, 1, 1);"
        "define(asinh(x), ((1 + (x^2))^(-1/2)));"
        "define(acosh(x), (((x^2) - 1)^(-1/2)));"
        "define(atanh(x), ((1 - (x^2))^(-1)));"
        );

    Eqo::EqObjPtr testeq = EvalExpr::evaluateExpression(expr, terrors);

    if (!terrors.empty())
    {
        std::ostringstream os; 
        os << "problems with creating default derivatives. " << expr << "\n";
        for (EvalExpr::error_t::iterator it = terrors.begin(); it != terrors.end(); ++it)
        {
            os << *it << "\n";
        }
        OutputStream::WriteOut(OutputStream::FATAL, os.str());
    }
    return testeq;
}

}
