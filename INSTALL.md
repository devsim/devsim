# DEVSIM Installation

## Introduction

Please visit the DEVSIM homepage at [https://devsim.org](https://devsim.org) for the latest information concerning the availability of the software.

## Installing the Binary Package

### Prerequisites

#### Windows

This runtime may be necessary to run the software.

*  Microsoft Visual C++ Redistributable for Visual Studio 2022
   - [https://www.visualstudio.com/downloads](https://www.visualstudio.com/downloads)
   - [https://aka.ms/vs/17/release/VC_redist.x64.exe](https://aka.ms/vs/17/release/vc_redist.x64.exe)


### Install Anaconda Python

*Please note that most Python 3 environments should work.  The Anaconda distributions are recommended since they provide ready access to the Intel Math Kernel Library (MKL).  As of DEVSIM version 2.1.0, you can configure the software to load alternative libraries implementing the math library functions.*

Install [Anaconda](https://www.anaconda.com/products/individual) or [Miniconda](https://docs.conda.io/en/latest/miniconda.html) environment.  The following install scripts are available in the ``devsim/scripts`` directory to install the latest version of Miniconda into ``${HOME}/anaconda``. 

* ``scripts/install_miniconda_macos.sh``
* ``scripts/install_miniconda_linux.sh``

The Python environment can be loaded in a terminal using:

```
source ${HOME}/anaconda/bin/activate
```

From a system command prompt, with an active Python environment, install the MKL.

    conda install mkl numpy

or alternatively,

    pip install mkl numpy

Please note it is not recommended to mix ``pip`` and Anaconda versions of these packages.

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

At this time, the macOS version only targets Intel-based systems.  There has been a report that users may have to grant permission for code from "unknown" developers.

## Building from Source

Please see [BUILD.md](BUILD.md) for instructions for building from source.

