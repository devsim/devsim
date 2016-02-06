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

#include "EquationFunctions.hh"
#include "Region.hh"
#include "ExprEquation.hh"
#include "ModelExprEval.hh"
#include "parseexpr.hh"
#include "NodeExprModel.hh"
#include "EdgeExprModel.hh"
#include "TriangleEdgeExprModel.hh"
#include "TetrahedronEdgeExprModel.hh"
#include "InterfaceNodeExprModel.hh"
#include "GeometryStream.hh"
#include "Interface.hh"

#include "EngineAPI.hh"
//#include "ModelCompiler.hh"

class Interface;
typedef Interface *InterfacePtr;

#include <sstream>
namespace dsHelper {

class EvalType {
  public:
    explicit EvalType (const Region *r) : interface_(0), region_(r) {};
    EvalType(const Interface *i) : interface_(i), region_(0) {};
    ~EvalType() {};

    bool inModelList(const std::string &x) {
      bool inlist = false;
      if (region_)
      {
        inlist = (
                  inNodeModelList(x, region_) ||
                  inEdgeModelList(x, region_) ||
                  inElementEdgeModelList(x, region_) ||
                  inAliasedEdgeModelList(x, region_)
                 );
      }
      else if (interface_)
      {
        inlist = inInterfaceModelList(x);
      }
      return inlist;
    }
  private:
    EvalType();
    EvalType(const EvalType &);
    EvalType &operator=(const EvalType &);

    bool inNodeModelList(const std::string &x, const Region *r) const
    {
      return (r->GetNodeModel(x).get());
    }
    bool inEdgeModelList(const std::string &x, const Region *r) const
    {
      return (r->GetEdgeModel(x).get());
    }

    bool inAliasedEdgeModelList(const std::string &x, const Region *r) const
    {
      bool inlist=false;

      std::string y(x);
      std::string::size_type rpos = y.size();
      if (rpos > 4)
      {
        rpos -= 4;

        if ((y.rfind("@en0") == rpos) || (y.rfind("@en1") == rpos))
        {
          y.erase(rpos+1, 1);
          inlist = inEdgeModelList(y, r);
        }
      }

      return inlist;
    }

    bool inElementEdgeModelList(const std::string &x, const Region *r) const
    {
      bool inlist = false;
      if (r->GetDimension() == 2)
      {
        inlist = inTriangleEdgeModelList(x, r);
      }
      else if (r->GetDimension() == 3)
      {
        inlist = inTetrahedronEdgeModelList(x, r);
      }
      return inlist;
    }

    bool inTriangleEdgeModelList(const std::string &x, const Region *r) const
    {
      return (r->GetTriangleEdgeModel(x).get());
    }
    bool inTetrahedronEdgeModelList(const std::string &x, const Region *r) const
    {
      return (r->GetTetrahedronEdgeModel(x).get());
    }

    bool inInterfaceModelList(const std::string &x) const
    {
      std::string::size_type rpos = x.size();
      if (rpos > 3)
      {
        rpos -= 3;
      }

      bool inlist = false;
      if (interface_->GetInterfaceNodeModel(x).get()) {
        inlist = true;
      }
      else if (x.rfind("@r0") == rpos)
      {
        std::string y(x);
        y.resize(rpos);
        inlist = (interface_->GetRegion0()->GetNodeModel(y).get());
      }
      else if (x.rfind("@r1") == rpos)
      {
        std::string y(x);
        y.resize(rpos);
        inlist = (interface_->GetRegion1()->GetNodeModel(y).get());
      }

      return inlist;
    }

    const Interface *interface_;
    const Region    *region_;
};

std::weak_ptr<EvalType> evaltype;

/**
 * This is awful, but we need this to prototype
 */
/** here we assume that node models can only reference local models */
bool inModelList(const std::string &x)
{
  std::shared_ptr<EvalType> et = evaltype.lock();

  bool inlist = false; 
  if (et)
  {
    inlist = et->inModelList(x);

    if (!inlist)
    {
      std::string::size_type pos = x.find(":");
      if ((pos != std::string::npos) && (pos != 0))
      {
        inlist = et->inModelList(x.substr(0, pos));
      }
    }
  }
  return inlist;
}

bool neverInModelList(const std::string &)
{
  return false;
}


Eqo::EqObjPtr DefaultDevsimDerivative(Eqo::EqObjPtr self, Eqo::EqObjPtr foo)
{
    Eqo::EqObjPtr res;
    const std::string &value = EngineAPI::getStringValue(self);
    const std::string &oval = EngineAPI::getStringValue(foo);
    if (oval == value)
    {
        res = EngineAPI::con(1.0);
    }
    else
    {
//// TODO: Set way of specifying whether our expression is on an interface or a device
//// TODO: Fix this hack
      std::string::size_type rpos = value.size();
      if (rpos > 3)
      {
        rpos -= 3;
      }

      std::string::size_type opos = oval.size();
      if (opos > 3)
      {
        opos -= 3;
      }

      if ((value.rfind("@r0") == rpos) || (value.rfind("@r1") == rpos))
      {

        if ((oval.rfind("@r0") == opos) || (oval.rfind("@r1") == opos))
        {
          if (value.substr(rpos) == oval.substr(opos))
          {
            std::string out = value.substr(0,rpos);
            out += ":";
//            out += oval.substr(0,opos);
            out += oval;
            res = EngineAPI::mod(out);
          }
          else
          {
            res = EngineAPI::con(0.0);
          }
        }
        else
        {
          res = EngineAPI::con(0.0);
        }
      }
      else
      {
          std::string out(value);
          out += ":"; 
          out += oval;
          res = EngineAPI::mod(out);
      }
    }

    return res;
}

/// The error string is not empty if there is a problem
/// will be nulled on entry
Eqo::EqObjPtr CreateExprModel(const std::string &nm, const std::string &expr, RegionPtr rp, std::string &errorstring)
{
    EvalExpr::error_t terrors;
    std::shared_ptr<EvalType> et = std::shared_ptr<EvalType>(new EvalType(rp));
    evaltype = et;
    EngineAPI::SetModelListCallBack(inModelList);
    EngineAPI::SetDerivativeRule(DefaultDevsimDerivative);
    Eqo::EqObjPtr testeq = EvalExpr::evaluateExpression(expr, terrors);

    std::ostringstream os;
    if (!terrors.empty())
    {
        os << "While evaluating expression \"" << expr << "\"\n";
        for (EvalExpr::error_t::iterator it = terrors.begin(); it != terrors.end(); ++it)
        {
            os << *it << "\n";
        }
    }
    errorstring = os.str();

    return testeq;
}

/// Differs only really in the model list and pointer to data (interface versus region)
Eqo::EqObjPtr CreateInterfaceExprModel(const std::string &nm, const std::string &expr, InterfacePtr ip, std::string &errorstring)
{
    EvalExpr::error_t terrors;

    std::shared_ptr<EvalType> et = std::shared_ptr<EvalType>(new EvalType(ip));
    evaltype = et;
    EngineAPI::SetModelListCallBack(inModelList);
    EngineAPI::SetDerivativeRule(DefaultDevsimDerivative);
    Eqo::EqObjPtr testeq = EvalExpr::evaluateExpression(expr, terrors);

    std::ostringstream os;
    if (!terrors.empty())
    {
        os << "while evaluating expression: " << expr << "\n";
        for (EvalExpr::error_t::iterator it = terrors.begin(); it != terrors.end(); ++it)
        {
            os << *it << "\n";
        }
    }
    errorstring = os.str();

    return testeq;
}

NodeModel::DisplayType getNodeModelDisplayType(const std::string &dt)
{
  NodeModel::DisplayType ret = NodeModel::NODISPLAY;
  if (dt == "nodisplay")
  {
    ret = NodeModel::NODISPLAY;
  }
  else if (dt == "scalar")
  {
    ret = NodeModel::SCALAR;
  }
  else
  {
    ret = NodeModel::UNKNOWN;
  }
  return ret;
}

EdgeModel::DisplayType getEdgeModelDisplayType(const std::string &dt)
{
  EdgeModel::DisplayType ret = EdgeModel::NODISPLAY;
  if (dt == "nodisplay")
  {
    ret = EdgeModel::NODISPLAY;
  }
  else if (dt == "scalar")
  {
    ret = EdgeModel::SCALAR;
  }
  else if (dt == "vector")
  {
    ret = EdgeModel::VECTOR;
  }
  else
  {
    ret = EdgeModel::UNKNOWN;
  }
  return ret;
}

TriangleEdgeModel::DisplayType getTriangleEdgeModelDisplayType(const std::string &dt)
{
  TriangleEdgeModel::DisplayType ret = TriangleEdgeModel::NODISPLAY;
  if (dt == "nodisplay")
  {
    ret = TriangleEdgeModel::NODISPLAY;
  }
  else if (dt == "scalar")
  {
    ret = TriangleEdgeModel::SCALAR;
  }
  else
  {
    ret = TriangleEdgeModel::UNKNOWN;
  }
  return ret;
}

TetrahedronEdgeModel::DisplayType getTetrahedronEdgeModelDisplayType(const std::string &dt)
{
  TetrahedronEdgeModel::DisplayType ret = TetrahedronEdgeModel::NODISPLAY;
  if (dt == "nodisplay")
  {
    ret = TetrahedronEdgeModel::NODISPLAY;
  }
  else if (dt == "scalar")
  {
    ret = TetrahedronEdgeModel::SCALAR;
  }
  else
  {
    ret = TetrahedronEdgeModel::UNKNOWN;
  }
  return ret;
}

ret_pair CreateNodeExprModel(const std::string &nm, const std::string &expr, RegionPtr rp, NodeModel::DisplayType dt, ContactPtr cp)
{
    bool ret = false;
    std::string error;
    Eqo::EqObjPtr equation;
    equation = CreateExprModel(nm, expr, rp, error);

    if (error.empty())
    {
        /// This should not fail
        /// Or maybe it should when replacing an existing model
        /// Worry about precedence as well (Edge versus Node versus Param)
        /// Create helper function to worry about this, which will be called before this
        new NodeExprModel(nm, equation, rp, dt, cp);
        error = EngineAPI::getStringValue(equation);
        ret = true;
    }
    return std::make_pair(ret, error);
}


ret_pair CreateEdgeExprModel(const std::string &nm, const std::string &expr, RegionPtr rp, EdgeModel::DisplayType dt, ContactPtr cp)
{
    bool ret = false;
    std::string error;
    Eqo::EqObjPtr equation;
    equation = CreateExprModel(nm, expr, rp, error);

    if (error.empty())
    {
        /// This should not fail
        /// Or maybe it should when replacing an existing model
        /// Worry about precedence as well (Edge versus Node versus Param)
        /// Create helper function to worry about this, which will be called before this
        new EdgeExprModel(nm, equation, rp, dt, cp);
        error = EngineAPI::getStringValue(equation);
        ret = true;
    }
    return std::make_pair(ret, error);
}

ret_pair CreateTriangleEdgeExprModel(const std::string &nm, const std::string &expr, RegionPtr rp, TriangleEdgeModel::DisplayType dt)
{
    bool ret = false;
    std::string error;
    Eqo::EqObjPtr equation;

    equation = CreateExprModel(nm, expr, rp, error);

    if (error.empty())
    {
        new TriangleEdgeExprModel(nm, equation, rp, dt);
        error = EngineAPI::getStringValue(equation);
        ret = true;
    }
    return std::make_pair(ret, error);
}

ret_pair CreateTetrahedronEdgeExprModel(const std::string &nm, const std::string &expr, RegionPtr rp, TetrahedronEdgeModel::DisplayType dt)
{
    bool ret = false;
    std::string error;
    Eqo::EqObjPtr equation;

    equation = CreateExprModel(nm, expr, rp, error);

    if (error.empty())
    {
        new TetrahedronEdgeExprModel(nm, equation, rp, dt);
        error = EngineAPI::getStringValue(equation);
        ret = true;
    }
    return std::make_pair(ret, error);
}

ret_pair CreateInterfaceNodeExprModel(const std::string &nm, const std::string &expr, InterfacePtr ip)
{
    bool ret = false;
    std::string error;
    Eqo::EqObjPtr equation;

    equation = CreateInterfaceExprModel(nm, expr, ip, error);
    if (error.empty())
    {
        new InterfaceNodeExprModel(nm, equation, ip);
        error = EngineAPI::getStringValue(equation);
        ret = true;
    }
    return std::make_pair(ret, error);
}

ret_pair SymdiffEval(const std::string &expr)
{
  bool ret = false;
  std::string retstring;

  EvalExpr::error_t terrors;

  EngineAPI::SetModelListCallBack(neverInModelList);
  EngineAPI::SetDerivativeRule(DefaultDevsimDerivative);
  Eqo::EqObjPtr testeq = EvalExpr::evaluateExpression(expr, terrors);

  if (!terrors.empty())
  {
    std::ostringstream os;
    os << "While evaluating expression \"" << expr << "\"\n";
    for (EvalExpr::error_t::iterator it = terrors.begin(); it != terrors.end(); ++it)
    {
      os << *it << "\n";
    }
    ret = false;
    retstring = os.str();
  }
  else
  {
    retstring = EngineAPI::getStringValue(testeq);
    ret = true;
  }

  return std::make_pair(ret, retstring);
}

}

