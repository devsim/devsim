/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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
  error_string_.clear();
}

void CommandHandler::SetErrorResult(const std::string &errorString)
{
  return_code_ = 0;
  return_object_.clear();
  error_string_ = errorString;
}

void CommandHandler::SetObjectResult(ObjectHolder obj)
{
  return_object_ = obj;
  return_code_ = 1;
  error_string_.clear();
}


bool CommandHandler::processOptions(dsGetArgs::optionList opts, std::string & errors) const
{
  dsGetArgs::CommandInfo &info = *(reinterpret_cast<dsGetArgs::CommandInfo *>(command_info_));
  dsAssert(!info.get_args_, "UNEXPECTED");
  info.get_args_ = new dsGetArgs::GetArgs(opts);
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


