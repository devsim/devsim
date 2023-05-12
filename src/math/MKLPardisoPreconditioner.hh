/***
DEVSIM
Copyright 2018 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef DS_MKL_PARDISO_HH
#define DS_MKL_PARDISO_HH
#include "Preconditioner.hh"


namespace dsMath {

class MKLPardisoData;

template <typename DoubleType>
class MKLPardisoPreconditioner : public Preconditioner<DoubleType>
{
    public:
        MKLPardisoPreconditioner(size_t, PEnum::TransposeType_t);
        dsMath::CompressionType GetRealMatrixCompressionType() const override;
        dsMath::CompressionType GetComplexMatrixCompressionType() const override;

    protected:
        bool DerivedLUFactor(Matrix<DoubleType> *) override;
        void DerivedLUSolve(DoubleVec_t<DoubleType> &x, const DoubleVec_t<DoubleType> &b) const override;
        void DerivedLUSolve(ComplexDoubleVec_t<DoubleType> &x, const ComplexDoubleVec_t<DoubleType> &b) const override;

        ~MKLPardisoPreconditioner();

    private:
        MKLPardisoPreconditioner();

        MKLPardisoPreconditioner(const MKLPardisoPreconditioner &);
        MKLPardisoPreconditioner &operator= (const MKLPardisoPreconditioner &);

        MKLPardisoData  *mklpardisodata_;
};

}
#endif

