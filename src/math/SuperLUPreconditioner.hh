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
