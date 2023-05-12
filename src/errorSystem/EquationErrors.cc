/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "EquationErrors.hh"
#include "GeneralErrors.hh"
#include "GeometryStream.hh"
#include "ErrorHelpers.hh"
#include "ContactEquation.hh"
#include "InterfaceEquation.hh"
#include "Equation.hh"
#include "Region.hh"
#include "Device.hh"

#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#endif

#include <sstream>

#include <string>
namespace dsErrors {
std::string MissingEquationModel(const Region &region, const std::string &equation_name, const std::string &model_name, ModelInfo::ModelType model_type, OutputStream::OutputType error_level) {
  std::ostringstream os;

  os << FormatEquationOnRegionName(region, equation_name)
        << " " <<
        FormatModelNameAndType(model_name, model_type)
        << " MISSING\n";
  GeometryStream::WriteOut(error_level, region, os.str());
  return os.str();
}

std::string MissingEquationIndex(const Region &region, const std::string &equation_name, const std::string &variable_name, OutputStream::OutputType error_level)
{
  std::ostringstream os;
  os << FormatEquationOnRegionName(region, equation_name);
  if (variable_name.empty())
  {
    os << " Cannot find equation associated with variable " << variable_name << "\n";
  }
  else
  {
    os << " Cannot find equation index.\n";
  }
  GeometryStream::WriteOut(error_level, region, os.str());
  return os.str();
}

template <typename DoubleType>
std::string MissingContactEquationModel(const Region &region, const ContactEquation<DoubleType> &contact_equation, const std::string &model_name, ModelInfo::ModelType model_type, OutputStream::OutputType error_level) {
  std::ostringstream os;
  os << FormatContactEquationOnContactName(region, contact_equation.GetContact(), contact_equation.GetName())
        << " " <<
        FormatModelNameAndType(model_name, model_type)
        << " MISSING\n";
  GeometryStream::WriteOut(error_level, contact_equation.GetContact(), os.str());
  return os.str();
}

std::string MissingContactEquationIndex(const Region &region, const Contact &contact, const std::string &equation_name, OutputStream::OutputType error_level)
{
  std::ostringstream os;
  os << FormatContactEquationOnContactName(region, contact, equation_name)
     << " Cannot find equation index associated with Equation\n";
  GeometryStream::WriteOut(error_level, contact, os.str());
  return os.str();
}

template <typename DoubleType>
std::string SolutionVariableNonPositive(const Region &region, const std::string &equation_name, const std::string &variable, DoubleType value, OutputStream::OutputType error_level)
{
  std::ostringstream os;
  os << FormatEquationOnRegionName(region, equation_name)
     << "Variable: " << variable << " "
     << "Solution Variable has negative or zero value before update. " << value << "\n";
  GeometryStream::WriteOut(error_level, region, os.str());
  return os.str();
}

template <typename DoubleType>
std::string MissingCircuitNodeOnContactEquation(const ContactEquation<DoubleType> &contact_equation,  const std::string &circuit_node, OutputStream::OutputType error_level)
{
  std::ostringstream os;
  os << FormatContactEquationOnContactName(contact_equation.GetRegion(), contact_equation.GetContact(), contact_equation.GetName());
  os << " Circuit node \"" << circuit_node << "\" does not exist\n";
  GeometryStream::WriteOut(error_level, contact_equation.GetContact(), os.str());
  return os.str();
}

std::string EquationMathErrorInfo(const Equation<double> &equation, OutputStream::OutputType error_level)
{
  std::ostringstream os;
  os << "Device: " << equation.GetRegion().GetDevice()->GetName() << "\t"
     << "Region: " << equation.GetRegion().GetName() << "\t"
     << "Equation: " << equation.GetName() << "\t"
     << "RelError: " << equation.GetRelError() << "\t"
     << "AbsError: " << equation.GetAbsError() << "\n";
  GeometryStream::WriteOut(error_level, equation.GetRegion(), os.str());
  return os.str();
}

std::string CreateModelOnRegion(const Region &region, const std::string &model_name, OutputStream::OutputType error_level)
{
  std::ostringstream os;
  os << "Device: " << region.GetDevice()->GetName() << "\t"
     << "Region: " << region.GetName() << "\t"
     << "Creating: " << model_name << "\n";
  GeometryStream::WriteOut(error_level, region, os.str());
  return os.str();
}

template <typename DoubleType>
std::string MissingInterfaceEquationModel(const Region &region, const InterfaceEquation<DoubleType> &interface_equation, const std::string &model_name, OutputStream::OutputType error_level)
{
  std::ostringstream os;
  os << FormatInterfaceEquationOnInterfaceName(region, interface_equation.GetInterface(), interface_equation.GetName())
      << " " <<
      FormatModelNameAndType(model_name, ModelInfo::INTERFACE)
     << " MISSING\n";
  GeometryStream::WriteOut(error_level, interface_equation.GetInterface(), os.str());
  return os.str();
}

template std::string MissingContactEquationModel(const Region &region, const ContactEquation<double> &contact_equation, const std::string &model_name, ModelInfo::ModelType model_type, OutputStream::OutputType error_level);
template std::string SolutionVariableNonPositive(const Region &region, const std::string &equation_name, const std::string &variable, double value, OutputStream::OutputType error_level);
template std::string MissingInterfaceEquationModel(const Region &region, const InterfaceEquation<double> &interface_equation, const std::string &model_name, OutputStream::OutputType error_level);
template std::string MissingCircuitNodeOnContactEquation(const ContactEquation<double> &/*contact_equation*/,  const std::string &/*circuit_node*/, OutputStream::OutputType /*error_level*/);
#ifdef DEVSIM_EXTENDED_PRECISION
template std::string MissingContactEquationModel(const Region &region, const ContactEquation<float128> &contact_equation, const std::string &model_name, ModelInfo::ModelType model_type, OutputStream::OutputType error_level);
template std::string SolutionVariableNonPositive(const Region &region, const std::string &equation_name, const std::string &variable, float128 value, OutputStream::OutputType error_level);
template std::string MissingInterfaceEquationModel(const Region &region, const InterfaceEquation<float128> &interface_equation, const std::string &model_name, OutputStream::OutputType error_level);
template std::string MissingCircuitNodeOnContactEquation(const ContactEquation<float128> &/*contact_equation*/,  const std::string &/*circuit_node*/, OutputStream::OutputType /*error_level*/);
#endif
}

