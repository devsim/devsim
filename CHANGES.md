
### Version 1.4.6

Parameter "info" can be queried for getting version information.  The file ``testing/info.py`` contains an example.

```
python info.py
{'copyright': 'DEVSIM LLC Copyright 2009-2020', 'direct_solver': 'mkl_pardiso', 'extended_precision': True, 'version': '1.4.6'}
```

The example ``examples/diode/gmsh_diode3d_float128`` provides an example where extended precision is enabled.

Microsoft Windows 10 is supported and is now compiled using Microsoft Visual Studio 2019.

Microsoft Windows 7 is no longer supported, as Microsoft has dropped support as of January 14, 2020.

Support for reading Genius Device Simulator Meshes has been removed from the software and documentation.

### Version 1.4.5

* Platform Support:
  * An MSYS2/Mingw-w64 build is available for 64-bit Windows.  This build, labeled ``devsim_msys_v1.4.5``, enables the use of the 128-bit floating point precision already available on the macOS and Linux platforms.

### Version 1.4.4

* Bug Fixes:
  * Intermittent crash on Windows 10 at the end of the program
* CHANGES.md containing version changes in markdown format.
* Internal changes:
  * Regression system script refactored to Python.
  * Refactor threading code using C++11 functions
  * Refactor timing functions for verbose mode using C++11 functions.
  * Refactor FPE detection code to C++11 standard.

