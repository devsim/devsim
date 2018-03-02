#!/bin/bash
set -e
# put the tag name in first argument used for distribution
# this script assumes git clone and submodule initialization has been done

# Centos Specific
#https://fedoraproject.org/wiki/EPEL
yum install -y epel-release git bison flex tcl tcl-devel
yum install -y cmake3
yum install -y centos-release-scl
yum install -y devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-libquadmath-devel devtoolset-6-gcc-gfortran


#download install Miniconda
cd /root
if [ ! -f Miniconda2-latest-Linux-x86_64.sh ]
then
curl -O https://repo.continuum.io/miniconda/Miniconda2-latest-Linux-x86_64.sh;
bash ~/Miniconda2-latest-Linux-x86_64.sh -b -p ${HOME}/anaconda;
fi
${HOME}/anaconda/bin/conda install -y numpy mkl

#download boost library
(cd /root/devsim/external && curl -O -L https://dl.bintray.com/boostorg/release/1.64.0/source/boost_1_64_0.tar.gz)
(cd /root/devsim/external && tar xzf boost_1_64_0.tar.gz)

#git clone https://github.com/devsim/symdiff
#cd symdiff
#bash scripts/setup_centos_6.sh
#cd linux_x86_64_release
#make -j2

cd /root
# add max depth or copy over
#git clone https://github.com/devsim/devsim
#git clone git@github.com:devsim/devsim
cd devsim
#git submodule init
#git submodule update

# SuperLU and CGNS Download
(cd external && curl -O http://crd-legacy.lbl.gov/~xiaoye/SuperLU/superlu_4.3.tar.gz && tar xzf superlu_4.3.tar.gz)
(cd external && curl -L -O https://github.com/CGNS/CGNS/archive/v3.1.4.tar.gz && tar xzf v3.1.4.tar.gz)

# quad precision getrf
(cd external/getrf && bash setup_centos6.sh && cd build && make -j2)

# SYMDIFF build
(cd external/symdiff && bash scripts/setup_centos_6.sh && cd linux_x86_64_release && make -j2);
# CGNSLIB build
(cd external && mkdir -p CGNS-3.1.4/build && cd CGNS-3.1.4/build && cmake3  -DCMAKE_C_COMPILER=/opt/rh/devtoolset-6/root/usr/bin/gcc -DBUILD_CGNSTOOLS=OFF -DCMAKE_INSTALL_PREFIX=$PWD/../../cgnslib .. && make -j2 && make install)
# SUPERLU build
(cd external/SuperLU_4.3 && sh ../superlu_centos6.sh)

bash scripts/setup_centos_6.sh
(cd linux_x86_64_release && make -j2)
(cd dist && bash package_linux.sh ${1})


