/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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
template <typename DoubleType>
EdgeExprModel<DoubleType>::EdgeExprModel(const std::string &nm, const Eqo::EqObjPtr eq, RegionPtr rp, EdgeModel::DisplayType dt, ContactPtr cp) : EdgeModel(nm, rp, dt, cp), equation(eq)
{
#if 0
    os << "creating EdgeExprModel " << nm << " with equation " << eq << "\n";
#endif
    RegisterModels();
}

template <typename DoubleType>
void EdgeExprModel<DoubleType>::RegisterModels()
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

template <typename DoubleType>
void EdgeExprModel<DoubleType>::calcEdgeScalarValues() const
{
    // need to write the calculator
//    dsAssert(false, "UNEXPECTED");
#if 0
        os << "updating EdgeExprModel " << GetName() << " from expression " << EngineAPI::getStringValue(equation) << "\n";
#endif
    typename MEE::ModelExprEval<DoubleType>::error_t errors;
    const Region *rp = &(this->GetRegion());
    MEE::ModelExprEval<DoubleType> mexp(rp, GetName(), errors);
    MEE::ModelExprData<DoubleType> out = mexp.eval_function(equation);

    std::string output_errors;
    if (!errors.empty())
    {
        std::ostringstream os;
        os << "while evaluating edge model " << GetName() <<
        " on Device: " << GetRegion().GetDeviceName() <<
        " on Region: " << GetRegion().GetName()
        << "\n";
        for (typename MEE::ModelExprEval<DoubleType>::error_t::iterator it = errors.begin(); it != errors.end(); ++it)
        {
            os << *it << "\n";
        }
        output_errors = os.str();
        GeometryStream::WriteOut(OutputStream::OutputType::ERROR, *rp, output_errors);
    }

    if (
        (out.GetType() == MEE::datatype::EDGEDATA)
       )
    {
        const MEE::ScalarValuesType<DoubleType> &tval = out.GetScalarValues();
        if (tval.IsUniform())
        {
          SetValues(tval.GetScalar());
        }
        else
        {
          EdgeScalarList<DoubleType> nsl(tval.GetVector());
          SetValues(nsl);
        }
    }
    else if (out.GetType() == MEE::datatype::DOUBLE)
    {
        const DoubleType v = out.GetDoubleValue();
        SetValues(v);
    }
    else
    {
        std::ostringstream os;
        os << output_errors;
        os << "while evaluating model " << GetName() << ": expression "
            << EngineAPI::getStringValue(equation) << " evaluates to " << MEE::datatypename[static_cast<size_t>(out.GetType())]
            << "\n";
        GeometryStream::WriteOut(OutputStream::OutputType::FATAL, *rp, os.str());
    }

}

template <typename DoubleType>
void EdgeExprModel<DoubleType>::Serialize(std::ostream &of) const
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

template class EdgeExprModel<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class EdgeExprModel<float128>;
#endif

EdgeModelPtr CreateEdgeExprModel(const std::string &nm, const Eqo::EqObjPtr eq, RegionPtr rp, EdgeModel::DisplayType dt, ContactPtr cp)
{
  return create_edge_model<EdgeExprModel<double>, EdgeExprModel<extended_type>>(rp->UseExtendedPrecisionModels(), nm, eq, rp, dt, cp);
}

