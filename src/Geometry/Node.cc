/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "Node.hh"
#include <cstddef>

Node::Node(size_t ind, ConstCoordinatePtr cp)
{
   index = ind;
   coord = cp;
}

