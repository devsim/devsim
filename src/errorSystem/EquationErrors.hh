/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef DS_EQUATION_ERRORS_HH
#define DS_EQUATION_ERRORS_HH
#include <string>
#include "GeneralErrors.hh"
#include "OutputStream.hh"
class Region;
class Contact;
template <typename DoubleType>
class Equation;
template <typename DoubleType>
class ContactEquation;
template <typename DoubleType>
class InterfaceEquation;

namespace dsErrors {


  std::string MissingEquationModel(const Region &, const std::string &/*equation_name*/, const std::string &/*model_name*/, ModelInfo::ModelType /*model_type*/, OutputStream::OutputType /*error_level*/);

  std::string MissingEquationIndex(const Region &, const std::string &/*equation_name*/, const std::string &/*variable_name*/, OutputStream::OutputType /*error_level*/);

  template <typename DoubleType>
  std::string MissingContactEquationModel(const Region &, const ContactEquation<DoubleType> &/*contact_equation*/, const std::string &/*model_name*/, ModelInfo::ModelType /*model_type*/, OutputStream::OutputType /*error_level*/);

  std::string MissingContactEquationIndex(const Region &, const Contact &, const std::string &/*equation_name*/, OutputStream::OutputType /*error_level*/);

  template <typename DoubleType>
  std::string SolutionVariableNonPositive(const Region &, const std::string &/*equation_name*/, const std::string &/*variable*/, DoubleType /*value*/, OutputStream::OutputType /*error_level*/);

  template <typename DoubleType>
  std::string MissingCircuitNodeOnContactEquation(const ContactEquation<DoubleType> &/*contact_equation*/,  const std::string &/*circuit_node*/, OutputStream::OutputType /*error_level*/);

  std::string EquationMathErrorInfo(const Equation<double> &/*equation*/, OutputStream::OutputType /*error_level*/);

  std::string CreateModelOnRegion(const Region &/*region*/, const std::string &/*model_name*/, OutputStream::OutputType /*error_level*/);

  template <typename DoubleType>
  std::string MissingInterfaceEquationModel(const Region &/*region*/, const InterfaceEquation<DoubleType> &/*interface_equation*/, const std::string &/*model_name*/, OutputStream::OutputType /*error_level*/);
}
#endif
