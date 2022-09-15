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

#include "Coordinate.hh"
#include "Vector.hh"

#include <cstddef>

using namespace std;

Coordinate::Coordinate(double x, double y, double z)
    : position(x, y, z), index(0) {}

const Vector<double> &Coordinate::Position() const { return position; }
