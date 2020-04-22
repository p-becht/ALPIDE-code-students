#!/bin/bash

#The goal with this script is to take in lots of Data files and automatically
#extract Data from them, and write them into an output file
#
#This file needs to be in the same directory as thresh.py
#You"ll need to be able to run python scripts via shell (./script.py)
#for that to work, you might need to change the first line of the python script
#to #!/usr/bin/env pythonX.X (depending on your version)

############################# USER INPUT SECTION ###############################
#If run from original dir: ../../data/thresh_scan_CHIP8_VBB3/
PATHTOFILES=$1
if [[ "$PATHTOFILES" == "" ]] ; then
    echo "Error: Please specify the directory of the data files"
    exit 1
else
    echo "Counting files..."
fi

#Count, how many files are to be analyzed
N=$(ls -la $PATHTOFILES | grep -c -e '.dat')
printf "Found $N .dat files, proceed? [y/n]\n"
read PROCEED
if [[ "$PROCEED" == "y" ]] ; then
    echo "Creating csv file..."
elif [[ "$PROCEED" == "n" ]] ; then
    echo "Script Cancelled"
    exit 1
else
    echo "Input not understood. Script Cancelled"
    exit 1
fi
################################################################################


#Create a csv file
printf "Timestamp,VCASN,ITHR,Threshold [DAC]\n" >> output.csv

#Start to loop over all N files
for i in $(ls $PATHTOFILES | grep '.dat'); do
    #define the files to be worked with by cutting out the Timestamp
    TIMESTAMP=$(echo $i | tail -c 18 | head -c 13)
    printf "Timestamp: $TIMESTAMP \n"
    CONFIG="ScanConfig_$TIMESTAMP.cfg"
    
    #Then extract Parameters from config file (Later add VBB)
    VCASN=$(cat $PATHTOFILES$CONFIG | grep 'VCASN' | awk -F ' ' '{print $2}' | head -1)
    ITHR=$(cat $PATHTOFILES$CONFIG | grep 'ITHR' | awk -F ' ' '{print $2}')

    #Then use python script to calculate Threshold for that run
    printf "Starting evaluation for run $TIMESTAMP with ITHR=$ITHR and VCASN=$VCASN \n"
    TRSH=$(./thresh.py $PATHTOFILES$i)
    printf "The calculated threshold in DAC values is $TRSH. Now writing to csv file...\n\n"

    # Write everything to the csv file
    printf '%s\n' "$TIMESTAMP" "$VCASN" "$ITHR" "$TRSH" | paste -sd ',' >> output.csv
done

echo Succeeded!
