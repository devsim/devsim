#!/bin/csh
set j=`uname -m`
foreach i (test testdev testmodel testmodel2 testmodel3 testmodel4 testmodel5 testmodel6)
${DEVSIMHOME}/${ARCH}/main/$i >&! $i.$j.out
end

${DEVSIMHOME}/${ARCH}/main/testtcl testtcl.tcl >&! testtcl.$j.out
${DEVSIMHOME}/${ARCH}/main/testtcl2 testtcl2.tcl >&! testtcl2.$j.out
