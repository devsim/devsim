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

#include "MaterialCommands.hh"
#include "dsCommand.hh"
#include "CommandHandler.hh"
#include "GlobalData.hh"
#include "Region.hh"
#include "CheckFunctions.hh"
#include "MaterialDB.hh"
#include "Device.hh"

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
            {"device",   "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL},
            {"region",   "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL},
            {"name",     "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
            {"value",    "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
            {NULL,  NULL,    dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
        };
        option = setoption;
    }
    else if (commandName == "get_parameter")
    {
        static dsGetArgs::Option getoption[] =
        {
            {"device",   "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, mustBeSpecifiedIfRegionSpecified},
            {"region",   "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL},
            {"name",   "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
            {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
        };
        option = getoption;
    }
    else if (commandName == "get_parameter_list")
    {
        static dsGetArgs::Option getoption[] =
        {
            {"device",   "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, mustBeSpecifiedIfRegionSpecified},
            {"region",   "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL},
            {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
        };
        option = getoption;
    }
    else if (commandName == "set_material")
    {
        static dsGetArgs::Option setoption[] =
        {
            {"device",   "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidDevice},
            {"region",   "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidRegion},
            {"material", "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
            {NULL,  NULL,    dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
        };
        option = setoption;
    }
    else if (commandName == "get_material")
    {
        static dsGetArgs::Option getoption[] =
        {
            {"device",   "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidDevice},
            {"region",   "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidRegion},
            {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
        };
        option = getoption;
    }
    else if (commandName == "get_dimension")
    {
        static dsGetArgs::Option getoption[] =
        {
            {"device",   "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, mustBeValidDevice},
            {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
        };
        option = getoption;
    }
    else
    {
        /// This should not be able to happen
        dsAssert(false, "UNEXPECTED");
        option = NULL;
    }

    dsGetArgs::switchList switches = NULL;


    bool error = data.processOptions(option, switches, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &deviceName = data.GetStringOption("device");
    std::string regionName;
    if (commandName != "get_dimension")
    {
      //// nasty hack
      regionName = data.GetStringOption("region");
    }

    Device *dev = NULL;
    Region *reg = NULL;

    if (!regionName.empty())
    {
      if (!deviceName.empty())
      {
        errorString = ValidateDeviceAndRegion(deviceName, regionName, dev, reg);
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
      reg->SetMaterial(materialName);
      data.SetEmptyResult();
    }
    else if (commandName == "get_material")
    {
      const std::string &material_name = reg->GetMaterialName();
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
          {"filename",   "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
          {"permission", "readonly", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL},
          {NULL,  NULL,    dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
      };
      option = openoption;
  }
  else if (commandName == "close_db" || commandName == "save_db")
  {
      static dsGetArgs::Option closeoption[] =
      {
          {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
      };
      option = closeoption;
  }
  else if (commandName == "create_db")
  {
      static dsGetArgs::Option createoption[] =
      {
          {"filename",   "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
          {NULL,  NULL,    dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
      };
      option = createoption;
  }
  else
  {
      /// This should not be able to happen
      dsAssert(false, "UNEXPECTED");
      option = NULL;
  }

  dsGetArgs::switchList switches = NULL;


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

    MaterialDB::OpenType_t otype = MaterialDB::READONLY;
    //// TODO: create special checker function for handler
    if (permission == "readonly")
    {
      otype = MaterialDB::READONLY;
    }
    else if (permission == "readwrite")
    {
      otype = MaterialDB::READWRITE;
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
      {"material",   "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
      {"parameter",   "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
      {"value", "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
      {"unit", "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, NULL},
      {"description", "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, NULL},
      {NULL,  NULL,    dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
  };

  option = openoption;


  dsGetArgs::switchList switches = NULL;


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

  mdb.AddDBEntry(material, parameter, MaterialDBEntry(unit, description, ObjectHolder(value), MaterialDBEntry::MODIFIED));
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
      {"material",   "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
      {"parameter",   "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
      {NULL,  NULL,    dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL, NULL}
  };

  option = openoption;

  dsGetArgs::switchList switches = NULL;


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
    {NULL, NULL}
};
//// TODO: Get material (get name of material)
//// TODO: Set material (get list of models loaded on material)
//// TODO: Delete db entry (trigger appropriate callbacks) 
}

