/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "TetrahedronEdgeExprModel.hh"
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
TetrahedronEdgeExprModel<DoubleType>::TetrahedronEdgeExprModel(const std::string &nm, const Eqo::EqObjPtr eq, RegionPtr rp, TetrahedronEdgeModel::DisplayType dt) : TetrahedronEdgeModel(nm, rp, dt), equation(eq)
{
#if 0
  os << "creating TetrahedronEdgeExprModel " << nm << " with equation " << eq << "\n";
#endif
  RegisterModels();
}

template <typename DoubleType>
void TetrahedronEdgeExprModel<DoubleType>::RegisterModels()
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
    os << "registering callback " << *it << " for TetrahedronEdgeExprModel " << this->GetName() << "\n";
#endif
        RegisterCallback(*it);
    }
}

template <typename DoubleType>
void TetrahedronEdgeExprModel<DoubleType>::calcTetrahedronEdgeScalarValues() const
{
    // need to write the calculator
//    dsAssert(false, "UNEXPECTED");
#if 0
        os << "updating TetrahedronEdgeExprModel " << GetName() << " from expression " << EngineAPI::getStringValue(equation) << "\n";
#endif
    typename MEE::ModelExprEval<DoubleType>::error_t errors;
    const Region *rp = &(this->GetRegion());
    MEE::ModelExprEval<DoubleType> mexp(rp, GetName(), errors);
    MEE::ModelExprData<DoubleType> out = mexp.eval_function(equation);

    std::string output_errors;
    if (!errors.empty())
    {
        std::ostringstream os;
        os << "while evaluating tetrahedron edge model " << GetName() <<
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

    /// implicit conversion to tetrahedronedgemodel from edgemodel
    if (
        (out.GetType() == MEE::datatype::EDGEDATA)
       )
    {
      out.convertToTetrahedronEdgeData();
    }

    if (
        (out.GetType() == MEE::datatype::TETRAHEDRONEDGEDATA)
       )
    {
      const MEE::ScalarValuesType<DoubleType> &tval = out.GetScalarValues();
      if (tval.IsUniform())
      {
        SetValues(tval.GetScalar());
      }
      else
      {
        TetrahedronEdgeScalarList<DoubleType> nsl(tval.GetVector());
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
void TetrahedronEdgeExprModel<DoubleType>::Serialize(std::ostream &of) const
{
  of << "COMMAND element_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -name \"" << GetName() << "\" -equation \"" << EngineAPI::getStringValue(equation) << ";\" -display_type \"" << GetDisplayTypeString() << "\"";
}

template class TetrahedronEdgeExprModel<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class TetrahedronEdgeExprModel<float128>;
#endif

TetrahedronEdgeModelPtr CreateTetrahedronEdgeExprModel(const std::string &nm, const Eqo::EqObjPtr eq, RegionPtr rp, TetrahedronEdgeModel::DisplayType dt)
{
  const bool use_extended = rp->UseExtendedPrecisionModels();
  return create_tetrahedron_edge_model<TetrahedronEdgeExprModel<double>, TetrahedronEdgeExprModel<extended_type>>(use_extended, nm, eq, rp, dt);
}

