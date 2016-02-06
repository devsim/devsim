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

#ifndef DSTIMER_HH
#define DSTIMER_HH
#include "OutputStream.hh"
#include <string>
#include <memory>

class dsTimer {
  public:
    dsTimer(const std::string &/*token*/, OutputStream::OutputType outtype = OutputStream::VERBOSE2);
    ~dsTimer();
  private:
    dsTimer(const dsTimer &);
    dsTimer &operator=(const dsTimer &);

    const                    std::string msg_;
    OutputStream::OutputType output_type_;
    void *tic_;
};
#endif
