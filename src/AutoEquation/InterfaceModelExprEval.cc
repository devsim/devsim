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

#include "InterfaceModelExprEval.hh"
#include "Interface.hh"
#include "FPECheck.hh"
#include "GlobalData.hh"
#include "MaterialDB.hh"
#include "NodeKeeper.hh"
#include "Region.hh"
#include "dsAssert.hh"
#include "MathEval.hh"
#include "InterfaceNodeScalarData.hh"
#include "NodeModel.hh"
#include "InterfaceNodeModel.hh"
#include "Node.hh"

#include "ObjectCache.hh"

#include "EngineAPI.hh"
#include <numeric>
#include <algorithm>

namespace IMEE {


/// Finds the appropriate region and the name in the region
/// Need to support the derivatives later
/// Create Interface model derivative type
void InterfaceModelExprEval::GetRegionAndName(const std::string &nm, std::string &name, const Region *&r)
{
  name = nm;

  r = 0;

  std::string::size_type rpos = nm.size();
  if (rpos > 3)
  {
    rpos -= 3;
  }

  if (nm.rfind("@r0") == rpos)
  {
    name.erase(rpos);
    r = data_ref->GetRegion0();
  }
  else if (nm.rfind("@r1") == rpos)
  {
    name.erase(rpos);
    r = data_ref->GetRegion1();
  }
}

InterfaceModelExprEval::InterfaceModelExprEval(data_ref_t &vals, error_t &er) : data_ref(vals), errors(er)
{
}

InterfaceModelExprEval::~InterfaceModelExprEval()
{
}

InterfaceModelExprData InterfaceModelExprEval::EvaluateConstantType(Eqo::EqObjPtr arg)
{
  InterfaceModelExprData out;

  out = InterfaceModelExprData(EngineAPI::getDoubleValue(arg));

  return out;
}

InterfaceModelExprData InterfaceModelExprEval::EvaluateVariableType(Eqo::EqObjPtr arg)
{
  InterfaceModelExprData out;

  GlobalData &gd  = GlobalData::GetInstance();
  MaterialDB &mdb = MaterialDB::GetInstance();
  NodeKeeper &nk = NodeKeeper::instance();

  const std::string &nm = EngineAPI::getName(arg);
  /**
   * Get the DB entry here
   */
  const Region *r;
  std::string name;
  GetRegionAndName(nm, name, r);

  if (r)
  {
      const GlobalData::DoubleDBEntry_t &gdbent = gd.GetDoubleDBEntryOnRegion(r, name);
      const MaterialDB::DoubleDBEntry_t &mdbentr = mdb.GetDoubleDBEntry(r->GetMaterialName(), nm);
      const MaterialDB::DoubleDBEntry_t &mdbentg = mdb.GetDoubleDBEntry("global", nm);
      if (gdbent.first)
      {
          out = InterfaceModelExprData(gdbent.second);
      }
      else if (mdbentr.first)
      {
          out = InterfaceModelExprData(mdbentr.second);
      }
      else if (mdbentg.first)
      {
          out = InterfaceModelExprData(mdbentg.second);
      }
      else if (nk.IsCircuitNode(nm))
      {
        const double val = nk.GetNodeValue("dcop", nm);
        out = InterfaceModelExprData(val);
      }
  }
  else
  {
    /// TODO: Fix this so we check on the device, or specific interface
    const GlobalData::DoubleDBEntry_t &gdbent0 = gd.GetDoubleDBEntryOnRegion(data_ref->GetRegion0(), nm);
    const GlobalData::DoubleDBEntry_t &gdbent1 = gd.GetDoubleDBEntryOnRegion(data_ref->GetRegion1(), nm);

    if (gdbent0.first && gdbent1.first)
    {
      if (gdbent0.second != gdbent1.second)
      {
        std::ostringstream os;
        os << "Regions on inteface have different material db value for \"" << nm << "\"";
        errors.push_back(os.str());
      }
      else
      {
        out = InterfaceModelExprData(gdbent0.second);
      }
    }
    else if (nk.IsCircuitNode(nm))
    {
      const double val = nk.GetNodeValue("dcop", nm);
      out = InterfaceModelExprData(val);
    }
    else
    {
      std::ostringstream os;
      os << "Cannot find material db value for \"" << nm << "\" not available, using 0.0";
      errors.push_back(os.str());
    }
  }

  return out;
}

/**
 Implement for InterfaceModels
 Language we will employ

 nodemodel
     gets the node model
 edgemodel
     gets the edge model
 nodemodel@n0 an edge model evaluated at the first node on the edge
 nodemodel@n1 an edge model evaluated at the second node on the edge
     For this we can create an node->edgemodel map in a region

  Derivatives for later on
 nodemodel:nodemodel
 nodemodel@n0:nodemodel@n0 = (nodemodel:nodemodel)@n0
 nodemodel@n1:nodemodel@n1 = (edgemodel:edgemodel)@n0
 nodemodel@n0:nodemodel@n1 = 0
 nodemodel@n1:nodemodel@n0 = 0
 edgemodel:nodemodel@n0
 edgemodel:nodemodel@n1
 edgemodel:edgemodel
     derivative with respect to local node quantities
*/
InterfaceModelExprData InterfaceModelExprEval::EvaluateInterfaceModelType(Eqo::EqObjPtr arg)
{
    const std::string &m= EngineAPI::getStringValue(arg);

    InterfaceModelExprData out;
    const Region *r = NULL;
    std::string model;

    GetRegionAndName(m, model, r);

    if (ConstInterfaceNodeModelPtr inmp = data_ref->GetInterfaceNodeModel(m))
    {
      if (inmp->IsInProcess())
      {
        std::ostringstream os;
        os << "Cyclic dependency while evaluating InterfaceNodeModel \"" << m;
        errors.push_back(os.str());
        out = InterfaceModelExprData();
      }
      else
      {
        out = InterfaceModelExprData(InterfaceNodeScalarData(*inmp));
      }
    }
    else if (r)
    {
        //// The assumption here is that all node models cannot depend on interface models
        if (ConstNodeModelPtr nm = r->GetNodeModel(model))
        {
            Interface::ConstNodeList_t nodevec;
            if (r == data_ref->GetRegion0())
            {
// TODO: note this means that we are evaluating models on an interface, which may be skipped as non-ActiveNodes() from the interface equation.
                nodevec = data_ref->GetNodes0();
            }
            else if (r == data_ref->GetRegion1())
            {
                nodevec = data_ref->GetNodes1();
            }
            else
            {
                dsAssert(0, "UNEXPECTED");
            }


            const NodeScalarList &nvals = nm->GetScalarValues(); 

//          dsAssert(nodevec.size() == nvals.size(), "UNEXPECTED");

            std::vector<double> data(nodevec.size());

            for (size_t i = 0; i < nodevec.size(); ++i)
            {
                const size_t index = nodevec[i]->GetIndex();
                data[i] = nvals[index];
            }


            out = InterfaceModelExprData(InterfaceNodeScalarData(data));
        }
    }

    if (out.GetType() == InterfaceModelExprData::INVALID)
    {
        std::ostringstream os;
        os << "Could not find or evaluate a model by the name of " << m << ", using 0.0";
        errors.push_back(os.str());
        out = InterfaceModelExprData(0.0);
    }

    return out;
}

InterfaceModelExprData InterfaceModelExprEval::EvaluateProductType(Eqo::EqObjPtr arg)
{
  InterfaceModelExprData out;

  /// do a *= multiple times to return result
  std::vector<Eqo::EqObjPtr> values = EngineAPI::getArgs(arg);
  /// Rewrite this later
  out = InterfaceModelExprData(1.0);
  /// Premature optimization
  /// short circuit multiplication
  //// TODO: optimize here and in ModelExprEval for case when one of these has zero data
  for (size_t i = 0; i < values.size(); ++i)
  {
      InterfaceModelExprData x = eval_function(values[i]);
      if (
          (x.GetType() == InterfaceModelExprData::DOUBLE) &&
          (x.GetDoubleValue() == 0.0)
         )
      {
          out = InterfaceModelExprData(0.0);
          break;
      }
      else
      {
          out *= x;
      }
  }

  return out;
}

InterfaceModelExprData InterfaceModelExprEval::EvaluateAddType(Eqo::EqObjPtr arg)
{
  InterfaceModelExprData out;

  std::vector<Eqo::EqObjPtr> values = EngineAPI::getArgs(arg);
  out = eval_function(values[0]);
  for (size_t i = 1; i < values.size(); ++i)
  {
      InterfaceModelExprData x = eval_function(values[i]);
      out += x;
  }

  return out;
}

InterfaceModelExprData InterfaceModelExprEval::EvaluateIfType(Eqo::EqObjPtr arg)
{
  InterfaceModelExprData out;

  out = InterfaceModelExprData(0.0);
  std::vector<Eqo::EqObjPtr> values = EngineAPI::getArgs(arg);

  dsAssert(values.size() == 2, "UNEXPECTED");

  InterfaceModelExprData test = eval_function(values[0]);
  if ((test.GetType() == InterfaceModelExprData::DOUBLE))
  {
      if (test.GetDoubleValue() == 0.0)
      {
        out = InterfaceModelExprData(0.0);
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

InterfaceModelExprData InterfaceModelExprEval::EvaluateIfElseType(Eqo::EqObjPtr arg)
{
  InterfaceModelExprData out;

  out = InterfaceModelExprData(0.0);
  std::vector<Eqo::EqObjPtr> values = EngineAPI::getArgs(arg);

  dsAssert(values.size() == 3, "UNEXPECTED");

  InterfaceModelExprData test = eval_function(values[0]);
  if ((test.GetType() == InterfaceModelExprData::DOUBLE))
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

InterfaceModelExprData InterfaceModelExprEval::EvaluateFunctionType(Eqo::EqObjPtr arg)
{
  const EngineAPI::EqObjType etype = EngineAPI::getEnumeratedType(arg);

  InterfaceModelExprData out;
  /// Implement for UserFuncs
  std::string nm = EngineAPI::getName(arg);

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

InterfaceModelExprData InterfaceModelExprEval::EvaluateInvalidType(Eqo::EqObjPtr arg)
{
  InterfaceModelExprData out;

  //out.type = INVALID;
  std::ostringstream os;
  os << "Could not evaluate expression type for "
      << EngineAPI::getStringValue(arg);
  errors.push_back(os.str());

  return out;
}

InterfaceModelExprData InterfaceModelExprEval::eval_function(Eqo::EqObjPtr arg)
{
  FPECheck::ClearFPE();

  const EngineAPI::EqObjType etype = EngineAPI::getEnumeratedType(arg);

#if 0
  os << "begin eval_function " << EngineAPI::getStringValue(arg) << "\n";
#endif

  InterfaceModelExprData out;

  //// TODO: Refactor so we don't become out of sync with the ModelExprEval

  bool cache_result = true;
  InterfaceModelExprDataCachePtr cache = const_cast<Interface *>(data_ref)->GetInterfaceModelExprDataCache();
  if (!cache)
  {
    cache = InterfaceModelExprDataCachePtr(new InterfaceModelExprDataCache()); 
    const_cast<Interface *>(data_ref)->SetInterfaceModelExprDataCache(cache);
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
        out = EvaluateInterfaceModelType(arg);
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
    out.type = InterfaceModelExprData::INVALID;
    std::ostringstream os;
    os << "There was a " << FPECheck::getFPEString() << " floating point exception while evaluating " << EngineAPI::getStringValue(arg);
    errors.push_back(os.str());
    FPECheck::ClearFPE();
  }


  if (cache_result && (out.type != InterfaceModelExprData::INVALID))
  {
    cache->SetEntry(EngineAPI::getStringValue(arg), out);
  }

  return out;
}

namespace {
    /// need to do a bunch of checks to find out what we are dealing with data-wise
    struct checks {
        bool allArgsSame;
        bool hasInvalid;
        bool hasNodeData;
        bool hasDouble;
        InterfaceModelExprData::datatype commonType;

        void doit(const InterfaceModelExprEval::margv_t &argv)
        {
            dsAssert(!argv.empty(), "UNEXPECTED");
            // if we have at least one invalid in arg list, we are screwed
            hasInvalid    = false;
            hasNodeData   = false;
            hasDouble     = false;
            for (size_t i = 0; i < argv.size(); ++i)
            {
                InterfaceModelExprData::datatype type = argv[i].GetType();
                if (type == InterfaceModelExprData::INVALID)
                {
                    hasInvalid = true;
                }
                else if (type == InterfaceModelExprData::NODEDATA)
                {
                    hasNodeData = true;
                }
                else if (type == InterfaceModelExprData::DOUBLE)
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

InterfaceModelExprData InterfaceModelExprEval::EvaluateExternalMath(const std::string &name, const margv_t &argv)
{
  bool invalid = false;
  // if one arg is invalid, we must abort
  InterfaceModelExprData out;
  checks mytest;
  mytest.doit(argv);
  if (mytest.hasInvalid)
  {
    std::ostringstream os;
    os << "Invalid argument while evaluating function " << name;
    errors.push_back(os.str());
    invalid = true;
  }

  /// We may have special conversion factors later on, but that will be very difficult to implement
  if (invalid)
  {
    return out;
  }

  // simplest case to handle
  if (mytest.allArgsSame && (mytest.commonType == InterfaceModelExprData::DOUBLE))
  {
    std::string resultstr;
    std::vector<double> dargs;
    dargs.reserve(argv.size());
    for (size_t i = 0; i < argv.size(); ++i)
    {
      dargs.push_back(argv[i].GetDoubleValue());
    }
    const MathEval &emath = MathEval::GetInstance(); 
    double res = emath.EvaluateMathFunc(name, dargs, resultstr);
    if (!resultstr.empty())
    {
      errors.push_back(resultstr);
      invalid = true;
    }
    out = InterfaceModelExprData(res);
  }
  else
  {
    // vector args
    // we use pointers so be extra careful
    std::vector<double> output;
    /// default initializer for a vector of pointers is 0
    std::vector<const std::vector<double> *> vargs(argv.size());
    bool   all_doubles = true;
    size_t vlen = 0;
    std::vector<double> dargs(argv.size());

    std::string resultstr;
    for (size_t i=0; i < argv.size(); ++i)
    {
      // todo employ caching scheme later on
      if (argv[i].GetType() == InterfaceModelExprData::DOUBLE)
      {
        dargs[i] = argv[i].GetDoubleValue();
        vargs[i] = NULL;
      }
      else if (argv[i].GetType() != InterfaceModelExprData::INVALID)
      {
        InterfaceModelExprData::ScalarValuesType tval = argv[i].GetScalarValues();
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

///// TODO: write test to make sure external math is working properly here
#if 0
      output.resize(vlen);
      for (size_t i=0; i < vlen; ++i)
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
          if (!resultstr.empty())
          {
              errors.push_back(resultstr);
              /// no need to continue if there are errors
              break;
          }
      }
#else
    const MathEval &emath = MathEval::GetInstance(); 
    if (all_doubles)
    {
      output.clear();
      double res = emath.EvaluateMathFunc(name, dargs, resultstr);
      if (resultstr.empty())
      {
        InterfaceNodeScalarData *mynsd = new InterfaceNodeScalarData(res, vlen);
        out.nsd = InterfaceModelExprData::nsd_ptr(mynsd);
        out.type = InterfaceModelExprData::NODEDATA;
      }
    }
    else
    {
      output.resize(vlen);
      emath.EvaluateMathFunc(name, dargs, vargs, resultstr, output, vlen);
      if (resultstr.empty())
      {
        if (name == "vec_sum")
        {
          double sum = std::accumulate(output.begin(), output.end(), 0.0);
          out = InterfaceModelExprData(sum);
        }
        else if (name == "vec_max")
        {
          double max = *(std::max_element(output.begin(), output.end()));
          out = InterfaceModelExprData(max);
        }
        else if (name == "vec_min")
        {
          double min = *(std::min_element(output.begin(), output.end()));
          out = InterfaceModelExprData(min);
        }
        else
        {
          InterfaceNodeScalarData *mynsd = new InterfaceNodeScalarData(output);
          out.nsd = InterfaceModelExprData::nsd_ptr(mynsd);
          out.type = InterfaceModelExprData::NODEDATA;
        }
      }
    }

#endif

    if (!resultstr.empty())
    {
      errors.push_back(resultstr);
    }
  }

  if (!errors.empty())
  {
    out.type = InterfaceModelExprData::INVALID;
  }

  return out;
}
};

