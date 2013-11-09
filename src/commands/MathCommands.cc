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

#include "MathCommands.hh"
#include "dsCommand.hh"
#include "CommandHandler.hh"

#include "Newton.hh"
#include "DirectLinearSolver.hh"
#include "IterativeLinearSolver.hh"
#if 0
#include "PetscSolver.hh"
#endif

#include "ContactEquation.hh"
#include "Region.hh"
#include "Contact.hh"

#include "Validate.hh"
#include "CheckFunctions.hh"
#include "dsAssert.hh"
#include <sstream>

using namespace dsValidate;

namespace dsCommand {
void
solveCmd(CommandHandler &data)
{
  std::string errorString;

  const std::string commandName = data.GetCommandName();

//    GlobalData &gdata = GlobalData::GetInstance();

  /// Will need someway of setting circuit node
  /// (This would be on the contact and not the contact equation??)
  static dsGetArgs::Option option[] =
  {
    {"type",               "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
    {"absolute_error",     "0", dsGetArgs::Types::FLOAT, dsGetArgs::Types::OPTIONAL},
    {"relative_error",     "0", dsGetArgs::Types::FLOAT, dsGetArgs::Types::OPTIONAL},
    {"maximum_iterations", "20", dsGetArgs::Types::INTEGER, dsGetArgs::Types::OPTIONAL},
    {"frequency",    "0.0", dsGetArgs::Types::FLOAT, dsGetArgs::Types::OPTIONAL},
    {"output_node",  "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL},
    {"solver_type",  "direct", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL},
    {"tdelta",       "0.0", dsGetArgs::Types::FLOAT, dsGetArgs::Types::OPTIONAL},
    {"charge_error", "0.0", dsGetArgs::Types::FLOAT, dsGetArgs::Types::OPTIONAL},
    {"gamma",        "1.0", dsGetArgs::Types::FLOAT, dsGetArgs::Types::OPTIONAL},
    {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL}
  };
//      {"callback",      "", dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL},

  dsGetArgs::switchList switches = NULL;


  bool error = data.processOptions(option, switches, errorString);

  if (error)
  {
      data.SetErrorResult(errorString);
      return;
  }

//    const std::string commandName = data.GetCommandName();

  const std::string &type = data.GetStringOption("type");
  const std::string &solver_type = data.GetStringOption("solver_type");

  const double tdelta = data.GetDoubleOption("tdelta");
  const double gamma  = data.GetDoubleOption("gamma");

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

#if 0
    const std::string &callback = data.GetStringOption("callback");
    if (!callback.empty())
    {
        errorString = "callback is not implemented yet\n";
        Tcl_SetResult(interp, const_cast<char *>(errorString.c_str()), TCL_VOLATILE);
        return TCL_ERROR;
    }
#endif

  const double charge_error = data.GetDoubleOption("charge_error");
  const double absolute_error = data.GetDoubleOption("absolute_error");
  const double relative_error = data.GetDoubleOption("relative_error");
  const int    maximum_iterations = data.GetIntegerOption("maximum_iterations");
  const double frequency = data.GetDoubleOption("frequency");
  const std::string &outputNode = data.GetStringOption("output_node");

  dsMath::Newton solver;
  solver.SetAbsError(absolute_error);
  solver.SetRelError(relative_error);
  solver.SetQRelError(charge_error);
  solver.SetMaxIter(maximum_iterations);

  std::unique_ptr<dsMath::LinearSolver> linearSolver;

  if (solver_type == "direct")
  {
    linearSolver = std::unique_ptr<dsMath::LinearSolver>(new dsMath::DirectLinearSolver);
  }
  else if (solver_type == "iterative")
  {
    linearSolver = std::unique_ptr<dsMath::LinearSolver>(new dsMath::IterativeLinearSolver);
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
    res = solver.Solve(*linearSolver, dsMath::TimeMethods::DCONLY);
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
    res = solver.Solve(*linearSolver, dsMath::TimeMethods::INTEGRATEDC);
  }
  else if (type == "transient_bdf1")
  {
    const double td = gamma * tdelta;
    const double tf = 1.0 / td;
    res = solver.Solve(*linearSolver, dsMath::TimeMethods::INTEGRATEBDF1, td, tf, -tf, 0.0, 1.0, 0.0, 0.0);
  }
  else if (type == "transient_tr")
  {
    const double td = gamma * tdelta;
    const double tf = 2.0 / td;
    res = solver.Solve(*linearSolver, dsMath::TimeMethods::INTEGRATETR,   td, tf, -tf, 0.0, 1.0,-1.0, 0.0);
  }
  else if (type == "transient_bdf2")
  {
    //// td for first order projection
    const double td = (1.0 - gamma) * tdelta;
    const double a0 = (2.0 - gamma) / td;
    const double a1 = (-1.0) / (gamma * td);
    const double a2 = (1.0 - gamma) / (gamma * tdelta);
    res = solver.Solve(*linearSolver, dsMath::TimeMethods::INTEGRATEBDF2, td, a0, a1, a2, 1.0, 0.0, 0.0);
  }

  if (!res)
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
getContactCurrentCmd(CommandHandler &data)
{
    std::string errorString;

    const std::string commandName = data.GetCommandName();

    static dsGetArgs::Option option[] = {
        {"device",   "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, mustBeValidDevice},
        {"equation", "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
        {"contact",  "", dsGetArgs::Types::STRING, dsGetArgs::Types::REQUIRED, stringCannotBeEmpty},
        {NULL,  NULL, dsGetArgs::Types::STRING, dsGetArgs::Types::OPTIONAL}
    };

    dsGetArgs::switchList switches = NULL;


    bool error = data.processOptions(option, switches, errorString);

    if (error)
    {
        data.SetErrorResult(errorString);
        return;
    }

    const std::string &deviceName   = data.GetStringOption("device");
    const std::string &contactName  = data.GetStringOption("contact");
    const std::string &equationName = data.GetStringOption("equation");

    Device   *device = NULL;
//    Region   *region = NULL;
    Contact  *contact = NULL;
    ContactEquation *eqn = NULL;

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
    const Contact::ContactEquationPtrMap_t &cepm = contact->GetEquationPtrList();

#if 0
    for (;cit != cend; ++cit)
#endif
    for (Contact::ContactEquationPtrMap_t::const_iterator cepmit = cepm.begin(); cepmit != cepm.end(); ++cepmit)
    {
        if (cepmit->second->GetName() == equationName)
        {
            eqn = cepmit->second;
        }
    }

    double val = 0.0;
    if (!eqn)
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
        val = eqn->GetCurrent();
    }
    else if (commandName == "get_contact_charge")
    {
        val = eqn->GetCharge();
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
    {NULL, NULL}
};

}

//TODO:  "noise solve (with output node)"
