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
