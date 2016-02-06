/***
DEVSIM
Copyright 2013 Devsim LLC

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
// Need to separate Preconditioner from matrix, just in case we want to share
// the same preconditioner over multiple blocks
namespace dsMath {

class SuperLUData;

//// Eventually we may put Compressed column between matrix and SuperLUPreconditioner
class SuperLUPreconditioner : public Preconditioner
{
    public:
        SuperLUPreconditioner(size_t, Preconditioner::TransposeType_t, LUType_t);

    protected:
        bool DerivedLUFactor(Matrix *);     // Factor the matrix
        void DerivedLUSolve(DoubleVec_t &x, const DoubleVec_t &b) const;
        void DerivedLUSolve(ComplexDoubleVec_t &x, const ComplexDoubleVec_t &b) const;

        ~SuperLUPreconditioner();

    private:
        SuperLUPreconditioner();
        // Make sure that we copy all aspects(including pointers) later on
        SuperLUPreconditioner(const SuperLUPreconditioner &);
        SuperLUPreconditioner &operator= (const SuperLUPreconditioner &);

        SuperLUData *superLUData_;
        LUType_t     lutype_;
};

}
#endif
