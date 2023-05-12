/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef GEOMETRY_STREAM_HH
#define GEOMETRY_STREAM_HH
class Device;
class Region;
class Contact;
class Interface;
#include "OutputStream.hh"
#include <string>
namespace GeometryStream {
void WriteOut(OutputStream::OutputType, const Device &, const std::string &);
void WriteOut(OutputStream::OutputType, const Region &, const std::string &);
void WriteOut(OutputStream::OutputType, const Contact &, const std::string &);
void WriteOut(OutputStream::OutputType, const Interface &, const std::string &);
}
#endif
