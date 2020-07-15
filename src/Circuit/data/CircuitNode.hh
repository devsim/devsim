#ifndef CIRCUIT_NODE_HH
#define CIRCUIT_NODE_HH
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


#include <memory>

class CircuitNode;

typedef std::shared_ptr<CircuitNode> CircuitNodePtr;

enum class CircuitUpdateType {DEFAULT, LOGDAMP, POSITIVE};

enum class CircuitNodeType {
    DEFAULT,          // Your typical node
    MNA,              // MNA for Voltage sources
    INTERNAL,         // INTERNAL for internal nodes
    GROUND            // Ground CircuitNodes are not solved
}; 

class CircuitNode {
    public:


        static const char * const CircuitNodeTypeStr[];

        inline size_t getNumber()
            {return nodeNumber_;}

        inline CircuitUpdateType getUpdateType()
            {return updateType;}

        inline CircuitNodeType getCircuitNodeType()
            {return nodeType_;}

        inline const char * getCircuitNodeTypeStr()
            {return CircuitNodeTypeStr[static_cast<size_t>(nodeType_)];}

        inline bool isGROUND() {return (nodeType_==CircuitNodeType::GROUND);}

        size_t GetNumber()
            {return nodeNumber_;}

    private:
        friend class NodeKeeper;
        /* Modified Nodal Analysis */
        CircuitNode();
        explicit CircuitNode(CircuitNodeType, CircuitUpdateType);
        CircuitNode operator=(const CircuitNode &);

        inline bool  isMNA() {return (nodeType_== CircuitNodeType::MNA);}
//      void setMNA(bool setm=true) {
//          nodeType_=(setm) ? MNA : DEFAULT;}

        void SetNumber(size_t n)
            {nodeNumber_=n;}

        size_t           nodeNumber_;
        CircuitNodeType nodeType_;
        CircuitUpdateType      updateType;
};


#endif
