#!/bin/bash
set -e
if [ "$1" = "gcc" ]
  then
  echo $1
elif [ "$1" = "clang" ]
  then
  echo $1
else
  echo "ERROR: FIRST ARGUMENT MUST BE gcc OR clang";
  exit 1;
fi


ARCH=x86_64
PLATFORM=osx
DEVSIM_SRC_DIR=../${PLATFORM}_${ARCH}_release/src/main
DIST_DIR=$2
DIST_BIN=${DIST_DIR}/bin
DIST_LIB=${DIST_DIR}/lib
DIST_PYDLL=${DIST_LIB}/devsim
DIST_VER=${DIST_DIR}
# DO NOT HAVE TRAILING SLASHES!
SYMDIFF_LIBRARY_DIR=../external/symdiff/lib/symdiff
SYMDIFF_EXAMPLES_DIR=../external/symdiff/examples
SYMDIFF_DOCUMENTATION_DIR=../external/symdiff/doc
UMFPACK_LIBRARY_FILE=../external/umfpack_lgpl/build/libumfpack_lgpl.dylib

# make the bin directory and copy binary in
# Assume libstdc++ is a standard part of the system
#http://developer.apple.com/library/mac/#documentation/DeveloperTools/Conceptual/CppRuntimeEnv/Articles/CPPROverview.html
mkdir -p ${DIST_BIN}
mkdir -p ${DIST_DIR}
mkdir -p ${DIST_PYDLL}

#cp -v ${DEVSIM_SRC_DIR}/devsim_py27.so ${DIST_PYDLL}
#cp -v ${DEVSIM_SRC_DIR}/devsim_tcl ${DIST_BIN}
cp -v __init__.py ${DIST_PYDLL}

# goes to lib/symdiff
rsync -aqP --delete ${SYMDIFF_LIBRARY_DIR} ${DIST_LIB}

# because the non gcc build uses the system python interpreter and python 3 is not available
cp -v ${DEVSIM_SRC_DIR}/devsim_py3.so ${DIST_PYDLL}

# INSTALL NAME CHANGE
if [ "$1" = "gcc" ]
then
###
### python libs
###
for i in ${DIST_PYDLL}/devsim_py*.so ${DIST_LIB}/symdiff/symdiff_py*.so
do
echo $i
# get otool dependencies from the gcc compiler
for j in `otool -L $i | egrep '/usr/local/' | sed -e 's/(.*//'`
do
OUTDIR=$(dirname $i)
cp -vf $j ${OUTDIR}
echo install_name_tool -change $j "@loader_path/`basename $j`" $i
install_name_tool -change $j "@loader_path/`basename $j`" $i
done
done
###
### fix issue on High Sierra (and possibly Mojave)
###
for i in ${DIST_PYDLL}/*.dylib ${DIST_LIB}/symdiff/*.dylib
do
echo $i
chmod u+w $i
# get otool dependencies from the gcc compiler
for j in `otool -L $i | egrep '/usr/local/' | sed -e 's/(.*//'`
do
#cp -vf $j ${DIST_LIB}/gcc
echo install_name_tool -change $j "@loader_path/`basename $j`" $i
install_name_tool -change $j "@loader_path/`basename $j`" $i
done
done

# MKL is now versioned
#for i in ${DIST_PYDLL}/devsim_py*.so
#do
#cp -vf "${CONDA_PREFIX}/lib/libmkl_rt.1.dylib" ${DIST_PYDLL}
#echo install_name_tool -change "@rpath/libmkl_rt.1.dylib" "@loader_path/libmkl_rt.1.dylib" ${DIST_PYDLL}/
#install_name_tool -change "@rpath/libmkl_rt.1.dylib" "@loader_path/libmkl_rt.1.dylib" ${DIST_PYDLL}/
#done


#for i in ${DIST_BIN}/devsim_tcl
#do
## get otool dependencies from the gcc compiler
#for j in `otool -L $i | egrep '\bgcc\b' | sed -e 's/(.*//'`
#do
#cp -vf $j ${DIST_LIB}/gcc
#echo install_name_tool -change $j "@executable_path/../lib/gcc/`basename $j`" $i
#install_name_tool -change $j "@executable_path/../lib/gcc/`basename $j`" $i
#done

fi

# strip unneeded symbols
#strip -arch all -u -r ${DIST_DIR}/bin/$i
#done
# keep a copy of unstripped binary
#cp ${SRC_BIN} ${DIST_VER}_unstripped

cp -vf ${UMFPACK_LIBRARY_FILE} ${DIST_PYDLL}


mkdir -p ${DIST_DIR}/doc
cp ../doc/devsim.pdf ${DIST_DIR}/doc
cp ${SYMDIFF_DOCUMENTATION_DIR}/symdiff.pdf ${DIST_DIR}/doc

for i in INSTALL.md NOTICE LICENSE README.md CHANGES.md install.py; do
cp ../$i ${DIST_DIR}
done


#### Python files and the examples
for i in examples testing
do
(cd ../$i; git clean -f -d -x )
rsync -aqP --delete ../$i ${DIST_DIR}
done
rsync -aqP --delete ../python_packages ${DIST_PYDLL}
rsync -aqP --delete ../umfpack ${DIST_PYDLL}

# Copy UMFPACK DLL
cp -vf ${UMFPACK_LIBRARY_FILE} ${DIST_PYDLL}/umfpack

mkdir -p ${DIST_DIR}/examples/symdiff
# add trailing slash for rsync
rsync -aqP --delete ${SYMDIFF_EXAMPLES_DIR}/ ${DIST_DIR}/examples/symdiff


COMMIT=`git rev-parse --verify HEAD`
cat <<EOF > ${DIST_DIR}/VERSION
Package released as:
${DIST_VER}.tgz

Source available from:
http://www.github.com/devsim/devsim
commit ${COMMIT}
EOF
tar czvf ${DIST_VER}.tgz ${DIST_DIR}

