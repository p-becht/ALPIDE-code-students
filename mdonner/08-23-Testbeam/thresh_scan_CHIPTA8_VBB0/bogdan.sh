#!/bin/bash

PATHTOFILES=$1

IFS=$'\n'                                                                                                                                             
set -f

if [[ "$PATHTOFILES" == "" ]] ; then
    echo "Error: Please specify the directory of the data files"
    exit 1
fi

N=$(ls $PATHTOFILES | grep -c "" )
printf "Found $N files\n"

FILENAME=$(ls -l $PATHTOFILES | awk -F ' ' '{print $NF}' | tail -n +2)

for i in $FILENAME; do
    SED=$(sed '3q;d' $i)
    echo "$i $SED"
done
