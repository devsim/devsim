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

#include "MaterialDB.hh"

#include "ObjectCache.hh"

#include "MathEval.hh"

#include "EngineAPI.hh"

#include <sstream>
#include <numeric>

namespace MEE {

ModelExprEval::ModelExprEval(data_ref_t &vals, const std::string &m, error_t &er) : data_ref(vals), model(m), errors(er), etype(UNKNOWN)
{
  const Region *rp = data_ref;
  dsAssert(rp != NULL, "UNEXPECTED");

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
    etype = NODE;
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
    etype = EDGE;
  }
  else if (rp->GetTriangleEdgeModel(model))
  {
    etype = TRIANGLEEDGE;
  }
  else if (rp->GetTetrahedronEdgeModel(model))
  {
    etype = TETRAHEDRONEDGE;
  }
}

ModelExprEval::~ModelExprEval()
{
}

ModelExprData ModelExprEval::EvaluateModelType(Eqo::EqObjPtr arg)
{
  const std::string &model = EngineAPI::getStringValue(arg);

  const Region *rp = data_ref;

  ModelExprData out(rp);
  dsAssert(rp != NULL, "UNEXPECTED");


  if (ConstNodeModelPtr nm = rp->GetNodeModel(model))
  {
    if (nm->IsInProcess())
    {
      std::ostringstream os;
      os << "Cyclic dependency while evaluating NodeModel \"" << model;
      errors.push_back(os.str());
      out = ModelExprData(rp);
    }
    else
    {
      out = ModelExprData(nm, rp);
    }
  }
  else if (ConstEdgeModelPtr nm = rp->GetEdgeModel(model))
  {
    if (nm->IsInProcess())
    {
      std::ostringstream os;
      os << "Cyclic dependency while evaluating EdgeModel \"" << model;
      errors.push_back(os.str());
      out = ModelExprData(rp);
    }
    else
    {
      out = ModelExprData(nm, rp);
    }
  }
  else if (ConstTriangleEdgeModelPtr nm = rp->GetTriangleEdgeModel(model))
  {
    if (nm->IsInProcess())
    {
      std::ostringstream os;
      os << "Cyclic dependency while evaluating TriangleEdgeModel \"" << model;
      errors.push_back(os.str());
      out = ModelExprData(rp);
    }
    else
    {
      out = ModelExprData(nm, rp);
    }
  }
  else if (ConstTetrahedronEdgeModelPtr nm = rp->GetTetrahedronEdgeModel(model))
  {
    if (nm->IsInProcess())
    {
      std::ostringstream os;
      os << "Cyclic dependency while evaluating TetrahedronEdgeModel \"" << model;
      errors.push_back(os.str());
      out = ModelExprData(rp);
    }
    else
    {
      out = ModelExprData(nm, rp);
    }
  }
  else
  {
//      std::ostringstream os;
//      os << "Could not find a model by the name of " << model << ", using 0.0";
//      errors.push_back(os.str());
    out = ModelExprData(0.0, rp);

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
          GeometryStream::WriteOut(OutputStream::VERBOSE1, *rp, os.str());

          if (nm->IsInProcess())
          {
            std::ostringstream os;
            os << "Cyclic dependency while evaluating EdgeModel \"" << model;
            errors.push_back(os.str());
            out = ModelExprData(rp);
          }
          else
          {
            out = ModelExprData(nm, rp);
          }
        }
      }
    }
  }

  return out;
}

ModelExprData ModelExprEval::EvaluateVariableType(Eqo::EqObjPtr arg)
{
  ModelExprData out;

  GlobalData &gd  = GlobalData::GetInstance();
  MaterialDB &mdb = MaterialDB::GetInstance();
  NodeKeeper &nk = NodeKeeper::instance();

  const std::string &nm = EngineAPI::getName(arg);
  /**
   * Get the DB entry here
   */
  const GlobalData::DoubleDBEntry_t &gdbent  = gd.GetDoubleDBEntryOnRegion(data_ref, nm);
  const MaterialDB::DoubleDBEntry_t &mdbentr = mdb.GetDoubleDBEntry(data_ref->GetMaterialName(), nm);
  const MaterialDB::DoubleDBEntry_t &mdbentg = mdb.GetDoubleDBEntry("global", nm);

  if (gdbent.first)
  {
    out = ModelExprData(gdbent.second, data_ref);
  }
  else if (mdbentr.first)
  {
    out = ModelExprData(mdbentr.second, data_ref);
  }
  else if (mdbentg.first)
  {
    out = ModelExprData(mdbentg.second, data_ref);
  }
  else if (nk.IsCircuitNode(nm))
  {
    const double val = nk.GetNodeValue("dcop", nm);
    out = ModelExprData(val, data_ref);
#if 0
    std::ostringstream os; 
    os << "circuit using : " << nm << " " << val << "\n";
    GeometryStream::WriteOut(OutputStream::INFO, os.str());
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

ModelExprData ModelExprEval::EvaluateProductType(Eqo::EqObjPtr arg)
{
  ModelExprData out;
  //
  /// do a *= multiple times to return result
  std::vector<Eqo::EqObjPtr> values = EngineAPI::getArgs(arg);
  /// Rewrite this later
  out = ModelExprData(1.0, data_ref);
//    out = eval_function(values[0]);
  /// Premature optimization
  /// short circuit multiplication
  for (size_t i = 0; i < values.size(); ++i)
  {
    ModelExprData x = eval_function(values[i]);
    if (
      (x.GetType() == ModelExprData::DOUBLE) &&
      (x.GetDoubleValue() == 0.0)
    )
    {
      out = ModelExprData(0.0, data_ref);
      break;
    }
    else
    {
      out *= x;
    }
  }

  return out;
}

ModelExprData ModelExprEval::EvaluateIfType(Eqo::EqObjPtr arg)
{
  ModelExprData out;

  out = ModelExprData(0.0, data_ref);
  std::vector<Eqo::EqObjPtr> values = EngineAPI::getArgs(arg);

  dsAssert(values.size() == 2, "UNEXPECTED");

  ModelExprData test = eval_function(values[0]);
  if ((test.GetType() == ModelExprData::DOUBLE))
  {
    if (test.GetDoubleValue() == 0.0)
    {
      out = ModelExprData(0.0, data_ref);
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

ModelExprData ModelExprEval::EvaluateIfElseType(Eqo::EqObjPtr arg)
{
  ModelExprData out;

  out = ModelExprData(0.0, data_ref);
  std::vector<Eqo::EqObjPtr> values = EngineAPI::getArgs(arg);

  dsAssert(values.size() == 3, "UNEXPECTED");

  ModelExprData test = eval_function(values[0]);
  if ((test.GetType() == ModelExprData::DOUBLE))
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

ModelExprData ModelExprEval::EvaluateAddType(Eqo::EqObjPtr arg)
{
  ModelExprData out;

//    out = ModelExprData(0.0, data_ref);
  std::vector<Eqo::EqObjPtr> values = EngineAPI::getArgs(arg);
  out = eval_function(values[0]);
  for (size_t i = 1; i < values.size(); ++i)
  {
    ModelExprData x = eval_function(values[i]);
    out += x;
  }

  return out;
}

ModelExprData ModelExprEval::EvaluateFunctionType(Eqo::EqObjPtr arg)
{
  ModelExprData out;

  const EngineAPI::EqObjType etype = EngineAPI::getEnumeratedType(arg);
  //
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

ModelExprData ModelExprEval::EvaluateInvalidType(Eqo::EqObjPtr arg)
{
  ModelExprData out;

  std::ostringstream os;
  os << "Could not evaluate expression type for "
      << EngineAPI::getStringValue(arg);
  errors.push_back(os.str());

  return out;
}

ModelExprData ModelExprEval::EvaluateConstantType(Eqo::EqObjPtr arg)
{
  ModelExprData out;
  out = ModelExprData(EngineAPI::getDoubleValue(arg), data_ref);
  return out;
}

ModelExprData ModelExprEval::eval_function(Eqo::EqObjPtr arg)
{
  FPECheck::ClearFPE();

  const EngineAPI::EqObjType etype = EngineAPI::getEnumeratedType(arg);

#if 0
  os << "begin eval_function " << model << " = " << EngineAPI::getStringValue(arg)  << " size " << indexes.size() << "\n";
#endif

  ModelExprData out(data_ref);

  bool cache_result = true;
  ModelExprDataCachePtr cache = const_cast<Region *>(data_ref)->GetModelExprDataCache();
  if (!cache)
  {
    cache = ModelExprDataCachePtr(new ModelExprDataCache()); 
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
    out.type = ModelExprData::INVALID;
    std::ostringstream os;
    os << "There was a " << FPECheck::getFPEString() << " floating point exception while evaluating " << EngineAPI::getStringValue(arg);
    errors.push_back(os.str());
    FPECheck::ClearFPE();
  }

  if (cache_result && (out.type != ModelExprData::INVALID))
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
struct checks {
  bool allArgsSame;
  bool hasInvalid;
  bool hasEdgeData;
  bool hasTriangleEdgeData;
  bool hasTetrahedronEdgeData;
  bool hasNodeData;
  bool hasDouble;
  ModelExprData::datatype commonType;

  void doit(const ModelExprEval::margv_t &argv)
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
      ModelExprData::datatype type = argv[i].GetType();
      if (type == ModelExprData::INVALID)
      {
        hasInvalid = true;
      }
      else if (type == ModelExprData::NODEDATA)
      {
        hasNodeData = true;
      }
      else if (type == ModelExprData::EDGEDATA)
      {
        hasEdgeData = true;
      }
      else if (type == ModelExprData::TRIANGLEEDGEDATA)
      {
        hasTriangleEdgeData = true;
      }
      else if (type == ModelExprData::TETRAHEDRONEDGEDATA)
      {
        hasTetrahedronEdgeData = true;
      }
      else if (type == ModelExprData::DOUBLE)
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
ModelExprData ModelExprEval::EvaluateExternalMath(const std::string &name, margv_t &argv)
{
  bool invalid = false;
  // if one arg is invalid, we must abort
  ModelExprData out(data_ref);
  checks mytest;
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
  if (mytest.allArgsSame && (mytest.commonType == ModelExprData::DOUBLE))
  {
      const MathEval &emath = MathEval::GetInstance(); 

      std::string resultstr;
      std::vector<double> dargs;
      dargs.reserve(argv.size());
      for (size_t i = 0; i < argv.size(); ++i)
      {
          dargs.push_back(argv[i].GetDoubleValue());
      }
      double res = emath.EvaluateMathFunc(name, dargs, resultstr);
      if (!resultstr.empty())
      {
          errors.push_back(resultstr);
          invalid = true;
      }
      out = ModelExprData(res, data_ref);
  }
  else
  {
    // vector args
    // we use pointers so be extra careful
    std::vector<double> output;
    /// default initializer for a vector of pointers is 0
    std::vector<const std::vector<double> *> vargs(argv.size());
    //// We enforce above that the vector length must be same across all arguments
    size_t vlen = 0;
    bool all_doubles = true;
    std::vector<double> dargs(argv.size());

    std::string resultstr;
    for (size_t i=0; i < argv.size(); ++i)
    {
      // todo employ caching scheme later on
      if (argv[i].GetType() == ModelExprData::DOUBLE)
      {
        dargs[i] = argv[i].GetDoubleValue();
        vargs[i] = NULL;
      }
      else if (argv[i].GetType() != ModelExprData::INVALID)
      {
        ModelExprData::ScalarValuesType tval = argv[i].GetScalarValues();

        if (tval.IsUniform())
        {
          dargs[i] = tval.GetScalar();
          vlen = tval.GetLength();
          vargs[i] = NULL;
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

    double res = 0.0;
    output.clear();
    output.resize(vlen);
    size_t k = 0;
    size_t indexsize = indexes.size();
//    os << "model " << model << " has indexsize " << indexsize << "\n"; 

    //// This is for our contacts
    //// It only evaluates indexes on our list
    if (indexsize)
    {
      bool tcheck = (((etype == NODE) && mytest.hasNodeData) 
                   ||((etype == EDGE) && mytest.hasEdgeData));

      double lres = 0.0;
      if (all_doubles)
      {
        const MathEval &emath = MathEval::GetInstance(); 
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
            if(vargs[j] != NULL)
            {
              dargs[j]=vargs[j]->operator[](i);
            }
          }

          const MathEval &emath = MathEval::GetInstance(); 
          double res = emath.EvaluateMathFunc(name, dargs, resultstr);
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
      const MathEval &emath = MathEval::GetInstance(); 

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

    //// This is the only function that can turn a vector into a scalar, and doesn't care if original data is edge or node
    if (name == "sum")
    {
      double sum = 0.0;
      if (all_doubles && output.empty())
      {
        sum = res * vlen;
      }
      else
      {
        sum = std::accumulate(output.begin(), output.end(), 0.0);
      }
      out = ModelExprData(sum, data_ref);
    }
    else if (all_doubles && output.empty())
    {
      if (mytest.hasNodeData)
      {
        NodeScalarData *mynsd = new NodeScalarData(res, vlen);
        out.nodeScalarData = ModelExprData::nodeScalarData_ptr(mynsd);
        out.type = ModelExprData::NODEDATA;
      }
      else if (mytest.hasEdgeData)
      {
        EdgeScalarData *mynsd = new EdgeScalarData(res, vlen);
        out.edgeScalarData = ModelExprData::edgeScalarData_ptr(mynsd);
        out.type = ModelExprData::EDGEDATA;
      }
      else if (mytest.hasTriangleEdgeData)
      {
        TriangleEdgeScalarData *mynsd = new TriangleEdgeScalarData(res, vlen);
        out.triangleEdgeScalarData = ModelExprData::triangleEdgeScalarData_ptr(mynsd);
        out.type = ModelExprData::TRIANGLEEDGEDATA;
      }
      else if (mytest.hasTetrahedronEdgeData)
      {
        TetrahedronEdgeScalarData *mynsd = new TetrahedronEdgeScalarData(output);
        out.tetrahedronEdgeScalarData = ModelExprData::tetrahedronEdgeScalarData_ptr(mynsd);
        out.type = ModelExprData::TETRAHEDRONEDGEDATA;
      }
    }
    else if (mytest.hasNodeData)
    {
      NodeScalarData *mynsd = new NodeScalarData(output);
      out.nodeScalarData = ModelExprData::nodeScalarData_ptr(mynsd);
      out.type = ModelExprData::NODEDATA;
    }
    else if (mytest.hasEdgeData)
    {
      EdgeScalarData *mynsd = new EdgeScalarData(output);
      out.edgeScalarData = ModelExprData::edgeScalarData_ptr(mynsd);
      out.type = ModelExprData::EDGEDATA;
    }
    else if (mytest.hasTriangleEdgeData)
    {
      TriangleEdgeScalarData *mynsd = new TriangleEdgeScalarData(output);
      out.triangleEdgeScalarData = ModelExprData::triangleEdgeScalarData_ptr(mynsd);
      out.type = ModelExprData::TRIANGLEEDGEDATA;
    }
    else if (mytest.hasTetrahedronEdgeData)
    {
      TetrahedronEdgeScalarData *mynsd = new TetrahedronEdgeScalarData(output);
      out.tetrahedronEdgeScalarData = ModelExprData::tetrahedronEdgeScalarData_ptr(mynsd);
      out.type = ModelExprData::TETRAHEDRONEDGEDATA;
    }
    else
    {
      dsAssert(false, "UNEXPECTED");
    }
  }

  if (!errors.empty())
  {
    out.type = ModelExprData::INVALID;
  }

  return out;
}
}

