
set -e
DEVSIM_CONFIG=appveyor
GENERATOR="$1"
AOPTION="$2"
TOOLSET="$3"
BUILDDIR="$4"
CONDA_PREFIX="$5"

    mkdir -p ${BUILDDIR}
    (cd ${BUILDDIR};
    cmake -G "${GENERATOR}" \
    -A ${AOPTION} \
    -T ${TOOLSET} \
    -DDEVSIM_EXTENDED_PRECISION=ON \
    -DEIGEN_DENSE_EXTENDED=ON \
    -DDEVSIM_CPP_BIN_FLOAT=ON \
    -DDEVSIM_CONFIG=${DEVSIM_CONFIG} \
    -DMKL_PARDISO=ON \
    -DCONDA_PREFIX=${CONDA_PREFIX} \
    -DPYTHON3=ON \
    -DPYTHON3_INCLUDE=${PYTHON3_INCLUDE} \
    -DPYTHON3_ARCHIVE=${PYTHON3_ARCHIVE} \
    -DBUILDDIR=${BUILDDIR} \
    ..)

