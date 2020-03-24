#!/bin/bash

### ----------------------------------------------------------
##  Simple script that runs different analysis macros
##  Uses (almost) the same commands as 'runTests'
##     and executes a corresponding analysis macro
##  First parameter is the path to the datafile
##
##  e.g. ./x_macro.sh GEN /path/to/SouceScan_*_*.dat 
##       plots source hit map
##  e.g. ./x_macro.sh THRESHOLD /path/to/ThresholdScan_*_*.sh
##       plots scan results and calculates thresholds
##
##  Written by Miljenko Suljic, m.suljic@cern.ch
### ----------------------------------------------------------

if [ "$#" -lt 1 ]
then
    echo "----------------------------------"
    echo "Number of arguments not correct!"
    echo "  1) analysis macro (or --help)"
    echo "  2) path to data file (if needed)"
    echo "----------------------------------"
    exit 1
fi

if [ "$#" -ge 2 ] 
then
    DATA_FILE=$2
    if [ ! -f $DATA_FILE ]
    then
	    echo "Data file does not exist!"
	    exit 2
    fi
else
    DATA_FILE=""
fi

if   [ "$1" = "GEN" ] || [ "$1" = "GENERAL" ]; then 
    root -l 'classes/helpers.cpp+' 'pALPIDEfsRawToPlot.C+("'$DATA_FILE'")'
elif [ "$1" = "SCANDACS" ]; then
    root -l scanDACs.C+
elif [ "$1" = "NOISEOCCSCAN" ]; then
    root -l 'classes/helpers.cpp+' 'noiseOccScan2D.C+("'$DATA_FILE'")'
elif [ "$1" = "THRESHOLD" ]; then
    [ "$#" -ge 3 ] && MPAR2=$3 || MPAR2="0"
    [ "$#" -ge 4 ] && MPAR3=$4 || MPAR3="0"
    [ "$#" -ge 5 ] && MPAR4=$5 || MPAR4="0"
    root -l 'ThresholdNew.C+("'$DATA_FILE'", '$MPAR2', '$MPAR3', '$MPAR4')'
elif [ "$1" = "RAWHITS" ]; then
    root -l 'classes/helpers.cpp+' 'RawHitsPlots.C+("'$DATA_FILE'")'
elif [ "$1" = "--help" ]; then
    echo "----------------------------------------------------------------------"
    echo "--help       - this message"
    echo "All the following commands take data file path as the first parameter:"
    echo "GEN[ERAL]    - plots data in the format <Doublecol Address NHits>"
    echo "               (produced by e.g. SCANDIGITAL, NOISEOCC)"
    echo "SCANDACS     - plots data produced by SCANDACS"
    echo "NOISEOCCSCAN - plots data produced by NOISEOCCSCAN"
    echo "THRESHOLD    - plots data produced by THRESHOLD"
    echo "               [PAR2/3/4] - WriteToFile/ITH/VCASN, see ThresholdNew.C"
    echo "RAWHITS      - plots data from RawHits_*_*.dat"
    echo "----------------------------------------------------------------------"
else
    echo "----------------------------------"
    echo "No macro associated to command $1"
    echo "See --help for the list of macros."
    echo "----------------------------------"
    exit 3
fi
