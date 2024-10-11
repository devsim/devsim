
# CHANGES

## Introduction

Please see the release notes in ``doc/devsim.pdf`` or at [https://devsim.net](https://devsim.net) for more detailed information about changes.

## Version 2.8.5

### VTK Writer

[#151](https://github.com/devsim/devsim/issues/151)

Use ``zlib`` from Python module instead of Anaconda ``zlib.dll`` or the system ``zlib`` for other operating systems.  The compressed binary data written to the ``.vtu`` files should be numerically the same.

### Clang build on Windows

## Version 2.8.4

### Serialization of equation command

Write ``variable_update`` when writing the ``equation`` command to the devsim file format.

### Simulation Matrix

Fix issue [#148](https://github.com/devsim/devsim/issues/148) segmentation fault in ``get_matrix_and_rhs``.  Matrix and RHS now printed in ``testing/cap2.py``.

### macOS Build

Fix issue [#149](https://github.com/devsim/devsim/issues/149) fix issue with macOS build scripts.

## Version 2.8.3

### Linux support

Due to the Red Hat 7 end of life on June 30, 2024, the minimum support level for Linux is now Red Hat 8 using the AlmaLinux 8 based [manylinux_2_28](https://github.com/pypa/manylinux?tab=readme-ov-file#manylinux_2_28-almalinux-8-based).

### Clang format

Add `.clang-format` file to provide assist automatic formatting for new source code.

### Get equation command

Fixed issue [#145](https://github.com/devsim/devsim/issues/145). `get_equation_command` now provides the `variable_update` option that was used.

### Exception propagation

Fixed issue where an internal C++ based exception, may not be caught properly on some platforms.

## Version 2.8.2

### Documentation updates

The documentation has undergone major updates.  Please see it at ``doc/devsim.pdf`` and online at [https://devsim.net](https://devsim.net).

## Version 2.8.1

### Help Files

Updated instructions.  Added additional documentation files.

| File | Purpose |
| --- | --- |
| [BUILD.md](BUILD.md) | Building from source |
| [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) | Code of conduct |
| [TEST.md](TEST.md) | Testing instructions |

and removed [install.py](install.py).

### Database Command Removal

Remove commands:

* `create_db`
* `open_db`
* `close_db`
* `save_db`
* `add_db_entry`
* `get_db_entry`

This feature was only being used in the `bioapp1` examples, and those tests have been updated.  This also removes the binary dependence on SQLite.

## Version 2.8.0

### Python Scripts

Based on a contribution by [@simbilod](https://github.com/simbilod), all of the Python scripts have been reformatted.  The build system was also updated to enforce Python script modifications are properly formatted when submitted to the project.

### Data output

#### Reduction in Data File Sizes

Based on a contribution by [@simbilod](https://github.com/simbilod) `devsim.write_devices` now supports reducing the file size of data files by specifying a callback function to reduce data usage.  This is through the `include_test` option.  This option is a function that takes a string and returns `True` if the model should be written.  It is only supported for the `tecplot` and `vtk` formats.

In this example, only the `NetDoping` field is written to the Tecplot data file.

```
devsim.write_devices(
    file="mesh2d_reduced.tec",
    type="tecplot",
    include_test=lambda x: x in ("NetDoping",),
)
```


#### FLOOPS Data File Output

The `floops` option for `devsim.write_devices` has been removed.


### Platform Support

### Windows Build Issue

During testing, it was found the Visual Studio 2022 builds were failing a test related to threading.  This was found to be a problem with version `17.10`, but not version `17.9`.  This affects the build automation, but should not affect the binary releases.

### Centos 7 End of Life

This is the last version to support Centos 7 before its end of life on June 30, 2024.  After this date we will be moving to the AlmaLinux 8 based `manylinux_2_28`.

## Version 2.7.3

Fixed issue [#133](https://github.com/devsim/devsim/issues/133) identified by [@yh-kwok](https://github.com/yh-kwok).

## Version 2.7.2

* macOS package is now Universal.  Both `x86_64` and `arm64` will use `clang` compiler and macOS 12.0 is now the minimum supported version.
* Update Boost version to 1.82
* Improvements to extended precision complex numbers implementation to work with newer compilers.
* Script to build for `manylinux_2_28` (Centos 8)
* Build Linux aarch64 from macOS system with docker

## Version 2.7.1

* Support for Linux aarch64, which can be run on Amazon AWS 64 bit instances.
* Better implicit float conversions in parameters.

## Version 2.7.0

More helpful exception information returned to python if the error is considered fatal.

## Version 2.6.5

### Bugs

Fixed issue [#123](https://github.com/devsim/devsim/issues/123) identified by [@gluek](https://github.com/gluek). This resulted in bad results on Windows and macOS in calculation of 3d mesh areas and volumes.

## Version 2.6.4

Use ``MMD_ATA`` preconditioner for SuperLU builds.

Android builds use special implementations for the Bernoulli function to prevent overflow.

``testing/pythonmesh1d.py`` demonstrates how to get mesh information using ``devsim.get_element_node_list``.

``examples/diode/tran_diode.py`` demonstrates transient diode simulation.

## Version 2.6.3

Allow python threading during long solve operations.

## Version 2.6.2

``delete_circuit``

``get_mesh_list``

## Version 2.6.1

### Bugs

Fix issue [#116](https://github.com/devsim/devsim/issues/116) where the contact current was being calculated incorrectly in transient mode.

## Version 2.6.0

### Symbolic Factorization Reuse

The Intel MKL solver will now use reuse the symbolic factorization, if the simulation matrix sparse matrix pattern has not changed after the second nonlinear solver iteration.  This reduces simulation time, but can result in numerical differences in the simulation result.  Setting the environment variable, ``DEVSIM_NEW_SYMBOLIC``, will do a new symbolic factorization for each iteration.

This behavior may be controlled by using this option in the solve command
```
solve(symbolic_iteration_limit = -1)
```
where setting the value to ``-1`` will create a new symbolic factorization for all nonlinear iterations.  Setting the value to a number greater than ``0`` will mark all iterations afterwards for reusing the previous symbolic factorization.

### Reset Simulator

The ``reset_devsim`` command will clear all simulator data, so that a program restart is not necessary.

### Build Infrastructure

#### LAPACK is Optional

When LAPACK functions are not available, it is now possible to use Eigen instead.  BLAS is still required.  It is up to the direct solver being used to determine necessary LAPACK functions.

#### Self Contained Build

The build infrastructure is being updated to support a small application build on different systems.  For these builds, some commands are removed and SuperLU is the only available solver.

#### SuperLU Solver

For self contained builds, some commands are removed and SuperLU is the only available solver.

### Citing DEVSIM

[CITATION.md](CITATION.md) has been updated with recent articles written about the simulator.

## Version 2.5.0

UMFPACK 5.1 is the new default when the Intel MKL is not available, making this the default for the macOS arm64 platform.

SuperLU is removed and no longer available as a solver.

Regression scripts were passing when there were numerical differences in the data diff comparison.  This is now corrected and the regression results have been updated on all platforms.

For those building the software, the ``EXPLICIT_MATH_LOAD`` CMAKE option has been removed, so that the software is not directly linked to any math library.

The license text for the Apache 2.0 license has been replaced with the SPDX format of the license string.  This ensures that the text can be used in a consistent manner across all source files.

## Version 2.4.0

### Determine Loaded Math Libraries

To determine the loaded math libraries, use
```
devsim.get_parameter(name='info')['math_libraries']
```

### UMFPACK 5.1 Solver

The ``UMFPACK`` 5.1 solver is now available as a shared library distributed with the software.  It is licensed under the terms of the LGPL 2.1 and our version is hosted here:

[https://github.com/devsim/umfpack_lgpl](https://github.com/devsim/umfpack_lgpl)

Please note that this version uses a scheme to provide the needed math library functions when the library is loaded.

In order to use this library, a shim script is provided to load UMFPACK and set it as the solver.  Please see this example:
```
python -mdevsim.umfpack.umfshim ssac_cap.py
```

### Direct Solver Callback

It is now possible to setup call a custom direct solver.  The direct solver is called from Python and the callback is implemented by setting these parameters:
```
devsim.set_parameter(name="direct_solver", value="custom")
devsim.set_parameter(name="solver_callback", value=local_solver_callback)
```

Where the first parameter enables the use of the second parameter to set a callback function.  Please see the ``testing/umfpack_shim.py`` for a sample implementation using UMFPACK 5.1.

### Apple M1

On this platform, the software does not check for floating point exceptions (FPEs) during usage of the direct solver.  During testing, it was discovered that FPEs were occuring during factorization for both the ``SuperLU`` and the ``UMFPACK``.  Removing this check allows more of the tests to run through to completion.

### Bugs

Fix issue [#104](https://github.com/devsim/devsim/issues/104) where the 2D MOSFET example was not fully connected across region interfaces.
```
testing/mos_2d.py
testing/mos_2d_restart.py
testing/mos_2d_restart2.py
```
This was resulting in an FPE during testing on macOS M1.

## Version 2.3.8

### Bugs

[@ryan3141](https://github.com/ryan3141) fixed an issue where math functions added with ``devsim.register_function`` were not available in extended precision model evaluation.  The ``testing/testfunc_extended.py`` test is added to validate the fix.

Update NOTICE with the license files from the various dependencies.

## Version 2.3.7

### Apple M1 Support

Intel MKL Pardiso not available, so using system BLAS/LAPACK or openblas by default.  In addition, SuperLU, is used instead of the MKL Pardiso.  This results in some test failures, based on the use of a different solver, and not the OS architecture.

Extended precision is enabled.

Enabled by running pip install.

The regression results are in this newly created repo:

* [devsim_tests_macos_arm64](https://github.com/devsim/devsim_tests_macos_arm64)


### Python Notebook Example With 3D Visualization


A plotting example using ``pyvista`` is presented in ``examples/plotting/visualization.ipynb``.  This example was provided by [@simbilod](https://github.com/simbilod).

### Bugs

When instantiating a mesh from Gmsh, contact and interface related errors to dimensionality have an improved error message.


## Version 2.3.6

On Windows the ``DEVSIM_MATH_LIBS`` now uses the ``;`` as the path separator, while macOS and Linux still use ``:``.

The math library search order is then:

* The math libraries listed in the ``DEVSIM_MATH_LIBS`` environment variable, with the appropriate separator.
* The Intel Math Kernel Library
* These dynamic libraries
  * OpenBLAS (e.g. libopenblas.so)
  * LAPACK (e.g. liblapack.so)
  * BLAS (e.g. libblas.so)

All platforms will search for the Intel MKL by trying several version numbers.  When the Intel MKL is not available, the direct solver will switch from Intel MKL Pardiso to SuperLU.

On macOS and Linux, the RPATH has been modified to look in places relative to the `devsim` module, instead of using ``CONDA_PREFIX`` or ``VIRTUAL_ENV``.

* ``macOS`` : ``@loader_path;@loader_path/../lib;@loader_path/../../../../lib;@executable_path/../lib``
* ``Linux`` : ``$ORIGIN:$ORIGIN/../lib:$ORIGIN/../../../../lib``

## Version 2.3.1

### Python PIP Package

DEVSIM is now available on pypi for macOS, Linux, and Microsoft Windows.  To install this package for your platform:

```
pip install devsim

```

Users of Anaconda Python should install the Intel MKL using
```
conda install mkl
```

and may find simulation files and documentation in ``$CONDA_PREFIX/devsim_data``.

Users of other Python distributions may install this dependency using:
```
pip install mkl
```

and may find simulation files and documentation in ``$VIRTUAL_ENV/devsim_data`` when using virtual environments.


### Remove Windows MSYS Build

The MSYS build is removed as an available binary package.  Windows is still supported through the use of the Visual C++ compiler.

### Build Notes

The compiler for the Linux build are now upgraded to ``devtoolset-10`` and is now built on ``manylinux2014``.

Boost is now added as a submodule, instead of using system libraries or Anaconda Python versions.  The Linux build no longer requires Anaconda Python.

## Version 2.2.0

It is possible to delete devices using the ``delete_device`` command.  Meshes used to instantiate devices may be deleted using the ``delete_mesh`` command.  Parameters set on a device and its regions are also cleared from the parameter database.

Extended precision is now available on Windows builds using the Visual Studio Compiler.  Note that this precision is not as accurate as the float128 type used on other systems.

Upgraded to SuperLU 5.3 from SuperLU 4.3.

Fixed defects found in Coverity scanning.

## Version 2.1.0

### Explicit math library loading

#### Introduction

Since the Intel Math Kernel Library started versioning the names of their dynamic link libraries, it has been difficult to maintain a proper Anaconda Python environment when the version has been updated.  With this release, it is possible to use any recent version of the Intel MKL.  In addition, the user is able to load alternative BLAS/LAPACK math libraries.

#### Intel MKL

From DEVSIM Version 2.1.0 onward, a specific version is not required when loading the Intel MKL.  If the Intel MKL is not found, the import of the ``devsim`` module will fail, and an error message will be printed.  This method is the default, and should work when using an Anaconda Python environment with the ``mkl`` package installed.

When using a different Python distribution, or having an installation in a different place, it is possible to specify the location by modifying the ``LD_LIBRARY_PATH`` environment variable on Linux, or using ``DYLD_LIBRARY_PATH`` on macOS.  The explicit path may be set to the MKL math libraries may be set using the method in the next section.

#### Loading other math libraries

It is possible to load alternative implementations of the BLAS/LAPACK used by the software.  The ``DEVSIM_MATH_LIBS`` environment variable may be used to set a ``:`` separated list of libraries.  These names may be based on relative or absolute paths.  The program will load the libraries in order, and stop when all of the necessary math symbols are supplied.  If symbols for the Intel MKL are detected, then the Pardiso direct solver will be enabled.

#### New CMAKE build option
For those building the software, the ``EXPLICIT_MATH_LOAD`` CMAKE option has been added to control the new explicit math loading feature.  An important benefit of this option is that it is possible to build a release version of the software, even if the Intel MKL has not been installed on the build computer.

### Direct solver selection

The direct solver may be selected by using the ``direct_solver`` parameter.
```
devsim.set_parameter(name='direct_solver', value='mkl_pardiso')
```

The following options are available:
- ``mkl_pardiso`` Intel MKL Pardiso
- ``superlu`` SuperLU 4.3

The default is ``mkl_pardiso`` when the Intel MKL is loaded.  Otherwise, the default will switch to ``superlu``.

### Kahan summation in extended precision mode

The ``kahan3`` and ``kahan4`` functions are now using the Kahan summation algorithm for extended precision model evaluation.  Previously, this algorithm was replaced with 128-bit floating point addition and subtraction in releases that support extended precision mode.  With this change, better than 128-bit floating precision is available when extended precision is enabled.
```
devsim.set_parameter(name = "extended_model", value=True)
```

The ``testing/kahan_float128.py`` test has been added.

### Visual Studio 2022

The Microsoft Windows``win64`` release version is now built using the Visual Studio 2022 compiler.  For users needing extended precision on the Windows platform, the ``msys`` build is recommended.

## Version 2.0.1

### Update documentation files

The following files were updated in the text documentation distributed with the software.
- ``CONTRIBUTING.md``
- ``INSTALL.md``
- ``README.md``

This was done to create a version to coincide with this paper in the Journal of Open Source Software.

Sanchez, J. E., (2022). DEVSIM: A TCAD Semiconductor Device Simulator. Journal of Open Source Software, 7(70), 3898, [https://doi.org/10.21105/joss.03898](https://doi.org/10.21105/joss.03898)

### Update MKL Version

The release version of this software is build against version 2 of the Intel MKL, which corresponds to the latest version of Anaconda Python.  If you have issues running ``DEVSIM`` with this new version, please contact us at [https://forum.devsim.org](https://forum.devsim.org) for assistance.

### Update SYMDIFF reference

The submodule reference to ``SYMDIFF`` was updated due some changes to its build files.  Some ``SYMDIFF`` build scripts in the ``DEVSIM`` repository were also updated.

## Version 2.0.0

### Versioned MKL DLL in release build

The Intel Math Kernel Library now uses versioned library names.  Binary releases are now updated against the latest versioned dll names from MKL available in the Anaconda Python distribution.

### Fixed issue in ramp function.

The ``rampbias`` function in the ``devsim.python_packages.ramp`` module has been fixed to properly reduce the bias when there is a convergence failure.

### Transient Simulation

Fixed bug with ``transient_tr`` (trapezoidal) time integration method where the wrong sign was used to integrate previous time steps.

Fixed bug in the charge error calculation, which calculates the simulation result with that a forward difference projection.

Added ``testing/transient_rc.py`` test which compares simulation with analytic result for RC circuit.

Added ``set_initial_condition`` command to provide initial transient conditions based on current solution.

### Create interface from node pairs

Added ``create_interface_from_nodes`` to make it possible to add interface from non-coincident pairs of nodes.

### Solver

#### Convergence Tests

The ``maximum_error`` and ``maximum_divergence`` options where added to the ``solve`` command.  If the absolute error of any iteration goes above ``maximum_error``, the simulation stops with a convergence failure.  The ``maximum_divergence`` is the maximum number of iterations that the simulator error may increase before stopping.

#### Verbosity

During the ``solve``, circuit node and circuit solution information is no longer printed to the screen for the default verbosity level.  In addition, the number of equations per device and region is no longer displayed at the start of the first iteration.

#### SuperLU

The code now supports newer versions of ``SuperLU``.  The release version is still using SuperLU 4.3 for the iterative solution method, and the Intel MKL Pardiso for the direct solve method.

#### Simulation Matrix

The ``get_matrix_and_rhs`` command was not properly accepting the ``format`` parameter, and was always returning the same type.

### Build Scripts

The build scripts have been updated on all platforms to be less dependent on specific Python 3 versions.

An updated fedora build script has been added.  It uses the system installed ``SuperLU`` as the direct solver.

### Documentation Files

Some out of date files (e.g. RELEASE, INSTALL, . . .) have been removed.  The [README.md](README.md) has been updated and the [INSTALL.md](INSTALL.md) have been updated.

### Command Options

The ``variable_name`` option is no longer recognized for the ``devsim.contact_equation`` and ``devsim.interface_equation`` as it was not being used.

## Version 1.6.0

### Array Type Input and Output

In most circumstances, the software now returns numerical data using the Python ``array`` class.  This is more efficient than using standard lists, as it encapsulates a contiguous block of memory.  More information about this class can be found at [https://docs.python.org/3/library/array.html](https://docs.python.org/3/library/array.html).  The representation can be easily converted to lists and ``numpy`` arrays for efficient manipulation.

When accepting user input involving lists of homogenous data, such as ``set_node_values`` the user may enter data using either a list, string of bytes, or the ``array`` class.  It may also be used to input ``numpy`` arrays or any other class with a ``tobytes`` method.

### Get Matrix and RHS for External Use

The ``get_matrix_and_rhs`` command has been added to assemble the static and dynamic matrices, as well as their right hand sides, based on the current state of the device being simulated.  The ``format`` option is used to specify the sparse matrix format, which may be either in the compressed column or compressed row formats, ``csc`` or ``csr``.

### Maximum Divergence Count

If the Newton iteration errors keep increasing for 20 iterations in a row, then the simulator stops.  This limit was previously 5.

### Mesh Visualization Element Orientation

Elements written to the ``tecplot`` format in 2d and 3d have node orderings compatible with the element connectivity in visualization formats.  Specifying the ``reorder=True`` option in ``get_element_node_list`` will result in node ordering compatible with meshing and visualization software.

## Version 1.5.1

### Math Functions

The following inverse functions and their derivatives are now available in the model interpreter.
- ``erf_inv`` Inverse Error Function
- ``erfc_inv`` Inverse Complimentary Error Function
- ``derf_invdx`` Derivative of Inverse Error Function
- ``derfc_invdx`` Derivative of Complimentary Inverse Error Function

The Gauss-Fermi Integral, using Paasch's equations are now implemented.
- ``gfi`` Gauss-Fermi Integral
- ``dgfidx`` Derivative of Gauss-Fermi Integral
- ``igfi`` Inverse Gauss-Fermi Integral
- ``digfidx`` Derivative of Inverse Gauss-Fermi Integral

Each of these functions take two arguments, ``zeta`` and ``s``.  The derivatives with respect to the first argument are provided.  Please see ``testing/GaussFermi.py`` for an example.

In extended precision mode, the following functions are now evaluated with full extended precision.
- ``Fermi``
- ``dFermidx``
- ``InvFermi``
- ``dInvFermidx``

The following double precision tests:

- ``testing/Fermi1.py`` Fermi Integral Test
- ``testing/GaussFermi.py`` Gauss Fermi Integral Test

Have extended precision variants:

- ``testing/Fermi1_float128.py``
- ``testing/GaussFermi_float128.py``

### Installation Script

A new installation script is in the base directory of the package.
It provides instructions of completing the installation to the ``python`` environment without having to set the ``PYTHONPATH`` environment variable.
It notifies the user of missing components to finish the installation within an ``Anaconda`` or ``Miniconda`` environment.


To use the script, use the following command inside of the ``devsim`` directory.

```
    python install.py
```

The install script will write a file named ``lib/setup.py``, which can be used to complete the installation using ``pip``.  The script provides instructions for the installation and deinstallation of ``devsim``.

```
    INFO: Writing setup.py
    INFO:
    INFO: Please type the following command to install devsim:
    INFO: pip install -e lib
    INFO:
    INFO: To remove the file, type:
    INFO: pip uninstall devsim
```

## Version 1.5.0

The ``custom_equation`` command has been modified to require a third return value.  This boolean value denotes whether the matrix entries should be row permutated or not.  For the bulk equations this value should be ``True``.  For interface and contact boundary conditions, this value should be ``False``.

It is now possible to replace an existing ``custom_equation``.

The file ``examples/diode/diode_1d_custom.py`` demonstrates custom matrix assembly and can be directly compared to ``examples/diode/diode_1d.py``.

The ``EdgeNodeVolume`` model is now available for the volume contained by an edge.

The ``contact_equation`` command now accepts 3 additional arguments.

- ``edge_volume_model``
- ``volume_node0_model``
- ``volume_node1_model``

These options provide the ability to do volume integration on contact nodes.

The ``equation`` command has replaced the ``volume_model`` option with:

- ``volume_node0_model``
- ``volume_node1_model``

so that nodal quantities can be more localized.

More details are in the manual.

## Version 1.4.14
### Platforms

Windows 32 bit is no longer supported.  Binary releases of the ``Visual Studio 2019`` ``MSYS2/Mingw-w64`` 64-bit builds are still available online.

On Linux, the releases are now on Centos 7, as Centos 6 has reached its end of life on November 30, 2020.

### C++ Standard

The C++ standard has been raised to C++17.

## Version 1.4.13

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


## Version 1.4.12

Element assembly for calculation of current and charges from the device into the circuit equation are fixed.  These tests are added:

- ``testing/cap_2d_edge.py``
- ``testing/cap_2d_element.py``
- ``testing/cap_3d_edge.py``
- ``testing/cap_3d_element.py``

The ``edge`` variant is using standard edge based assembly, and the ``element`` variant is using element-based assembly.

## Version 1.4.11

The ``element_pair_from_edge_model`` is available to calculate element edge components averaged onto each node of the element edge.  This makes it possible to create an edge weighting scheme different from those used in ``element_from_edge_model``.

Fixed issue where command option names where not always shown in the documentation.

The platform specific notes now clarify that any version of Python 3 (3.6 or higher) is supported.

- ``linux.txt``
- ``windows.txt``
- ``macos.txt``

## Version 1.4.10

Fixed crash when evaluating element edge model in 3D.

Fixed potential error using ``delete_node_model`` and similar deletion commands.

## Version 1.4.9

Support for loading mesh files containing element edge data.

## Version 1.4.8

In transient mode, the convergence test was flawed so that the ``charge_error`` was the only convergence check required for convergence.  The software now ensures all convergence criteria are met.

## Version 1.4.7

### Models

In the simple physics models, the sign for time-derivative terms was wrong for the electron and hole continuity equations.  This affects small-signal and noise simulations.  The example at ``examples/diode/ssac_diode.py`` was updated to reflect the change.

### Platforms

Fix build script issue for macOS on Travis CI, updated the compiler to ``g++-9``.

Update Centos 6 build from ``devtoolset-6`` to ``devtoolset-8``.

## Version 1.4.6

### Version Information

Parameter ``info`` can be queried for getting version information.  The file ``testing/info.py`` contains an example.

```
  python info.py
  {'copyright': 'Copyright © 2009-2020 DEVSIM LLC', 'direct_solver': 'mkl_pardiso', 'extended_precision': True, 'license': 'Apache License, Version 2.0', 'version': '1.4.6', 'website': 'https://devsim.org'}
```

### Extended Precision

The example ``examples/diode/gmsh_diode3d_float128.py`` provides an example where extended precision is enabled.

### Python Formatting

The Python scripts in the ``examples`` and ``testing`` directories have been reformatted to be more consistent with language standards.

### Platforms

Microsoft Windows 10 is supported and is now compiled using Microsoft Visual Studio 2019.

Microsoft Windows 7 is no longer supported, as Microsoft has dropped support as of January 14, 2020.

### External Meshing

Support for reading meshes from Genius Device Simulator has been completely removed from DEVSIM.


## Version 1.4.5

* Platform Support:
  * An MSYS2/Mingw-w64 build is available for 64-bit Windows.  This build, labeled ``devsim_msys_v1.4.5``, enables the use of the 128-bit floating point precision already available on the macOS and Linux platforms.

## Version 1.4.4

* Bug Fixes:
  * Intermittent crash on Windows 10 at the end of the program
* CHANGES.md containing version changes in markdown format.
* Internal changes:
  * Regression system script refactored to Python.
  * Refactor threading code using C++11 functions
  * Refactor timing functions for verbose mode using C++11 functions.
  * Refactor FPE detection code to C++11 standard.

