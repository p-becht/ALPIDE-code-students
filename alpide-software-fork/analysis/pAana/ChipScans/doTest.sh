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
${CMD} FIFO 2>&1 | tee DO_FIFO.log
touch FIFO_SCANDACS
#
exit 0
