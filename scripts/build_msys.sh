#!/bin/bash
set -e
export PATH=/mingw64/bin:/usr/bin:${PATH}
# msys Specific
pacman -Su --noconfirm rsync zip

# handle miniconda in appveyor.yml

# this script assumes git clone and submodule initialization has been done

# SYMDIFF build
(cd external/symdiff && bash ../symdiff_msys.sh && cd msys_x86_64_release && make -j2);

# SuperLU
(cd external && bsdtar xzf superlu_4.3.tar.gz && bash build_superlu_msys.sh)


# quad precision getrf
(cd external/getrf && bash setup_msys.sh && cd build && make -j2)


bash scripts/setup_msys.sh
(cd msys_x86_64_release && make -j2)
(cd dist && bash package_msys.sh ${1})

