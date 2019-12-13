#!/bin/bash
set -e

# msys Specific

# handle miniconda in appveyor.yml

# this script assumes git clone and submodule initialization has been done

# SuperLU
(cd external && bsdtar xzf superlu_4.3.tar.gz && bash build_superlu_msys.sh)

# SYMDIFF build
(cd external/symdiff && bash ../symdiff_msys.sh && cd msys_x86_64_release && make -j2);

# quad precision getrf
(cd external/getrf && bash setup_msys.sh && cd build && make -j2)


bash scripts/setup_msys.sh
(cd msys_x86_64_release && make -j2)
(cd dist && bash package_msys.sh ${1})


