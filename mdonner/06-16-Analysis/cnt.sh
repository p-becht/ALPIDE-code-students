#!/bin/bash

#This script creates compressed versions of the large Cosmic data files.
#Syntax: ./compress.sh [File or Directory]
#(Only .txt files)

################################ CODE ##########################################

# Set this, so that for loops run over lines instead of words
IFS=$'\n'
set -f

Compress() {

    touch tmp
    COUNTER=0

    for i in $(cat $FILE); do

	# If line starts with =, thats the line that contains the Event ID
	if [[ "$i" == *"==="* ]]; then

	    # Write the counter into a tmpfile
	    echo "$COUNTER" >> tmp
	    # Then reset it
	    COUNTER=0 

	    # Save EventID for analysis
	    EVENTID=$i

	# If a plane registered a hit, increase counter
	elif [[ "$i" == *"---"* ]]; then
	    COUNTER=$((COUNTER+1))
	    #if [ $COUNTER -eq 7 ]; then
	    #    echo "Seven plane event occured at $EVENTID" > /dev/tty
	    #fi

	# If cluster information is stored, planes are counted double!
	elif [[ "$i" == *"Cluster"* ]]; then
	    COUNTER=$((COUNTER-1))

	fi
    done

}

# For each session, we count the number of 1 Plane hits, 2 Plane hits etc.
Countplanes() {
    #Array that contains an entry for each number of planes
    PLANES=(0 0 0 0 0 0 0)

    # Count occurences
    for j in {0..6}; do
	PLANES[$j]=$(cat tmp | grep -c "$((j+1))")
    done

    #Write Everything into a csv
    printf '%s\n' "$FILENAME" "${PLANES[0]}" "${PLANES[1]}" "${PLANES[2]}" "${PLANES[3]}" "${PLANES[4]}" "${PLANES[5]}" "${PLANES[6]}" | paste -sd ',' >> $OUTPUT

    rm tmp
}
################################################################################

# Check the Argument given
if [[ "$1" == "" ]]; then
    echo "The argument is missing. Please enter a file or directory"
    exit 1

elif [ -d $1 ]; then ### IN CASE THE ARGUMENT IS A DIRECTORY
    CURRENT=$(pwd)
    OUTPUT="$CURRENT/output.csv"
    cd "$1"

    echo "Run Number, 1 pE, 2 pE, 3 pE, 4 pE, 5 pE, 6 pE, 7 pE" >> $OUTPUT
    # Loop over all files in the given Directory
    for file in $(ls | grep '.txt'); do
	FILE=$file
	FILENAME=$(echo "$FILE" | awk -F '.' '{print $1}')
	echo "Counting $FILENAME"
	$(Compress)
	$(Countplanes)
    done
    cd "$CURRENT"

elif [ -s $1 ]; then ### IN CASE THE ARGUMENT IS A FILE
    FILE=$1
    FILENAME=$(echo "$FILE" | awk -F '/' '{print $NF}' | awk -F '.' '{print $1}')
    OUTPUT="output.csv"
    echo "Run Number, 1 pE, 2 pE, 3 pE, 4 pE, 5 pE, 6 pE, 7 pE" >> $OUTPUT
    echo "Counting $FILENAME" > /dev/tty
    $(Compress)
    $(Countplanes)

else
    echo "Error: $1 is neither a file nor a directory"
    exit 1
fi
