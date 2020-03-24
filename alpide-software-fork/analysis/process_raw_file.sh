#!/bin/bash

## Experimental!! Should be similar to process_raw_dir.sh
## Written by Miljenko Suljic, m.suljic@cern.ch

if [ "$#" -lt 2 ]
then
    echo "------------------------------------"
    echo "Number of arguments not correct!"
    echo "  1) path to raw file"
    echo "  2) path to tree dir"
    echo "Optional:"
    echo "  3) path to mask file"
    echo "------------------------------------"
    exit 1
fi

DIR_ANALYSIS=${BASH_SOURCE%/*}
DIR_MACROS=$(readlink -f $DIR_ANALYSIS/clustering)/
DIR_CLASSES=$(readlink -f $DIR_ANALYSIS/classes)/
FILE_RAW=$1
FILENAME=`echo "$FILE_RAW" | grep -o "[0-9]\+_[0-9]\+"`
FILE_TREE="$2/$FILENAME.root"
FILE_IE="$2/$FILENAME_interesting_events.root"
if [ "$#" -ge 3 ] 
then
    if [ "$2" == "NULL" ] || [ "$2" == "0" ]
    then
        FILE_MASK=""
    else
        FILE_MASK=$DIR_RAW/$2
    fi
else
    FILE_MASK=""
fi

echo "------------------------------"
echo "Unique name:      $FILENAME"
echo "Raw file:         $FILE_RAW"
echo "Mask file:        $FILE_MASK"
echo "Tree file:        $FILE_TREE"
echo "Dir with macros:  $DIR_MACROS"
echo "Dir with classes: $DIR_CLASSES"
echo "------------------------------"

cd $DIR_CLASSES
root -l -b -q "compile_classes.C"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DIR_CLASSES

cd $DIR_MACROS

root -l -b -q "$DIR_CLASSES/load_classes.C" "csa.C+(\"$FILE_RAW\", \"$FILE_TREE\", \"$FILE_MASK\")"
root -l -b -q "$DIR_CLASSES/load_classes.C" "interesting_events.C+(\"$FILE_TREE\", \"$FILE_IE\")"
root -l       "$DIR_CLASSES/load_classes.C" "analysis_basic.C+(\"$FILE_TREE\", \"$2\")"
