#!/bin/bash
set -e
if [[ $# -eq 0 ]]; then
    echo 'no tag name!'
    exit -1
fi

export DEVSIM_ARCH=${2}

if [ "$DEVSIM_ARCH" = "arm64" ]; then
DEVSIM_ARCH=aarch64
DOCKER_ARCH="linux/arm64"
elif [ "$DEVSIM_ARCH" = "amd64" ]; then
DEVSIM_ARCH=x86_64
DOCKER_ARCH="linux/amd64"
fi
DOCKER_CID=manylinux2014_${DEVSIM_ARCH}

export DEVSIM_TAR_NAME=devsim_linux_${DEVSIM_ARCH}_${1}

docker run -it --detach -e README_BASE_URL="${README_BASE_URL}" --name ${DOCKER_CID} --platform ${DOCKER_ARCH} quay.io/pypa/${DOCKER_CID}
(cd .. && tar czf ${DOCKER_CID}.tgz devsim && docker cp ${DOCKER_CID}.tgz ${DOCKER_CID}:/root/)
docker exec ${DOCKER_CID} bash -c "git config --global --add safe.directory /root/devsim"
docker exec ${DOCKER_CID} bash -c "cd /root && tar xzf ${DOCKER_CID}.tgz";
docker exec ${DOCKER_CID} bash -c "cd /root/devsim && bash scripts/build_manylinux2014.sh ${DEVSIM_TAR_NAME}";
(cd dist && docker cp ${DOCKER_CID}:/root/devsim/dist/${DEVSIM_TAR_NAME}.tgz .);
(cd dist && for i in $(docker exec ${DOCKER_CID} bash -c "ls /root/devsim/dist/*.whl"); do docker cp ${DOCKER_CID}:${i} .; done)
docker stop ${DOCKER_CID}
docker rm ${DOCKER_CID}

