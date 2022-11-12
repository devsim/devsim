/***
DEVSIM
Copyright 2021 DEVSIM LLC

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
#ifndef GAUSS_FERMI_HH
#define GAUSS_FERMI_HH
template <typename T>
T gfi(T zeta, T s);

template <typename T>
T dgfidx(T zeta, T s);

template <typename T>
T igfi(T g, T s);

template <typename T>
T digfidx(T g, T s);
#endif
