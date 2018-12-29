#!/bin/bash
set -e
apt-get update
apt-get install -y git curl make cmake gcc g++ flex bison tcl8.5 tcl8.5-dev tcl-dev python2.7 python2.7-dev sqlite3 libsqlite3-dev zlib1g-dev libblas3gf liblapack3gf

cd /root
git clone https://github.com/devsim/devsim.git
cd devsim
git submodule init
git submodule update
# put the tag name in first argument used for distribution
# this script assumes git clone and submodule initialization has been done

# Centos Specific
#https://fedoraproject.org/wiki/EPEL
#yum install -y epel-release git cmake gcc gcc-c++ bison flex
#yum install -y git cmake gcc gcc-c++ gnu-fortran bison flex wget


#download install Miniconda
cd /root
#curl -O https://repo.continuum.io/miniconda/Miniconda2-latest-Linux-x86_64.sh
#bash ~/Miniconda2-latest-Linux-x86_64.sh -b -p ${HOME}/anaconda
#${HOME}/anaconda/bin/conda install -y numpy mkl

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
#(cd external && curl -O http://crd-legacy.lbl.gov/~xiaoye/SuperLU/superlu_4.3.tar.gz && tar xzf superlu_4.3.tar.gz)
(cd external && curl -L -O https://github.com/CGNS/CGNS/archive/v3.1.4.tar.gz && tar xzf v3.1.4.tar.gz)

# SYMDIFF build
(cd external/symdiff && bash scripts/setup_ubuntu_14.04.sh && cd linux_x86_64_release && make -j2);
# CGNSLIB build
(cd external && mkdir -p CGNS-3.1.4/build && cd cgnslib_3.1.4/build && cmake -DBUILD_CGNSTOOLS=OFF -DCMAKE_INSTALL_PREFIX=$PWD/../../cgnslib .. && make -j2 && make install)
# SUPERLU build
(cd external/SuperLU_4.3 && sh ../superlu_ubuntu_12.04.sh)

bash scripts/setup_ubuntu_14.04.sh
(cd linux_x86_64_release && make -j2)
(cd dist && bash package_linux.sh ${1})


