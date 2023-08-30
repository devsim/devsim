/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "ModelExprEval.hh"
#include "ModelExprData.hh"
#include "NodeScalarData.hh"
#include "EdgeScalarData.hh"
#include "NodeModel.hh"
#include "EdgeModel.hh"
#include "TriangleEdgeModel.hh"
#include "TetrahedronEdgeModel.hh"

#include "GlobalData.hh"
#include "NodeKeeper.hh"
#include "Region.hh"
#include "GeometryStream.hh"
#include "dsAssert.hh"

#include "FPECheck.hh"

#if defined(USE_MATERIALDB)
#include "MaterialDB.hh"
#endif

#include "ObjectCache.hh"

#include "MathEval.hh"

#include "EngineAPI.hh"

#include <sstream>
#include <numeric>
#include <algorithm>

namespace MEE {

template <typename DoubleType>
ModelExprEval<DoubleType>::ModelExprEval(data_ref_t &vals, const std::string &m, error_t &er) : data_ref(vals), model(m), errors(er), etype(ExpectedType::UNKNOWN)
{
  const Region *rp = data_ref;
  dsAssert(rp != nullptr, "UNEXPECTED");

  if (ConstNodeModelPtr nm = rp->GetNodeModel(model))
  {
    if (nm->AtContact())
    {
      indexes = nm->GetContactIndexes();
#if 0
      os << "indexes for contact model " << model << "\n";
      for (size_t i = 0; i < indexes.size(); ++i)
      {
        os << indexes[i] << "\n";
      }
#endif
    }
    etype = ExpectedType::NODE;
  }
  else if (ConstEdgeModelPtr nm = rp->GetEdgeModel(model))
  {
    if (nm->AtContact())
    {
      indexes = nm->GetContactIndexes();
#if 0
      os << "indexes for contact model " << model << "\n";
      for (size_t i = 0; i < indexes.size(); ++i)
      {
        os << indexes[i] << "\n";
      }
#endif
    }
    etype = ExpectedType::EDGE;
  }
  else if (rp->GetTriangleEdgeModel(model))
  {
    etype = ExpectedType::TRIANGLEEDGE;
  }
  else if (rp->GetTetrahedronEdgeModel(model))
  {
    etype = ExpectedType::TETRAHEDRONEDGE;
  }
}

template <typename DoubleType>
ModelExprEval<DoubleType>::~ModelExprEval()
{
}

template <typename DoubleType>
ModelExprData<DoubleType> ModelExprEval<DoubleType>::EvaluateModelType(Eqo::EqObjPtr arg)
{
  const std::string &model = EngineAPI::getStringValue(arg);

  const Region *rp = data_ref;

  ModelExprData<DoubleType> out(rp);
  dsAssert(rp != nullptr, "UNEXPECTED");


  if (ConstNodeModelPtr nm = rp->GetNodeModel(model))
  {
    if (nm->IsInProcess())
    {
      std::ostringstream os;
      os << "Cyclic dependency while evaluating NodeModel \"" << model;
      errors.push_back(os.str());
      out = ModelExprData<DoubleType>(rp);
    }
    else
    {
      out = ModelExprData<DoubleType>(nm, rp);
    }
  }
  else if (ConstEdgeModelPtr nm = rp->GetEdgeModel(model))
  {
    if (nm->IsInProcess())
    {
      std::ostringstream os;
      os << "Cyclic dependency while evaluating EdgeModel \"" << model;
      errors.push_back(os.str());
      out = ModelExprData<DoubleType>(rp);
    }
    else
    {
      out = ModelExprData<DoubleType>(nm, rp);
    }
  }
  else if (ConstTriangleEdgeModelPtr nm = rp->GetTriangleEdgeModel(model))
  {
    if (nm->IsInProcess())
    {
      std::ostringstream os;
      os << "Cyclic dependency while evaluating TriangleEdgeModel \"" << model;
      errors.push_back(os.str());
      out = ModelExprData<DoubleType>(rp);
    }
    else
    {
      out = ModelExprData<DoubleType>(nm, rp);
    }
  }
  else if (ConstTetrahedronEdgeModelPtr nm = rp->GetTetrahedronEdgeModel(model))
  {
    if (nm->IsInProcess())
    {
      std::ostringstream os;
      os << "Cyclic dependency while evaluating TetrahedronEdgeModel \"" << model;
      errors.push_back(os.str());
      out = ModelExprData<DoubleType>(rp);
    }
    else
    {
      out = ModelExprData<DoubleType>(nm, rp);
    }
  }
  else
  {
    std::ostringstream os;
    os << "Could not find a model by the name of " << model << ", using 0.0\n";
    GeometryStream::WriteOut(OutputStream::OutputType::VERBOSE1, *rp, os.str());

    out = ModelExprData<DoubleType>(0.0, rp);

    std::string alias_model(model);

    //// Why not just try it all the time???
//    if (model.find(":") != std::string::npos)
    {
      std::string::size_type rpos = alias_model.size();
      if (rpos > 4)
      {
        rpos -= 4;

        if ((alias_model.rfind("@en0") == rpos) || (alias_model.rfind("@en1") == rpos))
        {
          //// look for @n0, @n1 in place of @en0, @en11
          alias_model.erase(rpos+1, 1);
        }

        if (ConstEdgeModelPtr nm = rp->GetEdgeModel(alias_model))
        {
          std::ostringstream os;
          os << "Substituting Edge Model " << alias_model << " for Element Edge Model " << model << "\n";
          GeometryStream::WriteOut(OutputStream::OutputType::VERBOSE1, *rp, os.str());

          if (nm->IsInProcess())
          {
            std::ostringstream os;
            os << "Cyclic dependency while evaluating EdgeModel \"" << model;
            errors.push_back(os.str());
            out = ModelExprData<DoubleType>(rp);
          }
          else
          {
            out = ModelExprData<DoubleType>(nm, rp);
          }
        }
      }
    }
  }

  return out;
}

template <typename DoubleType>
ModelExprData<DoubleType> ModelExprEval<DoubleType>::EvaluateVariableType(Eqo::EqObjPtr arg)
{
  ModelExprData<DoubleType> out;

  GlobalData &gd  = GlobalData::GetInstance();
#if defined(USE_MATERIALDB)
  MaterialDB &mdb = MaterialDB::GetInstance();
#endif
  NodeKeeper &nk = NodeKeeper::instance();

  const std::string &nm = EngineAPI::getName(arg);
  /**
   * Get the DB entry here
   */
  const GlobalData::DoubleDBEntry_t &gdbent  = gd.GetDoubleDBEntryOnRegion(data_ref, nm);
#if defined(USE_MATERIALDB)
  const MaterialDB::DoubleDBEntry_t &mdbentr = mdb.GetDoubleDBEntry(data_ref->GetMaterialName(), nm);
  const MaterialDB::DoubleDBEntry_t &mdbentg = mdb.GetDoubleDBEntry("global", nm);
#endif

  if (gdbent.first)
  {
    out = ModelExprData<DoubleType>(gdbent.second, data_ref);
  }
#if defined(USE_MATERIALDB)
  else if (mdbentr.first)
  {
    out = ModelExprData<DoubleType>(mdbentr.second, data_ref);
  }
  else if (mdbentg.first)
  {
    out = ModelExprData<DoubleType>(mdbentg.second, data_ref);
  }
#endif
  else if (nk.IsCircuitNode(nm))
  {
    const DoubleType val = nk.GetNodeValue("dcop", nm);
    out = ModelExprData<DoubleType>(val, data_ref);
#if 0
    std::ostringstream os;
    os << "circuit using : " << nm << " " << val << "\n";
    GeometryStream::WriteOut(OutputStream::OutputType::INFO, os.str());
#endif
  }
  else
  {
    std::ostringstream os;
    os << "Value for \"" << nm << "\" not available.";
    errors.push_back(os.str());
  }

  return out;
}

template <typename DoubleType>
ModelExprData<DoubleType> ModelExprEval<DoubleType>::EvaluateProductType(Eqo::EqObjPtr arg)
{
  ModelExprData<DoubleType> out;
  //
  /// do a *= multiple times to return result
  std::vector<Eqo::EqObjPtr> values = EngineAPI::getArgs(arg);
  /// Rewrite this later
  out = ModelExprData<DoubleType>(1.0, data_ref);
//    out = eval_function(values[0]);
  /// Premature optimization
  /// short circuit multiplication
  for (size_t i = 0; i < values.size(); ++i)
  {
    ModelExprData<DoubleType> x = eval_function(values[i]);
    if (
      (x.GetType() == datatype::DOUBLE) &&
      (x.GetDoubleValue() == 0.0)
    )
    {
      out = ModelExprData<DoubleType>(0.0, data_ref);
      break;
    }
    else
    {
      out *= x;
    }
  }

  return out;
}

template <typename DoubleType>
ModelExprData<DoubleType> ModelExprEval<DoubleType>::EvaluateIfType(Eqo::EqObjPtr arg)
{
  ModelExprData<DoubleType> out;

  out = ModelExprData<DoubleType>(0.0, data_ref);
  std::vector<Eqo::EqObjPtr> values = EngineAPI::getArgs(arg);

  dsAssert(values.size() == 2, "UNEXPECTED");

  ModelExprData<DoubleType> test = eval_function(values[0]);
  if ((test.GetType() == datatype::DOUBLE))
  {
    if (test.GetDoubleValue() == 0.0)
    {
      out = ModelExprData<DoubleType>(0.0, data_ref);
    }
    else
    {
      out = eval_function(values[1]);
    }
  }
  else
  {
    // Assume we are getting an array of 1's and zeros
    test *= eval_function(values[1]);
    out = test;
  }

  return out;
}

template <typename DoubleType>
ModelExprData<DoubleType> ModelExprEval<DoubleType>::EvaluateIfElseType(Eqo::EqObjPtr arg)
{
  ModelExprData<DoubleType> out;

  out = ModelExprData<DoubleType>(0.0, data_ref);
  std::vector<Eqo::EqObjPtr> values = EngineAPI::getArgs(arg);

  dsAssert(values.size() == 3, "UNEXPECTED");

  ModelExprData<DoubleType> test = eval_function(values[0]);
  if ((test.GetType() == datatype::DOUBLE))
  {
    if (test.GetDoubleValue() == 0.0)
    {
      out = eval_function(values[2]);
    }
    else
    {
      out = eval_function(values[1]);
    }
  }
  else
  {
    // Assume we are getting an array of 1's and zeros
    margv_t margv;
    margv.reserve(3);
    margv.push_back(test);
    margv.push_back(eval_function(values[1]));
    margv.push_back(eval_function(values[2]));
    out = EvaluateExternalMath("ifelse", margv);
  }

  return out;
}

template <typename DoubleType>
ModelExprData<DoubleType> ModelExprEval<DoubleType>::EvaluateAddType(Eqo::EqObjPtr arg)
{
  ModelExprData<DoubleType> out;

//    out = ModelExprData<DoubleType>(0.0, data_ref);
  std::vector<Eqo::EqObjPtr> values = EngineAPI::getArgs(arg);
  out = eval_function(values[0]);
  for (size_t i = 1; i < values.size(); ++i)
  {
    ModelExprData<DoubleType> x = eval_function(values[i]);
    out += x;
  }

  return out;
}

template <typename DoubleType>
ModelExprData<DoubleType> ModelExprEval<DoubleType>::EvaluateFunctionType(Eqo::EqObjPtr arg)
{
  ModelExprData<DoubleType> out;

  /// Implement for UserFuncs
  const std::string &nm = EngineAPI::getName(arg);

  // Equation arguments
  std::vector<Eqo::EqObjPtr> eargv = EngineAPI::getArgs(arg);
  // converted to model expr data
  margv_t margv;
  margv.reserve(eargv.size());
  for (size_t i = 0; i < eargv.size(); ++i)
  {
      margv.push_back(eval_function(eargv[i]));
  }
  out = EvaluateExternalMath(nm, margv);

  return out;
}

template <typename DoubleType>
ModelExprData<DoubleType> ModelExprEval<DoubleType>::EvaluateInvalidType(Eqo::EqObjPtr arg)
{
  ModelExprData<DoubleType> out;

  std::ostringstream os;
  os << "Could not evaluate expression type for "
      << EngineAPI::getStringValue(arg);
  errors.push_back(os.str());

  return out;
}

template <typename DoubleType>
ModelExprData<DoubleType> ModelExprEval<DoubleType>::EvaluateConstantType(Eqo::EqObjPtr arg)
{
  ModelExprData<DoubleType> out;
  out = ModelExprData<DoubleType>(EngineAPI::getDoubleValue(arg), data_ref);
  return out;
}

template <typename DoubleType>
ModelExprData<DoubleType> ModelExprEval<DoubleType>::eval_function(Eqo::EqObjPtr arg)
{
  FPECheck::ClearFPE();

  const EngineAPI::EqObjType etype = EngineAPI::getEnumeratedType(arg);

#if 0
  os << "begin eval_function " << model << " = " << EngineAPI::getStringValue(arg)  << " size " << indexes.size() << "\n";
#endif

  ModelExprData<DoubleType> out(data_ref);

  bool cache_result = true;
  ModelExprDataCachePtr<DoubleType> cache = const_cast<Region *>(data_ref)->GetModelExprDataCache<DoubleType>();
  if (!cache)
  {
    cache = ModelExprDataCachePtr<DoubleType>(new ModelExprDataCache<DoubleType>());
    const_cast<Region *>(data_ref)->SetModelExprDataCache(cache);
  }

  if (cache->GetEntry(EngineAPI::getStringValue(arg), out))
  {
    cache_result = false;
  }
  else
  {
    switch(etype)
    {
      case EngineAPI::MODEL_OBJ:
        out = EvaluateModelType(arg);
        break;
      case EngineAPI::CONST_OBJ:
        out = EvaluateConstantType(arg);
        break;
      case EngineAPI::VARIABLE_OBJ:
        out = EvaluateVariableType(arg);
        break;
      case EngineAPI::PRODUCT_OBJ:
        out = EvaluateProductType(arg);
        break;
      case EngineAPI::ADD_OBJ:
        out = EvaluateAddType(arg);
        break;
      case EngineAPI::IF_OBJ:
        out = EvaluateIfType(arg);
        break;
      case EngineAPI::IFELSE_OBJ:
        out = EvaluateIfElseType(arg);
        break;
      case EngineAPI::USERFUNC_OBJ:
      case EngineAPI::EXPONENT_OBJ:
      case EngineAPI::POW_OBJ:
      case EngineAPI::LOG_OBJ:
      case EngineAPI::ULOGICAL_OBJ:
      case EngineAPI::BLOGICAL_OBJ:
        out = EvaluateFunctionType(arg);
        break;
      default:
        out = EvaluateInvalidType(arg);
        break;
    }
  }

  if (FPECheck::CheckFPE())
  {
    // invalid
    out = ModelExprData<DoubleType>(data_ref);
    std::ostringstream os;
    os << "There was a " << FPECheck::getFPEString() << " floating point exception while evaluating " << EngineAPI::getStringValue(arg);
    errors.push_back(os.str());
    FPECheck::ClearFPE();
  }

  if (cache_result && (out.GetType() != datatype::INVALID))
  {
    cache->SetEntry(EngineAPI::getStringValue(arg), out);
  }

#if 0
  os << "end eval_function " << model << " = " << EngineAPI::getStringValue(arg)  << " size " <<  indexes.size() << "\n";
#endif
  return out;
}

namespace {
/// need to do a bunch of checks to find out what we are dealing with data-wise
template <typename DoubleType>
struct checks {
  bool allArgsSame;
  bool hasInvalid;
  bool hasEdgeData;
  bool hasTriangleEdgeData;
  bool hasTetrahedronEdgeData;
  bool hasNodeData;
  bool hasDouble;
  datatype commonType;

  void doit(const typename ModelExprEval<DoubleType>::margv_t &argv)
  {
    dsAssert(!argv.empty(), "UNEXPECTED");
    // if we have at least one invalid in arg list, we are screwed
    hasInvalid          = false;
    hasEdgeData         = false;
    hasTriangleEdgeData = false;
    hasTetrahedronEdgeData = false;
    hasNodeData         = false;
    hasDouble           = false;
    for (size_t i = 0; i < argv.size(); ++i)
    {
      datatype type = argv[i].GetType();
      if (type == datatype::INVALID)
      {
        hasInvalid = true;
      }
      else if (type == datatype::NODEDATA)
      {
        hasNodeData = true;
      }
      else if (type == datatype::EDGEDATA)
      {
        hasEdgeData = true;
      }
      else if (type == datatype::TRIANGLEEDGEDATA)
      {
        hasTriangleEdgeData = true;
      }
      else if (type == datatype::TETRAHEDRONEDGEDATA)
      {
        hasTetrahedronEdgeData = true;
      }
      else if (type == datatype::DOUBLE)
      {
        hasDouble = true;
      }
      else
      {
        // should never happen
        dsAssert(false, "UNEXPECTED");
      }
    }

    commonType    = argv[0].GetType();
    allArgsSame   = true;
    for (size_t i = 1; i < argv.size(); ++i)
    {
      if (argv[i].GetType() != commonType)
      {
        allArgsSame = false;
        break;
      }
    }
  }
};
}

/// Need function for handling things that can be done locally by node and edge scalar data
/// We need to take ownership of data in argv to convert to triangle data
template <typename DoubleType>
ModelExprData<DoubleType> ModelExprEval<DoubleType>::EvaluateExternalMath(const std::string &name, margv_t &argv)
{
  bool invalid = false;
  // if one arg is invalid, we must abort
  ModelExprData<DoubleType> out(data_ref);
  checks<DoubleType> mytest;
  mytest.doit(argv);
  if (mytest.hasInvalid)
  {
    std::ostringstream os;
    os << "Invalid argument while evaluating function " << name;
    errors.push_back(os.str());
    invalid = true;
  }

  if (mytest.hasNodeData)
  {
    if (mytest.hasEdgeData)
    {
      std::ostringstream os;
      os << "Function " << name << " cannot have both edge and node model arguments";
      errors.push_back(os.str());
      invalid = true;
    }

    if (mytest.hasTriangleEdgeData || mytest.hasTetrahedronEdgeData)
    {
      std::ostringstream os;
      os << "Function " << name << " cannot have both element edge and node model arguments";
      errors.push_back(os.str());
      invalid = true;
    }
  }
  else if (mytest.hasEdgeData)
  {
    /// convert edgedata if one argument has triangle edge data
    if (mytest.hasTriangleEdgeData)
    {
      for (size_t i = 0; i < argv.size(); ++i)
      {
        argv[i].convertToTriangleEdgeData();
      }
      mytest.doit(argv);
    }

    /// convert edgedata if one argument has tetrahedron edge data
    if (mytest.hasTetrahedronEdgeData)
    {
      for (size_t i = 0; i < argv.size(); ++i)
      {
        argv[i].convertToTetrahedronEdgeData();
      }
      mytest.doit(argv);
    }

  }
  else if (mytest.hasTetrahedronEdgeData && mytest.hasTriangleEdgeData)
  {
    std::ostringstream os;
    os << "Function " << name << " cannot have both tetrahedron edge and triangle edge arguments";
    errors.push_back(os.str());
    invalid = true;
  }

  if (invalid)
  {
      return out;
  }

  // simplest case to handle
  if (mytest.allArgsSame && (mytest.commonType == datatype::DOUBLE))
  {
      const MathEval<DoubleType> &emath = MathEval<DoubleType>::GetInstance();

      std::string resultstr;
      std::vector<DoubleType> dargs;
      dargs.reserve(argv.size());
      for (size_t i = 0; i < argv.size(); ++i)
      {
          dargs.push_back(argv[i].GetDoubleValue());
      }
      DoubleType res = emath.EvaluateMathFunc(name, dargs, resultstr);
      if (!resultstr.empty())
      {
          errors.push_back(resultstr);
          invalid = true;
      }
      out = ModelExprData<DoubleType>(res, data_ref);
  }
  else
  {
    // vector args
    // we use pointers so be extra careful
    std::vector<DoubleType> output;
    /// default initializer for a vector of pointers is 0
    std::vector<const std::vector<DoubleType> *> vargs(argv.size());
    //// We enforce above that the vector length must be same across all arguments
    size_t vlen = 0;
    bool all_doubles = true;
    std::vector<DoubleType> dargs(argv.size());

    std::string resultstr;
    for (size_t i=0; i < argv.size(); ++i)
    {
      // todo employ caching scheme later on
      if (argv[i].GetType() == datatype::DOUBLE)
      {
        dargs[i] = argv[i].GetDoubleValue();
        vargs[i] = nullptr;
      }
      else if (argv[i].GetType() != datatype::INVALID)
      {
        ScalarValuesType<DoubleType> tval = argv[i].GetScalarValues();

        if (tval.IsUniform())
        {
          dargs[i] = tval.GetScalar();
          vlen = tval.GetLength();
          vargs[i] = nullptr;
        }
        else
        {
          all_doubles = false;
          vargs[i] = &(tval.GetVector());
          vlen = vargs[i]->size();
        }
      }
      else
      {
          /// We should never get here
        dsAssert(false, "UNEXPECTED");
      }
    }

    DoubleType res = 0.0;
    output.clear();
    output.resize(vlen);
    size_t k = 0;
    size_t indexsize = indexes.size();
//    os << "model " << model << " has indexsize " << indexsize << "\n";

    //// This is for our contacts
    //// It only evaluates indexes on our list
    if (indexsize)
    {
      bool tcheck = (((etype == ExpectedType::NODE) && mytest.hasNodeData)
                   ||((etype == ExpectedType::EDGE) && mytest.hasEdgeData));

      DoubleType lres = 0.0;
      if (all_doubles)
      {
        const MathEval<DoubleType> &emath = MathEval<DoubleType>::GetInstance();
        lres = emath.EvaluateMathFunc(name, dargs, resultstr);
      }

      //// for every number in our array
      for (size_t i=0; i < vlen; ++i)
      {
        /// This is in case we are processing edge data by mistake
        if (tcheck && (k < indexsize))
        {
          const size_t myindex = indexes[k];
//          os << "myindex " << myindex << "\t" << model << "\n";

          if (i < myindex)
          {
            continue;
          }
          else if (i == myindex)
          {
            ++k;
          }
        }
        else
        {
          //// If we are done
          continue;
        }

        if (all_doubles)
        {
          output[i] = lres;
        }
        else
        {
          for (size_t j=0; j < vargs.size(); ++j)
          {
            if(vargs[j] != nullptr)
            {
              dargs[j]=vargs[j]->operator[](i);
            }
          }

          const MathEval<DoubleType> &emath = MathEval<DoubleType>::GetInstance();
          DoubleType res = emath.EvaluateMathFunc(name, dargs, resultstr);
          output[i] = res;
        }

        if (!resultstr.empty())
        {
          errors.push_back(resultstr);
          /// no need to continue if there are errors
          break;
        }
      }
    }
    else
    {
      const MathEval<DoubleType> &emath = MathEval<DoubleType>::GetInstance();

      if (all_doubles)
      {
        output.clear();
        res = emath.EvaluateMathFunc(name, dargs, resultstr);
      }
      else
      {
//        res = 0.0;
        emath.EvaluateMathFunc(name, dargs, vargs, resultstr, output, vlen);
      }

      if (!resultstr.empty())
      {
        errors.push_back(resultstr);
      }
    }
    //// Apply similar refactoring for interface evaluation

    if (name == "vec_sum")
    {
      DoubleType sum = 0.0;
      if (all_doubles && output.empty())
      {
        sum = res * vlen;
      }
      else
      {
        sum = std::accumulate(output.begin(), output.end(), static_cast<DoubleType>(0.0));
      }
      out = ModelExprData<DoubleType>(sum, data_ref);
    }
    else if (name == "vec_max")
    {
      DoubleType max = 0.0;
      if (all_doubles && output.empty())
      {
        max = res;
      }
      else
      {
        max = *(std::max_element(output.begin(), output.end()));
      }
      out = ModelExprData<DoubleType>(max, data_ref);
    }
    else if (name == "vec_min")
    {
      DoubleType min = 0.0;
      if (all_doubles && output.empty())
      {
        min = res;
      }
      else
      {
        min = *(std::min_element(output.begin(), output.end()));
      }
      out = ModelExprData<DoubleType>(min, data_ref);
    }
    else if (all_doubles && output.empty())
    {
      if (mytest.hasNodeData)
      {
        out = ModelExprData<DoubleType>(NodeScalarData<DoubleType>(res, vlen), data_ref);
      }
      else if (mytest.hasEdgeData)
      {
        out = ModelExprData<DoubleType>(EdgeScalarData<DoubleType>(res, vlen), data_ref);
      }
      else if (mytest.hasTriangleEdgeData)
      {
        out = ModelExprData<DoubleType>(TriangleEdgeScalarData<DoubleType>(res, vlen), data_ref);
      }
      else if (mytest.hasTetrahedronEdgeData)
      {
        out = ModelExprData<DoubleType>(TetrahedronEdgeScalarData<DoubleType>(res, vlen), data_ref);
      }
    }
    else if (mytest.hasNodeData)
    {
      out = ModelExprData<DoubleType>(NodeScalarData<DoubleType>(output), data_ref);
    }
    else if (mytest.hasEdgeData)
    {
      out = ModelExprData<DoubleType>(EdgeScalarData<DoubleType>(output), data_ref);
    }
    else if (mytest.hasTriangleEdgeData)
    {
      out = ModelExprData<DoubleType>(TriangleEdgeScalarData<DoubleType>(output), data_ref);
    }
    else if (mytest.hasTetrahedronEdgeData)
    {
      out = ModelExprData<DoubleType>(TetrahedronEdgeScalarData<DoubleType>(output), data_ref);
    }
    else
    {
      dsAssert(false, "UNEXPECTED");
    }
  }

  if (!errors.empty())
  {
    // invalid
    out = ModelExprData<DoubleType>(data_ref);
  }

  return out;
}

template class ModelExprEval<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class ModelExprEval<float128>;
#endif
}

