#!/bin/bash

if [ -z "$1" ]
then
  echo "Usage: run_chip_test.sh <CHIPID> [-t]"
  exit 0
fi

ONLYTHRESHOLD=0
if [ "$2" == "-t" ]
then
    ONLYTHRESHOLD=1
fi

mkdir -p results

if [ "$ONLYTHRESHOLD" -eq "0" ] && [ -d "results/$1" ]
then
  echo "ERROR: Output directory already exists. Exiting."
  exit 1
fi

# reset FPGA
#../crystalball/reset_all_chips.py
#sleep 5

mkdir -p results/$1
cd results/$1
mkdir -p Data

EXECUTABLE=../../../pALPIDEfs-software/runTest

if [ "$ONLYTHRESHOLD" -eq "0" ]
then
    $EXECUTABLE READDACS 2>&1 | tee -a readdacs.txt
    $EXECUTABLE SCANDACS 8 2>&1 | tee -a scandacs.txt

    root -l -b -q ../../../analysis/scanDACs.C'("Data/",256/8)'

    eog dacscan.png &

    $EXECUTABLE FIFO 2>&1 | tee -a fifo.txt
fi

$EXECUTABLE THRESHOLD 164 0 50 2>&1 | tee -a threshold.txt

THRFILE=`ls Data/ThresholdScan*.dat | tail -n 1`
root -l -b -q ../../../analysis/ThresholdNew.C+'("'$THRFILE'",true,0,0,true)' | tee threshold_fit.txt

eog threshold_scan.png &

# reset FPGA
#(cd ../..; ../crystalball/reset_all_chips.py)
#sleep 5

#$EXECUTABLE NOISEMASK 100000 noise.out 2>&1 | tee -a noise.txt

$EXECUTABLE POWEROFF

