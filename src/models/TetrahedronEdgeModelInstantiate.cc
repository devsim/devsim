
template void TetrahedronEdgeModel::SetValues<DBLTYPE>(std::vector<DBLTYPE> const&);
template void TetrahedronEdgeModel::SetValues<DBLTYPE>(DBLTYPE const&);
template std::vector<DBLTYPE> const& TetrahedronEdgeModel::GetScalarValues<DBLTYPE>() const;
template const DBLTYPE &TetrahedronEdgeModel::GetUniformValue<DBLTYPE>() const;
template std::vector<DBLTYPE> TetrahedronEdgeModel::GetValuesOnEdges<DBLTYPE>() const;
template void TetrahedronEdgeModel::GetScalarValuesOnNodes<DBLTYPE>(TetrahedronEdgeModel::InterpolationType, std::vector<DBLTYPE>&) const;
template void TetrahedronEdgeModel::GetScalarValuesOnElements<DBLTYPE>(std::vector<DBLTYPE>&) const;
template void TetrahedronEdgeModel::SetValues<DBLTYPE>(std::vector<DBLTYPE> const&) const;
template void TetrahedronEdgeModel::SetValues<DBLTYPE>(DBLTYPE const&) const;

