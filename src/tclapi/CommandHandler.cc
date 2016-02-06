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

#include "CommandHandler.hh"
#include "CommandHandlerCommon.cc"
#include "TclCommands.hh"
#include "GetArgs.hh"
#include "dsAssert.hh"
#include <tcl.h>

CommandHandler::CommandHandler(void *command_info)
  : command_info_(command_info), return_code_(TCL_ERROR)
{
  dsGetArgs::CommandInfo &info = *(reinterpret_cast<dsGetArgs::CommandInfo *>(command_info_));
  command_name = dsTcl::dsClientData::GetCommandName(info.client_data_);
}

CommandHandler::~CommandHandler()
{
  dsGetArgs::CommandInfo &info = *(reinterpret_cast<dsGetArgs::CommandInfo *>(command_info_));
  delete info.get_args_;
}

const std::string &CommandHandler::GetCommandName() const
{
  return command_name;
}

void CommandHandler::SetEmptyResult()
{
  return_code_ = TCL_OK;
}

void CommandHandler::SetErrorResult(const std::string &errorString)
{
  return_code_ = TCL_ERROR;
  dsGetArgs::CommandInfo &info = *(reinterpret_cast<dsGetArgs::CommandInfo *>(command_info_));
  Tcl_SetResult(info.interp_, const_cast<char *>(errorString.c_str()), TCL_VOLATILE);
}



void CommandHandler::SetObjectResult(ObjectHolder obj)
{
  dsGetArgs::CommandInfo &info = *(reinterpret_cast<dsGetArgs::CommandInfo *>(command_info_));
  return_code_ = TCL_OK;
  Tcl_SetObjResult(info.interp_, reinterpret_cast<Tcl_Obj *>(obj.GetObject()));
}

bool CommandHandler::processOptions(dsGetArgs::optionList opts, dsGetArgs::switchList sl, std::string & errors) const
{
  dsGetArgs::CommandInfo &info = *(reinterpret_cast<dsGetArgs::CommandInfo *>(command_info_));
  dsAssert(!info.get_args_, "UNEXPECTED");
  info.get_args_ = new dsGetArgs::GetArgs(opts, sl);
  return info.get_args_->processOptions(info, errors);
}

std::string CommandHandler::GetStringOption(const std::string &s) const
{
  dsGetArgs::CommandInfo &info = *(reinterpret_cast<dsGetArgs::CommandInfo *>(command_info_));
  dsAssert(info.get_args_, "UNEXPECTED");
  return info.get_args_->GetStringOption(s);
}

double CommandHandler::GetDoubleOption(const std::string &s) const
{
  dsGetArgs::CommandInfo &info = *(reinterpret_cast<dsGetArgs::CommandInfo *>(command_info_));
  dsAssert(info.get_args_, "UNEXPECTED");
  return info.get_args_->GetDoubleOption(s);
}

int    CommandHandler::GetIntegerOption(const std::string &s) const
{
  dsGetArgs::CommandInfo &info = *(reinterpret_cast<dsGetArgs::CommandInfo *>(command_info_));
  dsAssert(info.get_args_, "UNEXPECTED");
  return info.get_args_->GetIntegerOption(s);
}

bool   CommandHandler::GetBooleanOption(const std::string &s) const
{
  dsGetArgs::CommandInfo &info = *(reinterpret_cast<dsGetArgs::CommandInfo *>(command_info_));
  dsAssert(info.get_args_, "UNEXPECTED");
  return info.get_args_->GetBooleanOption(s);
}

ObjectHolder CommandHandler::GetObjectHolder(const std::string &s) const
{
  dsGetArgs::CommandInfo &info = *(reinterpret_cast<dsGetArgs::CommandInfo *>(command_info_));
  dsAssert(info.get_args_, "UNEXPECTED");
  return info.get_args_->GetObjectHolder(s);
}

