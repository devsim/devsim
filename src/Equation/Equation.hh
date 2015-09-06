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

#ifndef EQUATION_HH
#define EQUATION_HH
#include "MathEnum.hh"
#include <string>
#include <vector>
#include <complex>
#include <map>
#include <iosfwd>
template <typename T> class ScalarData;
class NodeModel;
class EdgeModel;
class TriangleEdgeModel;
class TetrahedronEdgeModel;
typedef ScalarData<NodeModel> NodeScalarData;
typedef ScalarData<EdgeModel> EdgeScalarData;
typedef ScalarData<TriangleEdgeModel> TriangleEdgeScalarData;
typedef ScalarData<TetrahedronEdgeModel> TetrahedronEdgeScalarData;
typedef std::vector<double> NodeScalarList;

class Region;
typedef Region *RegionPtr;
typedef const Region *ConstRegionPtr;

class ObjectHolder;

// This would be for getting a stamp
// calculate stamp from first assembly
//typedef std::pair<int, int > RowColEntry;
//typedef std::vector<RowColEntry> RowColEntryVec;

//#include "MatrixEntries.hh"
namespace dsMath {
template <typename T> class RowColVal;
typedef RowColVal<double> RealRowColVal;
typedef std::vector<RealRowColVal > RealRowColValueVec;
typedef std::pair<int, double> RHSEntry;
typedef std::vector<RHSEntry> RHSEntryVec;
}

// base class
// called for assembly
// start with specific equation derived classes
// Then work out automatically derived Equations and Models
class Equation {
    public:
        enum UpdateType {DEFAULT, LOGDAMP, POSITIVE};
        static const char *UpdateTypeString;

        Equation(const std::string &, RegionPtr, const std::string &/*variable*/, UpdateType utype = DEFAULT/*update type*/);
        virtual ~Equation() = 0;

        void Assemble(dsMath::RealRowColValueVec &, dsMath::RHSEntryVec &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

        const std::string &GetName() const {
            return myname;
        }

        const std::string &GetVariable() const {
            return variable;
        }

        void Update(NodeModel &, const std::vector<double> &);

        void ACUpdate(NodeModel &, const std::vector<std::complex<double> > &);
        void NoiseUpdate(const std::string &, const std::vector<size_t> &, const std::vector<std::complex<double> > &);

/*
        enum SmallSignalUpdateType {SSAC, NOISE};
        void SmallSignalUpdate(const std::string &, const std::string &, const std::vector<std::complex<double> > &, SmallSignalUpdateType);
*/


        std::string GetNoiseRealName(const std::string &);
        std::string GetNoiseImagName(const std::string &);
        void DefaultNoiseUpdate(const std::string &, const std::vector<size_t> &, const std::vector<std::complex<double> > &);

        double GetAbsError() const;
        double GetRelError() const;
        void setMinError(double);
        double GetMinError() const;

        const Region &GetRegion() const
        {
            return *myregion;
        }

        void DevsimSerialize(std::ostream &) const;

        void GetCommandOptions(std::map<std::string, ObjectHolder> &) const;

    protected:

        virtual void Serialize(std::ostream &) const = 0;
        virtual void GetCommandOptions_Impl(std::map<std::string, ObjectHolder> &) const = 0;

        // for non negative variable
        void DefaultUpdate(NodeModel &, const std::vector<double> &);
        void DefaultACUpdate(NodeModel &, const std::vector<std::complex<double> > &);

        void setAbsError(double);
        void setRelError(double);

        /// Stuff like potential is symmetric.  It's derivative with respect to a node on either side is of opposite sign.
        /// Stuff should already be integrated w.r.t. EdgeCouple
        void EdgeAssembleRHS(dsMath::RHSEntryVec &, const EdgeScalarData &/*rhs*/, const double /*n0_sign*/, const double /*n1_sign*/);

        void TriangleEdgeAssembleRHS(dsMath::RHSEntryVec &, const TriangleEdgeScalarData &/*rhs*/, const double /*n0_sign*/, const double /*n1_sign*/);

        void TetrahedronEdgeAssembleRHS(dsMath::RHSEntryVec &, const TetrahedronEdgeScalarData &/*rhs*/, const double /*n0_sign*/, const double /*n1_sign*/);

//        void SymmetricEdgeAssembleJacobian(dsMath::RealRowColValueVec &, const EdgeScalarData &/*der*/, const std::string &/*var*/);

        void UnSymmetricEdgeAssembleJacobian(dsMath::RealRowColValueVec &, const EdgeScalarData &/*der0*/, const EdgeScalarData &/*der1*/, const std::string &/*var*/, const double /*n0_sign*/, const double /*n1_sign*/);
        void UnSymmetricTriangleEdgeAssembleJacobian(dsMath::RealRowColValueVec &, const TriangleEdgeScalarData &/*der0*/, const TriangleEdgeScalarData &/*der1*/, const TriangleEdgeScalarData &/*der2*/, const std::string &/*var*/, const double /*n0_sign*/, const double /*n1_sign*/);
        void UnSymmetricTetrahedronEdgeAssembleJacobian(dsMath::RealRowColValueVec &, const TetrahedronEdgeScalarData &/*der0*/, const TetrahedronEdgeScalarData &/*der1*/, const TetrahedronEdgeScalarData &/*der2*/, const TetrahedronEdgeScalarData &/*der3*/, const std::string &/*var*/, const double /*n0_sign*/, const double /*n1_sign*/);
        /// Stuff should already be integrated w.r.t. NodeVolume
        void NodeAssembleRHS(dsMath::RHSEntryVec &, const NodeScalarData &/*rhs*/);
        void NodeAssembleJacobian(dsMath::RealRowColValueVec &, const NodeScalarData &/*der*/, const std::string &/*var*/);

        void NodeVolumeAssemble(const std::string &, dsMath::RealRowColValueVec &, dsMath::RHSEntryVec &, dsMathEnum::WhatToLoad);
        void NodeVolumeAssemble(const std::string &, dsMath::RealRowColValueVec &, dsMath::RHSEntryVec &, dsMathEnum::WhatToLoad, const std::string &/*node_volume*/);
        void EdgeCoupleAssemble(const std::string &, dsMath::RealRowColValueVec &, dsMath::RHSEntryVec &, dsMathEnum::WhatToLoad);
        void EdgeNodeVolumeAssemble(const std::string &, dsMath::RealRowColValueVec &, dsMath::RHSEntryVec &, dsMathEnum::WhatToLoad);
        void EdgeCoupleAssemble(const std::string &, dsMath::RealRowColValueVec &, dsMath::RHSEntryVec &, dsMathEnum::WhatToLoad, const std::string &/*edge_couple*/, const double n0_sign, const double n1_sign);
        void ElementEdgeCoupleAssemble(const std::string &, dsMath::RealRowColValueVec &, dsMath::RHSEntryVec &, dsMathEnum::WhatToLoad);
        void ElementNodeVolumeAssemble(const std::string &, dsMath::RealRowColValueVec &, dsMath::RHSEntryVec &, dsMathEnum::WhatToLoad);
        void TriangleEdgeCoupleAssemble(const std::string &, dsMath::RealRowColValueVec &, dsMath::RHSEntryVec &, dsMathEnum::WhatToLoad, const std::string &, const double, const double);
        void TetrahedronEdgeCoupleAssemble(const std::string &, dsMath::RealRowColValueVec &, dsMath::RHSEntryVec &, dsMathEnum::WhatToLoad, const std::string &, const double, const double);

        std::string GetDerivativeModelName(const std::string &, const std::string &);
    private:
        virtual void DerivedAssemble(dsMath::RealRowColValueVec &, dsMath::RHSEntryVec &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode) = 0;

        virtual void UpdateValues(NodeModel &, const std::vector<double> &) = 0;
        virtual void ACUpdateValues(NodeModel &, const std::vector<std::complex<double> > &) = 0;
        virtual void NoiseUpdateValues(const std::string &, const std::vector<size_t> &, const std::vector<std::complex<double> > &) = 0;


        void PositiveSolutionUpdate(const NodeScalarList &, NodeScalarList &, NodeScalarList &);
        void LogSolutionUpdate(const      NodeScalarList &, NodeScalarList &, NodeScalarList &);
        void DefaultSolutionUpdate(const NodeScalarList &, NodeScalarList &, NodeScalarList &);

        Equation();
        Equation(const Equation &);
        Equation &operator=(const Equation &);

        const std::string myname;
        RegionPtr myregion;
        const std::string variable;
        double absError;
        double relError;
        double minError;
        static const double defminError;
        UpdateType updateType;
};
#endif

