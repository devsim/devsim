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

#ifndef NODEPOSITION_HH
#define NODEPOSITION_HH
#include "NodeModel.hh"
template <typename DoubleType>
class NodePosition : public NodeModel
{
    public:
      NodePosition(RegionPtr);
      ~NodePosition();

      void Serialize(std::ostream &) const;

    private:
      DoubleType calcNodePosition(ConstNodePtr) const;
      void calcNodeScalarValues() const;
      void setInitialValues();

      WeakNodeModelPtr yposition;
      WeakNodeModelPtr zposition;
      WeakNodeModelPtr node_index;
      WeakNodeModelPtr coordinate_index;
};

#endif
