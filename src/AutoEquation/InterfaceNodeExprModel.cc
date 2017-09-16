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
template <typename DoubleType>
InterfaceNodeExprModel<DoubleType>::InterfaceNodeExprModel(const std::string &nm, const Eqo::EqObjPtr eq, InterfacePtr ip) : InterfaceNodeModel(nm, ip), equation(eq)
{
#if 0
    os << "creating InterfaceNodeExprModel " << nm << " with equation " << eq << "\n";
#endif
  RegisterModels();
}

template <typename DoubleType>
void InterfaceNodeExprModel<DoubleType>::RegisterModels()
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

template <typename DoubleType>
void InterfaceNodeExprModel<DoubleType>::calcNodeScalarValues() const
{
#if 0
        os << "updating NodeExprModel " << GetName() << " from expression " << EngineAPI::getStringValue(equation) << "\n";
#endif
    typename IMEE::InterfaceModelExprEval<DoubleType>::error_t errors;
    const Interface *ip = &(this->GetInterface());
    IMEE::InterfaceModelExprEval<DoubleType> mexp(ip, errors);
    IMEE::InterfaceModelExprData<DoubleType> out = mexp.eval_function(equation);

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
        for (typename IMEE::InterfaceModelExprEval<DoubleType>::error_t::iterator it = errors.begin(); it != errors.end(); ++it)
        {
            os << *it << "\n";
        }
        GeometryStream::WriteOut(OutputStream::OutputType::ERROR, *ip, os.str());
    }

    if (
        (out.GetType() == IMEE::datatype::NODEDATA)
       )
    {
        const IMEE::ScalarValuesType<DoubleType> &tval = out.GetScalarValues();
        if (tval.IsUniform())
        {
          SetValues(tval.GetScalar());
        }
        else
        {
          NodeScalarList<DoubleType> nsl(tval.GetVector());
          SetValues(nsl);
        }
    }
    else if (out.GetType() == IMEE::datatype::DOUBLE)
    {
        const DoubleType v = out.GetDoubleValue();
        const Interface::ConstNodeList_t &nl = GetInterface().GetNodes0();
        std::vector<DoubleType> nsl(nl.size(), v);
        SetValues(nsl);
    }
    else
    {
        std::ostringstream os; 
        os << "while evaluating model " << GetName() << ": expression "
            << EngineAPI::getStringValue(equation) << " evaluates to a " << IMEE::datatypename[static_cast<size_t>(out.GetType())]
            << "\n";
        GeometryStream::WriteOut(OutputStream::OutputType::FATAL, *ip, os.str());
    }
        
}

#ifndef _WIN32
#warning "serialize extended option here"
#endif
template <typename DoubleType>
void InterfaceNodeExprModel<DoubleType>::Serialize(std::ostream &of) const
{
  of << "COMMAND interface_model -device \"" << GetDeviceName() << "\" -interface \"" << GetInterfaceName() << "\" -name \"" << GetName() << "\" -equation \"" << EngineAPI::getStringValue(equation) << ";\"";
}

template class InterfaceNodeExprModel<double>;

#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class InterfaceNodeExprModel<float128>;
#endif

