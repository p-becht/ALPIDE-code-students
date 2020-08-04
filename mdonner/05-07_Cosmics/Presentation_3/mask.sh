#!/bin/bash

#Checks Cosmic Data files for noisy pixels, that have not been previously masked

# Set this, so that for loops run over lines instead of words
IFS=$'\n'
set -f

Mask() {
    touch tmp
    touch tmp2
    echo "Searching for Multiple hits in $FILE..." > /dev/tty
    for i in $(cat "$FILE" | grep "Pixel"); do
	X=$(echo "$i" | awk -F ',' '{print $1}' | awk -F ' ' '{print $2}')
	Y=$(echo "$i" | awk -F ',' '{print $2}' | awk -F ' ' '{print $1}')
	COORD="$X $Y"
	if [[ $(cat tmp | grep -c "$COORD") == 10 ]]; then
	    #Search for the line this Pixel appeared
	    LINE=$(cat "$FILE" | grep -n "$i" | tail -n 1 | awk -F ':' '{print $1}')
	    #Go back line by line to find the plane
	    while true; do
		PLANE=$(sed "${LINE}q;d" $FILE)
		if [[ "$PLANE" == *"---"* ]]; then
		    PLANENO=$(echo "$PLANE" | awk -F ' ' '{print $2}' | awk -F '_' '{print $2}')
		    echo "Pixel $X, $Y on plane $PLANENO seems to be noisy" >> tmp2
		    break
		fi
		LINE=$((LINE-1))
	    done
	elif [[ $(cat tmp | grep -c "$COORD") == 100 ]]; then
	    echo "Pixel $X, $Y is very noisy. Please mask before running this script again to avoid long execution times..." >> tmp2
	    break 
	fi
	echo "$COORD" >> tmp
    done
    if [[ $(cat tmp2) != "" ]]; then
	cat tmp2 > /dev/tty
    else
	echo "No noisy pixels found in run $FILENAME" > /dev/tty
    fi
    rm tmp tmp2
}


# Check the Argument given
if [[ "$1" == "" ]]; then
    echo "The argument is missing. Please enter a file or directory"
    exit 1

elif [ -d $1 ]; then ### IN CASE THE ARGUMENT IS A DIRECTORY
    CURRENT=$(pwd)
    cd "$1"

    # Loop over all files in the given Directory
    for file in $(ls | grep '.txt'); do
	FILE=$file
	$(Mask)
    done
    cd "$CURRENT"

elif [ -s $1 ]; then ### IN CASE THE ARGUMENT IS A FILE
    FILE=$1
    $(Mask)

else
    echo "Error: $1 is neither a file nor a directory"
    exit 1
fi
