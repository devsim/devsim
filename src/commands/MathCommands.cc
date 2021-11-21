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

#include "MathCommands.hh"
#include "dsCommand.hh"
#include "CommandHandler.hh"

#include "Newton.hh"
#include "DirectLinearSolver.hh"
#include "IterativeLinearSolver.hh"

#include "ContactEquationHolder.hh"
#include "Region.hh"
#include "Contact.hh"

#include "Validate.hh"
#include "CheckFunctions.hh"
#include "dsAssert.hh"
#include "GlobalData.hh"
#include "CompressedMatrix.hh"
#include <sstream>

using namespace dsValidate;

namespace dsCommand {

template <typename DoubleType>
void
solveCmdImpl(CommandHandler &data)
{
  std::string errorString;
//    const std::string commandName = data.GetCommandName();
  const std::string &type = data.GetStringOption("type");
  const std::string &solver_type = data.GetStringOption("solver_type");

  const DoubleType tdelta = data.GetDoubleOption("tdelta");
  const DoubleType gamma  = data.GetDoubleOption("gamma");

  const bool convergence_info = data.GetBooleanOption("info");
  ObjectHolderMap_t ohm;
  ObjectHolderMap_t *p_ohm = nullptr;

  if (convergence_info)
  {
    if (type == "ac" || type == "noise")
    {
      errorString += "\"info\" option not supported for \"" + type + "\" analysis\n";
    }
    else
    {
      p_ohm = &ohm;
    }
  }


  if (type == "dc")
  {
  }
  else if (type == "ac")
  {
  }
  else if (type == "noise")
  {
  }
  else if (type == "transient_dc")
  {
  }
  else if ((type == "transient_bdf1")
          || (type == "transient_tr") || type == "transient_bdf2")
  {
    if (tdelta == 0.0)
    {
      std::ostringstream os;
      os << "\"tdelta\" cannot be " << tdelta << " for type " << type << "\n";
      errorString = os.str();
    }

    if (gamma == 0.0)
    {
      std::ostringstream os;
      os << "\"gamma\" cannot be " << gamma << " for type " << type << "\n";
      errorString = os.str();
    }
  }
  else
  {
    std::ostringstream os;
    os << "\"dc\", \"ac\", \"noise\", \"transient_dc\", \"transient_bdf1\", \"transient_tr\", \"transient_bdf2\", are the only valid simulation types\n";
    errorString = os.str();
  }

  if (!errorString.empty())
  {
    data.SetErrorResult(errorString);
    return;
  }

  const DoubleType charge_error = data.GetDoubleOption("charge_error");
  const DoubleType absolute_error = data.GetDoubleOption("absolute_error");
  const DoubleType relative_error = data.GetDoubleOption("relative_error");
  const DoubleType maximum_error = data.GetDoubleOption("maximum_error");
  const int    maximum_iterations = data.GetIntegerOption("maximum_iterations");
  const int    maximum_divergence = data.GetIntegerOption("maximum_divergence");
  const DoubleType frequency = data.GetDoubleOption("frequency");
  const std::string &outputNode = data.GetStringOption("output_node");

  dsMath::Newton<DoubleType> solver;
  solver.SetAbsError(absolute_error);
  solver.SetRelError(relative_error);
  solver.SetQRelError(charge_error);
  solver.SetMaxIter(maximum_iterations);
  solver.SetMaxDiv(maximum_divergence);
  solver.SetMaxAbsError(maximum_error);

  std::unique_ptr<dsMath::LinearSolver<DoubleType>> linearSolver;

  if (solver_type == "direct")
  {
    linearSolver = std::unique_ptr<dsMath::LinearSolver<DoubleType>>(new dsMath::DirectLinearSolver<DoubleType>);
  }
  else if (solver_type == "iterative")
  {
    linearSolver = std::unique_ptr<dsMath::LinearSolver<DoubleType>>(new dsMath::IterativeLinearSolver<DoubleType>);
  }
  else
  {
    std::ostringstream os;
    os << "\"direct\" and \"iterative\" are the only valid simulation types\n";
    errorString = os.str();
  }

  if (!errorString.empty())
  {
    data.SetErrorResult(errorString);
    return;
  }

  bool res = false;

  if (type == "dc")
  {
    res = solver.Solve(*linearSolver, dsMath::TimeMethods::DCOnly<DoubleType>(), p_ohm);
  }
  else if (type == "ac")
  {
    res = solver.ACSolve(*linearSolver, frequency);
  }
  else if (type == "noise")
  {
    res = solver.NoiseSolve(outputNode, *linearSolver, frequency);
  }
  else if (type == "transient_dc")
  {
    res = solver.Solve(*linearSolver, dsMath::TimeMethods::TransientDC<DoubleType>(), p_ohm);
  }
  else if (type == "transient_bdf1")
  {
    res = solver.Solve(*linearSolver, dsMath::TimeMethods::BDF1<DoubleType>(tdelta, gamma), p_ohm);
  }
  else if (type == "transient_tr")
  {
    res = solver.Solve(*linearSolver, dsMath::TimeMethods::TR<DoubleType>(tdelta, gamma), p_ohm);
  }
  else if (type == "transient_bdf2")
  {
    res = solver.Solve(*linearSolver, dsMath::TimeMethods::BDF2<DoubleType>(tdelta, gamma), p_ohm);
  }

  if (p_ohm)
  {
    data.SetObjectResult(ObjectHolder(ohm));
  }
  else if (!res)
  {
    std::ostringstream os;
    os << "Convergence failure!\n";
    errorString = os.str();
  }
  else
  {
    data.SetEmptyResult();
  }

  if (!errorString.empty())
  {
    data.SetErrorResult(errorString);
    return;
  }
}

void
solveCmd(CommandHandler &data)
{
  std::string errorString;

  const std::string commandName = data.GetCommandName();


  /// Will need someway of setting circuit node
  /// (This would be on the contact and not the contact equation??)
  static dsGetArgs::Option option[] =
  {
    {"type",               "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
    {"absolute_error",     "0", dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::OPTIONAL},
    {"relative_error",     "0", dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::OPTIONAL},
    {"maximum_error",      "MAXDOUBLE", dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::OPTIONAL},
    {"maximum_iterations", "20", dsGetArgs::optionType::INTEGER, dsGetArgs::requiredType::OPTIONAL},
    {"maximum_divergence", "20", dsGetArgs::optionType::INTEGER, dsGetArgs::requiredType::OPTIONAL},
    {"frequency",    "0.0", dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::OPTIONAL},
    {"output_node",  "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL},
    {"solver_type",  "direct", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL},
    {"tdelta",       "0.0", dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::OPTIONAL},
    {"charge_error", "0.0", dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::OPTIONAL},
    {"gamma",        "1.0", dsGetArgs::optionType::FLOAT, dsGetArgs::requiredType::OPTIONAL},
    // empty string converts to bool for python
    {"info", "", dsGetArgs::optionType::BOOLEAN, dsGetArgs::requiredType::OPTIONAL},
    {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL}
  };
//      {"callback",      "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL},

  dsGetArgs::switchList switches = nullptr;


  bool error = data.processOptions(option, switches, errorString);

  if (error)
  {
      data.SetErrorResult(errorString);
      return;
  }

  {
    bool extended_solver = false;
    GlobalData &gdata = GlobalData::GetInstance();
    auto dbent = gdata.GetDBEntryOnGlobal("extended_solver");
    if (dbent.first)
    {
      auto oh = dbent.second.GetBoolean();
      extended_solver = (oh.first && oh.second);
    }

    if (extended_solver)
    {
      solveCmdImpl<extended_type>(data);
    }
    else
    {
      solveCmdImpl<double>(data);
    }
  }
}

template <typename DoubleType>
void
getMatrixAndRHSCmdImpl(CommandHandler &data)
{
  std::string errorString;
  const std::string &format = data.GetStringOption("format");


  dsMath::CompressionType ct = dsMath::CompressionType::CCM;

  if (format == "csc")
  {
    dsMath::CompressionType ct = dsMath::CompressionType::CCM;
  }
  else if (format == "csr")
  {
    dsMath::CompressionType ct = dsMath::CompressionType::CRM;
  }
  else if (!format.empty())
  {
    errorString =+ "Expected \"csc\" or \"csr\" for \"format\" option";
  }

  if (!errorString.empty())
  {
    data.SetErrorResult(errorString);
    return;
  }


  dsMath::Newton<DoubleType> solver;

  ObjectHolderMap_t ohm;
  solver.GetMatrixAndRHSForExternalUse(ct, ohm);
  data.SetObjectResult(ObjectHolder(ohm));
}

void
getMatrixAndRHSCmd(CommandHandler &data)
{
  std::string errorString;

  const std::string commandName = data.GetCommandName();

  static dsGetArgs::Option option[] =
  {
    {"format",             "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL},
    {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL}
  };

  dsGetArgs::switchList switches = nullptr;


  bool error = data.processOptions(option, switches, errorString);

  if (error)
  {
      data.SetErrorResult(errorString);
      return;
  }

  bool extended_solver = false;
  GlobalData &gdata = GlobalData::GetInstance();
  auto dbent = gdata.GetDBEntryOnGlobal("extended_solver");
  if (dbent.first)
  {
    auto oh = dbent.second.GetBoolean();
    extended_solver = (oh.first && oh.second);
  }

  if (extended_solver)
  {
    getMatrixAndRHSCmdImpl<extended_type>(data);
  }
  else
  {
    getMatrixAndRHSCmdImpl<double>(data);
  }
}

void
getContactCurrentCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    static dsGetArgs::Option option[] = {
        {"device",   "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, mustBeValidDevice},
        {"equation", "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {"contact",  "", dsGetArgs::optionType::STRING, dsGetArgs::requiredType::REQUIRED, stringCannotBeEmpty},
        {nullptr,  nullptr, dsGetArgs::optionType::STRING, dsGetArgs::requiredType::OPTIONAL}
    };

    dsGetArgs::switchList switches = nullptr;


    bool error = data.processOptions(option, switches, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &deviceName   = data.GetStringOption("device");
    const std::string &contactName  = data.GetStringOption("contact");
    const std::string &equationName = data.GetStringOption("equation");

    Device   *device = nullptr;
//    Region   *region = nullptr;
    Contact  *contact = nullptr;
    ContactEquationHolder eqn;

    errorString = ValidateDeviceAndContact(deviceName, contactName, device, contact);

#if 0
    if (contact)
    {
      region = const_cast<Region *>(contact->GetRegion());
    }
#endif

    if (!errorString.empty())
    {
      data.SetErrorResult(errorString);
      return;
    }

/*
    const Region::ContactEquationPtrMap_t &cepm = region->GetContactEquationList();
    std::pair<Region::ContactEquationPtrMap_t::const_iterator, Region::ContactEquationPtrMap_t::const_iterator> cpair = cepm.equal_range(contactName);
    Region::ContactEquationPtrMap_t::const_iterator cit = cpair.first;
    Region::ContactEquationPtrMap_t::const_iterator cend = cpair.second;
*/
    const ContactEquationPtrMap_t &cepm = contact->GetEquationPtrList();

    for (ContactEquationPtrMap_t::const_iterator cepmit = cepm.begin(); cepmit != cepm.end(); ++cepmit)
    {
        if (cepmit->second.GetName() == equationName)
        {
            eqn = cepmit->second;
        }
    }

    double val = 0.0;
    if (eqn.GetName().empty())
    {
        std::ostringstream os;
        os << "Could not find contact equation \"" << equationName << "\" "
           << onContactonDevice(contactName, deviceName) << "\n";
        errorString = os.str();
        data.SetErrorResult(errorString);
        return;
    }
    else if (commandName == "get_contact_current")
    {
        val = eqn.GetCurrent<double>();
    }
    else if (commandName == "get_contact_charge")
    {
        val = eqn.GetCharge<double>();
    }
    else
    {
        dsAssert(false, "UNEXPECTED");
    }


    data.SetDoubleResult(val);
    return;
}

Commands MathCommands[] = {
    {"get_contact_current",  getContactCurrentCmd},
    {"get_contact_charge",   getContactCurrentCmd},
    {"solve",                solveCmd},
    {"get_matrix_and_rhs",   getMatrixAndRHSCmd},
    {nullptr, nullptr}
};

}

//TODO:  "noise solve (with output node)"
