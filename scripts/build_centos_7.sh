#!/bin/bash
set -e

# Centos Specific
#https://fedoraproject.org/wiki/EPEL
yum install -y git bison flex
yum install -y centos-release-scl
yum install -y devtoolset-9-gcc devtoolset-9-gcc-c++ devtoolset-9-libquadmath-devel devtoolset-9-gcc-gfortran make

export CMAKE="cmake"
export CMAKE_CXX_FLAGS=""
export CC="/opt/rh/devtoolset-9/root/usr/bin/gcc"
export CXX="/opt/rh/devtoolset-9/root/usr/bin/g++"
export F77="/opt/rh/devtoolset-9/root/usr/bin/gfortran"

#minimal conda environments to prevent linking against the wrong libraries
cd ${HOME}
if [ ! -f Miniconda3-latest-Linux-x86_64.sh ]
then
curl -L -O https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh;
bash Miniconda3-latest-Linux-x86_64.sh -b -p ${HOME}/anaconda;
${HOME}/anaconda/bin/conda create  -y --name python3_devsim_build python=3
${HOME}/anaconda/bin/conda install -y --name python3_devsim_build mkl mkl-devel mkl-include boost cmake
fi
source ${HOME}/anaconda/bin/activate python3_devsim_build

export PYTHON3_BIN=python
export PYTHON3_INCLUDE=$(python -c "from sysconfig import get_paths as gp; print(gp()['include'])")
export PYTHON3_ARCHIVE=""

#download boost library
#(cd ${HOME}/devsim/external && curl -O -L https://dl.bintray.com/boostorg/release/1.66.0/source/boost_1_66_0.tar.gz)
#(cd ${HOME}/devsim/external && tar xzf boost_1_66_0.tar.gz)


# this script assumes git clone and submodule initialization has been done
cd devsim

# SuperLU
#(cd external && curl -O http://crd-legacy.lbl.gov/~xiaoye/SuperLU/superlu_4.3.tar.gz && tar xzf superlu_4.3.tar.gz)
(cd external && tar xzf superlu_4.3.tar.gz)

# SYMDIFF build
(cd external/symdiff && bash ../symdiff_centos.sh && cd linux_x86_64_release && make -j2);

# CGNSLIB build
####(cd external && curl -L -O https://github.com/CGNS/CGNS/archive/v3.1.4.tar.gz && tar xzf v3.1.4.tar.gz)
####(cd external && mkdir -p CGNS-3.1.4/build && cd CGNS-3.1.4/build && cmake3  -DCMAKE_C_COMPILER=/opt/rh/devtoolset-8/root/usr/bin/gcc -DBUILD_CGNSTOOLS=OFF -DCMAKE_INSTALL_PREFIX=$PWD/../../cgnslib .. && make -j2 && make install)
# SUPERLU build
(cd external/SuperLU_4.3 && sh ../superlu_centos6.sh)

# quad precision getrf
(cd external/getrf && bash setup_centos6.sh && cd build && make -j2)

# start devsim build

bash scripts/setup_centos_6.sh

# overcome bad libstdc++ in anaconda directory
mkdir -p linux_x86_64_release/linklibs
for i in libmkl_rt.so libz.a libsqlite3*; do cp -f ${CONDA_PREFIX}/lib/$i linux_x86_64_release/linklibs/; done

(cd linux_x86_64_release && make -j2)
(cd dist && bash package_linux.sh ${1})


