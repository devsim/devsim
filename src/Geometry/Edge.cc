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

#include "Edge.hh"
#include "Node.hh"
#include "dsAssert.hh"

Edge::Edge(size_t ind, ConstNodePtr n1, ConstNodePtr n2) : nodes(2)
{
   index = ind;

   if (NodeCompIndex()(n1, n2))
   {
      nodes[0] = n1;
      nodes[1] = n2;
   }
   else
   {
      nodes[0] = n2;
      nodes[1] = n1;
   }
}

double Edge::GetNodeSign(ConstNodePtr np) const
{
    dsAssert((np == nodes[0]) || (np == nodes[1]), "UNEXPECTED");
    if (np == nodes[0])
        return 1.0;
    else
        return -1.0;
}

