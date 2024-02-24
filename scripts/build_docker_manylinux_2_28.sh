#!/bin/bash
set -e
if [[ $# -eq 0 ]]; then
    echo 'no tag name!'
    exit -1
fi

export DEVSIM_ARCH=$(uname -m)

if [ "$DEVSIM_ARCH" = "arm64" ]; then
DEVSIM_ARCH=aarch64
fi

export DEVSIM_TAR_NAME=devsim_linux_${DEVSIM_ARCH}_${1}

docker run -it -d -e README_BASE_URL="${README_BASE_URL}" --name manylinux_2_28 quay.io/pypa/manylinux_2_28_${DEVSIM_ARCH} &&
(cd .. && tar czf devsim.tgz devsim && docker cp devsim.tgz manylinux_2_28:/root/) &&
docker exec manylinux_2_28 bash -c "git config --global --add safe.directory /root/devsim"
docker exec manylinux_2_28 bash -c "cd /root && tar xzf devsim.tgz";
docker exec manylinux_2_28 bash -c "cd /root/devsim && bash scripts/build_manylinux_2_28.sh ${DEVSIM_TAR_NAME}";
(cd dist && docker cp manylinux_2_28:/root/devsim/dist/${DEVSIM_TAR_NAME}.tgz .);
(cd dist && for i in $(docker exec manylinux_2_28 bash -c "ls /root/devsim/dist/*.whl"); do docker cp manylinux_2_28:${i} .; done)
docker stop manylinux_2_28

