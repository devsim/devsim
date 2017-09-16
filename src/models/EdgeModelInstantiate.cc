
template void EdgeModel::SetValues<DBLTYPE>(std::vector<DBLTYPE> const&);
template void EdgeModel::SetValues<DBLTYPE>(DBLTYPE const&);
template std::vector<DBLTYPE> const& EdgeModel::GetScalarValues<DBLTYPE>() const;
template const DBLTYPE &EdgeModel::GetUniformValue<DBLTYPE>() const;
template std::vector<DBLTYPE> EdgeModel::GetScalarValuesOnNodes<DBLTYPE>() const;
template std::vector<Vector<DBLTYPE>, std::allocator<Vector<DBLTYPE> > > EdgeModel::GetVectorValuesOnNodes<DBLTYPE>() const;
template void EdgeModel::SetValues<DBLTYPE>(std::vector<DBLTYPE> const&) const;
template void EdgeModel::SetValues<DBLTYPE>(DBLTYPE const&) const;

