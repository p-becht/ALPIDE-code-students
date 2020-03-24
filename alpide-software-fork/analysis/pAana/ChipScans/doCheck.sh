#!/bin/bash
#
DONE="DONE:"
DOING="DOING:"
TODO="TODO:"
for scan in \
  SCANDACS \
  SCANDIGITAL \
  SCANANALOGUE \
  THRESHOLD \
  NOISEOCC \
  SOURCE
do
  if [ -f DO_${i}.log -a -f DONE_{i} ]
  then

  fi
#
  if [ -f DO_${i}.log -a ! -f DONE_{i} ]
  then

  fi
#
  if [ ! -f DO_${i}.log -a ! -f DONE_{i} ]
  then

  fi
done
#
exit 0
