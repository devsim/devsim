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

#include "EquationErrors.hh"
#include "GeneralErrors.hh"
#include "GeometryStream.hh"
#include "ErrorHelpers.hh"
#include "ContactEquation.hh"
#include "InterfaceEquation.hh"
#include "Equation.hh"
#include "Region.hh"
#include "Device.hh"

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

std::string MissingContactEquationModel(const Region &region, const ContactEquation<double> &contact_equation, const std::string &model_name, ModelInfo::ModelType model_type, OutputStream::OutputType error_level) {
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

std::string SolutionVariableNonPositive(const Region &region, const std::string &equation_name, const std::string &variable, double value, OutputStream::OutputType error_level)
{
  std::ostringstream os;
  os << FormatEquationOnRegionName(region, equation_name)
     << "Variable: " << variable << " "
     << "Solution Variable has negative or zero value before update. " << value << "\n";
  GeometryStream::WriteOut(error_level, region, os.str());
  return os.str();
}

std::string MissingCircuitNodeOnContactEquation(const ContactEquation<double> &contact_equation,  const std::string &circuit_node, OutputStream::OutputType error_level)
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

std::string MissingInterfaceEquationModel(const Region &region, const InterfaceEquation<double> &interface_equation, const std::string &model_name, OutputStream::OutputType error_level)
{
  std::ostringstream os; 
  os << FormatInterfaceEquationOnInterfaceName(region, interface_equation.GetInterface(), interface_equation.GetName())
      << " " <<
      FormatModelNameAndType(model_name, ModelInfo::INTERFACE)
     << " MISSING\n";
  GeometryStream::WriteOut(error_level, interface_equation.GetInterface(), os.str());
  return os.str();
}

}

