#!/bin/bash

if [[ "$1" == "" ]]; then
    echo "The argument is missing. Please enter a directory"
    exit 1
elif [ -s $1 ]; then
    FILE=$1
else
    echo "Error: $1 is empty"
    exit 1
fi

################################ CODE ##########################################

#For each line an event has taken place
for i in $(cat $1 | grep -n 'pALPIDEfs_' | awk -F ':' '{print $1}'); do
    LINENUM=$(($i-3))
    EVENTID=$(sed "$LINENUM""q;d" $FILE | awk -F ' ' '{print $2}')
    printf "i = $i\n"
    printf "Line number = $LINENUM\n"
    printf "Event ID = $EVENTID\n"
done
