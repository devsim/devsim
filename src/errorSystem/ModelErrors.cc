/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "ModelErrors.hh"
#include "GeneralErrors.hh"
#include "GeometryStream.hh"
#include "ErrorHelpers.hh"

#include <sstream>

#include <string>
namespace dsErrors {
std::string ModelModelDeletion(const Region &region, const std::string &dep_model, ModelInfo::ModelType dep_mt, const std::string &model, ModelInfo::ModelType mt, OutputStream::OutputType ot)
{
  std::ostringstream os;
  os << FormatDeviceAndRegionName(region)
     << " " <<
     FormatModelNameAndType(dep_model, dep_mt) << " is being deleted as " <<
     FormatModelNameAndType(model, mt) << " is being deleted\n";
  GeometryStream::WriteOut(ot, region, os.str());
  return os.str();
}

std::string MissingModelModelDependency(const Region &region, const std::string &req_model, ModelInfo::ModelType req_mt, const std::string &model, ModelInfo::ModelType mt, OutputStream::OutputType ot)
{
  std::ostringstream os;
  os << FormatDeviceAndRegionName(region)
     << " " <<
     FormatModelNameAndType(req_model, req_mt) << " does not exist so " <<
     FormatModelNameAndType(model, mt) << " cannot be calculated\n";
  GeometryStream::WriteOut(ot, region, os.str());
  return os.str();
}

std::string ReviveContainer(const Region &region, const std::string &rev_model, ModelInfo::ModelType rev_mt, const std::string &model, ModelInfo::ModelType mt, OutputStream::OutputType ot)
{
  std::ostringstream os;
  os << FormatDeviceAndRegionName(region)
     << " " <<
     FormatModelNameAndType(rev_model, rev_mt) << " appears to have been deleted.  Being recreated as calculating " <<
     FormatModelNameAndType(model, mt) << "\n";
  GeometryStream::WriteOut(ot, region, os.str());
  return os.str();
}

std::string ChangedModelModelDependency(const Region &region, const std::string &req_model, ModelInfo::ModelType req_mt, const std::string &model, ModelInfo::ModelType mt, OutputStream::OutputType ot)
{
  std::ostringstream os;
  os << FormatDeviceAndRegionName(region)
     << " " <<
     FormatModelNameAndType(req_model, req_mt) << " appears to have been redefined since last calculation of " <<
     FormatModelNameAndType(model, mt) << "\n";
  GeometryStream::WriteOut(ot, region, os.str());
  return os.str();
}

}

