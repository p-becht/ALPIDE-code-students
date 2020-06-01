#!/bin/bash

#This script creates compressed versions of the large Cosmic data files.
#Synopsis: ./compress.sh [File or Directory]
#(Only .txt files)

################################ CODE ##########################################
Compress() {

    # Set this, so that for loops run over lines instead of words
    IFS=$'\n'
    set -f

    # Write File Number
    echo "Run: $FILENAME" > "$OUTPUT"

    # For each line a hit has been found
    for i in $(cat $FILE | grep -n 'pALPIDEfs_' | awk -F ':' '{print $1}'); do

	# Take the line number
	LINENUM=$(($i-3))

	# If line starts with =, thats the line that contains the Event ID
	# This condition is necessary because there are multiple planes
	if [[ $(sed "${LINENUM}q;d" $FILE | cut -c 1) == "=" ]] ; then

	    # Print, in which Event a hit has been found
	    EVENTID=$(sed "$LINENUM""q;d" $FILE | awk -F ' ' '{print $2}')
	    echo "Event Found at === $EVENTID === on line $LINENUM! Writing..." > /dev/tty
	    echo "=== $EVENTID ===" >> "$OUTPUT"

	    # We want to count the number of planes that registered a hit
	    COUNTER=0 

	    # Now loop over each line, and stop when next event starts
	    LINEEND=$(($i+1000))
	    for j in $(sed -n "$i,$LINEEND"p $FILE); do

		# Extract the Line from the .txt
		LINE="$j"
		# If line is start of next event, break
		if [[ $LINE == *"==="* ]]; then
		    break

		# If a plane registered a hit, increase counter
		elif [[ $LINE == *"---"* ]]; then
		    COUNTER=$((COUNTER+1))
		    echo "$LINE" >> "$OUTPUT"

		# If cluster information is stored, planes are counted double!
		elif [[ $LINE == *"Cluster"* ]]; then
		    COUNTER=$((COUNTER-1))
		    echo "$LINE" >> "$OUTPUT"

		else
		    echo "$LINE" >> "$OUTPUT" #Otherwise it will write the line
		fi

	    done

	    # Write the counter into a tmpfile
	    echo "$COUNTER" >> tmp.txt
	fi
    done
}

# For each session, we count the number of 1 Plane hits, 2 Plane hits etc.
Countplanes() {
    #Array that contains an entry for each number of planes
    PLANES=(0 0 0 0 0 0)

    for i in $(cat tmp.txt); do	
	i=$((i-1))
	#Increase the variable for that amount of Planes by 1
	PLANES[$i]=$((PLANES[$i]+1))
    done

    #Write Everything into a csv
    printf '%s\n' "$FILENAME" "${PLANES[0]}" "${PLANES[1]}" "${PLANES[2]}" "${PLANES[3]}" "${PLANES[4]}" "${PLANES[5]}" "${PLANES[6]}" | paste -sd ',' >> output.csv

    rm tmp.txt
}
################################################################################

# Check the Argument given
if [[ "$1" == "" ]]; then
    echo "The argument is missing. Please enter a file or directory"
    exit 1

elif [ -d $1 ]; then ### IN CASE THE ARGUMENT IS A DIRECTORY
    CURRENT=$(pwd)
    cd "$1"

    echo "Run Number, 1 pE, 2 pE, 3 pE, 4 pE, 5 pE, 6 pE, 7 pE" >> output.csv
    # Loop over all files in the given Directory
    for file in $(ls | grep '.txt'); do
	FILE=$file
	FILENAME=$(echo "$FILE" | awk -F '.' '{print $1}')
	OUTPUT="$FILENAME""_Compressed.txt"
	$(Compress)
	$(Countplanes)
    done
    cd "$CURRENT"

elif [ -s $1 ]; then ### IN CASE THE ARGUMENT IS A FILE
    echo "Run Number, 1 pE, 2 pE, 3 pE, 4 pE, 5 pE, 6 pE, 7 pE" >> output.csv
    FILE=$1
    FILENAME=$(echo "$FILE" | awk -F '/' '{print $NF}' | awk -F '.' '{print $1}')
    OUTPUT="$FILENAME""_Compressed.txt"
    echo "Compressing file $FILE" > /dev/tty
    $(Compress)
    $(Countplanes)

else
    echo "Error: $1 is neither a file nor a directory"
    exit 1
fi
