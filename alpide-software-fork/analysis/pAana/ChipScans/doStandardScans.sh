#!/bin/bash
#
if [ ! -d Data ]
then
  mkdir Data
fi
#
pALPIDEfs_SW_PATH="/home/xzhang/ws/local/pALPIDEfs/pALPIDEfs-software"
export PATH=${pALPIDEfs_SW_PATH}:${PATH}
CMD="runTest"
#
${CMD} SCANDACS                   2>&1 | tee DO_SCANDACS.log
touch DONE_SCANDACS
#
${CMD} SCANDIGITAL 50 16384       2>&1 | tee DO_SCANDIGITAL.log
touch DONE_SCANDIGITAL
#
${CMD} SCANANALOGUE 100 50 16384  2>&1 | tee DO_SCANANALOGUE.log
touch DONE_SCANANALOGUE
#
${CMD} THRESHOLD 16384 0 70       2>&1 | tee DO_THRESHOLD.log
touch DONE_THRESHOLD
#
${CMD} NOISEOCC 1000000           2>&1 | tee DO_NOISEOCC.log
touch DONE_NOISEOCC
#
${CMD} SOURCE 1000000             2>&1 | tee DO_SOURCE.log
touch DONE_SOURCE
#
exit 0
