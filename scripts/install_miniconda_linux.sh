#!/bin/bash
#set -e

if [ ! -f Miniconda3-latest-Linux-x86_64.sh ]
then
curl -L -O https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh;
bash Miniconda3-latest-Linux-x86_64.sh -b -p ${HOME}/anaconda;
fi
source ${HOME}/anaconda/bin/activate

