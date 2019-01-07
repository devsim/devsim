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
template <typename DoubleType>
void InterfaceModelExprEval<DoubleType>::GetRegionAndName(const std::string &nm, std::string &name, const Region *&r)
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

template <typename DoubleType>
InterfaceModelExprEval<DoubleType>::InterfaceModelExprEval(data_ref_t &vals, error_t &er) : data_ref(vals), errors(er)
{
}

template <typename DoubleType>
InterfaceModelExprEval<DoubleType>::~InterfaceModelExprEval()
{
}

template <typename DoubleType>
InterfaceModelExprData<DoubleType> InterfaceModelExprEval<DoubleType>::EvaluateConstantType(Eqo::EqObjPtr arg)
{
  InterfaceModelExprData<DoubleType> out;

  out = InterfaceModelExprData<DoubleType>(EngineAPI::getDoubleValue(arg));

  return out;
}

template <typename DoubleType>
InterfaceModelExprData<DoubleType> InterfaceModelExprEval<DoubleType>::EvaluateVariableType(Eqo::EqObjPtr arg)
{
  InterfaceModelExprData<DoubleType> out;

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
          out = InterfaceModelExprData<DoubleType>(gdbent.second);
      }
      else if (mdbentr.first)
      {
          out = InterfaceModelExprData<DoubleType>(mdbentr.second);
      }
      else if (mdbentg.first)
      {
          out = InterfaceModelExprData<DoubleType>(mdbentg.second);
      }
      else if (nk.IsCircuitNode(nm))
      {
        const DoubleType val = nk.GetNodeValue("dcop", nm);
        out = InterfaceModelExprData<DoubleType>(val);
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
        out = InterfaceModelExprData<DoubleType>(gdbent0.second);
      }
    }
    else if (nk.IsCircuitNode(nm))
    {
      const DoubleType val = nk.GetNodeValue("dcop", nm);
      out = InterfaceModelExprData<DoubleType>(val);
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
template <typename DoubleType>
InterfaceModelExprData<DoubleType> InterfaceModelExprEval<DoubleType>::EvaluateInterfaceModelType(Eqo::EqObjPtr arg)
{
    const std::string &m= EngineAPI::getStringValue(arg);

    InterfaceModelExprData<DoubleType> out;
    const Region *r = nullptr;
    std::string model;

    GetRegionAndName(m, model, r);

    if (ConstInterfaceNodeModelPtr inmp = data_ref->GetInterfaceNodeModel(m))
    {
      if (inmp->IsInProcess())
      {
        std::ostringstream os;
        os << "Cyclic dependency while evaluating InterfaceNodeModel \"" << m;
        errors.push_back(os.str());
        out = InterfaceModelExprData<DoubleType>();
      }
      else
      {
        out = InterfaceModelExprData<DoubleType>(InterfaceNodeScalarData<DoubleType>(*inmp));
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


            const NodeScalarList<DoubleType> &nvals = nm->GetScalarValues<DoubleType>(); 

//          dsAssert(nodevec.size() == nvals.size(), "UNEXPECTED");

            std::vector<DoubleType> data(nodevec.size());

            for (size_t i = 0; i < nodevec.size(); ++i)
            {
                const size_t index = nodevec[i]->GetIndex();
                data[i] = nvals[index];
            }


            out = InterfaceModelExprData<DoubleType>(InterfaceNodeScalarData<DoubleType>(data));
        }
    }

    if (out.GetType() == datatype::INVALID)
    {
        std::ostringstream os;
        os << "Could not find or evaluate a model by the name of " << m << ", using 0.0";
        errors.push_back(os.str());
        out = InterfaceModelExprData<DoubleType>(0.0);
    }

    return out;
}

template <typename DoubleType>
InterfaceModelExprData<DoubleType> InterfaceModelExprEval<DoubleType>::EvaluateProductType(Eqo::EqObjPtr arg)
{
  InterfaceModelExprData<DoubleType> out;

  /// do a *= multiple times to return result
  std::vector<Eqo::EqObjPtr> values = EngineAPI::getArgs(arg);
  /// Rewrite this later
  out = InterfaceModelExprData<DoubleType>(1.0);
  /// Premature optimization
  /// short circuit multiplication
  //// TODO: optimize here and in ModelExprEval for case when one of these has zero data
  for (size_t i = 0; i < values.size(); ++i)
  {
      InterfaceModelExprData<DoubleType> x = eval_function(values[i]);
      if (
          (x.GetType() == datatype::DOUBLE) &&
          (x.GetDoubleValue() == 0.0)
         )
      {
          out = InterfaceModelExprData<DoubleType>(0.0);
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
InterfaceModelExprData<DoubleType> InterfaceModelExprEval<DoubleType>::EvaluateAddType(Eqo::EqObjPtr arg)
{
  InterfaceModelExprData<DoubleType> out;

  std::vector<Eqo::EqObjPtr> values = EngineAPI::getArgs(arg);
  out = eval_function(values[0]);
  for (size_t i = 1; i < values.size(); ++i)
  {
      InterfaceModelExprData<DoubleType> x = eval_function(values[i]);
      out += x;
  }

  return out;
}

template <typename DoubleType>
InterfaceModelExprData<DoubleType> InterfaceModelExprEval<DoubleType>::EvaluateIfType(Eqo::EqObjPtr arg)
{
  InterfaceModelExprData<DoubleType> out;

  out = InterfaceModelExprData<DoubleType>(0.0);
  std::vector<Eqo::EqObjPtr> values = EngineAPI::getArgs(arg);

  dsAssert(values.size() == 2, "UNEXPECTED");

  InterfaceModelExprData<DoubleType> test = eval_function(values[0]);
  if ((test.GetType() == datatype::DOUBLE))
  {
      if (test.GetDoubleValue() == 0.0)
      {
        out = InterfaceModelExprData<DoubleType>(0.0);
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
InterfaceModelExprData<DoubleType> InterfaceModelExprEval<DoubleType>::EvaluateIfElseType(Eqo::EqObjPtr arg)
{
  InterfaceModelExprData<DoubleType> out;

  out = InterfaceModelExprData<DoubleType>(0.0);
  std::vector<Eqo::EqObjPtr> values = EngineAPI::getArgs(arg);

  dsAssert(values.size() == 3, "UNEXPECTED");

  InterfaceModelExprData<DoubleType> test = eval_function(values[0]);
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
InterfaceModelExprData<DoubleType> InterfaceModelExprEval<DoubleType>::EvaluateFunctionType(Eqo::EqObjPtr arg)
{
  const EngineAPI::EqObjType etype = EngineAPI::getEnumeratedType(arg);

  InterfaceModelExprData<DoubleType> out;
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

template <typename DoubleType>
InterfaceModelExprData<DoubleType> InterfaceModelExprEval<DoubleType>::EvaluateInvalidType(Eqo::EqObjPtr arg)
{
  InterfaceModelExprData<DoubleType> out;

  //out.type = datatype::INVALID;
  std::ostringstream os;
  os << "Could not evaluate expression type for "
      << EngineAPI::getStringValue(arg);
  errors.push_back(os.str());

  return out;
}

template <typename DoubleType>
InterfaceModelExprData<DoubleType> InterfaceModelExprEval<DoubleType>::eval_function(Eqo::EqObjPtr arg)
{
  FPECheck::ClearFPE();

  const EngineAPI::EqObjType etype = EngineAPI::getEnumeratedType(arg);

#if 0
  os << "begin eval_function " << EngineAPI::getStringValue(arg) << "\n";
#endif

  InterfaceModelExprData<DoubleType> out;

  bool cache_result = true;
  InterfaceModelExprDataCachePtr<DoubleType> cache = const_cast<Interface *>(data_ref)->GetInterfaceModelExprDataCache<DoubleType>();
  if (!cache)
  {
    cache = InterfaceModelExprDataCachePtr<DoubleType>(new InterfaceModelExprDataCache<DoubleType>()); 
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
    //invalid
    out = InterfaceModelExprData<DoubleType>();
    std::ostringstream os;
    os << "There was a " << FPECheck::getFPEString() << " floating point exception while evaluating " << EngineAPI::getStringValue(arg);
    errors.push_back(os.str());
    FPECheck::ClearFPE();
  }

  if (cache_result && (out.GetType() != datatype::INVALID))
  {
    cache->SetEntry(EngineAPI::getStringValue(arg), out);
  }

  return out;
}

namespace {
    /// need to do a bunch of checks to find out what we are dealing with data-wise
    template <typename DoubleType>
    struct checks {
        bool allArgsSame;
        bool hasInvalid;
        bool hasNodeData;
        bool hasDouble;
        datatype commonType;

        void doit(const typename InterfaceModelExprEval<DoubleType>::margv_t &argv)
        {
            dsAssert(!argv.empty(), "UNEXPECTED");
            // if we have at least one invalid in arg list, we are screwed
            hasInvalid    = false;
            hasNodeData   = false;
            hasDouble     = false;
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

template <typename DoubleType>
InterfaceModelExprData<DoubleType> InterfaceModelExprEval<DoubleType>::EvaluateExternalMath(const std::string &name, const margv_t &argv)
{
  bool invalid = false;
  // if one arg is invalid, we must abort
  InterfaceModelExprData<DoubleType> out;
  checks<DoubleType> mytest;
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
  if (mytest.allArgsSame && (mytest.commonType == datatype::DOUBLE))
  {
    std::string resultstr;
    std::vector<DoubleType> dargs;
    dargs.reserve(argv.size());
    for (size_t i = 0; i < argv.size(); ++i)
    {
      dargs.push_back(argv[i].GetDoubleValue());
    }
    const MathEval<DoubleType> &emath = MathEval<DoubleType>::GetInstance(); 
    DoubleType res = emath.EvaluateMathFunc(name, dargs, resultstr);
    if (!resultstr.empty())
    {
      errors.push_back(resultstr);
      invalid = true;
    }
    out = InterfaceModelExprData<DoubleType>(res);
  }
  else
  {
    // vector args
    // we use pointers so be extra careful
    std::vector<DoubleType> output;
    /// default initializer for a vector of pointers is 0
    std::vector<const std::vector<DoubleType> *> vargs(argv.size());
    bool   all_doubles = true;
    size_t vlen = 0;
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

///// TODO: write test to make sure external math is working properly here
#if 0
      output.resize(vlen);
      for (size_t i=0; i < vlen; ++i)
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
          if (!resultstr.empty())
          {
              errors.push_back(resultstr);
              /// no need to continue if there are errors
              break;
          }
      }
#else
    const MathEval<DoubleType> &emath = MathEval<DoubleType>::GetInstance(); 
    if (all_doubles)
    {
      output.clear();
      DoubleType res = emath.EvaluateMathFunc(name, dargs, resultstr);
      if (resultstr.empty())
      {
        out = InterfaceModelExprData<DoubleType>(InterfaceNodeScalarData<DoubleType>(res, vlen));
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
          DoubleType sum = std::accumulate(output.begin(), output.end(), static_cast<DoubleType>(0.0));
          out = InterfaceModelExprData<DoubleType>(sum);
        }
        else if (name == "vec_max")
        {
          DoubleType max = *(std::max_element(output.begin(), output.end()));
          out = InterfaceModelExprData<DoubleType>(max);
        }
        else if (name == "vec_min")
        {
          DoubleType min = *(std::min_element(output.begin(), output.end()));
          out = InterfaceModelExprData<DoubleType>(min);
        }
        else
        {
          out = InterfaceModelExprData<DoubleType>(InterfaceNodeScalarData<DoubleType>(output));
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
    //invalid
    out = InterfaceModelExprData<DoubleType>();
  }

  return out;
}

template class InterfaceModelExprEval<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
template class InterfaceModelExprEval<float128>;
#endif
};

