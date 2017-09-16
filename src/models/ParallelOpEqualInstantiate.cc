
template class OpEqualPacket<SerialVectorVectorOpEqual<ScalarDataHelper::times_equal<DBLTYPE>, DBLTYPE> >;
template class OpEqualPacket<SerialVectorVectorOpEqual<ScalarDataHelper::plus_equal<DBLTYPE>, DBLTYPE> >;

template
void OpEqualRun<SerialVectorVectorOpEqual<ScalarDataHelper::plus_equal<DBLTYPE>, DBLTYPE> >(SerialVectorVectorOpEqual<ScalarDataHelper::plus_equal<DBLTYPE>, DBLTYPE>&, size_t);

template
void OpEqualRun<SerialVectorVectorOpEqual<ScalarDataHelper::times_equal<DBLTYPE>, DBLTYPE> >(SerialVectorVectorOpEqual<ScalarDataHelper::times_equal<DBLTYPE>, DBLTYPE>&, size_t);

template class OpEqualPacket<SerialVectorScalarOpEqual<ScalarDataHelper::times_equal<DBLTYPE>, DBLTYPE> >;
template class OpEqualPacket<SerialVectorScalarOpEqual<ScalarDataHelper::plus_equal<DBLTYPE>, DBLTYPE> >;

template
void OpEqualRun<SerialVectorScalarOpEqual<ScalarDataHelper::plus_equal<DBLTYPE>, DBLTYPE> >(SerialVectorScalarOpEqual<ScalarDataHelper::plus_equal<DBLTYPE>, DBLTYPE>&, size_t);

template
void OpEqualRun<SerialVectorScalarOpEqual<ScalarDataHelper::times_equal<DBLTYPE>, DBLTYPE> >(SerialVectorScalarOpEqual<ScalarDataHelper::times_equal<DBLTYPE>, DBLTYPE>&, size_t);

