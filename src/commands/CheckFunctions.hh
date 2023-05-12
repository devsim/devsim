/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef CHECK_FUNCTIONS_HH
#define CHECK_FUNCTIONS_HH
#include <string>
namespace dsGetArgs {
class GetArgs;
}
class CommandHandler;
namespace dsCommand {
bool mustBePositive(const std::string &/*optionName*/, std::string &/*errorString*/, const CommandHandler &);
bool stringCannotBeEmpty(const std::string &/*optionName*/,std::string &/*errorString*/, const CommandHandler &);
bool meshCannotExist(const std::string &/*optionName*/, std::string &/*errorString*/, const CommandHandler &);
bool meshMustExist(const std::string &/*optionName*/, std::string &/*errorString*/, const CommandHandler &);
bool meshMustNotBeFinalized(const std::string &/*optionName*/, std::string &/*errorString*/, const CommandHandler &);
bool meshMustBeFinalized(const std::string &/*optionName*/, std::string &/*errorString*/, const CommandHandler &);
bool mustBeValidDevice(const std::string &/*optionName*/, std::string &/*errorString*/, const CommandHandler &);
bool mustNotBeValidDevice(const std::string &/*optionName*/, std::string &/*errorString*/, const CommandHandler &);
bool mustBeValidRegion(const std::string &/*optionName*/, std::string &/*errorString*/, const CommandHandler &);
bool mustBeValidContact(const std::string &/*optionName*/, std::string &/*errorString*/, const CommandHandler &);
bool mustBeValidInterface(const std::string &/*optionName*/, std::string &/*errorString*/, const CommandHandler &);
bool mustBeSpecifiedIfRegionSpecified(const std::string &/*optionName*/, std::string &/*errorString*/, const CommandHandler &);
}
#endif

