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

