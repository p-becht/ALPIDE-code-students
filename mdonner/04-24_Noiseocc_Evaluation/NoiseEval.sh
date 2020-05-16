#!/bin/bash

#The goal with this script is to take in lots of Data files and automatically
#extract Data from them, and write them into an output file
#
#Syntax is ./NoiseEval.sh <PATH> [#TRIGGERS]
#
#This file needs to be in the same directory as FakeHit.py
#You"ll need to be able to run python scripts via shell (./script.py)
#for that to work, you might need to change the first line of the python script
#to #!/usr/bin/env pythonX.X (depending on your version)

############################# USER INPUT SECTION ###############################

#1st argument is path
PATHTOFILES=$1
if [[ "$PATHTOFILES" == "" ]] ; then
    echo "Error: Please specify the directory of the data files"
    exit 1
fi

NTRIGGERS=$2
if [[ "$NTRIGGERS" == "" ]] ; then
    echo "How many Triggers were used?"
    read NTRIGGERS
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
printf "Timestamp,VCASN,ITHR,NTRIGGERS, FHR, FHR_Error\n" >> output.csv

#Also need the Sensitivity limit, create temporary .dat file for that
touch tmp.dat
SENLIM=$(./FakeHit.py tmp.dat $NTRIGGERS | head -n 1)
rm tmp.dat

#Start to loop over all N files
for i in $(ls $PATHTOFILES | grep '.cfg'); do
    #define the files to be worked with by cutting out the Timestamp
    TIMESTAMP=$(echo $i | tail -c 18 | head -c 13)
    printf "Timestamp: $TIMESTAMP \n"
    CONFIG="$PATHTOFILES""ScanConfig_$TIMESTAMP.cfg"
    DAT="$PATHTOFILES""NoiseOccupancy_$TIMESTAMP.dat"
    #Extract Parameters from config file
    VCASN=$(cat $PATHTOFILES$i | grep 'VCASN' | awk -F ' ' '{print $2}' | head -1)
    ITHR=$(cat $PATHTOFILES$i | grep 'ITHR' | awk -F ' ' '{print $2}')

    #If config file is missing, fill column with senlim, otherwise, run script
    MISSING=$(ls $PATHTOFILES | grep "NoiseOccupancy_$TIMESTAMP.dat")
    if [[ "$MISSING" == "" ]]; then
	printf '%s\n' "$TIMESTAMP" "$VCASN" "$ITHR" "$SENLIM" "0" | paste -sd ',' >> output.csv
	echo "No hits registered, continuing"
    else
	#Then use python script to calculate Fake hit rate for that Timestamp
	printf "Starting evaluation for run $TIMESTAMP with ITHR=$ITHR and VCASN=$VCASN \n"
	FHR=$(./FakeHit.py $DAT $NTRIGGERS | head -n 1)
	DFHR=$(./FakeHit.py $DAT $NTRIGGERS | tail -n 1)
	printf "The calculated Fake hit rate is $FHR. Now writing to csv file...\n\n"
	# Write to csv file
	printf '%s\n' "$TIMESTAMP" "$VCASN" "$ITHR" "$NTRIGGERS" "$FHR" "$DFHR" | paste -sd ',' >> output.csv
    fi
done

echo Succeeded!
