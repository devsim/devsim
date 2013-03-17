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

#include "InterfaceNodeExprModel.hh"
#include "InterfaceModelExprEval.hh"
#include "Interface.hh"
#include "GeometryStream.hh"
#include "dsAssert.hh"
#include "Region.hh"

#include "EngineAPI.hh"
#include <set>
#include <string>
#include <sstream>


// Must be valid equation object which is passed
InterfaceNodeExprModel::InterfaceNodeExprModel(const std::string &nm, const Eqo::EqObjPtr eq, InterfacePtr ip) : InterfaceNodeModel(nm, ip), equation(eq)
{
#if 0
    os << "creating InterfaceNodeExprModel " << nm << " with equation " << eq << "\n";
#endif
  RegisterModels();
}

void InterfaceNodeExprModel::RegisterModels()
{
    typedef std::set<std::string> refmodels_t;
    refmodels_t refs;

    if (EngineAPI::getEnumeratedType(equation) == EngineAPI::MODEL_OBJ)
    {
        refs.insert(EngineAPI::getStringValue(equation));
    }
    else if (EngineAPI::getEnumeratedType(equation) == EngineAPI::VARIABLE_OBJ)
    {
        refs.insert(EngineAPI::getStringValue(equation));
    }
    else
    {
        refmodels_t refmodels    = EngineAPI::getReferencedType(equation, EngineAPI::MODEL_OBJ);
        refmodels_t refvariables = EngineAPI::getReferencedType(equation, EngineAPI::VARIABLE_OBJ);

        for (refmodels_t::iterator it = refmodels.begin(); it != refmodels.end(); ++it)
        {
            refs.insert(*it);
        }
        for (refmodels_t::iterator it = refvariables.begin(); it != refvariables.end(); ++it)
        {
            refs.insert(*it);
        }

    }

    for (refmodels_t::iterator it = refs.begin(); it != refs.end(); ++it)
    {
#if 0
    os << "registering callback " << *it << " for NodeExprModel " << this->GetName() << "\n";
#endif
        RegisterCallback(*it);
    }
}

void InterfaceNodeExprModel::calcNodeScalarValues() const
{
#if 0
        os << "updating NodeExprModel " << GetName() << " from expression " << EngineAPI::getStringValue(equation) << "\n";
#endif
    IMEE::InterfaceModelExprEval::error_t errors;
    const Interface *ip = &(this->GetInterface());
    IMEE::InterfaceModelExprEval mexp(ip, errors);
    IMEE::InterfaceModelExprData out = mexp.eval_function(equation);

    if (!errors.empty())
    {
        std::ostringstream os; 
        //// TODO: put interface/device/region0/region1 names here
        //// Need to put helper function for getting all of these names
        os << "while evaluating interface model " << GetName()
        << " on device "  << GetInterface().GetRegion0()->GetDeviceName()
        << " on interface " << GetInterface().GetName()
        << " with expression " << EngineAPI::getStringValue(equation)
        << "\n";
        for (IMEE::InterfaceModelExprEval::error_t::iterator it = errors.begin(); it != errors.end(); ++it)
        {
            os << *it << "\n";
        }
        GeometryStream::WriteOut(OutputStream::ERROR, *ip, os.str());
    }

    if (
        (out.GetType() == IMEE::InterfaceModelExprData::NODEDATA)
       )
    {
        const IMEE::InterfaceModelExprData::ScalarValuesType &tval = out.GetScalarValues();
        if (tval.IsUniform())
        {
          SetValues(tval.GetScalar());
        }
        else
        {
          NodeScalarList nsl(tval.GetVector());
          SetValues(nsl);
        }
    }
    else if (out.GetType() == IMEE::InterfaceModelExprData::DOUBLE)
    {
        const double v = out.GetDoubleValue();
        const Interface::ConstNodeList_t &nl = GetInterface().GetNodes0();
        std::vector<double> nsl(nl.size(), v);
        SetValues(nsl);
    }
    else
    {
        std::ostringstream os; 
        os << "while evaluating model " << GetName() << ": expression "
            << EngineAPI::getStringValue(equation) << " evaluates to a " << IMEE::InterfaceModelExprData::datatypename[out.GetType()]
            << "\n";
        GeometryStream::WriteOut(OutputStream::FATAL, *ip, os.str());
    }
        
}

void InterfaceNodeExprModel::Serialize(std::ostream &of) const
{
  of << "COMMAND interface_model -device \"" << GetDeviceName() << "\" -interface \"" << GetInterfaceName() << "\" -name \"" << GetName() << "\" -equation \"" << EngineAPI::getStringValue(equation) << ";\"";
}

