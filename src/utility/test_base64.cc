/***
DEVSIM
Copyright 2017 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "base64.hh"
#include <iostream>
int main()
{
  std::string x = "this is a base 64 encoding test string";
  std::string y = dsUtility::encodeBase64(x.c_str(), x.length());
  std::string z;
  dsUtility::decodeBase64(y, z);
  std::cout << x << "\n";
  std::cout << y << "\n";
  std::cout << z << "\n";
}
