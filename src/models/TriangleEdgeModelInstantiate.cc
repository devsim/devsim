
template void TriangleEdgeModel::SetValues<DBLTYPE>(std::vector<DBLTYPE> const&);
template void TriangleEdgeModel::SetValues<DBLTYPE>(DBLTYPE const&);
template std::vector<DBLTYPE> const& TriangleEdgeModel::GetScalarValues<DBLTYPE>() const;
template const DBLTYPE &TriangleEdgeModel::GetUniformValue<DBLTYPE>() const;
template std::vector<DBLTYPE> TriangleEdgeModel::GetValuesOnEdges<DBLTYPE>() const;
template void TriangleEdgeModel::GetScalarValuesOnNodes<DBLTYPE>(TriangleEdgeModel::InterpolationType, std::vector<DBLTYPE>&) const;
template void TriangleEdgeModel::GetScalarValuesOnElements<DBLTYPE>(std::vector<DBLTYPE>&) const;
template void TriangleEdgeModel::SetValues<DBLTYPE>(std::vector<DBLTYPE> const&) const;
template void TriangleEdgeModel::SetValues<DBLTYPE>(DBLTYPE const&) const;

