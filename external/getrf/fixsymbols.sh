#!/bin/bash

for i in `nm ../build/getrf/libgetrf.a | grep ' T ' | sed -e 's/^.* T _//' | sed -e 's/_$//'` ;
do \
  perl -p -i -e "s/\b$i\b/quad_$i/gi" *.[fF];
done
