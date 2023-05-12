/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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

