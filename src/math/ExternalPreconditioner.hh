/***
DEVSIM
Copyright 2023 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef DS_EXTERNAL_PRECONDITIONER_HH
#define DS_EXTERNAL_PRECONDITIONER_HH
#include "Preconditioner.hh"
#include "ObjectHolder.hh"

namespace dsMath {
enum class CompressionType;

template <typename DoubleType>
class ExternalPreconditioner : public Preconditioner<DoubleType>
{
    public:
        ExternalPreconditioner(size_t, PEnum::TransposeType_t);
        bool init (ObjectHolder, std::string &);
        dsMath::CompressionType GetRealMatrixCompressionType() const override;
        dsMath::CompressionType GetComplexMatrixCompressionType() const override;

    protected:
        bool DerivedLUFactor(Matrix<DoubleType> *) override;
        void DerivedLUSolve(DoubleVec_t<DoubleType> &x, const DoubleVec_t<DoubleType> &b) const override;
        void DerivedLUSolve(ComplexDoubleVec_t<DoubleType> &x, const ComplexDoubleVec_t<DoubleType> &b) const override;

        ~ExternalPreconditioner();

    private:
        ExternalPreconditioner() = delete;
        ExternalPreconditioner(const ExternalPreconditioner &) = delete;
        ExternalPreconditioner &operator= (const ExternalPreconditioner &) = delete;

        mutable ObjectHolder command_handle_;
        mutable ObjectHolder command_data_;
        CompressionType compression_type_;
};
}

#endif

