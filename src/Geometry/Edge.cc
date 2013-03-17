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

