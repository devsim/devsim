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

#ifndef AVERAGE_EDGE_MODEL_HH
#define AVERAGE_EDGE_MODEL_HH
#include "EdgeModel.hh"

#include <memory>
#include <string>

class NodeModel;
using ConstNodeModelPtr = std::shared_ptr<const NodeModel>;

#include <string>

namespace AverageEdgeModelEnum {
enum AverageType_t {
  ARITHMETIC,
  GEOMETRIC,
  GRADIENT,
  NEGATIVE_GRADIENT,
  UNKNOWN
};
// This model depends on this Node model to calculate values
// must match AverageTypeNames below
struct TypeNameMap_t {
  const char *str;
  AverageType_t type;
};
extern TypeNameMap_t AverageTypeNames[];

AverageEdgeModelEnum::AverageType_t GetTypeName(const std::string &,
                                                std::string &);
} // namespace AverageEdgeModelEnum

EdgeModelPtr CreateAverageEdgeModel(
    const std::string & /*edgemodel*/, const std::string & /*nodemodel*/,
    AverageEdgeModelEnum::AverageType_t /*averagetype*/, RegionPtr /*rp*/);
EdgeModelPtr CreateAverageEdgeModel(
    const std::string & /*edgemodel*/, const std::string & /*nodemodel*/,
    const std::string & /*var*/,
    AverageEdgeModelEnum::AverageType_t /*averagetype*/, RegionPtr /*rp*/);

template <typename DoubleType> class AverageEdgeModel : public EdgeModel {
public:
  ~AverageEdgeModel();

  void Serialize(std::ostream &) const;

  AverageEdgeModel(const std::string & /*edgemodel*/,
                   const std::string & /*nodemodel*/,
                   AverageEdgeModelEnum::AverageType_t /*averagetype*/,
                   RegionPtr);
  AverageEdgeModel(const std::string & /*edgemodel*/,
                   const std::string & /*nodemodel*/,
                   const std::string & /*var*/,
                   AverageEdgeModelEnum::AverageType_t /*averagetype*/,
                   RegionPtr);

private:
  template <typename T>
  void doMath(ConstNodeModelPtr, EdgeScalarList<DoubleType> &, const T &) const;
  template <typename T>
  void doMath(ConstNodeModelPtr, ConstNodeModelPtr,
              EdgeScalarList<DoubleType> &, EdgeScalarList<DoubleType> &,
              const T &) const;

  void doGradient(ConstNodeModelPtr, EdgeScalarList<DoubleType> &,
                  DoubleType) const;
  void doGradient(ConstNodeModelPtr, ConstNodeModelPtr,
                  EdgeScalarList<DoubleType> &, EdgeScalarList<DoubleType> &,
                  DoubleType) const;

  void calcEdgeScalarValues() const;
  void setInitialValues();

  const std::string originalEdgeModelName;
  const std::string nodeModelName;
  const std::string edgeModel1Name;
  const std::string variableName;
  std::string derivativeModelName;
  //// derivative with respect to first edge
  mutable WeakEdgeModelPtr node1EdgeModel;
  const AverageEdgeModelEnum::AverageType_t averageType;
};

#endif
