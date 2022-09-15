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

#ifndef DS_MODEL_ERRORS_HH
#define DS_MODEL_ERRORS_HH
#include "GeneralErrors.hh"
#include "OutputStream.hh"
#include <string>
class Region;
class Contact;

namespace dsErrors {
std::string ModelModelDeletion(const Region &,
                               const std::string & /*dependent_model*/,
                               ModelInfo::ModelType /*mt*/,
                               const std::string & /*model*/,
                               ModelInfo::ModelType, OutputStream::OutputType);

std::string MissingModelModelDependency(const Region &,
                                        const std::string & /*required_model*/,
                                        ModelInfo::ModelType /*mt*/,
                                        const std::string & /*model*/,
                                        ModelInfo::ModelType,
                                        OutputStream::OutputType);

std::string ChangedModelModelDependency(const Region &,
                                        const std::string & /*required_model*/,
                                        ModelInfo::ModelType /*mt*/,
                                        const std::string & /*model*/,
                                        ModelInfo::ModelType,
                                        OutputStream::OutputType);

std::string ReviveContainer(const Region &,
                            const std::string & /*revived_model*/,
                            ModelInfo::ModelType /*mt*/,
                            const std::string & /*model*/, ModelInfo::ModelType,
                            OutputStream::OutputType);
} // namespace dsErrors
#endif
