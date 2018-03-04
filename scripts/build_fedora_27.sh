#!/bin/bash
set -e
# put the tag name in first argument used for distribution
# this script assumes git clone and submodule initialization has been done

# Fedora Specific
#https://fedoraproject.org/wiki/EPEL
sudo dnf install -y git bison flex tcl tcl-devel cmake3 gcc gcc-c++ libquadmath-devel \
     gcc-gfortran bzip2 boost boost-devel SuperLU SuperLU-devel cgnslib cgnslib-devel \
     sqlite-devel python2-devel blas-devel lapack-devel


#download install Miniconda
mkdir -p ~/DevSim
cd ~/DevSim
if [ ! -f Miniconda2-latest-Linux-x86_64.sh ]; then
    curl -O https://repo.continuum.io/miniconda/Miniconda2-latest-Linux-x86_64.sh;
    chmod +x Miniconda2-latest-Linux-x86_64.sh
    ./Miniconda2-latest-Linux-x86_64.sh -b -p ${HOME}/DevSim/anaconda;
fi
./anaconda/bin/conda install -y numpy mkl

# add max depth or copy over
if [ ! -d ~/DevSim/devsim ]; then
    git clone https://github.com/devsim/devsim
    cd devsim
    git submodule init
    git submodule update
fi

# SuperLU build
cd ~/DevSim/devsim/external
if [ ! -d SuperLU_4.3 ]; then
    curl -O http://crd-legacy.lbl.gov/~xiaoye/SuperLU/superlu_4.3.tar.gz
    tar xzf superlu_4.3.tar.gz
    cd SuperLU_4.3
    make CC=/usr/bin/gcc SuperLUroot=`pwd` SUPERLULIB=`pwd`/lib/libsuperlu_4.3.a NOOPTS="" CFLAGS="-O3 -DPRNTlevel=0" CDEFS="-Dadd_" superlulib
fi

# quad precision getrf
cd ~/DevSim/devsim/external/getrf
mkdir -p build
cd build
cmake -DCMAKE_Fortran_FLAGS="-freal-8-real-16 -ffixed-line-length-0" -DCMAKE_BUILD_TYPE=RELEASE ..
make -j2



# SYMDIFF build
cd ~/DevSim/devsim/external/symdiff
./scripts/setup_fedora_27.sh
cd linux_x86_64_release && make -j2

# Finally, build devsim
cd ~/DevSim/devsim/
scripts/setup_fedora_27.sh
(cd linux_x86_64_release && make -j2)
#(cd dist && bash package_linux.sh ${1})


