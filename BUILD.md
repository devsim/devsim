# DEVSIM Installation from source

## Introduction


It is usually not necessary to build from source, however the instructions below are provided  If you need support, please post a message at [https://forum.devsim.org](https://forum.devsim.org).


## Prerequisites

A Python 3.9 or higher installation needed when building a non-threaded version of the software  A Python 3.15 or higher version is required to build both a threaded and non-threaded version at the same time.  Optionally, a Python 3.15t or higher version can be used instead.

Please see the GitHub Actions workflow files to see how each platform is build for the distriburted packages.

## Free-threaded Python build variant

The CMake option `-DPYTHON3T=ON` adds the `devsim_py3t` extension target
for the free-threaded Stable ABI (`abi3t`) introduced in CPython 3.15.
Both extensions can be built together, or `-DPYTHON3=OFF` can be used to
build only the free-threaded variant. Set `PYTHON3T_INCLUDE` to the include
directory from the intended free-threaded interpreter, for example:

```sh
python3.15t -c "import sysconfig; print(sysconfig.get_path('include'))"
```

Pass that path with `-DPYTHON3T_INCLUDE=...`, along with the other settings
required for your platform. If building both variants, also set
`PYTHON3_INCLUDE` for the regular Python extension. On Windows, set
`PYTHON3T_ARCHIVE` to the Stable ABI import library `python3t.lib` provided
with the free-threaded Python installation.

The `devsim_py3t` target uses the CPython 3.15 `abi3t` Limited API, declares
that it can run with the GIL disabled, and produces `devsim_py3.abi3t.so`
on macOS and Linux or `devsim_py3.pyd` on Windows. The `devsim_py3` target
remains unchanged. The new extension supports at most one module instance
per process and does not support subinterpreters. DEVSIM uses a recursive
mutex to serialize module API calls. Concurrent calls from different threads
will raise a `RuntimeError`. Recursive calls from the same thread are allowed,
supporting the use case where internal module code spawns threads that must
not call back into the public API. The macOS, Linux, and Windows release scripts
build separate `cp39-abi3` and `cp315-abi3t` wheels.

## Setting Version Number

Please see [HOWTO.md](HOWTO.md) for notes on changing the version numbers of `devsim` and Python.

## Platform Instructions

### Linux

#### Building Red Hat 8 compatible version

The `AlmaLinux 8` based [manylinux_2_28](https://github.com/pypa/manylinux?tab=readme-ov-file#manylinux_2_28-almalinux-8-based) distribution is used for binary Linux releases, as it is forward compatible with most Linux distributions.  It will install all required dependencies, including Python.  It is recommended to run this in a docker image.

First download the source on your host Linux machine:
```
git clone https://github.com/devsim/devsim
cd devsim
git submodule init
git submodule update
```

Then to build on the local system, use
```
bash scripts/build_manylinux_2_28.sh version
```

or in a docker image.
```
bash scripts/build_docker_manylinux_2_28.sh version
```

Note `version` is replaced with the desired version. Both wheels are placed in
the `dist` directory. The manylinux build uses regular CPython 3.15 from
`/opt/python/cp315-cp315/bin/python` for both variants; `PYTHON3_BIN` can
select another regular CPython 3.15+ when running the script directly. The
direct build replaces the packaging directories `<version>_abi3` and
`<version>_abi3t` on subsequent runs. The Docker wrapper uses a
`devsim_linux_<architecture>_` prefix for those directories inside the image.

#### Other Linux Distributions

Additional build scripts are available for `Ubuntu` and `Fedora` in the `scripts` directory.


### Building macOS

The macOS release version can be built locally with the following prerequisites:

* Xcode
* Homebrew [https://brew.sh/](https://brew.sh/)
* Regular CPython 3.15 or newer, with development headers and pip

The script uses `/opt/homebrew/bin/python3` by default; set `PYTHON3_BIN` to
choose another regular CPython 3.15+ interpreter. Both extension variants are
built together using that installation's Python headers.

```
git clone https://github.com/devsim/devsim.git
cd devsim
git submodule init
git submodule update
bash scripts/build_macos.sh version
```

where `version` is replaced with the desired version. The two Python wheel
files are placed in `devsim/dist`. The packaging directories
`devsim_macos_arm64_<version>_abi3` and `devsim_macos_arm64_<version>_abi3t`
remain in `devsim/dist` and are replaced when rebuilding the same version.
Note that the resulting package file does not have any dependencies on Homebrew to run.

The macOS release workflow builds both wheels on the same runner using the regular Python installation and its headers.

### Building on Windows

***Please note that the Windows build scripts will make changes to an existing MSYS 2 installation.***

#### Visual Studio 2026 version

This version requires installing:

* Python 3.15 or higher
* MSYS 2 [https://www.msys2.org/](https://www.msys2.org/)
* Microsoft Visual Studio 2026 [https://visualstudio.microsoft.com/vs/](https://visualstudio.microsoft.com/vs/)

as well as the prerequisites for the MSYS version above.

From a Python 3.15 environment command prompt:

```
scripts\build_appveyor.bat x64 python version
```

where `version` is replaced with the desired version name.  The wheel files are then in the `dist` directory.
