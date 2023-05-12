/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef NODEKEEPER_HH
#define NODEKEEPER_HH

#include <vector>
#include <string>
#include <utility>
#include <map>
#include <complex>

#include "CircuitNode.hh"

//typedef std::map<std::string, std::vector<std::complex> > ComplexSolution;

/**
 * This is a Singleton Class which maintains a list of node names
 * as well as the number of times it is referenced (Reference Count).
 *
 * In order to control sparsity of the matrix.  The entries will be
 * sorted alphabetically.  This makes controls proximity since adjacent
 * nodes will share the same prefix.
 *
 * Naming convention:
 *
 * node
 * instance.internalnode
 * subcircuit.instance.internalnode
 *
 * when each instance receives its name, it suffixes its internal node names
 * with a . and internal name.
 */
class NodeKeeper {
    public:
        typedef std::map<std::string, CircuitNodePtr> NodeTable_t;
        typedef std::map<std::string, std::string>    NodeAliasTable_t;
        typedef std::map<std::string, double>         NormMap_t;

        // Solution is by name, vector of nodes
        typedef std::vector<double>   Solution;
        typedef std::vector<std::complex<double> >   ACSolution;
        typedef std::map<std::string, Solution *> SolutionMap_t;

        static NodeKeeper &instance();
        static void delete_instance();

        CircuitNodePtr AddNode(const std::string &/*name*/,
                CircuitNodeType x=CircuitNodeType::DEFAULT, CircuitUpdateType y=CircuitUpdateType::DEFAULT);

        CircuitNodePtr AddNodeAlias(const std::string &/*alias*/, const std::string &/*name*/);

        bool HaveNodes();

        void DeleteNode(const std::string & name);

        void PrintSolution(const std::string &);
        void ACPrintSolution(const std::string &, const std::string &);
        ~NodeKeeper();

        // not many get the privilege to set values
        // please don't const_cast this away!!
        const Solution &GetSolutionVector(const std::string &) ;

        void CreateSolution(const std::string &);
        void InitializeSolution(const std::string &);
        void DestroySolution(const std::string &);
//      bool HasSolution(const std::string &);

        std::vector<double> *GetSolution(const std::string &);

        // Update the solution with the given name with the given vector
        void UpdateSolution(const std::string &, const std::vector<double> &);

        void ACUpdateSolution(const std::string &, const std::string &, const std::vector<std::complex<double>> &);
        // Copy Solution from one vector to another

        void CopySolution(const std::string &, const std::string &);

        size_t getNumberNodes() {return numberOfNodes_;}

        //// 0 based
        size_t GetIndexNumber(const std::string &);
        //// 0 based on min equation number
        size_t GetEquationNumber(const std::string &);

        void   SetNodeNumbers(size_t /*start*/, bool /*verbose*/);
        size_t GetMaxEquationNumber();
        size_t GetMinEquationNumber();

        const NodeTable_t &getNodeList() { return NodeTable_;}

        const SolutionMap_t &getSolutionList() { return Sol_;}

        double GetAbsError(const std::string &/*solution name*/);
        double GetRelError(const std::string &/*solution name*/);

        bool   IsCircuitNode(const std::string &);
        bool   IsCircuitSolution(const std::string &);
        double GetNodeValue(const std::string &, const std::string &);

        void SetNodeValue(const std::string &, const std::string &, double value);
        void TriggerCallbacksOnNodes();
        void TriggerCallbackOnNode(const std::string &);
    private:
        NodeKeeper();
        NodeKeeper(const NodeKeeper &);
        NodeKeeper operator=(const NodeKeeper &);

        static NodeKeeper *instance_;

        NodeTable_t       NodeTable_;
        NodeAliasTable_t  NodeAliasTable_;
        SolutionMap_t     Sol_;
        size_t          numberOfNodes_ = 0;
        bool            SolutionLocked_ = false;
        bool            NodesNumbered_ = false;
        size_t          minEquationNumber = 0;
        size_t          maxEquationNumber = 0;
        NormMap_t       absError;
        NormMap_t       relError;
};
#endif

