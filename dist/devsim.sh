#!/bin/sh
progname="$0"
#echo $progname
curdir=`dirname "$progname"`
progbase=`basename "$progname"`
#if [ -z "${LD_LIBRARY_PATH}" ]
#then
#  export LD_LIBRARY_PATH="$curdir/../lib/shared"
#else
#  export LD_LIBRARY_PATH="$curdir/../lib/shared:${LD_LIBRARY_PATH}"
#fi
#echo $LD_LIBRARY_PATH
  
exec $curdir/devsim.bin $*
