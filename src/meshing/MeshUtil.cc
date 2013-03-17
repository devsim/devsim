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

#include "MeshUtil.hh"
#include "dsAssert.hh"
#include "Region.hh"
#include "Vector.hh"
#include "EdgeModel.hh"

#include <cmath>

namespace MeshUtil {
const double tolerance=1.0e-10;

pts_t equidistance(double xl, double xh, double sl)
{
    pts_t ret;
    ret.push_back(xl);
    double x = xl;
    while (x < xh)
    {
        x += sl;
        if (x > (xh - tolerance))
        {
            x = xh;
        }
        ret.push_back(x);
    }
    return ret;

}

pts_t variable(double xl, double xh, double sl, double sh)
{
    pts_t ret;
    double L = xh - xl;
    double r = (L - sl)/(L - sh);

//    size_t N = size_t(floor(log(sh/sl)/log(r)))+1;

    double x = xl;
    ret.push_back(xl);
    for (size_t i = 0; x < xh; ++i)
    {
        x += sl*pow(r, int(i));
        if (x > (xh - tolerance))
        {
            x = xh;

            /// need to have at least one pt with the minimum spacing
            double tpos = x - sh;
            if (ret.back() < tpos)
            {
                ret.push_back(tpos);
            }
        }

        ret.push_back(x);
    }

    return ret;
}

pts_t getPoints(double xl, double xh, double sl, double sh)
{
    dsAssert(xl < xh, "UNEXPECTED");
/*
    if (xh < xl)
    {
        double tmp = xl;
        xl = xh;
        xh = tmp;
    }
*/

    pts_t pts;
    /// If the negative spacing is about equal to the positive spacing, then we use equidistance method
    if ((fabs(sh-sl)/sh) < tolerance)
    {
        pts = equidistance(xl, xh, sl);
    }
    else if ((xl + sl) > (xh - sh))
    {
        ///// bad input so just use this
        pts = equidistance(xl, xh, (xh - xl));
    }
    else
    {
        pts = variable(xl, xh, sl, sh);
    }

    return pts;
}

std::vector<Vector> GetUnitVector(const Region &reg)
{
    const ConstEdgeList &cel = reg.GetEdgeList();
    const size_t nvals = cel.size();

    std::vector <Vector> unitvec(nvals, Vector(0, 0, 0));
    {
      ConstEdgeModelPtr uxp = reg.GetEdgeModel("unitx");
      ConstEdgeModelPtr uyp = reg.GetEdgeModel("unity");
      ConstEdgeModelPtr uzp = reg.GetEdgeModel("unitz");
      for (size_t i = 0; i < nvals; ++i)
      {
        double x = 0.0;
        double y = 0.0;
        double z = 0.0;
        if (uxp)
        {
          x = uxp->GetScalarValues()[i];
        }
        if (uyp)
        {
          y = uyp->GetScalarValues()[i];
        }
        if (uzp)
        {
          z = uzp->GetScalarValues()[i];
        }
        unitvec[i] = Vector(x, y, z);
      }
      
    }
    return unitvec;
}
}
