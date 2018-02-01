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

#ifndef EQUATION_HH
#define EQUATION_HH
#include "MathEnum.hh"
#include <string>
#include <vector>
#include <complex>
#include <map>
#include <iosfwd>

class PermutationEntry;

template <typename T, typename U> class ScalarData;

class NodeModel;
template<typename T>
using NodeScalarData = ScalarData<NodeModel, T>;

class EdgeModel;

class TriangleEdgeModel;

class TetrahedronEdgeModel;


template<typename T>
using EdgeScalarData = ScalarData<EdgeModel, T>;

template<typename T>
using TriangleEdgeScalarData = ScalarData<TriangleEdgeModel, T>;

template<typename T>
using TetrahedronEdgeScalarData = ScalarData<TetrahedronEdgeModel, T>;

template<typename T>
using NodeScalarList = std::vector<T>;


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

template <typename DoubleType>
using RealRowColVal = RowColVal<DoubleType>;

template <typename DoubleType>
using RealRowColValueVec = std::vector<RealRowColVal<DoubleType>>;

template <typename DoubleType>
using RHSEntry = std::pair<int, DoubleType>;

template <typename DoubleType>
using RHSEntryVec = std::vector<RHSEntry<DoubleType>>;
}

namespace EquationEnum
{
  enum UpdateType {DEFAULT, LOGDAMP, POSITIVE};
  extern const char *UpdateTypeString;
}
// base class
// called for assembly
// start with specific equation derived classes
// Then work out automatically derived Equations and Models
template <typename DoubleType>
class Equation {
    public:

        Equation(const std::string &, RegionPtr, const std::string &/*variable*/, EquationEnum::UpdateType utype = EquationEnum::DEFAULT/*update type*/);
        virtual ~Equation() = 0;

        void Assemble(dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

        const std::string &GetName() const {
            return myname;
        }

        const std::string &GetVariable() const {
            return variable;
        }

        void Update(NodeModel &, const std::vector<DoubleType> &);

        void ACUpdate(NodeModel &, const std::vector<std::complex<DoubleType> > &);
        void NoiseUpdate(const std::string &, const std::vector<PermutationEntry> &, const std::vector<std::complex<DoubleType> > &);

        std::string GetNoiseRealName(const std::string &);
        std::string GetNoiseImagName(const std::string &);
        void DefaultNoiseUpdate(const std::string &, const std::vector<PermutationEntry> &, const std::vector<std::complex<DoubleType> > &);

        DoubleType GetAbsError() const;
        DoubleType GetRelError() const;
        void setMinError(DoubleType);
        DoubleType GetMinError() const;

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
        void DefaultUpdate(NodeModel &, const std::vector<DoubleType> &);
        void DefaultACUpdate(NodeModel &, const std::vector<std::complex<DoubleType> > &);

        void setAbsError(DoubleType);
        void setRelError(DoubleType);

        /// Stuff like potential is symmetric.  It's derivative with respect to a node on either side is of opposite sign.
        /// Stuff should already be integrated w.r.t. EdgeCouple
        void EdgeAssembleRHS(dsMath::RHSEntryVec<DoubleType> &, const EdgeScalarData<DoubleType> &/*rhs*/, const DoubleType /*n0_sign*/, const DoubleType /*n1_sign*/);

        void TriangleEdgeAssembleRHS(dsMath::RHSEntryVec<DoubleType> &, const TriangleEdgeScalarData<DoubleType> &/*rhs*/, const DoubleType /*n0_sign*/, const DoubleType /*n1_sign*/);

        void TetrahedronEdgeAssembleRHS(dsMath::RHSEntryVec<DoubleType> &, const TetrahedronEdgeScalarData<DoubleType> &/*rhs*/, const DoubleType /*n0_sign*/, const DoubleType /*n1_sign*/);

//        void SymmetricEdgeAssembleJacobian(dsMath::RealRowColValueVec<DoubleType> &, const EdgeScalarData<DoubleType> &/*der*/, const std::string &/*var*/);

        void UnSymmetricEdgeAssembleJacobian(dsMath::RealRowColValueVec<DoubleType> &, const EdgeScalarData<DoubleType> &/*der0*/, const EdgeScalarData<DoubleType> &/*der1*/, const std::string &/*var*/, const DoubleType /*n0_sign*/, const DoubleType /*n1_sign*/);
        void UnSymmetricTriangleEdgeAssembleJacobian(dsMath::RealRowColValueVec<DoubleType> &, const TriangleEdgeScalarData<DoubleType> &/*der0*/, const TriangleEdgeScalarData<DoubleType> &/*der1*/, const TriangleEdgeScalarData<DoubleType> &/*der2*/, const std::string &/*var*/, const DoubleType /*n0_sign*/, const DoubleType /*n1_sign*/);
        void UnSymmetricTetrahedronEdgeAssembleJacobian(dsMath::RealRowColValueVec<DoubleType> &, const TetrahedronEdgeScalarData<DoubleType> &/*der0*/, const TetrahedronEdgeScalarData<DoubleType> &/*der1*/, const TetrahedronEdgeScalarData<DoubleType> &/*der2*/, const TetrahedronEdgeScalarData<DoubleType> &/*der3*/, const std::string &/*var*/, const DoubleType /*n0_sign*/, const DoubleType /*n1_sign*/);
        /// Stuff should already be integrated w.r.t. NodeVolume
        void NodeAssembleRHS(dsMath::RHSEntryVec<DoubleType> &, const NodeScalarData<DoubleType> &/*rhs*/);
        void NodeAssembleJacobian(dsMath::RealRowColValueVec<DoubleType> &, const NodeScalarData<DoubleType> &/*der*/, const std::string &/*var*/);

        void NodeVolumeAssemble(const std::string &, dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, dsMathEnum::WhatToLoad);
        void NodeVolumeAssemble(const std::string &, dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, dsMathEnum::WhatToLoad, const std::string &/*node_volume*/);
        void EdgeCoupleAssemble(const std::string &, dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, dsMathEnum::WhatToLoad);
        void EdgeNodeVolumeAssemble(const std::string &, dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, dsMathEnum::WhatToLoad);
        void EdgeCoupleAssemble(const std::string &, dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, dsMathEnum::WhatToLoad, const std::string &/*edge_couple*/, const DoubleType n0_sign, const DoubleType n1_sign);
        void ElementEdgeCoupleAssemble(const std::string &, dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, dsMathEnum::WhatToLoad);
        void ElementNodeVolumeAssemble(const std::string &, dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, dsMathEnum::WhatToLoad);
        void TriangleEdgeCoupleAssemble(const std::string &, dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, dsMathEnum::WhatToLoad, const std::string &, const DoubleType, const DoubleType);
        void TetrahedronEdgeCoupleAssemble(const std::string &, dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, dsMathEnum::WhatToLoad, const std::string &, const DoubleType, const DoubleType);

        std::string GetDerivativeModelName(const std::string &, const std::string &);
    private:
        virtual void DerivedAssemble(dsMath::RealRowColValueVec<DoubleType> &, dsMath::RHSEntryVec<DoubleType> &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode) = 0;

        virtual void UpdateValues(NodeModel &, const std::vector<DoubleType> &) = 0;
        virtual void ACUpdateValues(NodeModel &, const std::vector<std::complex<DoubleType> > &) = 0;
        virtual void NoiseUpdateValues(const std::string &, const std::vector<PermutationEntry> &, const std::vector<std::complex<DoubleType> > &) = 0;


        void PositiveSolutionUpdate(const NodeScalarList<DoubleType> &, NodeScalarList<DoubleType> &, NodeScalarList<DoubleType> &);
        void LogSolutionUpdate(const      NodeScalarList<DoubleType> &, NodeScalarList<DoubleType> &, NodeScalarList<DoubleType> &);
        void DefaultSolutionUpdate(const NodeScalarList<DoubleType> &, NodeScalarList<DoubleType> &, NodeScalarList<DoubleType> &);

        Equation();
        Equation(const Equation &);
        Equation &operator=(const Equation &);

        const std::string myname;
        RegionPtr myregion;
        const std::string variable;
        DoubleType absError;
        DoubleType relError;
        DoubleType minError;
        static const DoubleType defminError;
        EquationEnum::UpdateType updateType;
};
#endif

