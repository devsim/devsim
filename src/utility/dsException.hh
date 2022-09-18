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

#ifndef DS_EXCEPTION_HH
#define DS_EXCEPTION_HH
class dsException {
  public:
    //// Should avoid passing something which is going to be deleted in the scope being called.
    dsException() throw () {};
    dsException(const dsException &) throw () {};
    dsException &operator=(const dsException &) throw () {return *this;};

    const char *what() const {
      return msg;
    }
  private:
    static const char *msg;
};
#endif

