/***
DEVSIM
Copyright 2013 DEVSIM LLC

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

#ifndef DS_SUPERLU_PRECONDITIONER_HH
#define DS_SUPERLU_PRECONDITIONER_HH
#include "Preconditioner.hh"

namespace dsMath {

class SuperLUData;

template <typename DoubleType>
class SuperLUPreconditioner : public Preconditioner<DoubleType>
{
    public:
        SuperLUPreconditioner(size_t, PEnum::TransposeType_t, PEnum::LUType_t);
        dsMath::CompressionType GetRealMatrixCompressionType() const override;
        dsMath::CompressionType GetComplexMatrixCompressionType() const override;

    protected:
        bool DerivedLUFactor(Matrix<DoubleType> *) override;
        void DerivedLUSolve(DoubleVec_t<DoubleType> &x, const DoubleVec_t<DoubleType> &b) const override;
        void DerivedLUSolve(ComplexDoubleVec_t<DoubleType> &x, const ComplexDoubleVec_t<DoubleType> &b) const override;

        ~SuperLUPreconditioner();

    private:
        SuperLUPreconditioner();

        SuperLUPreconditioner(const SuperLUPreconditioner &);
        SuperLUPreconditioner &operator= (const SuperLUPreconditioner &);

        SuperLUData *superLUData_;
        PEnum::LUType_t     lutype_;
};

}
#endif

