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

#include "dsAssert.hh"
#include "OutputStream.hh"
#include <sstream>
#include <string>

void dsAssert_(bool x, const std::string &msg) {
  if (!x) {
    std::ostringstream os;
    os << "There was a fatal exception in the program.  Shutting down.  Please "
          "inspect program output.\n";
    os << msg << "\n";
    OutputStream::WriteOut(OutputStream::OutputType::FATAL, os.str().c_str());
  }
}
