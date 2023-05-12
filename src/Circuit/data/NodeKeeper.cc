/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "NodeKeeper.hh"
#include "CircuitNode.hh"
#include "GlobalData.hh"
#include "dsAssert.hh"
#include "OutputStream.hh"

#include <sstream>
#include <climits>
#include <cmath>


NodeKeeper *NodeKeeper::instance_ = 0;

NodeKeeper &NodeKeeper::instance()
{
    if (!instance_)
    {
        instance_ = new NodeKeeper;
    }
    return *instance_;
}

void NodeKeeper::delete_instance()
{
    delete instance_;
    instance_ = nullptr;
}

NodeKeeper::NodeKeeper()
{
}

NodeKeeper::~NodeKeeper() {
    SolutionMap_t::iterator iter, end=Sol_.end();
    for (iter=Sol_.begin(); iter != end; ++iter)
    {
        delete (*iter).second;
    }
}

/**
 * Add a node with the suggested name
 * and a default type
 */
CircuitNodePtr NodeKeeper::AddNode(const std::string & name, CircuitNodeType nt, CircuitUpdateType ut)
{
//    os << "NodeKeeper::AddNode\n";

    if (SolutionLocked_)
    {
        std::ostringstream os;
        os << "CircuitNode " << name << " cannot be added once an analysis has been performed.\n";
        OutputStream::WriteOut(OutputStream::OutputType::FATAL, os.str());
    }


    if (NodeTable_.count(name))
    {
/*
        std::ostringstream os;
        os << name << " is a preexisting node\n";
        OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
*/
    }
    else
    {
/*
        std::ostringstream os;
        os << name << " is being added\n";
        OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
*/
        CircuitNodePtr node=CircuitNodePtr(new CircuitNode(nt, ut));
        NodeTable_[name]=node;
    }
    return NodeTable_[name];
}

CircuitNodePtr NodeKeeper::AddNodeAlias(const std::string &alias, const std::string &name)
{
    NodeTable_t::iterator nname  = NodeTable_.find(name);
    NodeTable_t::iterator nalias = NodeTable_.find(alias);
    if (nname == NodeTable_.end())
    {
        std::ostringstream os;
        os << "CircuitNode " << name << " must exist to create alias " << alias << ".\n";
        OutputStream::WriteOut(OutputStream::OutputType::FATAL, os.str());
        return nullptr;
    }

    if (nalias != NodeTable_.end())
    {
        std::ostringstream os;
        os << "CircuitNode " << alias << " cannot be an alias since it is already a real node.\n";
        OutputStream::WriteOut(OutputStream::OutputType::FATAL, os.str());
        return nullptr;
    }

    NodeAliasTable_[alias] = name;
    return nname->second;
}

#if 0
//// Untested
void NodeKeeper::DeleteNode(const std::string & name)
{
    {
        std::ostringstream os;
        os << "NodeKeeper::DeleteNode\n";
        OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
    }
    if (SolutionLocked_)
    {
       std::ostringstream os;
        os << "CircuitNode " << name << " cannot be deleted once an analysis has been performed.\n";
        OutputStream::WriteOut(OutputStream::OutputType::FATAL, os.str());
    }

    if (NodeTable_.count(name))
    {
        std::ostringstream os;
        os << name << " is being deleted\n";
        OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
        NodeTable_.erase(name);
    }
    else
    {
        std::ostringstream os;
        os << name << " does not exist\n";
        OutputStream::WriteOut(OutputStream::OutputType::ERROR, os.str());
    }
    dsAssert(NodeTable_.count(name), "CIRCUIT_UNEXPECTED");
}
#endif

/**
 * Goes through all nodes in the map and assigns
 * node numbers in alphabetical order
 *
 * Note we may want to experiment with ordering schemes
 * later.
 */
void NodeKeeper::SetNodeNumbers(size_t start, bool verbose) {
    NodeTable_t::iterator iter;
    NodeTable_t::iterator end = NodeTable_.end();

    minEquationNumber = start;

    bool hasGround = false;

    std::ostringstream os;

    size_t i = 0; // Assume c-style indexing
    for (iter = NodeTable_.begin();
            iter != end;
            ++iter)
    {
        if (iter->second->isGROUND())
        {
            iter->second->SetNumber(size_t(-1));
            hasGround = true;
        }
        else
        {
            iter->second->SetNumber(i);
            ++i;
        }
        if (verbose)
        {
          os << iter->first << "\t" << iter->second->GetNumber() << "\t" << iter->second->getCircuitNodeTypeStr() << "\n";
        }
    }
    if (verbose)
    {
      OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
    }

    // Need to find out which node is ground later on
    numberOfNodes_=i;

    maxEquationNumber = minEquationNumber + i - 1;

    dsAssert(hasGround == true, "CIRCUIT_UNEXPECTED");

    NodesNumbered_ = true;

}

size_t NodeKeeper::GetMaxEquationNumber()
{
    dsAssert(NodesNumbered_, "CIRCUIT_UNEXPECTED");
    return maxEquationNumber;
}

size_t NodeKeeper::GetMinEquationNumber()
{
    dsAssert(NodesNumbered_, "CIRCUIT_UNEXPECTED");
    return minEquationNumber;
}

size_t NodeKeeper::GetIndexNumber(const std::string &name)
{
    std::string node = name;
    if (NodeAliasTable_.count(name))
    {
        node = NodeAliasTable_[name];
    }

    NodeTable_t::iterator it = NodeTable_.find(node);
    size_t index = size_t(-1);

    if (it != NodeTable_.end())
    {
        CircuitNode &cn = *(it->second);
        index = cn.GetNumber();
    }

    return index;
}

size_t NodeKeeper::GetEquationNumber(const std::string &name)
{
    size_t eqnum = this->GetIndexNumber(name);
    if (eqnum != size_t(-1))
    {
        eqnum += minEquationNumber;
    }

    return eqnum;
}


/**
 * Set the policy that you can only access these values and not change them
 * Make sure that the field already exists and that the size is not 0
 */
const NodeKeeper::Solution &NodeKeeper::GetSolutionVector(const std::string &key)
{
    dsAssert(Sol_.count(key) == 1, "CIRCUIT_UNEXPECTED");
    dsAssert(Sol_[key] != nullptr, "CIRCUIT_UNEXPECTED");
    return *Sol_[key];
}

void NodeKeeper::CreateSolution(const std::string &key)
{
    // Once we have created first solution, we cannot add or delete nodes
    SolutionLocked_ = true;

#if 0
    if (!NodesNumbered_)
    {
       SetNodeNumbers(0);
    }
#endif

    dsAssert(Sol_.count(key) == 0, "CIRCUIT_UNEXPECTED");

    // entry has now been created
    dsAssert(Sol_[key] == nullptr, "CIRCUIT_UNEXPECTED");
    dsAssert(numberOfNodes_ != 0, "CIRCUIT_UNEXPECTED");

    // technically this needs to be deleted in the destructor
    // migrate to smart pointer
    // Should call initializer for each node (dc case only)
    Sol_[key] = new Solution(numberOfNodes_,0.0);
    std::ostringstream os;
    os << "creating solution " << key << " with " << numberOfNodes_ << " nodes" << "\n";
    OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());

}

// This should perhaps take care of default values
// Do we keep previous guess
void NodeKeeper::InitializeSolution(const std::string &key)
{
   if (Sol_.count(key) == 0)
   {
      CreateSolution(key);
   }
}

// Call this to remove previous guess
void NodeKeeper::DestroySolution(const std::string &key)
{
    dsAssert(Sol_.count(key) == 1, "CIRCUIT_UNEXPECTED");

    delete Sol_[key];
    Sol_.erase(key);
}

// For now assume update can be of any value,  In the future
// each node must specify its update methodology (positive, nonzero, etc)
void NodeKeeper::UpdateSolution(const std::string &key, const Solution &rhs)
{
    dsAssert(Sol_.count(key) == 1, "CIRCUIT_UNEXPECTED");

    Solution *sol = Sol_[key];

    dsAssert(sol != nullptr, "CIRCUIT_UNEXPECTED");


    //// TODO: get rid of this hack
    //// And make configurable for circuits
    const double minError = 1.0e-10;

    double aerr = 0.0;
    double rerr = 0.0;

    for (NodeTable_t::iterator it = NodeTable_.begin(); it != NodeTable_.end(); ++it)
    {
        CircuitNode &cn = *(it->second);
        if (cn.isGROUND())
        {
            continue;
        }

        const size_t       cind     = cn.GetNumber();
        const size_t       eqrow    = cind + minEquationNumber;
        const CircuitUpdateType ut    = cn.getUpdateType();

        /// Assume the equations are numbered in order
        const double oval = sol->operator[](cind);

        double upd = rhs[eqrow];

        if (ut == CircuitUpdateType::LOGDAMP)
        {
            /// hard code for VT at room temp
            if ( std::abs(upd) > 0.0259 )
            {
                const double sign = (upd > 0.0) ? 1.0 : -1.0;
                upd = sign*0.0259*log(1+std::abs(upd)/0.0259);
            }
        }
        else if (ut == CircuitUpdateType::POSITIVE)
        {
            while ((upd + oval) <=  0.0)
            {
                upd /= 2.0;
            }
        }
        else
        {
            dsAssert(ut == CircuitUpdateType::DEFAULT, "CIRCUIT_UNEXPECTED");
        }

        const double nval = upd + oval;
        sol->operator[](cind) = nval;

        const double n1 = std::abs(upd);
        aerr += n1;

        const double n2 = std::abs(nval);
        const double nrerror =  n1 / (n2 + minError);
        if (nrerror > rerr)
            rerr = nrerror;

        // should really set all at once
    }
    absError[key] = aerr;
    relError[key] = rerr;
}

void NodeKeeper::ACUpdateSolution(const std::string &rkey, const std::string &ikey, const ACSolution &rhs)
{
    dsAssert(Sol_.count(rkey) == 1, "CIRCUIT_UNEXPECTED");
    dsAssert(Sol_.count(ikey) == 1, "CIRCUIT_UNEXPECTED");

    Solution *rsol = Sol_[rkey];
    Solution *isol = Sol_[ikey];

    dsAssert(rsol != nullptr, "CIRCUIT_UNEXPECTED");
    dsAssert(isol != nullptr, "CIRCUIT_UNEXPECTED");

    double aerr = 0.0;
    double rerr = 0.0;

    for (NodeTable_t::iterator it = NodeTable_.begin(); it != NodeTable_.end(); ++it)
    {
        CircuitNode &cn = *(it->second);
        if (cn.isGROUND())
        {
            continue;
        }

        const size_t       cind     = cn.GetNumber();
        const size_t       eqrow    = cind + minEquationNumber;

        std::complex<double> upd = rhs[eqrow];

        const std::string &nodename = it->first;
        std::ostringstream os;
        os << "Setting " << nodename << " = " << upd << std::endl;
        OutputStream::WriteOut(OutputStream::OutputType::VERBOSE1, os.str());

        rsol->operator[](cind) = upd.real();
        isol->operator[](cind) = upd.imag();

        const double n1 = std::abs(upd);
        aerr += n1;
    }
    absError[rkey] = aerr;
    absError[ikey] = aerr;
    relError[rkey] = rerr;
    relError[ikey] = rerr;
}

void NodeKeeper::CopySolution(const std::string &from, const std::string &to)
{
    dsAssert(Sol_.count(from) == 1, "CIRCUIT_UNEXPECTED");
    Solution *src = Sol_[from];
    dsAssert(Sol_.count(to) == 1, "CIRCUIT_UNEXPECTED");
    Solution *dest = Sol_[to];

    (*dest)=(*src);

}

void NodeKeeper::PrintSolution(const std::string &sn)
{
    Solution *sol = Sol_[sn];
    dsAssert(sol != nullptr, "CIRCUIT_UNEXPECTED");

    {
        std::ostringstream os;
        os << "Circuit Solution:\n";
        OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
    }

    NodeTable_t::iterator iter;
    NodeTable_t::iterator end = NodeTable_.end();

    for (iter = NodeTable_.begin();
            iter != end;
            ++iter)
    {
       if (iter->second->isGROUND())
          continue;

        std::ostringstream os;
        os << iter->first << "\t" << sol->operator[](iter->second->GetNumber()) << "\n";
        OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
    }
}

void NodeKeeper::ACPrintSolution(const std::string &rk, const std::string &ik)
{
    Solution *rsol = Sol_[rk];
    Solution *isol = Sol_[ik];
    dsAssert(rsol != nullptr, "CIRCUIT_UNEXPECTED");
    dsAssert(isol != nullptr, "CIRCUIT_UNEXPECTED");

    std::ostringstream os;
    os << "Circuit AC Solution:\n";
    NodeTable_t::iterator iter;
    NodeTable_t::iterator end = NodeTable_.end();

    for (iter = NodeTable_.begin();
            iter != end;
            ++iter)
    {
        if (iter->second->isGROUND())
        {
            continue;
        }

        const std::complex<double> ccol (
            rsol->operator[](iter->second->GetNumber()),
            isol->operator[](iter->second->GetNumber())
        );

        os << iter->first << "\t" << ccol << "\n";
    }
    OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
}


double NodeKeeper::GetAbsError(const std::string &nm)
{
    dsAssert(absError.count(nm) != 0, "CIRCUIT_UNEXPECTED");
    return absError[nm];
}

double NodeKeeper::GetRelError(const std::string &nm)
{
    dsAssert(relError.count(nm) != 0, "CIRCUIT_UNEXPECTED");
    return relError[nm];
}

bool NodeKeeper::HaveNodes()
{
    return NodeTable_.size() != 0;
}

bool NodeKeeper::IsCircuitNode(const std::string &name)
{
    std::string node = name;
    if (NodeAliasTable_.count(name))
    {
        node = NodeAliasTable_[name];
    }

    return NodeTable_.count(node) != 0;
}

bool NodeKeeper::IsCircuitSolution(const std::string &name)
{
    std::string node = name;

    return Sol_.count(node) != 0;
}

double NodeKeeper::GetNodeValue(const std::string &solutionname, const std::string &nodename)
{
    double ret = 0.0;

    if (Sol_.count(solutionname))
    {
        size_t index = GetIndexNumber(nodename);
        if (index != size_t(-1))
        {
            dsAssert(index < (*Sol_[solutionname]).size(), "CIRCUIT_UNEXPECTED");
            ret = Sol_[solutionname]->operator[](index);
        }
    }

    return ret;
}

NodeKeeper::Solution *NodeKeeper::GetSolution(const std::string &key)
{
    Solution *ret = nullptr;
    if (Sol_.count(key))
    {
        ret = Sol_[key];
    }
    return ret;
}

void NodeKeeper::SetNodeValue(const std::string &solutionname, const std::string &nodename, double value)
{
    if (Sol_.count(solutionname))
    {
        size_t index = GetIndexNumber(nodename);
        if (index != size_t(-1))
        {
            dsAssert(index < (*Sol_[solutionname]).size(), "CIRCUIT_UNEXPECTED");
            Sol_[solutionname]->operator[](index) = value;
        }
    }
}

void NodeKeeper::TriggerCallbackOnNode(const std::string &nm)
{
    GlobalData &gdata = GlobalData::GetInstance();
    gdata.SignalCallbacksOnGlobal(nm);
}


void NodeKeeper::TriggerCallbacksOnNodes()
{
    for (NodeTable_t::const_iterator it = NodeTable_.begin(); it != NodeTable_.end(); ++it)
    {
        TriggerCallbackOnNode(it->first);
    }
    for (NodeAliasTable_t::const_iterator it = NodeAliasTable_.begin(); it != NodeAliasTable_.end(); ++it)
    {
        TriggerCallbackOnNode(it->first);
    }
}

