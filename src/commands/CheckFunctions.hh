/***
DEVSIM
Copyright 2013 DEVSIM LLC

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

