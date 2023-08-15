/***
Copyright 2012 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/
#ifndef DEVSIM_IMPORT_HH
#define DEVSIM_IMPORT_HH
#if defined _WIN32
#define DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#define DLL_LOCAL __declspec(dllimport)
#else
#define DLL_PUBLIC __attribute__ ((visibility("default")))
#define DLL_LOCAL  __attribute__ ((visibility("hidden")))
#endif
#endif

