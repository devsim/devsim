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
SRC_DIR=../${PLATFORM}_${ARCH}_release/src/main
DIST_DIR=$2
DIST_BIN=${DIST_DIR}/bin
DIST_LIB=${DIST_DIR}/lib
DIST_VER=${DIST_DIR}


# make the bin directory and copy binary in
# Assume libstdc++ is a standard part of the system
#http://developer.apple.com/library/mac/#documentation/DeveloperTools/Conceptual/CppRuntimeEnv/Articles/CPPROverview.html
mkdir -p ${DIST_BIN}
cp ${SRC_DIR}/devsim_py ${DIST_BIN}/devsim
cp ${SRC_DIR}/devsim_tcl ${DIST_BIN}/devsim_tcl

if [ "$1" = "gcc" ]
then
mkdir -p ${DIST_LIB}
for i in ${DIST_BIN}/devsim ${DIST_BIN}/devsim_tcl
do
# get otool dependencies from the gcc compiler
for j in `otool -L $i | egrep '\bgcc\b' | sed -e 's/(.*//'`
do
cp -vf $j ${DIST_LIB}
install_name_tool -change $j "@executable_path/../lib/`basename $j`" $i
done
done
fi

# strip unneeded symbols
#strip -arch all -u -r ${DIST_DIR}/bin/$i
#done
# keep a copy of unstripped binary
#cp ${SRC_BIN} ${DIST_VER}_unstripped


mkdir -p ${DIST_DIR}/doc
cp ../doc/devsim.pdf ${DIST_DIR}/doc
for i in INSTALL NOTICE LICENSE RELEASE macos.txt scripts/anaconda_vars.sh scripts/anaconda_vars.csh; do
cp ../$i ${DIST_DIR}
done


#### Python files and the examples
for i in python_packages examples testing
do
(cd ../$i; git clean -f -d -x )
rsync -aP --delete ../$i ${DIST_DIR}
done



COMMIT=`git rev-parse --verify HEAD`
cat <<EOF > ${DIST_DIR}/VERSION
Package released as:
${DIST_VER}.tgz

Source available from:
http://www.github.com/devsim/devsim 
commit ${COMMIT}
EOF
tar czf ${DIST_VER}.tgz ${DIST_DIR}

