/***
DEVSIM
Copyright 2013 Devsim LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
***/

#include "Mesh2dStructs.hh"
namespace dsMesh {

MeshLine2d::MeshLine2d(double p, double ps, double ns)
    : position(p), posSpacing(ps), negSpacing(ns) {}

MeshRegion2d::MeshRegion2d(const std::string &r, const std::string &m)
    : name_(r), material_(m) {}

MeshRegion2d::MeshRegion2d() {}

void MeshRegion2d::AddBoundingBox(const MeshRegion2d &mr) {
  for (BoundingBoxList_t::const_iterator it = mr.bboxes_.begin();
       it != mr.bboxes_.end(); ++it) {
    bboxes_.push_back(*it);
  }
}

void MeshInterface2d::AddBoundingBox(const MeshInterface2d &mr) {
  for (BoundingBoxList_t::const_iterator it = mr.bboxes_.begin();
       it != mr.bboxes_.end(); ++it) {
    bboxes_.push_back(*it);
  }
}

void MeshContact2d::AddBoundingBox(const MeshContact2d &mr) {
  for (BoundingBoxList_t::const_iterator it = mr.bboxes_.begin();
       it != mr.bboxes_.end(); ++it) {
    bboxes_.push_back(*it);
  }
}

namespace {
bool InsideBoundingBox(const dsMesh::BoundingBoxList_t &bblist, double x,
                       double y) {
  bool ret = false;
  for (dsMesh::BoundingBoxList_t::const_iterator it = bblist.begin();
       it != bblist.end(); ++it) {
    if (it->IsPointInside(x, y)) {
      ret = true;
      break;
    }
  }
  return ret;
}
} // namespace

bool MeshRegion2d::IsPointInside(double x, double y) const {
  return InsideBoundingBox(bboxes_, x, y);
}
bool MeshContact2d::IsPointInside(double x, double y) const {
  return InsideBoundingBox(bboxes_, x, y);
}
bool MeshInterface2d::IsPointInside(double x, double y) const {
  return InsideBoundingBox(bboxes_, x, y);
}

#if 0
bool MeshRegion2d::IsCoordinateInside(const MeshCoordinate &mc) const
{
    return IsPointInside(mc.GetX(), mc.GetY());
}
#endif

MeshInterface2d::MeshInterface2d(const std::string &i, const std::string &r0,
                                 const std::string &r1)
    : name(i), Region0(r0), Region1(r1) {}

MeshContact2d::MeshContact2d(const std::string &c, const std::string &m,
                             const std::string &r)
    : name(c), material(m), Region(r) {}

} // namespace dsMesh
