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

#include "EdgeExprModel.hh"
#include "Region.hh"
#include "Edge.hh"
#include "Vector.hh"
#include "ModelExprEval.hh"
#include "GeometryStream.hh"
#include "dsAssert.hh"

#include "EngineAPI.hh"
#include <set>
#include <string>
#include <sstream>


// Must be valid equation object which is passed
EdgeExprModel::EdgeExprModel(const std::string &nm, const Eqo::EqObjPtr eq, RegionPtr rp, EdgeModel::DisplayType dt, ContactPtr cp) : EdgeModel(nm, rp, dt, cp), equation(eq)
{
#if 0
    os << "creating EdgeExprModel " << nm << " with equation " << eq << "\n";
#endif
    RegisterModels();
}

void EdgeExprModel::RegisterModels()
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
    os << "registering callback " << *it << " for EdgeExprModel " << this->GetName() << "\n";
#endif
        RegisterCallback(*it);
    }
}

void EdgeExprModel::calcEdgeScalarValues() const
{
    // need to write the calculator
//    dsAssert(false, "UNEXPECTED");
#if 0
        os << "updating EdgeExprModel " << GetName() << " from expression " << EngineAPI::getStringValue(equation) << "\n";
#endif
    MEE::ModelExprEval::error_t errors;
    const Region *rp = &(this->GetRegion());
    MEE::ModelExprEval mexp(rp, GetName(), errors);
    MEE::ModelExprData out = mexp.eval_function(equation);

    if (!errors.empty())
    {
        std::ostringstream os; 
        os << "while evaluating edge model " << GetName() << 
        " on Device: " << GetRegion().GetDeviceName() <<
        " on Region: " << GetRegion().GetName()
        << "\n";
        for (MEE::ModelExprEval::error_t::iterator it = errors.begin(); it != errors.end(); ++it)
        {
            os << *it << "\n";
        }
        GeometryStream::WriteOut(OutputStream::ERROR, *rp, os.str());
    }

    if (
        (out.GetType() == MEE::ModelExprData::EDGEDATA)
       )
    {
        const MEE::ModelExprData::ScalarValuesType &tval = out.GetScalarValues();
        if (tval.IsUniform())
        {
          SetValues(tval.GetScalar());
        }
        else
        {
          EdgeScalarList nsl(tval.GetVector());
          SetValues(nsl);
        }
    }
    else if (out.GetType() == MEE::ModelExprData::DOUBLE)
    {
        const double v = out.GetDoubleValue();
        SetValues(v);
    }
    else
    {
        std::ostringstream os; 
        os << "while evaluating model " << GetName() << ": expression "
            << EngineAPI::getStringValue(equation) << " evaluates to " << MEE::ModelExprData::datatypename[out.GetType()]
            << "\n";
        GeometryStream::WriteOut(OutputStream::FATAL, *rp, os.str());
    }
        
}

void EdgeExprModel::Serialize(std::ostream &of) const
{
  of << "COMMAND ";

  if (AtContact())
  {
    of << "contact_edge_model -device \"" << GetDeviceName() << "\" -contact \"" << GetContactName() << "\" ";
  }
  else
  {
    of << "edge_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" ";
  }

  of << "-name \"" << GetName() << "\" -equation \"" << EngineAPI::getStringValue(equation) << ";\" -display_type \"" << GetDisplayTypeString() << "\"";

}

