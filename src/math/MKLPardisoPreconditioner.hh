/***
DEVSIM
Copyright 2018 DEVSIM LLC

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

