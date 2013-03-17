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

#include "TriangleEdgeExprModel.hh"
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
TriangleEdgeExprModel::TriangleEdgeExprModel(const std::string &nm, const Eqo::EqObjPtr eq, RegionPtr rp, TriangleEdgeModel::DisplayType dt) : TriangleEdgeModel(nm, rp, dt), equation(eq)
{
#if 0
  os << "creating TriangleEdgeExprModel " << nm << " with equation " << eq << "\n";
#endif
  RegisterModels();
}

void TriangleEdgeExprModel::RegisterModels()
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
    os << "registering callback " << *it << " for TriangleEdgeExprModel " << this->GetName() << "\n";
#endif
        RegisterCallback(*it);
    }
}

void TriangleEdgeExprModel::calcTriangleEdgeScalarValues() const
{
    // need to write the calculator
//    dsAssert(false, "UNEXPECTED");
#if 0
        os << "updating TriangleEdgeExprModel " << GetName() << " from expression " << EngineAPI::getStringValue(equation) << "\n";
#endif
    MEE::ModelExprEval::error_t errors;
    const Region *rp = &(this->GetRegion());
    MEE::ModelExprEval mexp(rp, GetName(), errors);
    MEE::ModelExprData out = mexp.eval_function(equation);

    if (!errors.empty())
    {
        std::ostringstream os; 
        os << "while evaluating triangle edge model " << GetName() << 
        " on Device: " << GetRegion().GetDeviceName() <<
        " on Region: " << GetRegion().GetName()
        << "\n";
        for (MEE::ModelExprEval::error_t::iterator it = errors.begin(); it != errors.end(); ++it)
        {
            os << *it << "\n";
        }
        GeometryStream::WriteOut(OutputStream::ERROR, *rp, os.str());
    }

    /// implicit conversion to triangleedgemodel from edgemodel
    if (
        (out.GetType() == MEE::ModelExprData::EDGEDATA)
       )
    {
      out.convertToTriangleEdgeData();
    }

    if (
        (out.GetType() == MEE::ModelExprData::TRIANGLEEDGEDATA)
       )
    {
      const MEE::ModelExprData::ScalarValuesType &tval = out.GetScalarValues();
      if (tval.IsUniform())
      {
        SetValues(tval.GetScalar());
      }
      else
      {
        TriangleEdgeScalarList nsl(tval.GetVector());
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

void TriangleEdgeExprModel::Serialize(std::ostream &of) const
{
  of << "COMMAND element_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -name \"" << GetName() << "\" -equation \"" << EngineAPI::getStringValue(equation) << ";\"";
}

