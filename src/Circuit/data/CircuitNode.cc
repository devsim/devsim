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
