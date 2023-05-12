/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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

