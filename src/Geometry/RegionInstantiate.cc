
#if 0
// see actual implementations in Region.cc
template ModelExprDataCachePtr<DBLTYPE> Region::GetModelExprDataCache();
template void Region::SetModelExprDataCache(ModelExprDataCachePtr<DBLTYPE>);
#endif

template void Region::Update(const std::vector<DBLTYPE> &result);
template void Region::ACUpdate<DBLTYPE>(const dsMath::ComplexDoubleVec_t<DBLTYPE> &result);
template void Region::NoiseUpdate<DBLTYPE>(const std::string &output, const std::vector<PermutationEntry> &permvec, const dsMath::ComplexDoubleVec_t<DBLTYPE> &result);
template void Region::Assemble(dsMath::RealRowColValueVec<DBLTYPE> &m, dsMath::RHSEntryVec<DBLTYPE> &v, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t);
template const TriangleElementField<DBLTYPE> &Region::GetTriangleElementField<DBLTYPE>() const;
template const TetrahedronElementField<DBLTYPE> &Region::GetTetrahedronElementField<DBLTYPE>() const;
template const std::vector<Vector<DBLTYPE> > &Region::GetTriangleCenters() const;
template const std::vector<Vector<DBLTYPE> > &Region::GetTetrahedronCenters() const;
template const GradientField<DBLTYPE> &Region::GetGradientField() const;

