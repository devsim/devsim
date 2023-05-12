/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef DS_MODEL_ERRORS_HH
#define DS_MODEL_ERRORS_HH
#include <string>
#include "GeneralErrors.hh"
#include "OutputStream.hh"
class Region;
class Contact;

namespace dsErrors {
std::string ModelModelDeletion(const Region &, const std::string &/*dependent_model*/, ModelInfo::ModelType /*mt*/, const std::string &/*model*/, ModelInfo::ModelType, OutputStream::OutputType);

std::string MissingModelModelDependency(const Region &, const std::string &/*required_model*/, ModelInfo::ModelType /*mt*/, const std::string &/*model*/, ModelInfo::ModelType, OutputStream::OutputType);

std::string ChangedModelModelDependency(const Region &, const std::string &/*required_model*/, ModelInfo::ModelType /*mt*/, const std::string &/*model*/, ModelInfo::ModelType, OutputStream::OutputType);

std::string ReviveContainer(const Region &, const std::string &/*revived_model*/, ModelInfo::ModelType /*mt*/, const std::string &/*model*/, ModelInfo::ModelType, OutputStream::OutputType);
}
#endif
