#ifndef CIRCUIT_NODE_HH
#define CIRCUIT_NODE_HH
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


#include <memory>

class CircuitNode;

typedef std::shared_ptr<CircuitNode> CircuitNodePtr;

namespace CUT {
    enum UpdateType {DEFAULT, LOGDAMP, POSITIVE};
};

namespace CNT {
    enum CircuitNodeType {
        DEFAULT,          // Your typical node
        MNA,              // MNA for Voltage sources
        INTERNAL,         // INTERNAL for internal nodes
        GROUND            // Ground CircuitNodes are not solved
    }; 
};

class CircuitNode {
    public:


        static const char * const CircuitNodeTypeStr[];

        inline size_t getNumber()
            {return nodeNumber_;}

        inline CUT::UpdateType getUpdateType()
            {return updateType;}
        
        inline CNT::CircuitNodeType getCircuitNodeType()
            {return nodeType_;}

        inline const char * getCircuitNodeTypeStr()
            {return CircuitNodeTypeStr[nodeType_];}

        inline bool isGROUND() {return (nodeType_==CNT::GROUND);}

        size_t GetNumber()
            {return nodeNumber_;}

    private:
        friend class NodeKeeper;
        /* Modified Nodal Analysis */
        CircuitNode();
        explicit CircuitNode(CNT::CircuitNodeType, CUT::UpdateType);
        CircuitNode operator=(const CircuitNode &);

        inline bool  isMNA() {return (nodeType_== CNT::MNA);}
//      void setMNA(bool setm=true) {
//          nodeType_=(setm) ? MNA : DEFAULT;}

        void SetNumber(size_t n)
            {nodeNumber_=n;}

        size_t           nodeNumber_;
        CNT::CircuitNodeType nodeType_;
        CUT::UpdateType      updateType;
};


#endif
