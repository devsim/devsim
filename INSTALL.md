# DEVSIM Installation

## Introduction

Please visit the DEVSIM homepage at [https://devsim.org](https://devsim.org) for the latest information concerning the availability of the software.

## Installing the Binary Package

### Prerequisites

#### Windows

This runtime may be necessary to run the software.

*  [Visual C++ v14 Redistributable](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170#visual-c-v14-redistributable)

### Install Python

Python 3.9 or higher versions will work.  Python 3.15t or higher is needed for the free threading version.  Anaconda and Miniconda distributions are supported.

### Install a Math Library

#### Intel MKL

The Intel MKL is the preferred option for ``x86_64`` bit systems.

From a system command prompt, with an active Python environment, install the MKL.

    conda install mkl numpy

or alternatively,

    pip install mkl numpy

Please note it is not recommended to mix ``pip`` and Anaconda versions of these packages.

#### Other Math Libraries

The Intel MKL is not available on ``arm64`` or ``aarch64`` systems.  On macOS, the system math libraries will work.  For Linux systems, the OpenBLAS or other math libraries providing the BLAS/LAPACK routines may be installed and should work.  An environment variable can be used to set the name of other libraries providing these symbols.

```
DEVSIM_MATH_LIBS="libopenblas.dylib:liblapack.dylib:libblas.dylib"
```

When a system library is not readily available, like on Windows, it is possible to install this package:

```
pip install scipy_openblas32
```

and can be loaded using these environment variables, as set in ``testing/other_blas.py``.

However, this particular library is fragile and makes no guarantees about future naming compatibility for the symbols.  The environment variables would need to be adapted in the case that the symbol naming convention has changed.

### Install DEVSIM

#### Python Package Download Using pip

It is now possible to download and install ``devsim`` using ``pip``.
```
pip install devsim
```

#### Alternative Package Download

Binary releases are available from [Zenodo](https://doi.org/10.5281/zenodo.1186952), or from [GitHub](https://github.com/devsim/devsim/releases).  Uncompress the file to the desired directory.  The preferred installation method is to install the `.whl` file using `pip`.

#### Basic Test

You should then be able to run an example in the ``testing`` directory.

    cd testing
    python cap2.py

If using the ``pip`` installed version, the data files should be available in your environment in the `devsim_data` directory.

### Troubleshooting

If you have any issues with this process.  Please contact us through the DEVSIM forum at [https://forum.devsim.org](https://forum.devsim.org).

## Building from Source

Please see [BUILD.md](BUILD.md) for instructions for building from source.

