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

#include "CircuitCommands.hh"
#include "dsCommand.hh"
#include "CommandHandler.hh"
#include "CheckFunctions.hh"

#include "NodeKeeper.hh"
#include "InstanceKeeper.hh"
#include "IdealVoltage.hh"
#include "IdealCurrent.hh"
#include "IdealCapacitor.hh"
#include "IdealInductor.hh"
#include "IdealResistor.hh"
#include "Signal.hh"

#include "Validate.hh"

#include <sstream>

using namespace dsValidate;

namespace dsCommand {

namespace {
void AddGroundNode()
{
    NodeKeeper &nk = NodeKeeper::instance();
    if (!nk.IsCircuitNode("0"))
    {
        nk.AddNode("0", CNT::GROUND, CUT::DEFAULT);
    }

    if (!nk.IsCircuitNode("GND"))
    {
        nk.AddNode("GND", CNT::GROUND, CUT::DEFAULT);
    }
}

void CreateNode(const std::string &name, CUT::UpdateType ut)
{
    AddGroundNode();

    NodeKeeper &nk = NodeKeeper::instance();
    if (!nk.IsCircuitNode(name))
    {
        nk.AddNode(name, CNT::DEFAULT, ut);
    }

}
}

void addCircuitNodeCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    static dsGetArgs::Option option[] =
    {
        {"name",          "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
        {"variable_update",      "default", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL},
        {"value",                "0.0", dsGetArgs::Types::FLOAT, dsGetArgs::Types::OPTIONAL},
        {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL}
    };

    dsGetArgs::switchList switches = NULL;


    bool error = data.processOptions(option, switches, errorString);

    if (error)
    {
      data.SetErrorResult(errorString);
      return;
    }
    const std::string &name = data.GetStringOption("name");
    const std::string &variable_update = data.GetStringOption("variable_update");
    double value = data.GetDoubleOption("value");

    CUT::UpdateType updateType = CUT::DEFAULT;
    if (variable_update == "default")
    {
        updateType = CUT::DEFAULT;
    }
    else if (variable_update == "log_damp")
    {
        updateType = CUT::LOGDAMP;
    }
    else if (variable_update == "positive")
    {
        updateType = CUT::POSITIVE;
    }
    else
    {
        std::ostringstream os;
        os << "-variable_update passed invalid option, \"" << variable_update <<
              "\".  Valid options: \"default\", \"log_damp\", \"positive\".\n";
        errorString += os.str();
    }

    if (updateType == CUT::POSITIVE)
    {
        if (value <= 0.0)
        {
            std::ostringstream os;
            os << "-variable_update cannot be \"positive\" when value is " << value << "\n";
            errorString += os.str();
        }
    }

    if (!errorString.empty())
    {
        data.SetErrorResult(errorString);
        return;
    }

    NodeKeeper &nk = NodeKeeper::instance();

    AddGroundNode();

    if (nk.IsCircuitNode(name))
    {
        std::ostringstream os;
        os << "circuit node \"" << name <<
              "\"  already exists\n";
        errorString += os.str();
    }
    else
    {
        CreateNode(name, CUT::DEFAULT);
        data.SetEmptyResult();
    }

    if (!errorString.empty())
    {
        data.SetErrorResult(errorString);
        return;
    }
}

void 
circuitElementCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

//    GlobalData &gdata = GlobalData::GetInstance();

    /// Will need someway of setting circuit node
    /// (This would be on the contact and not the contact equation??)
    static dsGetArgs::Option option[] =
    {
        {"name",          "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
        {"n1",      "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, NULL},
        {"n2",      "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, NULL},
        {"value",   "0.0", dsGetArgs::Types::FLOAT, dsGetArgs::Types::OPTIONAL},
        {"acreal",   "0.0", dsGetArgs::Types::FLOAT, dsGetArgs::Types::OPTIONAL},
        {"acimag",   "0.0", dsGetArgs::Types::FLOAT, dsGetArgs::Types::OPTIONAL},
        {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL}
    };

    dsGetArgs::switchList switches = NULL;


    bool error = data.processOptions(option, switches, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

//    const std::string commandName = data.GetCommandName();

    const std::string &name = data.GetStringOption("name");
    const std::string &n1 = data.GetStringOption("n1");
    const std::string &n2 = data.GetStringOption("n2");
    double value          = data.GetDoubleOption("value");
    double acreal          = data.GetDoubleOption("acreal");
    double acimag          = data.GetDoubleOption("acimag");

    char type = name[0];

    if (type != 'V' && type != 'v')
    {
        if ((acreal != 0.0) || (acimag != 0.0))
        {
            std::ostringstream os;
            os << "Cannot specify -acreal or -acimag for non-voltage circuit element  \"" << name <<
                  "\"\n";
            errorString += os.str();
            error = true;
        }
        if (error)
        {
            data.SetErrorResult(errorString);
            return;
        }
    }

    //// Should not fail
    CreateNode(n1, CUT::DEFAULT);
    CreateNode(n2, CUT::DEFAULT);

    NodeKeeper &nk = NodeKeeper::instance();
    InstanceKeeper &ik = InstanceKeeper::instance();
    InstanceModel  *im = NULL;

    if (type == 'V' || type == 'v')
    {
        SignalPtr x(new DCOnly(value));
        im = new IdealVoltage(&nk, name.c_str(), n1.c_str(), n2.c_str(), value, acreal, acimag, x);
        ik.addInstanceModel(im);
        data.SetEmptyResult();
    }
    else if (type == 'I' || type == 'i')
    {
        SignalPtr x(new DCOnly(value));
        im = new IdealCurrent(&nk, name.c_str(), n1.c_str(), n2.c_str(), value, x);
        ik.addInstanceModel(im);
        data.SetEmptyResult();
    }
    else if (type == 'C' || type == 'c')
    {
        im = new IdealCapacitor(&nk, name.c_str(), n1.c_str(), n2.c_str());
        im->addParam(std::string("C"), value);
        ik.addInstanceModel(im);
        data.SetEmptyResult();
    }
    else if (type == 'L' || type == 'l')
    {
        im = new IdealInductor(&nk, name.c_str(), n1.c_str(), n2.c_str());
        im->addParam(std::string("L"), value);
        ik.addInstanceModel(im);
        data.SetEmptyResult();
    }
    else if (type == 'R' || type == 'r')
    {
        im = new IdealResistor(&nk, name.c_str(), n1.c_str(), n2.c_str());
        im->addParam(std::string("R"), value);
        ik.addInstanceModel(im);
        data.SetEmptyResult();
    }
    else
    {
        std::ostringstream os;
        os << "Cannot figure out what element type -name  \"" << name <<
              "\" is\n";
        errorString += os.str();
    }

    if (!errorString.empty())
    {
        data.SetErrorResult(errorString);
        return;
    }

}

void
circuitAlterCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

//    GlobalData &gdata = GlobalData::GetInstance();

    /// Will need someway of setting circuit node
    /// (This would be on the contact and not the contact equation??)
    static dsGetArgs::Option option[] =
    {
        {"name",  "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
        {"param", "value", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL},
        {"value", "0.0", dsGetArgs::Types::FLOAT, dsGetArgs::Types::REQUIRED, NULL},
        {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
    };

    dsGetArgs::switchList switches = NULL;


    bool error = data.processOptions(option, switches, errorString);

    if (error)
    {
      data.SetErrorResult(errorString);
      return;
    }

//    const std::string commandName = data.GetCommandName();

    const std::string &name = data.GetStringOption("name");
    std::string param = data.GetStringOption("param");
    double value          = data.GetDoubleOption("value");


    InstanceKeeper &ik = InstanceKeeper::instance();
    InstanceModelPtr im = ik.getInstanceModel(name);


    char type = name[0];

    if (!im)
    {
        std::ostringstream os;
        os << "Cannot find circuit element -name  \"" << name <<
              "\"\n";
        errorString += os.str();
        data.SetErrorResult(errorString.c_str());
        return;
    }

    if (param.empty())
    {
        // just in case someone tries to add an empty parameter, ""
        param = "value";
    }

    if (param == "value")
    {
        if (type == 'V' || type == 'v')
        {
            param = "V";
        }
        else if (type == 'I' || type == 'i')
        {
            param = "I";
        }
        else if (type == 'C' || type == 'c')
        {
            param = "C";
        }
        else if (type == 'L' || type == 'l')
        {
            param = "L";
        }
        else if (type == 'R' || type == 'r')
        {
            param = "R";
        }
    }

    bool ok = im->addParam(param, value);

    if (ok)
    {
      data.SetEmptyResult();
    }
    else
    {
        std::ostringstream os;
        os << "Circuit element -name  \"" << name <<
              "\" does not accept parameter \"" << param << "\" with value \"" << value << "\"\n";
        errorString += os.str();
    }

    if (!errorString.empty())
    {
        data.SetErrorResult(errorString);
    }
}

void
circuitNodeAliasCmd(CommandHandler &data)
{
    AddGroundNode();

    std::string errorString;

    const std::string commandName = data.GetCommandName();

//    GlobalData &gdata = GlobalData::GetInstance();

    /// Will need someway of setting circuit node
    /// (This would be on the contact and not the contact equation??)
    static dsGetArgs::Option option[] =
    {
        {"node",  "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
        {"alias", "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
        {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
    };

    dsGetArgs::switchList switches = NULL;


    bool error = data.processOptions(option, switches, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
    }

//    const std::string commandName = data.GetCommandName();

    const std::string &node = data.GetStringOption("node");
    const std::string &alias = data.GetStringOption("alias");

    NodeKeeper &nk = NodeKeeper::instance();
    if (!nk.IsCircuitNode(node))

    {
        std::ostringstream os;
        os << "Cannot find circuit node -node  \"" << node <<
              "\"\n";
        errorString += os.str();
        data.SetErrorResult(errorString);
        return;
    }

    if (nk.IsCircuitNode(alias))

    {
        std::ostringstream os;
        os << "Circuit node already exists for -alias  \"" << alias <<
              "\"\n";
        errorString += os.str();
        data.SetErrorResult(errorString);
        return;
    }

    nk.AddNodeAlias(alias, node);
    data.SetEmptyResult();
}

void circuitGetCircuitNodeListCmd(CommandHandler &data)
{
    AddGroundNode();

    std::string errorString;

    const std::string commandName = data.GetCommandName();

//    GlobalData &gdata = GlobalData::GetInstance();

    /// Will need someway of setting circuit node
    /// (This would be on the contact and not the contact equation??)
    static dsGetArgs::Option option[] =
    {
        {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
    };

    dsGetArgs::switchList switches = NULL;


    bool error = data.processOptions(option, switches, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
    }

    NodeKeeper &nk = NodeKeeper::instance();
    const NodeKeeper::NodeTable_t &nt = nk.getNodeList();

    std::vector<std::string> nodelist;
    for (NodeKeeper::NodeTable_t::const_iterator it = nt.begin(); it != nt.end(); ++it)
    {
      if ((it->second)->isGROUND())
      {
          continue;
      }
      nodelist.push_back(it->first);
    }
    data.SetStringListResult(nodelist);

    return;
}

void circuitGetCircuitSolutionListCmd (CommandHandler &data)
{
    AddGroundNode();

    std::string errorString;

    const std::string commandName = data.GetCommandName();

//    GlobalData &gdata = GlobalData::GetInstance();

    /// Will need someway of setting circuit node
    /// (This would be on the contact and not the contact equation??)
    static dsGetArgs::Option option[] =
    {
        {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
    };

    dsGetArgs::switchList switches = NULL;


    bool error = data.processOptions(option, switches, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
    }

    NodeKeeper &nk = NodeKeeper::instance();
    const NodeKeeper::SolutionMap_t &nt = nk.getSolutionList();

    data.SetStringListResult(GetKeys(nt));

    return;
}

void circuitGetCircuitNodeValueCmd (CommandHandler &data)
{
    AddGroundNode();

    std::string errorString;

    const std::string commandName = data.GetCommandName();

    /// Will need someway of setting circuit node
    /// (This would be on the contact and not the contact equation??)
    static dsGetArgs::Option option[] =
    {
        {"node",  "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
        {"solution",  "dcop", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, stringCannotBeEmpty},
        {"value",  "0.0", dsGetArgs::Types::FLOAT, dsGetArgs::Types::OPTIONAL, NULL},
        {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
    };

    dsGetArgs::switchList switches = NULL;


    bool error = data.processOptions(option, switches, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
    }

//    const std::string commandName = data.GetCommandName();

    const std::string &solution = data.GetStringOption("solution");
    const std::string &node = data.GetStringOption("node");
    //// TODO: warn when command name is wrong
    const double val = data.GetDoubleOption("value");

    NodeKeeper &nk = NodeKeeper::instance();
    if (!nk.IsCircuitNode(node))

    {
        std::ostringstream os;
        os << "Cannot find circuit node -node  \"" << node <<
              "\"\n";
        errorString += os.str();
    }

    if (!nk.IsCircuitSolution(solution))
    {
        std::ostringstream os;
        os << "Cannot find circuit solution -solution  \"" << solution <<
              "\"\n";
        errorString += os.str();
    }

    if (!errorString.empty())
    {
        data.SetErrorResult(errorString);
        return;
    }   

    if (commandName == "get_circuit_node_value")
    {
        data.SetDoubleResult(nk.GetNodeValue(solution, node));
    }
    else if (commandName == "set_circuit_node_value")
    {
        nk.SetNodeValue(solution, node, val);
        nk.TriggerCallbackOnNode(node);
        data.SetEmptyResult();
    }
}

void
circuitGetCircuitEquationNumberCmd (CommandHandler &data)
{
  std::string errorString;

  const std::string commandName = data.GetCommandName();

  static dsGetArgs::Option option[] =
  {
    {"node",  "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
    {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
  };

  dsGetArgs::switchList switches = NULL;


  bool error = data.processOptions(option, switches, errorString);

  if (error)
  {
      data.SetErrorResult(errorString);
  }

  // const std::string commandName = data.GetCommandName();

  const std::string &node = data.GetStringOption("node");

  NodeKeeper &nk = NodeKeeper::instance();
  if (!nk.IsCircuitNode(node))
  {
    std::ostringstream os;
    os << "Cannot find circuit node -node  \"" << node <<
          "\"\n";
    errorString += os.str();
    data.SetErrorResult(errorString);
    return;

  }

  size_t number = nk.GetEquationNumber(node);

  if (number == size_t(-1))
  {
    data.SetIntResult(-1);
  }
  else
  {
    data.SetIntResult(static_cast<int>(number));
  }
}

Commands CircuitCommands[] = {
    {"add_circuit_node",   addCircuitNodeCmd},
    {"circuit_element",    circuitElementCmd},
    {"circuit_alter",      circuitAlterCmd},
    {"circuit_node_alias", circuitNodeAliasCmd},
    {"get_circuit_node_list", circuitGetCircuitNodeListCmd},
    {"get_circuit_solution_list", circuitGetCircuitSolutionListCmd},
    {"get_circuit_node_value", circuitGetCircuitNodeValueCmd},
    {"set_circuit_node_value", circuitGetCircuitNodeValueCmd},
    {"get_circuit_equation_number", circuitGetCircuitEquationNumberCmd},
    {NULL, NULL}
};

}

