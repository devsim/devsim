/***
DEVSIM
Copyright 2013 DEVSIM LLC

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
template <typename DoubleType>
TriangleEdgeExprModel<DoubleType>::TriangleEdgeExprModel(const std::string &nm, const Eqo::EqObjPtr eq, RegionPtr rp, TriangleEdgeModel::DisplayType dt) : TriangleEdgeModel(nm, rp, dt), equation(eq)
{
#if 0
  os << "creating TriangleEdgeExprModel " << nm << " with equation " << eq << "\n";
#endif
  RegisterModels();
}

template <typename DoubleType>
void TriangleEdgeExprModel<DoubleType>::RegisterModels()
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

template <typename DoubleType>
void TriangleEdgeExprModel<DoubleType>::calcTriangleEdgeScalarValues() const
{
    // need to write the calculator
//    dsAssert(false, "UNEXPECTED");
#if 0
        os << "updating TriangleEdgeExprModel " << GetName() << " from expression " << EngineAPI::getStringValue(equation) << "\n";
#endif
    typename MEE::ModelExprEval<DoubleType>::error_t errors;
    const Region *rp = &(this->GetRegion());
    MEE::ModelExprEval<DoubleType> mexp(rp, GetName(), errors);
    MEE::ModelExprData<DoubleType> out = mexp.eval_function(equation);

    if (!errors.empty())
    {
        std::ostringstream os;
        os << "while evaluating triangle edge model " << GetName() <<
        " on Device: " << GetRegion().GetDeviceName() <<
        " on Region: " << GetRegion().GetName()
        << "\n";
        for (typename MEE::ModelExprEval<DoubleType>::error_t::iterator it = errors.begin(); it != errors.end(); ++it)
        {
            os << *it << "\n";
        }
        GeometryStream::WriteOut(OutputStream::OutputType::ERROR, *rp, os.str());
    }

    /// implicit conversion to triangleedgemodel from edgemodel
    if (
        (out.GetType() == MEE::datatype::EDGEDATA)
       )
    {
      out.convertToTriangleEdgeData();
    }

    if (
        (out.GetType() == MEE::datatype::TRIANGLEEDGEDATA)
       )
    {
      const MEE::ScalarValuesType<DoubleType> &tval = out.GetScalarValues();
      if (tval.IsUniform())
      {
        SetValues(tval.GetScalar());
      }
      else
      {
        TriangleEdgeScalarList<DoubleType> nsl(tval.GetVector());
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
        os << "while evaluating model " << GetName() << ": expression "
            << EngineAPI::getStringValue(equation) << " evaluates to " << MEE::datatypename[static_cast<size_t>(out.GetType())]
            << "\n";
        GeometryStream::WriteOut(OutputStream::OutputType::FATAL, *rp, os.str());
    }

}

template <typename DoubleType>
void TriangleEdgeExprModel<DoubleType>::Serialize(std::ostream &of) const
{
  of << "COMMAND element_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -name \"" << GetName() << "\" -equation \"" << EngineAPI::getStringValue(equation) << ";\"";
}

template class TriangleEdgeExprModel<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class TriangleEdgeExprModel<float128>;
#endif

TriangleEdgeModelPtr CreateTriangleEdgeExprModel(const std::string &nm, const Eqo::EqObjPtr eq, RegionPtr rp, TriangleEdgeModel::DisplayType dt)
{
  const bool use_extended = rp->UseExtendedPrecisionModels();
  return create_triangle_edge_model<TriangleEdgeExprModel<double>, TriangleEdgeExprModel<extended_type>>(use_extended, nm, eq, rp, dt);
}

