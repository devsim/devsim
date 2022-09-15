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

#include "CylindricalEdgeCouple.hh"
#include "Region.hh"
#include "TriangleEdgeModel.hh"
#include "dsAssert.hh"

template <typename DoubleType>
CylindricalEdgeCouple<DoubleType>::CylindricalEdgeCouple(RegionPtr rp)
    : EdgeModel("CylindricalEdgeCouple", rp, EdgeModel::DisplayType::SCALAR) {
  const size_t dimension = rp->GetDimension();
  dsAssert(dimension == 2, "CylindricalEdgeCouple 2d Only");
  if (dimension == 2) {
    RegisterCallback("ElementCylindricalEdgeCouple");
  }
}

template <typename DoubleType>
void CylindricalEdgeCouple<DoubleType>::calcEdgeScalarValues() const {
  const size_t dimension = GetRegion().GetDimension();

  if (dimension == 1) {
    dsAssert(false, "CylindricalEdgeCouple not supported in 1d");
  } else if (dimension == 2) {
    calcCylindricalEdgeCouple2d();
  } else if (dimension == 3) {
    dsAssert(false, "CylindricalEdgeCouple not supported in 3d");
  } else {
    dsAssert(false, "UNEXPECTED");
  }
}

template <typename DoubleType>
void CylindricalEdgeCouple<DoubleType>::calcCylindricalEdgeCouple2d() const {
  ConstTriangleEdgeModelPtr eec =
      GetRegion().GetTriangleEdgeModel("ElementCylindricalEdgeCouple");
  dsAssert(eec.get(), "ElementCylindricalEdgeCouple missing");

  std::vector<DoubleType> ev = eec->GetValuesOnEdges<DoubleType>();
  SetValues(ev);
}

template <typename DoubleType>
void CylindricalEdgeCouple<DoubleType>::Serialize(std::ostream &of) const {
  of << "DATAPARENT \"ElementCylindricalEdgeCouple\"";
}

template class CylindricalEdgeCouple<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class CylindricalEdgeCouple<float128>;
#endif

EdgeModelPtr CreateCylindricalEdgeCouple(RegionPtr rp) {
  const bool use_extended = rp->UseExtendedPrecisionModels();
  return create_edge_model<CylindricalEdgeCouple<double>,
                           CylindricalEdgeCouple<extended_type>>(use_extended,
                                                                 rp);
}
