### Version 1.4.14
#### Platforms

Windows 32 bit is no longer supported.  Binary releases of the ``Visual Studio 2019`` ``MSYS2/Mingw-w64`` 64-bit builds are still available online.

On Linux, the releases are now on Centos 7, as Centos 6 has reached its end of life on November 30, 2020.

#### C++ Standard

The C++ standard has been raised to C++17.

### Version 1.4.13

The node indexes with the maximum error for each equation will be printed when ``debug_level`` is ``verbose``.

```
devsim.set_parameter(name="debug_level", value="verbose")
```

These are printed as ``RelErrorNode`` and ``AbsErrorNode``:

```
    Region: "gate"	RelError: 5.21531e-14	AbsError: 4.91520e+04
      Equation: "ElectronContinuityEquation"	RelError: 4.91520e-16	AbsError: 4.91520e+04
	RelErrorNode: 129	AbsErrorNode: 129
```

This information is also returned when using the ``info=True`` option on the ``solve`` command for each equation on each region of a device.

If the ``info`` flag is set to ``True`` on the ``solve`` command, the iteration information will be returned, and an exception for convergence will no longer be thrown.  It is the responsibility of the caller to test the result of the ``solve`` command to see if the simulation converged.  Other types of exceptions, such as floating point errors, will still result in a Python exception that needs to be caught.


### Version 1.4.12

Element assembly for calculation of current and charges from the device into the circuit equation are fixed.  These tests are added:

- ``testing/cap_2d_edge.py``
- ``testing/cap_2d_element.py``
- ``testing/cap_3d_edge.py``
- ``testing/cap_3d_element.py``

The ``edge`` variant is using standard edge based assembly, and the ``element`` variant is using element-based assembly.

### Version 1.4.11

The ``element_pair_from_edge_model`` is available to calculate element edge components averaged onto each node of the element edge.  This makes it possible to create an edge weighting scheme different from those used in ``element_from_edge_model``.

Fixed issue where command option names where not always shown in the documentation.

The platform specific notes now clarify that any version of Python 3 (3.6 or higher) is supported.

- ``linux.txt``
- ``windows.txt``
- ``macos.txt``

### Version 1.4.10

Fixed crash when evaluating element edge model in 3D.

Fixed potential error using ``delete_node_model`` and similar deletion commands.

### Version 1.4.9

Support for loading mesh files containing element edge data.

### Version 1.4.8

In transient mode, the convergence test was flawed so that the ``charge_error`` was the only convergence check required for convergence.  The software now ensures all convergence criteria are met.

### Version 1.4.7

#### Models

In the simple physics models, the sign for time-derivative terms was wrong for the electron and hole continuity equations.  This affects small-signal and noise simulations.  The example at ``examples/diode/ssac_diode.py`` was updated to reflect the change.

#### Platforms

Fix build script issue for macOS on Travis CI, updated the compiler to ``g++-9``.

Update Centos 6 build from ``devtoolset-6`` to ``devtoolset-8``.

### Version 1.4.6

#### Version Information

Parameter ``info`` can be queried for getting version information.  The file ``testing/info.py`` contains an example.

```
  python info.py
  {'copyright': 'Copyright © 2009-2020 DEVSIM LLC', 'direct_solver': 'mkl_pardiso', 'extended_precision': True, 'license': 'Apache License, Version 2.0', 'version': '1.4.6', 'website': 'https://devsim.org'}
```

#### Extended Precision

The example ``examples/diode/gmsh_diode3d_float128.py`` provides an example where extended precision is enabled.

#### Python Formatting

The Python scripts in the ``examples`` and ``testing`` directories have been reformatted to be more consistent with language standards.

#### Platforms

Microsoft Windows 10 is supported and is now compiled using Microsoft Visual Studio 2019.

Microsoft Windows 7 is no longer supported, as Microsoft has dropped support as of January 14, 2020.

#### External Meshing

Support for reading meshes from Genius Device Simulator has been completely removed from DEVSIM.


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

