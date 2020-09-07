#!/bin/bash

DAQS=$(lsusb -v | grep -o DAQ-.*)
VCASN=(100 105 110 115)
ITHR=(50 100)

for daq in $DAQS; do
    for vcasn in "${VCASN[@]}"; do
        vcasn2=$((VCASN+12))
        for ithr in "${ITHR[@]}"; do
            awk 'BEGIN {for (i=0;i<512;i+=64) print "-r " i}' | xargs ./thrscan.py --serial $daq -v $vcasn -w $vcasn2 -i $ithr &
        done
    done
done
