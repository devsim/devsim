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

#ifndef INSTANCE_MODEL_HH
#define INSTANCE_MODEL_HH
#include "CircuitNode.hh"
#include "NodeKeeper.hh"

#include <string>

namespace dsMath {
template <typename T> class RowColVal;
typedef std::vector<RowColVal<double> > RealRowColValueVec;
typedef std::pair<int, double> RHSEntry;
typedef std::vector<RHSEntry> RHSEntryVec;
}

class InstanceModel
{
    public:
        virtual ~InstanceModel() = 0;
        const std::string &getName() {return name_;}

        virtual void assembleDC(const NodeKeeper::Solution &sol, dsMath::RealRowColValueVec &mat, dsMath::RHSEntryVec &rhs)=0;

        // Transient stamp is only the AC part
        // scl is the scaling factor to multiply by the time-dependent elements
        // If the matrix is a NULL pointer, only the rhs is calculated
        // This is so that it can be used for the charge contributions of
        // previous time steps
        virtual void assembleTran(const double scl, const NodeKeeper::Solution &sol, dsMath::RealRowColValueVec *mat, dsMath::RHSEntryVec &rhs) = 0;

        virtual bool addParam(const std::string &, double) = 0;
        virtual void assembleACRHS(std::vector<std::pair<size_t, std::complex<double> > > &) {} 

    protected:
        // Only derived classes can instantiate the base class
        InstanceModel(NodeKeeper *, const char *name);
        CircuitNodePtr AddCircuitNode(const char *);
        CircuitNodePtr AddInternalNode(const char *);
        CircuitNodePtr AddMNANode(const char *);
    private:
        InstanceModel();
        InstanceModel(const InstanceModel &);
//      InstanceModel operator=(const InstanceModel &);

        NodeKeeper *nodekeep_;
        std::string name_;
        // Needs to query the model keeper
        // and find the appropriate Model,
        // if it has already been defined.
        //
        // Getter's and Setter's are provided
        // so that the derived class is able to
        // get the model info it needs
        // ModelPtr
};

#endif /* INSTANCE_MODEL_HH */
