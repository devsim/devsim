#!/bin/bash
set -e

cd ${HOME}
if [ ! -f Miniconda3-latest-Linux-x86_64.sh ]
then
curl -L -O https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh;
bash Miniconda3-latest-Linux-x86_64.sh -b -p ${HOME}/anaconda;
fi
${HOME}/anaconda/bin/conda create  -y --name python3_devsim_build python=3
${HOME}/anaconda/bin/conda install -y --name python3_devsim_build mkl mkl-devel mkl-include boost cmake
source ${HOME}/anaconda/bin/activate python3_devsim_build

