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

#ifndef DS_OUTPUTSTREAM_HH
#define DS_OUTPUTSTREAM_HH
#include <string>
class OutputStream {
public:
  enum class OutputType { INFO, VERBOSE1, VERBOSE2, ERROR, FATAL };
  enum class Verbosity_t { V0 = 0, V1, V2, UNKNOWN };
  static void WriteOut(OutputType, const std::string &);
  static void SetInterpreter(void *);
  static void WriteOut(OutputType, Verbosity_t verbosity, const std::string &);
  static Verbosity_t GetVerbosity(const std::string &);

private:
  OutputStream();
  OutputStream(const OutputStream &);
  OutputStream &operator=(const OutputStream &);

  static OutputStream *instance;
  std::string verbosity;
  static void *interpreter;
};
#endif
