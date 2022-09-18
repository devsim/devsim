/*
This file is derived from from:
http://math.nist.gov/iml++/gmres.h.txt
*/
#include "gmres.hh"
#include "Matrix.hh"
#include "Preconditioner.hh"
#include "DenseMatrix.hh"
#include "BlasHeaders.hh"
#include <cmath>
#include <vector>
#include <complex>
namespace iml
{

template < class Matrix, class Vector >
void Update(Vector &x, int k, Matrix &h, Vector &s, Vector v[])
{
  Vector y(s);

  // Backsolve:
  for (int i = k; i >= 0; i--) {
    y(i) /= h(i,i);
    for (int j = i - 1; j >= 0; j--)
      y(j) -= h(j,i) * y(i);
  }

  for (int j = 0; j <= k; j++)
    x += v[j] * y(j);
}

template < class Real > Real
abs(Real x)
{
  return (x > 0 ? x : -x);
}

#if 0
template<class Real>
void GeneratePlaneRotation(Real &dx, Real &dy, Real &cs, Real &sn)
{
  if (dy == 0.0) {
    cs = 1.0;
    sn = 0.0;
  } else if (abs(dy) > abs(dx)) {
    Real temp = dx / dy;
    sn = 1.0 / sqrt( 1.0 + temp*temp );
    cs = temp * sn;
  } else {
    Real temp = dy / dx;
    cs = 1.0 / sqrt( 1.0 + temp*temp );
    sn = temp * cs;
  }
}
#else
void GeneratePlaneRotation(const double &dx, const double &dy, double &cs, double &sn)
{
  double dxcopy = dx;
  double dycopy = dy;
  //// must use copies since entries are overwritten
  drotg(&dxcopy, &dycopy, &cs, &sn);
}

void GeneratePlaneRotation(const std::complex<double> &dx, const std::complex<double> &dy, std::complex<double> &cs, std::complex<double> &sn)
{
  std::complex<double> dxcopy = dx;
  std::complex<double> dycopy = dy;
  //// must use copies since entries are overwritten
  zrotg(&dxcopy, &dycopy, &cs, &sn);
}
#endif


template<class Real>
void ApplyPlaneRotation(Real &dx, Real &dy, Real &cs, Real &sn)
{
  Real temp  =  cs * dx + sn * dy;
  dy = -sn * dx + cs * dy;
  dx = temp;
}

template <typename T> class IMLPreconditioner;

template <typename T>
class IMLVector {
  public:
    explicit IMLVector(const std::vector<T> &x) : vec_(x) {}
    explicit IMLVector(int m) : vec_(m) {}
    IMLVector() {}

    const std::vector<T> &GetSTLVector() const
    {
      return vec_;
    }

    T dot(const IMLVector &y) const
    {
      T ret = 0;
      for (size_t i = 0; i < vec_.size(); ++i)
      {
        ret += vec_[i] * y.vec_[i];
      }
      return ret;
    }

    IMLVector<T> operator-(const IMLVector &y) const
    {
      std::vector<T> ret(vec_);
      const std::vector<T> &yv = y.GetSTLVector();
      for (size_t i = 0; i < ret.size(); ++i)
      {
        ret[i] -= yv[i];
      }
      return IMLVector(ret);
    }

    IMLVector<T> operator+(const IMLVector &y) const
    {
      std::vector<T> ret(vec_);
      const std::vector<T> &yv = y.GetSTLVector();
      for (size_t i = 0; i < ret.size(); ++i)
      {
        ret[i] += yv[i];
      }
      return ret;
    }

    IMLVector<T> operator*(const T &v) const
    {
      IMLVector<T> x = *this;
      x *= v;
      return x;
    }

    IMLVector<T> &operator*=(const T &v)
    {
      for (size_t i = 0; i < vec_.size(); ++i)
      {
        vec_[i] *= v;
      }
      return *this;
    }

    IMLVector<T> &operator=(const T &v)
    {
      vec_ = std::vector<T>(vec_.size(), v);
      return *this;
    }

    IMLVector<T> &operator-=(const IMLVector<T> &v)
    {
      for (size_t i = 0; i < vec_.size(); ++i)
      {
        vec_[i] -= v(i);
      }
      return *this;
    }

    IMLVector<T> &operator+=(const IMLVector<T> &v)
    {
      for (size_t i = 0; i < vec_.size(); ++i)
      {
        vec_[i] += v(i);
      }
      return *this;
    }

    T &operator()(int i)
    {
      return vec_[i];
    }

    T operator()(int i) const
    {
      return vec_[i];
    }

    size_t size()
    {
      return vec_.size();
    }

  private:
    std::vector<T> vec_;
};

template <typename T>
IMLVector<T> operator*(const double x, const IMLVector<T> &v)
{
  return v * x;
}

template <typename T>
IMLVector<T> operator-(const double x, const IMLVector<T> &v)
{
  return (-1.0*v) + x;
}

template <typename T>
T dot(const IMLVector<T> &x, const IMLVector<T> &y)
{
  return x.dot(y);
}


template <typename U>
class IMLPreconditioner {
  public:
    IMLPreconditioner(const dsMath::Preconditioner<double>& p) : pre_(p) {};

    IMLVector<U> solve(const IMLVector<U> &bv) const
    {
      const std::vector<U> &b = bv.GetSTLVector();
      std::vector<U> x(b.size());
      pre_.LUSolve(x, b);
      IMLVector<U> xv(x);
      return xv;
    }

  private:
    const dsMath::Preconditioner<double> &pre_;
};

#if 0
template <typename T>
std::vector operator-(const std::vector<T> &v0, const std::vector<T> &v1)
{
  std::vector<T> ret(v0);
  for (size_t i = 0; i < v0.size(); ++i)
  {
    ret[i] -= v1[i];
  }
  return ret;
}
#endif

template <typename T>
T norm(const IMLVector<T> &vec)
{
  T ret = sqrt(dot(vec, vec));

  return ret;
}

template <typename T>
class IMLMatrix {
  public:
    IMLMatrix (const dsMath::Matrix<double> &m) : mat_(m) {}

    IMLVector<T> operator*(const IMLVector<T> &x) const
    {
      std::vector<T> y;
      mat_.Multiply(x.GetSTLVector(), y);
      return IMLVector<T>(y);
    }

  private:
    const dsMath::Matrix<double> &mat_;
};


template < class Operator, class Vector, class Preconditioner,
           class Matrix, class Real >
int GMRES(const Operator &A, Vector &x, const Vector &b,
      const Preconditioner &M, Matrix &H, int &m, int &max_iter,
      Real &tol)
{
  Real resid;
  int i, j = 1, k;
  Vector s(m+1), cs(m+1), sn(m+1), w;

  Real normb = norm(M.solve(b));
  Vector r = M.solve(b - A * x);
  Real beta = norm(r);

  if (normb == 0.0)
    normb = 1;

  if ((resid = norm(r) / normb) <= tol) {
    tol = resid;
    max_iter = 0;
    return 0;
  }

  Vector *v = new Vector[m+1];

  while (j <= max_iter) {
    v[0] = r * (1.0 / beta);    // ??? r / beta
    s = 0.0;
    s(0) = beta;

    for (i = 0; i < m && j <= max_iter; i++, j++) {
      w = M.solve(A * v[i]);
      for (k = 0; k <= i; k++) {
        H(k, i) = dot(w, v[k]);
        w -= H(k, i) * v[k];
      }
      H(i+1, i) = norm(w);
      v[i+1] = w * (1.0 / H(i+1, i)); // ??? w / H(i+1, i)

      for (k = 0; k < i; k++)
        ApplyPlaneRotation(H(k,i), H(k+1,i), cs(k), sn(k));

      GeneratePlaneRotation(H(i,i), H(i+1,i), cs(i), sn(i));
      ApplyPlaneRotation(H(i,i), H(i+1,i), cs(i), sn(i));
      ApplyPlaneRotation(s(i), s(i+1), cs(i), sn(i));

      if ((resid = abs<Real>(s(i+1)) / normb) < tol) {
        Update(x, i, H, s, v);
        tol = resid;
        max_iter = j;
        delete [] v;
        return 0;
      }
    }
    Update(x, m - 1, H, s, v);
    r = M.solve(b - A * x);
    beta = norm(r);
    if ((resid = beta / normb) < tol) {
      tol = resid;
      max_iter = j;
      delete [] v;
      return 0;
    }
  }

  tol = resid;
  delete [] v;
  return 1;
}


//template int GMRES<Matrix, std::vector<double>, std::vector<double>, Preconditioner, DenseMatrix<double>, double>
template int GMRES
(
    const iml::IMLMatrix<double> &,
    iml::IMLVector<double> &,
    const iml::IMLVector<double> &,
    const iml::IMLPreconditioner<double> &,
    dsMath::DenseMatrix<double> &,
    int &,
    int &,
    double &);
}

//#include <iostream>
namespace dsMath {
int GMRES(const Matrix<double> &A, DoubleVec_t<double> &x, const DoubleVec_t<double> &b, const Preconditioner<double> &M, int &m, int &max_iter, double &tol)
{
  iml::IMLVector<double> ix(x);
  RealDenseMatrix<double> H(m+1);
  int ret = GMRES(iml::IMLMatrix<double>(A), ix, iml::IMLVector<double>(b), iml::IMLPreconditioner<double>(M), H, m, max_iter, tol);
//  std::cerr << m << " " << max_iter << " " << tol << "\n";
//  ix *= (-1.0);
  x = ix.GetSTLVector();
  return ret;
}
}

