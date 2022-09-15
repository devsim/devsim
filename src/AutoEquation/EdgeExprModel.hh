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

#ifndef EDGEEXPRMODEL_HH
#define EDGEEXPRMODEL_HH
#include "EdgeModel.hh"
#include <memory>
#include <string>
namespace Eqo {
class EquationObject;
typedef std::shared_ptr<EquationObject> EqObjPtr;

} // namespace Eqo

EdgeModelPtr CreateEdgeExprModel(const std::string &, Eqo::EqObjPtr, RegionPtr,
                                 EdgeModel::DisplayType, ContactPtr);

template <typename DoubleType> class EdgeExprModel : public EdgeModel {
public:
  void Serialize(std::ostream &) const;

  EdgeExprModel(const std::string &, Eqo::EqObjPtr, RegionPtr,
                EdgeModel::DisplayType, ContactPtr cp = nullptr);

private:
  void RegisterModels();
  EdgeExprModel();
  EdgeExprModel(const EdgeExprModel &);

  void calcEdgeScalarValues() const;

  const Eqo::EqObjPtr equation;
};

#endif
