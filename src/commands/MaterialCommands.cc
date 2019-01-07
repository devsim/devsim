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

#include "MaterialCommands.hh"
#include "dsCommand.hh"
#include "CommandHandler.hh"
#include "GlobalData.hh"
#include "Region.hh"
#include "CheckFunctions.hh"
#include "MaterialDB.hh"
#include "Device.hh"
#include "Contact.hh"

#include "Validate.hh"
#include "dsAssert.hh"
#include <sstream>

using namespace dsValidate;


namespace dsCommand {
/// This is the setter/getter
void
getParameterCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    GlobalData &gdata = GlobalData::GetInstance();

    using namespace dsGetArgs;
    dsGetArgs::Option *option;
    if (commandName == "set_parameter")
    {
        static dsGetArgs::Option setoption[] =
        {
            {"device",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr},
            {"region",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr},
            {"name",     "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
            {"value",    "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
            {nullptr,  nullptr,    dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
        };
        option = setoption;
    }
    else if (commandName == "get_parameter")
    {
        static dsGetArgs::Option getoption[] =
        {
            {"device",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, mustBeSpecifiedIfRegionSpecified},
            {"region",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr},
            {"name",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
            {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
        };
        option = getoption;
    }
    else if (commandName == "get_parameter_list")
    {
        static dsGetArgs::Option getoption[] =
        {
            {"device",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, mustBeSpecifiedIfRegionSpecified},
            {"region",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr},
            {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
        };
        option = getoption;
    }
    else if (commandName == "set_material")
    {
        static dsGetArgs::Option setoption[] =
        {
            {"device",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
            {"region",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr},
            {"contact",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr},
            {"material", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
            {nullptr,  nullptr,    dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
        };
        option = setoption;
    }
    else if (commandName == "get_material")
    {
        static dsGetArgs::Option getoption[] =
        {
            {"device",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
            {"region",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr},
            {"contact",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr},
            {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
        };
        option = getoption;
    }
    else if (commandName == "get_dimension")
    {
        static dsGetArgs::Option getoption[] =
        {
            {"device",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, mustBeValidDevice},
            {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
        };
        option = getoption;
    }
    else
    {
        /// This should not be able to happen
        dsAssert(false, "UNEXPECTED");
        option = nullptr;
    }

    dsGetArgs::switchList switches = nullptr;


    bool error = data.processOptions(option, switches, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &deviceName = data.GetStringOption("device");
    std::string regionName;
    std::string contactName;
    if (commandName != "get_dimension")
    {
      //// nasty hack
      regionName = data.GetStringOption("region");
    }

    if (commandName == "get_material" || commandName == "set_material")
    {
      contactName = data.GetStringOption("contact");
    }

    Device *dev = nullptr;
    Region *reg = nullptr;
    Contact *contact = nullptr;


    if (!regionName.empty() && !contactName.empty())
    {
      errorString += "region and contact cannot be specified at same time\n";
    }

    if (!regionName.empty() || !contactName.empty())
    {
      if (!deviceName.empty())
      {
        if (!regionName.empty())
        {
          errorString = ValidateDeviceAndRegion(deviceName, regionName, dev, reg);
        }
        else if (!contactName.empty())
        {
          errorString = ValidateDeviceAndContact(deviceName, contactName, dev, contact);
        }
      }
      else
      {
        errorString += "device must be specified when region is specified\n";
      }
    }
    else if (!deviceName.empty())
    {
        errorString = ValidateDevice(deviceName, dev);
    }

    if (!errorString.empty())
    {
        data.SetErrorResult(errorString);
        return;
    }

    if (commandName == "get_parameter")
    {
        const std::string &paramName = data.GetStringOption("name");

        GlobalData::DBEntry_t mdbentry;
        if (!regionName.empty())
        {
            mdbentry = gdata.GetDBEntryOnRegion(reg, paramName);
        }
        else if (!deviceName.empty())
        {
            mdbentry = gdata.GetDBEntryOnDevice(deviceName, paramName);
        }
        else
        {
            mdbentry = gdata.GetDBEntryOnGlobal(paramName);
        }

        if (mdbentry.first)
        {
            data.SetObjectResult(mdbentry.second);
        }
        else
        {
            std::ostringstream os;
            os << "Cannot find parameter \"" << paramName << "\"\n";
            errorString = os.str();
            data.SetErrorResult(errorString);
            return;
        }
    }
    else if (commandName == "get_parameter_list")
    {
      std::vector<std::string> names;
      if (!regionName.empty())
      {
        names = gdata.GetDBEntryListOnRegion(deviceName, regionName);
      }
      else if (!deviceName.empty())
      {
        names = gdata.GetDBEntryListOnDevice(deviceName);
      }
      else
      {
        names = gdata.GetDBEntryListOnGlobal();
      }

//      if (!names.empty())
      {
        data.SetStringListResult(names);
      }
/*
      else
      {
          std::ostringstream os;
          os << "Cannot find any parameters\n";
          errorString = os.str();
          error = TCL_ERROR;
          data.SetErrorResult(errorString);
          return error;
      }
*/
    }
    else if (commandName == "set_parameter")
    {
        const std::string &paramName = data.GetStringOption("name");

        /// Write now support only the float value for the material db
        const ObjectHolder value = data.GetObjectHolder("value");
        if (!regionName.empty())
        {
            gdata.AddDBEntryOnRegion(reg, paramName, value);
        }
        else if (!deviceName.empty())
        {
            gdata.AddDBEntryOnDevice(deviceName, paramName, value);
        }
        else
        {
            gdata.AddDBEntryOnGlobal(paramName, value);
        }
        data.SetEmptyResult();
    }
    else if (commandName == "set_material")
    {
      const std::string &materialName = data.GetStringOption("material");
      dsAssert(!materialName.empty(), "UNEXPECTED");
      if (reg)
      {
        reg->SetMaterial(materialName);
      }
      else if (contact)
      {
        contact->SetMaterial(materialName);
      }
      data.SetEmptyResult();
    }
    else if (commandName == "get_material")
    {
    
      std::string material_name;
      if (reg)
      {
         material_name = reg->GetMaterialName();
      }
      else if (contact)
      {
         material_name = contact->GetMaterialName();
      }
      data.SetStringResult(material_name);
    }
    else if (commandName == "get_dimension")
    {
      const size_t dim = dev->GetDimension();
      data.SetIntResult(dim);
    }

    return;
}

void
openDBCmd(CommandHandler &data)
{
  std::string errorString;

  const std::string commandName = data.GetCommandName();

  using namespace dsGetArgs;
  dsGetArgs::Option *option;
  if (commandName == "open_db")
  {
      static dsGetArgs::Option openoption[] =
      {
          {"filename",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
          {"permission", "readonly", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr},
          {nullptr,  nullptr,    dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
      };
      option = openoption;
  }
  else if (commandName == "close_db" || commandName == "save_db")
  {
      static dsGetArgs::Option closeoption[] =
      {
          {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
      };
      option = closeoption;
  }
  else if (commandName == "create_db")
  {
      static dsGetArgs::Option createoption[] =
      {
          {"filename",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
          {nullptr,  nullptr,    dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
      };
      option = createoption;
  }
  else
  {
      /// This should not be able to happen
      dsAssert(false, "UNEXPECTED");
      option = nullptr;
  }

  dsGetArgs::switchList switches = nullptr;


  bool error = data.processOptions(option, switches, errorString);

  if (error)
  {
      data.SetErrorResult(errorString);
      return;
  }

  MaterialDB &mdb = MaterialDB::GetInstance();

  if (commandName == "open_db")
  {
    const std::string &filename   = data.GetStringOption("filename");
    const std::string &permission = data.GetStringOption("permission");

    MaterialDB::OpenType_t otype = MaterialDB::OpenType_t::READONLY;
    //// TODO: create special checker function for handler
    if (permission == "readonly")
    {
      otype = MaterialDB::OpenType_t::READONLY;
    }
    else if (permission == "readwrite")
    {
      otype = MaterialDB::OpenType_t::READWRITE;
    }
    else
    {
      std::ostringstream os;
      os << "Invalid permission \"" << permission << "\"\n";
      errorString = os.str();
      data.SetErrorResult(errorString);
      return;
    }

    bool res = mdb.OpenDB(filename, otype, errorString);

    if (!res)
    {
      data.SetErrorResult(errorString);
      return;
    }
    else
    {
      data.SetEmptyResult();
    }
  }
  else if (commandName == "create_db")
  {
    const std::string &filename   = data.GetStringOption("filename");

    bool res = mdb.CreateDB(filename, errorString);

    if (res)
    {
      data.SetEmptyResult();
    }
    else
    {
      data.SetErrorResult(errorString);
      return;
    }

  }
  else if (commandName == "close_db")
  {
    mdb.CloseDB();
    data.SetEmptyResult();
  }
  else if (commandName == "save_db")
  {
    bool res = mdb.SaveDB(errorString);

    if (!res)
    {
      data.SetErrorResult(errorString);
      return;
    }
    else
    {
      data.SetEmptyResult();
    }
  }
}

void
addDBEntryCmd(CommandHandler &data)
{
  std::string errorString;

  const std::string commandName = data.GetCommandName();

  using namespace dsGetArgs;
  dsGetArgs::Option *option;

  static dsGetArgs::Option openoption[] =
  {
      {"material",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
      {"parameter",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
      {"value", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
      {"unit", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, nullptr},
      {"description", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, nullptr},
      {nullptr,  nullptr,    dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
  };

  option = openoption;


  dsGetArgs::switchList switches = nullptr;


  bool error = data.processOptions(option, switches, errorString);

  if (error)
  {
      data.SetErrorResult(errorString);
      return;
  }

  MaterialDB &mdb = MaterialDB::GetInstance();

  const std::string &material   = data.GetStringOption("material");
  const std::string &parameter   = data.GetStringOption("parameter");
  const std::string &value   = data.GetStringOption("value");
  const std::string &unit   = data.GetStringOption("unit");
  const std::string &description   = data.GetStringOption("description");

  mdb.AddDBEntry(material, parameter, MaterialDBEntry(unit, description, ObjectHolder(value), MaterialDBEntry::EntryType_t::MODIFIED));
  data.SetEmptyResult();

}

void 
getDBEntryCmd(CommandHandler &data)
{
  std::string errorString;

  const std::string commandName = data.GetCommandName();

  using namespace dsGetArgs;
  dsGetArgs::Option *option;

  static dsGetArgs::Option openoption[] =
  {
      {"material",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
      {"parameter",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
      {nullptr,  nullptr,    dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL, nullptr}
  };

  option = openoption;

  dsGetArgs::switchList switches = nullptr;


  bool error = data.processOptions(option, switches, errorString);

  if (error)
  {
      data.SetErrorResult(errorString);
      return;
  }

  MaterialDB &mdb = MaterialDB::GetInstance();

  const std::string &material   = data.GetStringOption("material");
  const std::string &parameter   = data.GetStringOption("parameter");

  MaterialDB::DBEntry_t dbent = mdb.GetDBEntry(material, parameter);

  if (dbent.first)
  {
//    data.SetDBEntryResult(dbent);
    std::vector<ObjectHolder> res;
    res.push_back(dbent.second.GetValue());
    res.push_back(ObjectHolder(dbent.second.GetUnit()));
    res.push_back(ObjectHolder(dbent.second.GetDescription()));
    data.SetListResult(res);
  }
  else
  {
    std::ostringstream os;
    os << "Material parameter not found \"" << material << "\" \"" << parameter << "\"\n";
    errorString = os.str();
    data.SetErrorResult(errorString);
    return;
  }
}

Commands MaterialCommands[] = {
    {"set_parameter", getParameterCmd},
    {"get_parameter", getParameterCmd},
    {"get_parameter_list", getParameterCmd},
    {"set_material", getParameterCmd},
    {"get_material", getParameterCmd},
    {"create_db",       openDBCmd},
    {"open_db",         openDBCmd},
    {"close_db",        openDBCmd},
    {"save_db",         openDBCmd},
    {"add_db_entry",    addDBEntryCmd},
    {"get_db_entry",    getDBEntryCmd},
    {"get_dimension",   getParameterCmd},
    {nullptr, nullptr}
};
//// TODO: Get material (get name of material)
//// TODO: Set material (get list of models loaded on material)
//// TODO: Delete db entry (trigger appropriate callbacks) 
}

