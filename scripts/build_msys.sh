#!/bin/bash
set -e
export PATH=/mingw64/bin:/usr/bin:${PATH}
# msys Specific
#pacman -Su --noconfirm rsync zip
pacman -Su --noconfirm make mingw-w64-x86_64-gcc mingw-w64-x86_64-gcc-fortran

# handle miniconda in appveyor.yml
export CXX=g++
export PYTHON3_BIN=python
export PYTHON3_INCLUDE=$(python -c "from sysconfig import get_paths as gp; print(gp()['include'])")
export PYTHON3_ARCHIVE=$(cygpath -w ${CONDA_PREFIX}/libs/python3.lib)

# this script assumes git clone and submodule initialization has been done

# SYMDIFF build
(cd external/symdiff && bash ../symdiff_msys.sh && cd msys_x86_64_release && make -j3);

# quad precision getrf
(cd external/getrf && bash setup_msys.sh && cd msys && make -j3)


bash scripts/setup_msys.sh
(cd msys_x86_64_release && make -j3)
(cd dist && bash package_msys.sh ${1})

