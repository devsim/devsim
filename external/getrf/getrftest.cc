
#include <vector>
#include <iostream>

#define dgetrf quad_dgetrf_
#define dgetrs quad_dgetrs_
extern "C" {
void dgetrf( int *m, int *n, __float128 *a, int *lda, int *ipiv, int *info );
void dgetrs( char *trans, int *n, int *nrhs, __float128 *a, int *lda, int *ipiv, __float128 *b, int *ldb, int *info);
}

int main()
{
    std::vector<__float128> A_;
    std::vector<int>    ipiv_;
    int                 dim_;
    bool                factored_;
    int                 info_;

    dim_=2;
    A_.resize(dim_*dim_);
    ipiv_.resize(dim_);

    for (size_t i = 0; i < 4; ++i)
    {
      A_[i] = static_cast<__float128>(i+1);
    }

    A_[1] = 8;
    dgetrf(&dim_, &dim_, &A_[0], &dim_, &ipiv_[0], &info_);

    std::cout << info_ << std::endl;

    for (size_t i = 0; i < 4; ++i)
    {
    std::cout << " " << static_cast<double>(A_[i]);
    }
    std::cout << std::endl;

    static char trans = 'N';
    static int  nrhs   = 1;

    std::vector<__float128> B;
    B.resize(dim_);

    for (size_t i = 0; i < 2; ++i)
    {
      B[i] = static_cast<__float128>(i+1);
    }

    dgetrs(&trans, &dim_, &nrhs, &A_[0], &dim_, &ipiv_[0], &B[0], &dim_, &info_);

    for (size_t i = 0; i < 2; ++i)
    {
      std::cout << static_cast<double>(B[i]) << std::endl;
    }
}


