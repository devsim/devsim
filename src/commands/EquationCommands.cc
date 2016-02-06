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

#include "EquationCommands.hh"
#include "dsCommand.hh"
#include "CommandHandler.hh"

#include "Equation.hh"
#include "ExprEquation.hh"
#include "InterfaceExprEquation.hh"
#include "ExprContactEquation.hh"
#include "CheckFunctions.hh"

#include "Contact.hh"
#include "Region.hh"
#include "Interface.hh"

#include "Validate.hh"
#include "GlobalData.hh"
#include "dsAssert.hh"
#include <sstream>

using namespace dsValidate;


namespace dsCommand {
void
createEquationCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] =
    {
        {"device",          "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidDevice},
        {"region",          "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidRegion},
        {"name",            "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
        {"variable_name",   "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
        {"node_model",      "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL},
        {"edge_model",      "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL},
        {"edge_volume_model", "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL},
        {"element_model",      "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL},
        {"volume_model",      "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL},
        {"time_node_model", "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL},
        {"variable_update", "default", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL},
        {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
    };

    dsGetArgs::switchList switches = NULL;


    bool error = data.processOptions(option, switches, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &name      = data.GetStringOption("name");
    const std::string &variable_name   = data.GetStringOption("variable_name");
    const std::string &deviceName      = data.GetStringOption("device");
    const std::string &regionName      = data.GetStringOption("region");
    const std::string &node_model      = data.GetStringOption("node_model");
    const std::string &time_node_model = data.GetStringOption("time_node_model");
    const std::string &edge_model      = data.GetStringOption("edge_model");
    const std::string &edge_volume_model      = data.GetStringOption("edge_volume_model");
    const std::string &elementedge_model = data.GetStringOption("element_model");
    const std::string &volume_model = data.GetStringOption("volume_model");
    /// This should be using switch list ultimately
    /// Maybe this should be a property of a NodeSolution instead of an equation
    const std::string &variable_update = data.GetStringOption("variable_update");

    Device *dev = NULL;
    Region *reg = NULL;

    errorString = ValidateDeviceAndRegion(deviceName, regionName, dev, reg);

    if (!errorString.empty())
    {
      data.SetErrorResult(errorString);
      return;
    }

    // implement in terms of ValidateNodeModeName
    errorString  = ValidateOptionalNodeModelName(dev, reg, node_model);
    errorString += ValidateOptionalNodeModelName(dev, reg, time_node_model);
// TODO:"Add Triangle Version"
    errorString += ValidateOptionalEdgeModelName(dev, reg, edge_model);
    errorString += ValidateOptionalEdgeModelName(dev, reg, edge_volume_model);

    Equation::UpdateType updateType = Equation::DEFAULT;
    if (variable_update == "default")
    {
        updateType = Equation::DEFAULT;
    }
    else if (variable_update == "log_damp")
    {
        updateType = Equation::LOGDAMP;
    }
    else if (variable_update == "positive")
    {
        updateType = Equation::POSITIVE;
    }
    else
    {
        std::ostringstream os;
        os << "-variable_update passed invalid option, \"" << variable_update <<
              "\".  Valid options: \"default\", \"log_damp\", \"positive\".\n";
        errorString += os.str();
    }

    if (!errorString.empty())
    {
      data.SetErrorResult(errorString);
      return;
    }

    new ExprEquation(name, reg, variable_name, node_model, edge_model, edge_volume_model, elementedge_model, volume_model, time_node_model, updateType);
    data.SetEmptyResult();
}

void
getEquationListCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] =
    {
        {"device",          "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidDevice},
        {"region",          "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidRegion},
        {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
    };

    dsGetArgs::switchList switches = NULL;


    bool error = data.processOptions(option, switches, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &deviceName      = data.GetStringOption("device");
    const std::string &regionName      = data.GetStringOption("region");

    Device *dev = NULL;
    Region *reg = NULL;

    errorString = ValidateDeviceAndRegion(deviceName, regionName, dev, reg);

    if (!errorString.empty())
    {
      data.SetErrorResult(errorString);
      return;
    }

    //// This should be in the same order as the equation index
    const std::vector<std::string> &slist = GetKeys(reg->GetEquationPtrList());
    std::vector<std::string> olist(slist.size());
    for (size_t i = 0; i < slist.size(); ++i)
    {
      const std::string &s = slist[i]; 
      size_t ei = reg->GetEquationIndex(s);
      dsAssert((ei < slist.size()) && (olist[ei].empty()), "UNEXPECTED");
      olist[ei] = s;
    }
    data.SetStringListResult(olist);
}

void
deleteEquationCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] =
    {
        {"device",          "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidDevice},
        {"region",          "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidRegion},
        {"name",            "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
        {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
    };

    dsGetArgs::switchList switches = NULL;


    bool error = data.processOptions(option, switches, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &name            = data.GetStringOption("name");
    const std::string &deviceName      = data.GetStringOption("device");
    const std::string &regionName      = data.GetStringOption("region");

    Device *dev = NULL;
    Region *reg = NULL;
    Equation *eqn = NULL;

    errorString = ValidateDeviceAndRegion(deviceName, regionName, dev, reg);

    if (reg && (!(reg->GetEquationPtrList().count(name))))
    {
      std::ostringstream os;
      os << "Equation " << name << " does not exist\n";
      errorString += os.str();
    }
    else
    {
      eqn = reg->GetEquationPtrList()[name];
    }

    if (!errorString.empty())
    {
      data.SetErrorResult(errorString);
      return;
    }

    if (commandName == "delete_equation")
    {
      reg->DeleteEquation(eqn);
      data.SetEmptyResult();
    }
    else if (commandName == "get_equation_command")
    {
      ObjectHolderMap_t omap;
      eqn->GetCommandOptions(omap);
      data.SetMapResult(omap);
    }
}

void
createInterfaceEquationCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] =
    {
        {"device",          "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidDevice},
        {"interface",       "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidInterface},
        {"name",            "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
        {"variable_name",   "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
        {"type",            "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
        {"interface_model", "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL},
        {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
    };

    dsGetArgs::switchList switches = NULL;


    bool error = data.processOptions(option, switches, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &name                 = data.GetStringOption("name");
    const std::string &variable_name        = data.GetStringOption("variable_name");
    const std::string &deviceName           = data.GetStringOption("device");
    const std::string &interfaceName        = data.GetStringOption("interface");
    const std::string &interface_model = data.GetStringOption("interface_model");
    const std::string &type                 = data.GetStringOption("type");

    Device    *dev = NULL;
    Interface *interface = NULL;

    errorString = ValidateDeviceAndInterface(deviceName, interfaceName, dev, interface);
    if (!errorString.empty())
    {
      data.SetErrorResult(errorString);
      return;
    }

    // implement in terms of ValidateNodeModeName
    errorString  = ValidateInterfaceNodeModelName(dev, interface, interface_model);

    InterfaceExprEquation::EquationType et = InterfaceExprEquation::UNKNOWN;
    if (type == "continuous")
    {
        et = InterfaceExprEquation::CONTINUOUS;
    }
    else if (type == "fluxterm")
    {
        et = InterfaceExprEquation::FLUXTERM;
    }
    else
    {
        std::ostringstream os;
        os << "-type must be specified as \"continuous\" or \"fluxterm\"\n";
        errorString += os.str();
    }


    if (!errorString.empty())
    {
      data.SetErrorResult(errorString);
      return;
    }

    new InterfaceExprEquation(name, interface, variable_name, interface_model, et);
    data.SetEmptyResult();
}

void
getInterfaceEquationListCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] =
    {
        {"device",          "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidDevice},
        {"interface",       "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidInterface},
        {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
    };

    dsGetArgs::switchList switches = NULL;

    bool error = data.processOptions(option, switches, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &deviceName           = data.GetStringOption("device");
    const std::string &interfaceName        = data.GetStringOption("interface");

    Device    *dev = NULL;
    Interface *interface = NULL;

    errorString = ValidateDeviceAndInterface(deviceName, interfaceName, dev, interface);

    if (!errorString.empty())
    {
      data.SetErrorResult(errorString);
      return;
    }

    data.SetStringListResult(GetKeys(interface->GetInterfaceEquationList()));
}

void
deleteInterfaceEquationCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;
    static dsGetArgs::Option option[] =
    {
        {"device",          "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidDevice},
        {"interface",       "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidInterface},
        {"name",            "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
        {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
    };

    dsGetArgs::switchList switches = NULL;

    bool error = data.processOptions(option, switches, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &name                 = data.GetStringOption("name");
    const std::string &deviceName           = data.GetStringOption("device");
    const std::string &interfaceName        = data.GetStringOption("interface");

    Device    *dev = NULL;
    Interface *interface = NULL;
    InterfaceEquation *ieqn = NULL;

    errorString = ValidateDeviceAndInterface(deviceName, interfaceName, dev, interface);

    if (interface && (!(interface->GetInterfaceEquationList().count(name))))
    {
      std::ostringstream os;
      os << "Interface Equation " << name << " does not exist\n";
      errorString += os.str();
    }
    else
    {
      ieqn = interface->GetInterfaceEquationList()[name];
    }

    if (!errorString.empty())
    {
      data.SetErrorResult(errorString);
      return;
    }

    if (commandName == "delete_interface_equation")
    {
      interface->DeleteInterfaceEquation(ieqn);
      data.SetEmptyResult();
    }
    else if (commandName == "get_interface_equation_command")
    {
      ObjectHolderMap_t omap;
      ieqn->GetCommandOptions(omap);
      data.SetMapResult(omap);
    }
}

void
createContactEquationCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;

    static dsGetArgs::Option option[] =
    {
        {"device",        "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidDevice},
        {"contact",       "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidContact},
        {"name",          "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
        {"variable_name", "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
        {"node_model",    "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL},
        {"edge_model",    "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL},
        {"element_model",    "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL},
        {"node_current_model", "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL},
        {"edge_current_model", "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL},
        {"element_current_model", "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL},
        {"node_charge_model",  "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL},
        {"edge_charge_model",  "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL},
        {"element_charge_model",  "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL},
        {"circuit_node",  "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL},
        {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL}
    };

// TODO:"Test element_model"
// TODO:"Test element_current_model"
// TODO:"Test element_charge_model"

    dsGetArgs::switchList switches = NULL;


    bool error = data.processOptions(option, switches, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &name                 = data.GetStringOption("name");
    const std::string &deviceName           = data.GetStringOption("device");
    const std::string &contactName          = data.GetStringOption("contact");
    const std::string &variable_name        = data.GetStringOption("variable_name");
    const std::string &node_model           = data.GetStringOption("node_model");
    const std::string &edge_model           = data.GetStringOption("edge_model");
    const std::string &element_model   = data.GetStringOption("element_model");
    const std::string &node_charge_model    = data.GetStringOption("node_charge_model");
    const std::string &edge_charge_model    = data.GetStringOption("edge_charge_model");
    const std::string &element_charge_model    = data.GetStringOption("element_charge_model");
    const std::string &node_current_model   = data.GetStringOption("node_current_model");
    const std::string &edge_current_model   = data.GetStringOption("edge_current_model");
    const std::string &element_current_model   = data.GetStringOption("element_current_model");
    const std::string &circuit_node         = data.GetStringOption("circuit_node");

    Device    *dev = NULL;
    Region    *region = NULL;
    Contact   *contact = NULL;

    errorString = ValidateDeviceAndContact(deviceName, contactName, dev, contact);

    region = const_cast<Region *>(contact->GetRegion());

    if (!errorString.empty())
    {
      data.SetErrorResult(errorString);
      return;
    }

    if (!errorString.empty())
    {
      data.SetErrorResult(errorString);
      return;
    }

    ContactEquation *ce = new ExprContactEquation(name, variable_name, contact, region,
        node_model, edge_model, element_model, node_current_model, edge_current_model, element_current_model, node_charge_model, edge_charge_model, element_charge_model);
    if (!circuit_node.empty())
    {
        ce->SetCircuitNode(circuit_node);
    }
    data.SetEmptyResult();
}

void
getContactEquationListCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;

    static dsGetArgs::Option option[] =
    {
        {"device",        "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidDevice},
        {"contact",       "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidContact},
        {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL}
    };

    dsGetArgs::switchList switches = NULL;

    bool error = data.processOptions(option, switches, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &deviceName           = data.GetStringOption("device");
    const std::string &contactName          = data.GetStringOption("contact");

    Device    *dev = NULL;
    Contact   *contact = NULL;

    errorString = ValidateDeviceAndContact(deviceName, contactName, dev, contact);

    if (!errorString.empty())
    {
      data.SetErrorResult(errorString);
      return;
    }

    data.SetStringListResult(GetKeys(contact->GetEquationPtrList()));

}

void
deleteContactEquationCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    using namespace dsGetArgs;

    static dsGetArgs::Option option[] =
    {
        {"device",        "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidDevice},
        {"contact",       "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidContact},
        {"name",          "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
        {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL}
    };

    dsGetArgs::switchList switches = NULL;

    bool error = data.processOptions(option, switches, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &name                 = data.GetStringOption("name");
    const std::string &deviceName           = data.GetStringOption("device");
    const std::string &contactName          = data.GetStringOption("contact");

    Device    *dev = NULL;
    Region    *region = NULL;
    Contact   *contact = NULL;
    ContactEquation *ceqn = NULL;

    errorString = ValidateDeviceAndContact(deviceName, contactName, dev, contact);


    if (contact && (!(contact->GetEquationPtrList().count(name))))
    {
      std::ostringstream os;
      os << "Contact Equation " << name << " does not exist\n";
      errorString += os.str();
    }
    else
    {
      ceqn = contact->GetEquationPtrList()[name];
    }

    if (!errorString.empty())
    {
      data.SetErrorResult(errorString);
      return;
    }

    if (commandName == "delete_contact_equation")
    {
      contact->DeleteEquation(ceqn);
      data.SetEmptyResult();
    }
    else if (commandName == "get_contact_equation_command")
    {
      ObjectHolderMap_t omap;
      ceqn->GetCommandOptions(omap);
      data.SetMapResult(omap);
    }
}

void
createCustomEquationCmd(CommandHandler &data)
{
    std::string errorString;

    GlobalData &gdata = GlobalData::GetInstance();

    using namespace dsGetArgs;
    /// Will need someway of setting circuit node
    /// (This would be on the contact and not the contact equation??)
    static dsGetArgs::Option option[] =
    {
        {"name",        "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
        {"procedure",       "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
        {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL}
    };

    dsGetArgs::switchList switches = NULL;


    bool error = data.processOptions(option, switches, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &name                 = data.GetStringOption("name");
    const std::string &procedure            = data.GetStringOption("procedure");

    gdata.AddTclEquation(name, procedure, errorString);
    data.SetEmptyResult();

    if (!errorString.empty())
    {
      data.SetErrorResult(errorString);
      return;
    }
}

void
getEquationNumbersCmd(CommandHandler &data)
{
  std::string errorString;

  static dsGetArgs::Option option[] =
  {
    {"device",   "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidDevice},
    {"region",   "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidRegion},
    {"variable", "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL},
    {"equation", "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL},
    {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL}
  };

  dsGetArgs::switchList switches = NULL;


  bool error = data.processOptions(option, switches, errorString);

  if (error)
  {
      data.SetErrorResult(errorString);
      return;
  }

  const std::string &regionName = data.GetStringOption("region");
  const std::string &deviceName = data.GetStringOption("device");
  const std::string &variableName = data.GetStringOption("variable");
  std::string equationName = data.GetStringOption("equation");

  if (variableName.empty() && equationName.empty())
  {
    errorString += "-variable and -equation cannot both be unspecified\n";
  }
  else if (!(variableName.empty() || equationName.empty()))
  {
    errorString += "-variable and -equation cannot both be specified\n";
  }

  Device *dev = NULL;
  Region *reg = NULL;

  errorString += ValidateDeviceAndRegion(deviceName, regionName, dev, reg);

  if (!errorString.empty())
  {
    data.SetErrorResult(errorString);
    return;
  }

  if (!variableName.empty())
  {
    equationName = reg->GetEquationNameFromVariable(variableName);
  }

  if (!equationName.empty())
  {
    size_t eqindex = reg->GetEquationIndex(equationName);

    if (eqindex == size_t(-1))
    {
      errorString += std::string("Could not find equation index for ") + equationName + "\n";
    }
    else
    {
      const ConstNodeList &cnl = reg->GetNodeList();

      std::vector<int> result;
      result.reserve(cnl.size());

      for (ConstNodeList::const_iterator it = cnl.begin(); it != cnl.end(); ++it)
      {
        size_t eqnum = reg->GetEquationNumber(eqindex, (*it));
        result.push_back(eqnum);

      }
      data.SetIntListResult(result);
      return;
    }
  }

  if (!errorString.empty())
  {
    data.SetErrorResult(errorString);
    return;
  }
}

Commands EquationCommands[] = {
    {"equation",             createEquationCmd},
    {"interface_equation",   createInterfaceEquationCmd},
    {"contact_equation",     createContactEquationCmd},
    {"custom_equation",      createCustomEquationCmd},
    {"get_equation_numbers", getEquationNumbersCmd},
    {"get_equation_list",    getEquationListCmd},
    {"get_interface_equation_list", getInterfaceEquationListCmd},
    {"get_contact_equation_list", getContactEquationListCmd},
    {"delete_equation", deleteEquationCmd},
    {"delete_interface_equation", deleteInterfaceEquationCmd},
    {"delete_contact_equation", deleteContactEquationCmd},
    {"get_equation_command",    deleteEquationCmd},
    {"get_contact_equation_command", deleteContactEquationCmd},
    {"get_interface_equation_command", deleteInterfaceEquationCmd},
    {NULL, NULL}
};
}

