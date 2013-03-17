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

#include "TclCommands.hh"
#include "CommandHandler.hh"
#include "GetArgs.hh"
#include "dsCommand.hh"
#include "Validate.hh"

#include "GeometryCommands.hh"
#include "MaterialCommands.hh"
#include "ModelCommands.hh"
#include "MathCommands.hh"
#include "EquationCommands.hh"
#include "MeshingCommands.hh"
#include "CircuitCommands.hh"
#include "dsException.hh"
#include "FPECheck.hh"
#include "OutputStream.hh"

#include <tcl.h>

#include <new>
#include <sstream>

using namespace dsValidate;

namespace mthread {
extern Tcl_ThreadId mthread;
extern const char * const mthread_error;
}

namespace dsTcl {

int 
CmdDispatch(ClientData clientData, Tcl_Interp *interp,
          int objc, Tcl_Obj *CONST objv[])
{
  int myerror = TCL_OK;

  FPECheck::ClearFPE();

  Tcl_ThreadId thisthread = Tcl_GetCurrentThread();
  
  if (mthread::mthread != thisthread)
  {
    myerror = TCL_ERROR;
    Tcl_SetResult(interp, const_cast<char *>(mthread::mthread_error), TCL_VOLATILE);
    return myerror;
  }

  try
  {
//    double * x = new double[size_t(-1)];
    newcmd cmd = reinterpret_cast<dsClientData *>(clientData)->GetCommand();
    dsGetArgs::CommandInfo command_info;
    command_info.interp_          =  interp;
    command_info.objc_            =  objc;
    command_info.objv_            =  objv;
    command_info.client_data_     =  clientData;
    CommandHandler data(&command_info);
    command_info.command_handler_ = &data;
    cmd(data);
    myerror = data.GetReturnCode();
  }
  catch (const dsException &x)
  {
    myerror = TCL_ERROR;
    Tcl_SetResult(interp, const_cast<char *>(x.what()), TCL_VOLATILE);
  }
  catch (std::bad_alloc &)
  {
    myerror = TCL_ERROR;
    Tcl_SetResult(interp, const_cast<char *>("OUT OF MEMORY"), TCL_VOLATILE);
  }
  catch (std::exception &e)
  {
    myerror = TCL_ERROR;
    Tcl_SetResult(interp, const_cast<char *>("UNEXPECTED ERROR"), TCL_VOLATILE);
  }


  if (FPECheck::CheckFPE())
  {
    std::ostringstream os;
    os << "There was an uncaught floating point exception of type \"" << FPECheck::getFPEString() << "\"\n";
    FPECheck::ClearFPE();
    OutputStream::WriteOut(OutputStream::ERROR, os.str().c_str());
    myerror = TCL_ERROR;
  }


  return myerror;
}

void AddCommands(Tcl_Interp *interp, dsCommand::Commands *clist)
{
  for ( ; (clist->name) != NULL; ++clist)
  {
    void *cdata = dsClientData::CreateData(clist->name, clist->command);

    Tcl_CreateObjCommand(interp, clist->name, CmdDispatch,
            (ClientData) cdata, (Tcl_CmdDeleteProc *) dsClientData::DeleteData);
  }
}

int 
Commands_Init(Tcl_Interp *interp) {

    int myerror = TCL_OK;

    if (myerror == TCL_OK)
    {
      AddCommands(interp, dsCommand::GeometryCommands);
      AddCommands(interp, dsCommand::MaterialCommands);
      AddCommands(interp, dsCommand::ModelCommands);
      AddCommands(interp, dsCommand::MathCommands);
      AddCommands(interp, dsCommand::EquationCommands);
      AddCommands(interp, dsCommand::MeshingCommands);
      AddCommands(interp, dsCommand::CircuitCommands);
    }
    return myerror;
}
}

