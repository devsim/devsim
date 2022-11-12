#!/bin/bash
set -e
if [[ $# -eq 0 ]]; then
    echo 'no tag name!'
    exit -1
fi
docker run -it -d --name centos7 centos:7 &&
(cd .. && tar czf devsim.tgz devsim && docker cp devsim.tgz centos7:/root/) &&
docker exec centos7 bash -c "cd /root && tar xzf devsim.tgz";
docker exec centos7 bash -c "cd /root/devsim && bash scripts/build_centos_7.sh devsim_linux_${1}";
#docker exec centos7 bash -c "cd /root/devsim && source scripts/install_miniconda_linux.sh && bash scripts/build_centos_7.sh devsim_linux_${1}";
(cd dist && docker cp centos7:/root/devsim/dist/devsim_linux_${1}.tgz .);
docker stop centos7
