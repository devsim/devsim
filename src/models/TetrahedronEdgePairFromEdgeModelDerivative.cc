/***
DEVSIM
Copyright 2020 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "TetrahedronEdgePairFromEdgeModelDerivative.hh"
#include "TetrahedronEdgeSubModel.hh"
#include "Region.hh"
#include "TetrahedronElementField.hh"
#include "dsAssert.hh"
#include "Vector.hh"

template <typename DoubleType>
TetrahedronEdgePairFromEdgeModelDerivative<DoubleType>::TetrahedronEdgePairFromEdgeModelDerivative(
        const std::string &edgemodel,
        const std::string &derivative,
        RegionPtr rp
    )
    : TetrahedronEdgeModel(edgemodel + "_node0_x:" + derivative + "@en0", rp, TetrahedronEdgeModel::DisplayType::NODISPLAY),
      edgeModelName(edgemodel),
      nodeModelName(derivative)
{

  for (size_t i = 0; i < 2; ++i)
  {
    edgeModelNames[i] = edgeModelName + ":" + nodeModelName + "@n" + std::to_string(i);
    RegisterCallback(edgeModelNames[i]);
  }

  for (size_t i = 0; i < 4; ++i)
  {
    const std::string suffix = ":" + nodeModelName + "@en" + std::to_string(i);
    for (size_t j = 0; j < 2; ++j)
    {
      const std::string prefix = edgeModelName + "_node" + std::to_string(j);
      model_names[i][j][0] = prefix + "_x" + suffix;
      model_names[i][j][1] = prefix + "_y" + suffix;
      model_names[i][j][2] = prefix + "_z" + suffix;
    }
  }

  for (size_t i = 0; i < 4; ++i)
  {
    for (size_t j = 0; j < 2; ++j)
    {
      for (size_t k = 0; k < 3; ++k)
      {
        if ((i + j + k ) != 0)
        {
          new TetrahedronEdgeSubModel<DoubleType>(model_names[i][j][k], rp, TetrahedronEdgeModel::DisplayType::NODISPLAY, this->GetSelfPtr());
        }
      }
    }
  }
}

template <typename DoubleType>
void TetrahedronEdgePairFromEdgeModelDerivative<DoubleType>::calcTetrahedronEdgeScalarValues() const
{
  const Region &reg = GetRegion();

  std::array<ConstEdgeModelPtr, 2> emp;
  for (size_t i = 0; i < 2; ++i)
  {
    emp[i] = reg.GetEdgeModel(edgeModelNames[i]);
    dsAssert(emp[i].get(), "UNEXPECTED");
  }

  const ConstTetrahedronList &tl = GetRegion().GetTetrahedronList();

  std::array<std::array<std::array<TetrahedronEdgeModelPtr, 3>, 2>, 4> temp;
  std::array<std::array<std::array<std::vector<DoubleType>, 3>, 2>, 4> ev;
  for (size_t i = 0; i < 4; ++i)
  {
    for (size_t j = 0; j < 2; ++j)
    {
      for (size_t k = 0; k < 3; ++k)
      {
        ConstTetrahedronEdgeModelPtr t = reg.GetTetrahedronEdgeModel(model_names[i][j][k]);
        dsAssert(t.get(), "UNEXPECTED");
        temp[i][j][k] = std::const_pointer_cast<TetrahedronEdgeModel, const TetrahedronEdgeModel>(t);
        ev[i][j][k].resize(6*tl.size());
      }
    }
  }

  const TetrahedronElementField<DoubleType> &efield = reg.GetTetrahedronElementField<DoubleType>();
  std::array<typename TetrahedronElementField<DoubleType>::DerivativeEdgeVectors_t, 2> v;

  for (size_t i = 0; i < tl.size(); ++i)
  {
    const Tetrahedron &tetrahedron = *tl[i];
    efield.GetTetrahedronElementFieldPairs(tetrahedron, *emp[0], *emp[1], v[0], v[1]);
    for (size_t nindex = 0; nindex < 4; ++nindex)
    {
      for (size_t eindex = 0; eindex < 6; ++eindex)
      {
        const size_t oindex = 6*i + eindex;
        for (size_t j = 0; j < 2; ++j)
        {
          const Vector<DoubleType> &vec = v[j][nindex][eindex];
          ev[nindex][j][0][oindex] = vec.Getx();
          ev[nindex][j][1][oindex] = vec.Gety();
          ev[nindex][j][2][oindex] = vec.Getz();
        }
      }
    }
  }

  for (size_t i = 0; i < 4; ++i)
  {
    for (size_t j = 0; j < 2; ++j)
    {
      for (size_t k = 0; k < 3; ++k)
      {
        temp[i][j][k]->SetValues(ev[i][j][k]);
      }
    }
  }
}

template <typename DoubleType>
void TetrahedronEdgePairFromEdgeModelDerivative<DoubleType>::Serialize(std::ostream &of) const
{
  of << "COMMAND element_pair_from_edge_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -edge_model \"" << edgeModelName << "\" -derivative \"" << nodeModelName << "\"";
}

template class TetrahedronEdgePairFromEdgeModelDerivative<double>;
#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
template class TetrahedronEdgePairFromEdgeModelDerivative<float128>;
#endif

TetrahedronEdgeModelPtr CreateTetrahedronEdgePairFromEdgeModelDerivative(
        const std::string &edgemodel,
        const std::string &derivative,
        RegionPtr rp
    )
{
  const bool use_extended = rp->UseExtendedPrecisionModels();
  return create_tetrahedron_edge_model<TetrahedronEdgePairFromEdgeModelDerivative<double>, TetrahedronEdgePairFromEdgeModelDerivative<extended_type>>(use_extended, edgemodel, derivative, rp);
}

