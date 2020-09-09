#!/bin/bash

#For scanning multiple planes
DAQS=$(lsusb -v | grep -o DAQ-.*)

###### Type in ranges here #######
VCASN=(133 129 125 121 117 113 109 105)
ITHR=(30 40 50 60 70)
##################################

for vcasn in "${VCASN[@]}"; do
    vcasn2=$((VCASN+12))
    for ithr in "${ITHR[@]}"; do
        #Scan only Chip in slot 3 (DUT)
        awk 'BEGIN {for (i=0;i<512;i+=64) print "-r " i}' | xargs ../scans/thrscan.py -v $vcasn -w $vcasn2 -i $ithr --serial DAQ-000904250102133C --vclip 60 --dvmax 50
    done
done

PATHTOFILES="../scans/"

#Create a Directory for the Analysis
CHIPBB=$1
if [[ "$CHIPBB" == "" ]]; then
    echo "Please enter the Name of the Chip and the applied Back Bias like R24_3"
    read CHIPBB
fi
mkdir $CHIPBB

CHIP=$(echo "$CHIPBB" | awk -F '_' '{print $1}')
VBB=$(echo "$CHIPBB" | awk -F '_' '{print $2}')
echo "Chip,VBB,VCASN,ITHR,Threshold,ThresholdErr" >> output.csv

#Start to loop over all N files
for i in $(ls $PATHTOFILES | grep '.raw'); do
    FILENAME=$(echo $i | rev | cut -c5- | rev)

    V=$(cat "$FILENAME.json" | awk -F ' ' '{print $6}')
    I=$(cat "$FILENAME.json" | awk -F ' ' '{print $10}')
    #Then use python script to calculate Threshold for that run
    printf "Starting evaluation for run $FILENAME \n"

    #Write Threshold into tmp file
    ../analysis/thrscanana.py "$PATHTOFILES$FILENAME.raw" "$PATHTOFILES$FILENAME.json" >> tmp
    TRSH=$(cat tmp | tail -n 1 | awk -F ' ' '{print $2}' | awk -F ' ' '{print $1}')
    TRSHERR=$(cat tmp| tail -n 1 | awk -F ' ' '{print $4}' | awk -F ' ' '{print $1}')
    echo "The Threshold of $FILENAME is $TRSH +- $TRSHERR"
    echo "$CHIP,$VBB,$V""$I""$TRSH,$TRSHERR" >> output.csv

    if [[ "$TRSH" == "" ]] | [[ "$TRSH" == "nan" ]]; then
        TRSH=0
        TRSHERR=0
    fi
    mv *.npy "$CHIPBB/$FILENAME.npy"
    mv "$FILENAME.raw" "$CHIPBB/"
    mv "$FILENAME.json" "$CHIPBB/"
    rm tmp
done

../analysis/plot.py output.csv $CHIPBB
../analysis/newplot.py output.csv $CHIPBB
mv "Graph.png" "$CHIPBB/"
mv "Heatmap.png" "$CHIPBB/"
mv "output.csv" "$CHIPBB/"
