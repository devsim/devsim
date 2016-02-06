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
