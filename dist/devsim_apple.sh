#!/bin/sh
progname="$0"
#echo $progname
curdir=`dirname "$progname"`
progbase=`basename "$progname"`
if [ -z "${DYLD_LIBRARY_PATH}" ]
then
  export DYLD_LIBRARY_PATH="$curdir/../lib/shared"
else
  export DYLD_LIBRARY_PATH="$curdir/../lib/shared:${DYLD_LIBRARY_PATH}"
fi
#echo $LD_LIBRARY_PATH
  
exec $curdir/devsim.bin $*
