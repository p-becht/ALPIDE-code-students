#!/bin/bash

#This script creates compressed versions of the large Cosmic data files.
#Synopsis: ./compress.sh [File or Directory]
#(Only .txt files)

################################ CODE ##########################################
Compress() {
    #For each line an event has taken place
    for i in $(cat $FILE | grep -n 'pALPIDEfs_' | awk -F ':' '{print $1}'); do
	LINENUM=$(($i-3)) #Remember the line number
	CHECKIFID=$(sed "${LINENUM}q;d" $FILE | cut -c 1) #Check for the Event ID
	#If line starts with =, we just found the EVENT ID
	if [[ $CHECKIFID == "=" ]] ; then
	    EVENTID=$(sed "$LINENUM""q;d" $FILE | awk -F ' ' '{print $2}')
	    echo "Event Found at === $EVENTID === on line $LINENUM! Writing..." > /dev/tty
	    echo "=== $LINENUM ===" >> "$OUTPUT"

	    IFS=$'\n' #This will let the loop run over lines instead of words
	    set -f
	    LINEEND=$(($i+1000))
	    for j in $(sed -n "$i,$LINEEND"p $FILE); do #Now write all pixel data to output
		LINE="$j" #This extracts the Lines from the .txt
		if [[ $LINE == *"==="* ]]; then #This checks start of next event
		    break #And stops the loop
		else
		    echo "$LINE" >> "$OUTPUT" #Otherwise it will write the line
		fi
	    done
	fi
    done
}
################################################################################

#Check the Argument given
if [[ "$1" == "" ]]; then
    echo "The argument is missing. Please enter a file or directory"
    exit 1
elif [ -d $1 ]; then ### IN CASE THE ARGUMENT IS A DIRECTORY
    CURRENT=(pwd)
    cd $1
    for file in $(ls | grep '.txt'); do
	FILE=$file
	printf "Compressing file $FILE\n"
	FILENAME=$(echo "$FILE" | awk -F '.' '{print $1}')
	OUTPUT="$FILENAME""_Compressed.txt"
	$(Compress)
    done
    cd $CURRENT
elif [ -s $1 ]; then ### IN CASE THE ARGUMENT IS A FILE
    printf "Compressing file $1\n"
    FILE=$1
    FILENAME=$(echo "$FILE" | awk -F '/' '{print $NF}' | awk -F '.' '{print $1}')
    OUTPUT="$FILENAME""_Compressed.txt"
    $(Compress)
else
    echo "Error: $1 is neither a file nor a directory"
    exit 1
fi
