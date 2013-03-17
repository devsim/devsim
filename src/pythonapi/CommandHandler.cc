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

#include "Python.h"
#include "CommandHandler.hh"
#include "CommandHandlerCommon.cc"
#include "GetArgs.hh"
#include "dsAssert.hh"

CommandHandler::CommandHandler(void *command_info)
  : command_info_(command_info), return_code_(0)
{
  dsGetArgs::CommandInfo &info = *(reinterpret_cast<dsGetArgs::CommandInfo *>(command_info_));
  command_name = info.command_name_;
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
  return_code_ = 1;
  Py_INCREF(Py_None);
  return_object_ = ObjectHolder(Py_None);
}

void CommandHandler::SetErrorResult(const std::string &errorString)
{
  dsGetArgs::CommandInfo &info = *(reinterpret_cast<dsGetArgs::CommandInfo *>(command_info_));
  return_code_ = 0;
  return_object_.clear();
  PyErr_SetString(info.exception_, errorString.c_str());
}

void CommandHandler::SetObjectResult(ObjectHolder obj)
{
  return_object_ = obj;
  return_code_ = 1;
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


