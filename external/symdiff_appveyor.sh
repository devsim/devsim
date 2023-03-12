# Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

# Note: run this file using bash
set -e
set -u
SYMDIFF_CONFIG="appveyor"

GENERATOR="$1"
AOPTION="$2"
BUILDDIR="$3"
CONDA_PREFIX="$4"

mkdir -p ${BUILDDIR}
(cd ${BUILDDIR}; cmake -G "${GENERATOR}" -A "${AOPTION}" -DSYMDIFF_CONFIG="${SYMDIFF_CONFIG}" -DANACONDA_PATH="${CONDA_PREFIX}" -DTCLMAIN=OFF -DPYTHON3=ON ..)

