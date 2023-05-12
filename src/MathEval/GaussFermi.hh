/***
DEVSIM
Copyright 2021 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
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
