//*****************************************************************
// Iterative template routine -- GMRES
//
// GMRES solves the unsymmetric linear system Ax = b using the
// Generalized Minimum Residual method
//
// GMRES follows the algorithm described on p. 20 of the
// SIAM Templates book.
//
// The return value indicates convergence within max_iter (input)
// iterations (0), or no convergence within max_iter iterations (1).
//
// Upon successful return, output arguments have the following values:
//
//        x  --  approximate solution to Ax = b
// max_iter  --  the number of iterations performed before the
//               tolerance was reached
//      tol  --  the residual after the final iteration
//
//*****************************************************************
/*
This file is derived from from:
http://math.nist.gov/iml++/gmres.h.txt
*/

#if !defined(USE_BLAS)
#error "Do not include if USE_BLAS is not set"
#endif

#ifndef IML_GMRES_HH
#define IML_GMRES_HH
#include "dsMathTypes.hh"
namespace dsMath {
template <typename DoubleType>
class Matrix;
template <typename DoubleType>
class Preconditioner;
int GMRES(const Matrix<double> &A, DoubleVec_t<double> &x, const DoubleVec_t<double> &b, const Preconditioner<double> &M, int &m, int &max_iter, double &tol);
}
namespace iml {
template < class Operator, class Vector, class Preconditioner,
           class Matrix, class Real >
int GMRES(const Operator &A, Vector &x, const Vector &b, const Preconditioner &M, Matrix &H, int &m, int &max_iter, Real &tol);

}
#endif

