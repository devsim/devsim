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

#ifndef DS_EQUATION_ERRORS_HH
#define DS_EQUATION_ERRORS_HH
#include <string>
#include "GeneralErrors.hh"
#include "OutputStream.hh"
class Region;
class Contact;
class Equation;
class ContactEquation;
class InterfaceEquation;

namespace dsErrors {


  std::string MissingEquationModel(const Region &, const std::string &/*equation_name*/, const std::string &/*model_name*/, ModelInfo::ModelType /*model_type*/, OutputStream::OutputType /*error_level*/);

  std::string MissingEquationIndex(const Region &, const std::string &/*equation_name*/, const std::string &/*variable_name*/, OutputStream::OutputType /*error_level*/);

  std::string MissingContactEquationModel(const Region &, const ContactEquation &/*contact_equation*/, const std::string &/*model_name*/, ModelInfo::ModelType /*model_type*/, OutputStream::OutputType /*error_level*/);

  std::string MissingContactEquationIndex(const Region &, const Contact &, const std::string &/*equation_name*/, OutputStream::OutputType /*error_level*/);

  std::string SolutionVariableNonPositive(const Region &, const std::string &/*equation_name*/, const std::string &/*variable*/, double /*value*/, OutputStream::OutputType /*error_level*/);
  std::string MissingCircuitNodeOnContactEquation(const ContactEquation &/*contact_equation*/,  const std::string &/*circuit_node*/, OutputStream::OutputType /*error_level*/);

  std::string EquationMathErrorInfo(const Equation &/*equation*/, OutputStream::OutputType /*error_level*/);

  std::string CreateModelOnRegion(const Region &/*region*/, const std::string &/*model_name*/, OutputStream::OutputType /*error_level*/);

  std::string MissingInterfaceEquationModel(const Region &/*region*/, const InterfaceEquation &/*interface_equation*/, const std::string &/*model_name*/, OutputStream::OutputType /*error_level*/);
}
#endif
