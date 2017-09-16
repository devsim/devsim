
template void NodeModel::SetNodeValue<DBLTYPE>(size_t, DBLTYPE);
template void NodeModel::SetValues<DBLTYPE>(const NodeScalarList<DBLTYPE> &);
template void NodeModel::SetValues<DBLTYPE>(const DBLTYPE &);
template void NodeModel::SetValues<DBLTYPE>(const NodeScalarList<DBLTYPE> &) const;
template void NodeModel::SetValues<DBLTYPE>(const DBLTYPE &) const;
template const DBLTYPE &NodeModel::GetUniformValue<DBLTYPE>() const;
template const std::vector<DBLTYPE> &NodeModel::GetScalarValues<DBLTYPE>() const;


