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

#ifndef DS_TCL_COMMANDS_HH
#define DS_TCL_COMMANDS_HH
class CommandHandler;
typedef void (*newcmd)(CommandHandler &);
extern "C" {
struct Tcl_Interp;
}
namespace dsTcl {
#if 0
typedef int (*tclcmd) (ClientData clientData, Tcl_Interp *interp,
      int objc, Tcl_Obj *CONST objv[]);

struct Commands
{
    const char *name;
    tclcmd     command;
};
#endif

int Commands_Init(Tcl_Interp *interp);

class dsClientData {
  private:
    dsClientData(const char *name, newcmd impl) : command_name_(name), command_impl_(impl)
    {
    }

  public:
    static void *CreateData(const char *name, newcmd impl)
    {

      void *handle;
      handle = new dsClientData(name, impl);
      return handle;
    } 

    static void DeleteData(void *handle)
    {
      delete reinterpret_cast<dsClientData *>(handle);
    }

    static const char *GetCommandName(void *handle)
    {
      return reinterpret_cast<dsClientData *>(handle)->command_name_;
    }

    newcmd GetCommand() {
      return command_impl_;
    }

  private:
    dsClientData();
    dsClientData(const dsClientData &);
    dsClientData &operator=(const dsClientData &);


    ~dsClientData()
    {
    }

    const char *command_name_;
    newcmd      command_impl_;
};
}
#endif
