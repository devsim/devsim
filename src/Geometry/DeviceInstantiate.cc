
template void Device::NoiseUpdate<DBLTYPE>(const std::string &output, const std::vector<PermutationEntry> &permvec, const dsMath::ComplexDoubleVec_t<DBLTYPE> &result);
template void Device::RegionAssemble(dsMath::RealRowColValueVec<DBLTYPE> &m, dsMath::RHSEntryVec<DBLTYPE> &v, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t);
template void Device::ContactAssemble(dsMath::RealRowColValueVec<DBLTYPE> &m, dsMath::RHSEntryVec<DBLTYPE> &v, PermutationMap &p, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t);
template void Device::InterfaceAssemble(dsMath::RealRowColValueVec<DBLTYPE> &m, dsMath::RHSEntryVec<DBLTYPE> &v, PermutationMap &p, dsMathEnum::WhatToLoad w, dsMathEnum::TimeMode t);
template void Device::Update(const std::vector<DBLTYPE> &result);
template void Device::ACUpdate<DBLTYPE>(const dsMath::ComplexDoubleVec_t<DBLTYPE> &result);
