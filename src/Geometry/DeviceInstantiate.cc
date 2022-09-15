
template void
Device::NoiseUpdate(const std::string &output,
                    const std::vector<PermutationEntry> &permvec,
                    const std::vector<std::complex<DBLTYPE>> &result);
template void Device::RegionAssemble(dsMath::RealRowColValueVec<DBLTYPE> &m,
                                     dsMath::RHSEntryVec<DBLTYPE> &v,
                                     dsMathEnum::WhatToLoad w,
                                     dsMathEnum::TimeMode t);
template void Device::ContactAssemble(dsMath::RealRowColValueVec<DBLTYPE> &m,
                                      dsMath::RHSEntryVec<DBLTYPE> &v,
                                      PermutationMap &p,
                                      dsMathEnum::WhatToLoad w,
                                      dsMathEnum::TimeMode t);
template void Device::InterfaceAssemble(dsMath::RealRowColValueVec<DBLTYPE> &m,
                                        dsMath::RHSEntryVec<DBLTYPE> &v,
                                        PermutationMap &p,
                                        dsMathEnum::WhatToLoad w,
                                        dsMathEnum::TimeMode t);
template void Device::Update(const std::vector<DBLTYPE> &result);
template void
Device::ACUpdate(const std::vector<std::complex<DBLTYPE>> &result);
