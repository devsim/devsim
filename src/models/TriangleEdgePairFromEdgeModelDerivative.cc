/***
DEVSIM
Copyright 2020 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "TriangleEdgePairFromEdgeModelDerivative.hh"
#include "TriangleEdgeSubModel.hh"
#include "Region.hh"
#include "TriangleElementField.hh"
#include "dsAssert.hh"
#include "Vector.hh"



template <typename DoubleType>
TriangleEdgePairFromEdgeModelDerivative<DoubleType>::TriangleEdgePairFromEdgeModelDerivative(
        const std::string &edgemodel,
        const std::string &derivative,
        RegionPtr rp
    )
    : TriangleEdgeModel(edgemodel + "_node0_x:" + derivative + "@en0", rp, TriangleEdgeModel::DisplayType::NODISPLAY),
      edgeModelName(edgemodel),
      nodeModelName(derivative)
{
  const std::string tmpname = edgeModelName + ":" + nodeModelName;
  edgeModelNames[0] = tmpname + "@n0";
  edgeModelNames[1] = tmpname + "@n1";
  RegisterCallback(edgeModelNames[0]);
  RegisterCallback(edgeModelNames[1]);

  for (size_t i = 0; i < 3; ++i) // derivatives
  {
    const std::string suffix = ":" + nodeModelName + "@en" + std::to_string(i);

    for (size_t j = 0; j < 2; ++j) // edge node
    {
      std::string tmp =  edgeModelName + "_node" + std::to_string(j) + "_";
      model_names[i][j][0] = tmp + "x" + suffix;
      model_names[i][j][1] = tmp + "y" + suffix;
    }
  }

  for (size_t i = 0; i < 3; ++i)
  {
    for (size_t j = 0; j < 2; ++j)
    {
      for (size_t k = 0; k < 2; ++k)
      {
        if ((i + j + k) != 0)
        {
          new TriangleEdgeSubModel<DoubleType>(model_names[i][j][k], rp, TriangleEdgeModel::DisplayType::NODISPLAY, this->GetSelfPtr());
        }
      }
    }
  }
}

template <typename DoubleType>
void TriangleEdgePairFromEdgeModelDerivative<DoubleType>::calcTriangleEdgeScalarValues() const
{
  const Region &reg = GetRegion();

  ConstEdgeModelPtr emp[2];
  for (size_t i = 0; i < 2; ++i)
  {
    emp[i] = reg.GetEdgeModel(edgeModelNames[i]);
    dsAssert(emp[i].get(), "UNEXPECTED");
  }

  const ConstTriangleList &tl = GetRegion().GetTriangleList();

  std::array<std::array<std::array<TriangleEdgeModelPtr, 2>, 2>, 3> temp;
  std::array<std::array<std::array<std::vector<DoubleType>, 2>, 2>, 3> ev;
  for (size_t i = 0; i < 3; ++i)
  {
    for (size_t j = 0; j < 2; ++j)
    {
      for (size_t k = 0; k < 2; ++k)
      {
        ConstTriangleEdgeModelPtr t = reg.GetTriangleEdgeModel(model_names[i][j][k]);
        dsAssert(t.get(), "UNEXPECTED");
        temp[i][j][k] = std::const_pointer_cast<TriangleEdgeModel, const TriangleEdgeModel>(t);
        ev[i][j][k].resize(3*tl.size());
      }
    }
  }


  const ConstTriangleEdgeModelPtr eec = reg.GetTriangleEdgeModel("ElementEdgeCouple");
  dsAssert(eec.get(), "UNEXPECTED");

  const TriangleElementField<DoubleType> &efield = reg.GetTriangleElementField<DoubleType>();

  std::array<typename TriangleElementField<DoubleType>::DerivativeEdgeVectors_t, 2> v;
  for (size_t i = 0; i < tl.size(); ++i)
  {
    const Triangle &triangle = *tl[i];
    efield.GetTriangleElementFieldPairs(triangle, *eec, *emp[0], *emp[1], v[0], v[1]);
    for (size_t nindex = 0; nindex < 3; ++nindex)
    {
      for (size_t eindex = 0; eindex < 3; ++eindex)
      {
        const size_t oindex = 3*i + eindex;
        for (size_t j = 0; j < 2; ++j)
        {
          const auto &vec = v[j][nindex][eindex];
          ev[nindex][j][0][oindex] = vec.Getx();
          ev[nindex][j][1][oindex] = vec.Gety();
        }
      }
    }
  }
  for (size_t i = 0; i < 3; ++i)
  {
    for (size_t j = 0; j < 2; ++j)
    {
      for (size_t k = 0; k < 2; ++k)
      {
        temp[i][j][k]->SetValues(ev[i][j][k]);
      }
    }
  }
}

template <typename DoubleType>
void TriangleEdgePairFromEdgeModelDerivative<DoubleType>::Serialize(std::ostream &of) const
{
  of << "COMMAND element_pair_from_edge_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -edge_model \"" << edgeModelName << "\" -derivative \"" << nodeModelName << "\"";
}

template class TriangleEdgePairFromEdgeModelDerivative<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class TriangleEdgePairFromEdgeModelDerivative<float128>;
#endif

TriangleEdgeModelPtr CreateTriangleEdgePairFromEdgeModelDerivative(
        const std::string &edgemodel,
        const std::string &derivative,
        RegionPtr rp
    )
{
  const bool use_extended = rp->UseExtendedPrecisionModels();
  return create_triangle_edge_model<TriangleEdgePairFromEdgeModelDerivative<double>, TriangleEdgePairFromEdgeModelDerivative<extended_type>>(use_extended, edgemodel, derivative, rp);
}


