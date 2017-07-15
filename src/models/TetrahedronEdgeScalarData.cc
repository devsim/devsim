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

#include "TetrahedronEdgeScalarData.hh"
#include "TetrahedronEdgeModel.hh"
#include "ScalarData.cc"

//// Manual Template Instantiation
template class ScalarData<TetrahedronEdgeModel, double>;

template class ScalarData<TetrahedronEdgeModel, double>& TetrahedronEdgeScalarData<double>::op_equal<ScalarDataHelper::times_equal<double>>(const double &, const ScalarDataHelper::times_equal<double> &);

template class ScalarData<TetrahedronEdgeModel, double>& TetrahedronEdgeScalarData<double>::op_equal<ScalarDataHelper::times_equal<double>>(const TetrahedronEdgeScalarData<double> &, const ScalarDataHelper::times_equal<double> &);

template class ScalarData<TetrahedronEdgeModel, double>& TetrahedronEdgeScalarData<double>::op_equal<ScalarDataHelper::plus_equal<double>>(const double &, const ScalarDataHelper::plus_equal<double> &);

template class ScalarData<TetrahedronEdgeModel, double>& TetrahedronEdgeScalarData<double>::op_equal<ScalarDataHelper::plus_equal<double>>(const TetrahedronEdgeScalarData<double> &, const ScalarDataHelper::plus_equal<double> &);

