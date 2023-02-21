/***
DEVSIM
Copyright 2023 DEVSIM LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
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
        bool DerivedLUFactor(Matrix<DoubleType> *);
        void DerivedLUSolve(DoubleVec_t<DoubleType> &x, const DoubleVec_t<DoubleType> &b) const;
        void DerivedLUSolve(ComplexDoubleVec_t<DoubleType> &x, const ComplexDoubleVec_t<DoubleType> &b) const;

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

