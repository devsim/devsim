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

