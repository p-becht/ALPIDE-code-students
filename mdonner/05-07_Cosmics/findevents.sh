#!/bin/bash

if [[ "$1" == "" ]]; then
    echo "The argument is missing. Please enter a directory"
    exit 1
elif [ -d $1 ]; then
    FILE=$1
else
    echo "Error: $1 is not a directory"
    exit 1
fi

################################ CODE ##########################################

#For each line an event has taken place
for i in $(cat $1 | grep -n 'pALPIDEfs_' | awk -F ':' '{print $1}')
    

