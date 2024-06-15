# DEVSIM Installation from source

## Introduction


It is usually not necessary to build from source, however the instructions below are provided  If you need support, please post a message at [https://forum.devsim.org](https://forum.devsim.org).


## Prerequisites

An Anaconda (or Miniconda) installation is optional for `Linux` and `macOS`, but is highly recommended for `Windows`.  The full package may be downloaded from [https://anaconda.com](https://anaconda.com).  The minimal install is available from. [https://docs.anaconda.com/free/miniconda/](https://docs.anaconda.com/free/miniconda/)


For `x86_64` or `x64` systems, the Intel MKL is recommended, but it is not needed for building the application.

## Setting Version Number

Set the version number by changing `DEVSIM_VERSION_STRING` in `CMakeLists.txt`.  The `version` also needs to be changed in `dist/bdist_wheel/setup.cfg`.

## Platform Instructions

### Linux

#### Building Red Hat 7 compatible version

The `Centos 7` compatible [manylinux2014](https://github.com/pypa/manylinux?tab=readme-ov-file#manylinux2014-centos-7-based-glibc-217) distribution is used for binary Linux releases, as it is forward compatible with most Linux distributions and used the Intel MKL.  It will install all required dependencies, including Anaconda Python.  It is recommended to run this in a docker image.

First download the source on your host Linux machine:
```
git clone https://github.com/devsim/devsim
cd devsim
git submodule init
git submodule update
```

Then to build on the local system, use
```
bash scripts/build_manylinux2014.sh version
```

or in a docker image.
```
bash scripts/build_docker_manylinux2014.sh version
```

Note `version` is replaced with the desired version.  There should then be a Python `.whl` file in the `dist` directory.

#### Other Linux Distributions

Additional build scripts are available for `Ubuntu` and `Fedora` in the `scripts` directory.


### Building macOS

The macOS release version is built using a Travis build server (see `.travis.yml`), but it can be built locally with the following prerequisites:

* Xcode
* Homebrew [https://brew.sh/](https://brew.sh/)

The rest of the components, including Anaconda Python and various build components are downloaded and installed in the scripts below.

```
git clone https://github.com/devsim/devsim.git
cd devsim
git submodule init
git submodule update
bash scripts/build_macos.sh clang version
```

where `version` is replaced with the desired version.  The Python wheel file is then in `devsim/dist`
Note that the resulting package file does not have any dependencies on Homebrew to run.

### Building on Windows

***Please note that the Windows build scripts will make changes to an existing MSYS 2 installation.  It will also create new Anaconda environments.***

#### Visual Studio 2022 version

This version requires installing:

* Anaconda Python (or Miniconda)
* MSYS 2 [https://www.msys2.org/](https://www.msys2.org/)
* Microsoft Visual Studio 2022 [https://visualstudio.microsoft.com/vs/](https://visualstudio.microsoft.com/vs/)

as well as the prerequisites for the MSYS version above.

From a Anaconda environment command prompt:

```
scripts\build_appveyor.bat x64 conda version
```

where `version` is replaced with the desired version name.  The Python wheel file is then in the `dist` directory.

