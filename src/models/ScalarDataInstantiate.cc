

//// Manual Template Instantiation
template class ScalarData<SCMODELTYPE, SCDBLTYPE>;

template class ScalarData<SCMODELTYPE, SCDBLTYPE>& SCCLASSNAME<SCDBLTYPE>::op_equal_scalar<ScalarDataHelper::times_equal<SCDBLTYPE>>(const SCDBLTYPE &, const ScalarDataHelper::times_equal<SCDBLTYPE> &);

template class ScalarData<SCMODELTYPE, SCDBLTYPE>& SCCLASSNAME<SCDBLTYPE>::op_equal_data<ScalarDataHelper::times_equal<SCDBLTYPE>>(const SCCLASSNAME<SCDBLTYPE> &, const ScalarDataHelper::times_equal<SCDBLTYPE> &);

template class ScalarData<SCMODELTYPE, SCDBLTYPE>& SCCLASSNAME<SCDBLTYPE>::op_equal_scalar<ScalarDataHelper::plus_equal<SCDBLTYPE>>(const SCDBLTYPE &, const ScalarDataHelper::plus_equal<SCDBLTYPE> &);

template class ScalarData<SCMODELTYPE, SCDBLTYPE>& SCCLASSNAME<SCDBLTYPE>::op_equal_data<ScalarDataHelper::plus_equal<SCDBLTYPE>>(const SCCLASSNAME<SCDBLTYPE> &, const ScalarDataHelper::plus_equal<SCDBLTYPE> &);

