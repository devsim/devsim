/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "CircuitNode.hh"

CircuitNode::CircuitNode() : nodeNumber_(0), nodeType_(CircuitNodeType::DEFAULT), updateType(CircuitUpdateType::DEFAULT)
{
}

CircuitNode::CircuitNode(CircuitNodeType nt, CircuitUpdateType ut) : nodeNumber_(0), nodeType_(nt), updateType(ut)
{
}

const char * const CircuitNode::CircuitNodeTypeStr[] = {
           "default",
           "mna",
           "internal",
           "ground"
        };
