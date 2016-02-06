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

#include "CircuitNode.hh"

CircuitNode::CircuitNode() : nodeNumber_(0), nodeType_(CNT::DEFAULT), updateType(CUT::DEFAULT)
{
}

CircuitNode::CircuitNode(CNT::CircuitNodeType nt, CUT::UpdateType ut) : nodeNumber_(0), nodeType_(nt), updateType(ut)
{
}

const char * const CircuitNode::CircuitNodeTypeStr[] = {
           "default",
           "mna",
           "internal",
           "ground"
        };
