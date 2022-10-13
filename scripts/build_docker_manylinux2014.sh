#!/bin/bash
set -e
if [[ $# -eq 0 ]]; then
    echo 'no tag name!'
    exit -1
fi
docker run -it -d --name manylinux2014 quay.io/pypa/manylinux2014_x86_64 &&
(cd .. && tar czf devsim.tgz devsim && docker cp devsim.tgz manylinux2014:/root/) &&
docker exec manylinux2014 bash -c "git config --global --add safe.directory /root/devsim"
docker exec manylinux2014 bash -c "cd /root && tar xzf devsim.tgz";
docker exec manylinux2014 bash -c "cd /root/devsim && bash scripts/build_manylinux2014.sh devsim_linux_${1}";
#docker exec manylinux2014 bash -c "cd /root/devsim && source scripts/install_miniconda_linux.sh && bash scripts/build_centos_7.sh devsim_linux_${1}";
(cd dist && docker cp manylinux2014:/root/devsim/dist/devsim_linux_${1}.tgz .);
(cd dist && for i in $(docker exec manylinux2014 bash -c "ls /root/devsim/dist/*.whl"); do docker cp manylinux2014:${i} .; done)
docker stop manylinux2014
