/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/


#include <iostream>
#include "NodeKeeper.hh"

int main() {
    NodeKeeper &inst = NodeKeeper::instance();
    inst.AddNode("foo");
    inst.AddNode("cat");
//    inst.DeleteNode("dog");
    inst.SetNodeNumbers();
    inst.CreateSolution("dcop");
    return 0;
}
