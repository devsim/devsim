# DEVSIM Installation

## Introduction

Please visit the DEVSIM homepage at [https://devsim.org](https://devsim.org) for the latest information concerning the availability of the software.

Supported Platforms:

* Centos, Red Hat 7
* macOS
* Microsoft Windows (64 bit)

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

Binary releases are available from [Zenodo](https://doi.org/10.5281/zenodo.1186952), or from [GitHub](https://github.com/devsim/devsim/releases).  Uncompress the file to the desired directory.  Then go into the the directory and run the Python installer.

    cd devsim
    python install.py

This script will install DEVSIM into your environment.  If you should need to remove this environment, you can use ``pip``.  Alternatively, you can set the PYTHONPATH environment variable to the ``devsim/lib`` directory.

#### Basic Test

You should then be able to run an example in the ``testing`` directory.

    cd testing
    python cap2.py

If using the ``pip`` installed version, the data files should be available in your environment in the `devsim_data` directory.

### Troubleshooting

If you have any issues with this process.  Please contact us through the DEVSIM forum at [https://forum.devsim.org](https://forum.devsim.org).

At this time, the macOS version only targets Intel-based systems.  There has been a report that users may have to grant permission for code from "unknown" developers.

## Building from Source

***For the builds in this section, an Anaconda (or Miniconda) installation is required.  Please install using the instructions above.  Ensure that you are running the scripts below from a shell with an activated Python environment, having the ``conda`` command in your path.  Please note that the build scripts will create a conda environment named ``devsim_python3_build``.***

#### Building Centos 7 Linux

***The scripts will install many system packages.  It is highly recommended to run these scripts in a docker container.***

The Centos 7 version is used for binary Linux releases, as it is forward compatible with most Linux distributions and used the Intel MKL.  It will install all required dependencies, including Anaconda Python.  It is recommended to run this in a docker image.  ***There is no need to run ``install_miniconda_linux.sh`` if it is already installed***.

```
git clone https://github.com/devsim/devsim.git
cd devsim
git submodule init
git submodule update
bash scripts/build_centos_7.sh devsim_linux_version
```

where ``version`` is replaced with the desired version.  The compressed file is then ``devsim/dist/devsim_linux_version.tgz``

#### Building Other Linux Systems

***Please note that these scripts may attempt to install packages using the system installer on your system.  Please evaluate the scripts carefully to ensure you agree with these changes to your system.  It is highly recommended to run these scripts in a docker container.  Anaconda Python is not required for these builds.***

Additional build scripts are available for ``Ubuntu`` and ``Fedora`` in the ``scripts`` directory.  These version do not use the Intel MKL and replace the use of the Intel MKL Pardiso Solver with SuperLU.

### Building macOS

#### Release Version

***The builds scripts will make changes to an existing Homebrew installation.***

The macOS release version is built using a Travis build server (see ``.travis.yml``), but it can be built locally with the following prerequisites:

* Xcode 10.1
* Homebrew [https://brew.sh/](https://brew.sh/)
* GCC Compiler (Brew package gcc@9)

The rest of the components, including Anaconda Python and various build components are downloaded and installed in the scripts below.  ***There is no need to run ``install_miniconda_macos.sh`` if Anaconda Python is already installed.***.

```
git clone https://github.com/devsim/devsim.git
cd devsim
git submodule init
git submodule update
source scripts/install_miniconda_macos.sh
bash scripts/build_macos.sh gcc version
```

where ``version`` is replaced with the desired version.  The compressed file is then ``devsim/dist/devsim_macos_version.tgz``
Note that the resulting package file does not have any dependencies on Homebrew on the target system.

#### Using macOS system compiler

Please note that you will need to install the Xcode developer tools, including the C++ compiler for your system.  This version does not have 128-bit floating point precision support and is not provided as part of the regular release.  By default it uses SuperLU 4.3 as the direct solver, but the build scripts can be modified to support Intel MKL Pardiso.  The rest of the components, including Anaconda Python and various build components are downloaded and installed in the scripts below.  ***There is no need to run ``install_miniconda_macos.sh`` if Anaconda Python is already installed.***

```
git clone https://github.com/devsim/devsim.git
cd devsim
git submodule init
git submodule update
source scripts/install_miniconda_macos.sh
bash scripts/build_macos.sh gcc version
```

### Building on Windows

***Please note that the Windows build scripts will make changes to an existing MSYS 2 installation.  It will also create new Anaconda environments.***

#### MSYS version

Please see ``appveyor.yml`` in the source directory for more information about the automated build process.

To build manually, the prerequisites are to have installed:

* Anaconda Python (or Miniconda)
* MSYS 2 [https://www.msys2.org/](https://www.msys2.org/)

First download the source and initialize the submodule from [https://github.com/devsim/devsim.git](https://github.com/devsim/devsim.git).

From a Anaconda environment command prompt:

    scripts\build_appveyor.bat msys conda version

where ``version`` is replaced with the desired version.  The compressed file is then ``devsim/dist/devsim_msys_version.zip``.


#### Visual Studio 2022 version

This version requires installing:

* Anaconda Python (or Miniconda)
* MSYS 2 [https://www.msys2.org/](https://www.msys2.org/)
* Microsoft Visual Studio 2022 [https://visualstudio.microsoft.com/vs/](https://visualstudio.microsoft.com/vs/)

as well as the prerequisites for the MSYS version above.

From a Anaconda environment command prompt:

    scripts\build_appveyor.bat x64 conda version

where ``version`` is replaced with the desired version.  The compressed file is then ``devsim/dist/devsim_win64_version.zip``


