#!/usr/bin/python

# loading packages
import sys
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import os
import tempfile
import subprocess
import csv

### we want to import the the txt files
# import the files and get the information out of them 

# get the path to read the files, U HAVE TO BE IN "dschledewitz"
data_path = os.path.join(os.path.split(os.path.split(os.path.dirname(__file__))[0])[0],"data","compressed")
# create container to store multi-plane-evnts
pE = []
run= []
# contatiner for the number of events in each plane
hits_plane = []

# container for DUT events
dut = []

# container to store all evetns sequenced
seq = []

###### defenition to get the value of a specific keyword from a file
def get_value(filename, key):

    # open file
    file = open(filename, "r")

    # read the lines
    for line in file.readlines():

        # find the line with the given keyword
        if line.startswith(key):

            # get the value corresponding to the keyword ("---") ("===")
            value = line.split()[1]
            return value

##### definition for filling data in the arrays
def fill(num,container):
    if num == 1:
        container[0] += 1
    elif num == 2:
        container[1] += 1
    elif num == 3:
        container[2] += 1
    elif num == 4:
        container[3] += 1
    elif num == 5:
        container[4] += 1
    elif num == 6:
        container[5] += 1
    elif num == 7:
        container[6] += 1


###### defenition to count the number of n-plane-events and save the sequence
def counter(filename):

    # initilize counter container
    global counts
    global sequence
    global count_seq
    sequence = []
    counts = [0,0,0,0,0,0,0]
    count_seq = [[],[],[],[],[],[]]

    # Keys
    key_event = "==="
    key_num = "--- pALPIDEfs_"

    # open file
    file = open(filename, "r")

    # create array
    dat = file.readlines()

    # starting variable
    startv = False

    # read the lines
    for line in range(len(dat)):
        # skip first 500 lines, since a double column fires without hit
        if line < 500:
            iter = 0

        else:
            # looking for hits, signed with keyword ("--- pALPIDEfs_") 
            if dat[line].startswith(key_num):

                ### check, if event started with this hit, signed with ("===")
                # if yes, start new event
                # if no, count hit to the current event

                if dat[line-1].startswith(key_event):

                    # exclude process for very first event
                    if startv == False:
                        startv = True
                        iter += 1

                        # get the number of the plane for this event
                        plane_n = int((dat[line].split("_")[1]).split(" ")[0])+1
                        sequence.append(plane_n)

                    else:
                        # count the number of events in this run and reset counting
                        fill(iter,counts)

                        # select only sequences with more than 1 entries
                        if len(sequence)>1:
                            count_seq[(len(sequence)-2)].append(sequence)
                        sequence = []
                        # get the number of the plane for this event
                        plane_n = int((dat[line].split("_")[1]).split(" ")[0])+1
                        sequence.append(plane_n)
                        iter = 1
                else:
                    # count as hit in the event
                    iter += 1
                    # get the number of the plane for this event
                    plane_n = int((dat[line].split("_")[1]).split(" ")[0])+1
                    sequence.append(plane_n)
    return counts, count_seq

#### definition to count the total number of hits in each plane --- pALPIDEfs_

def hitnum_plane(filename):

    # initilize counter container
    global hits
    hits = [0,0,0,0,0,0,0]
    key = "--- pALPIDEfs_"

    # open file
    file = open(filename, "r")

    # read the lines
    for line in file.readlines():

        # find the line with the given keyword
        if line.startswith(key):

            # get the number of the plane
            value = int((line.split("_")[1]).split(" ")[0])+1
            fill(value,hits)
    return hits


##### defintion for selecting events, which include DUT

def DUT_events(filename):

    # initilize counter container
    global counts
    counts = [0,0,0,0,0,0,0]

    # Keys
    key_plane = "---"
    key_event = "==="
    key_DUT = "--- pALPIDEfs_4"
    # open file
    file = open(filename, "r")

    # create array
    dat = file.readlines()

    # starting variable
    startv = False
    DUT = False

    # read the lines
    for line in range(len(dat)):
        # skip first 500 lines, since a double column fires without hit
        if line < 500:
            iter = 0

        else:
            # looking for hits, signed with keyword ("---") 
            if dat[line].startswith(key_plane):

                #### check, if event was from DUT
                if dat[line].startswith(key_DUT):
                    DUT = True
                ## check, if event started with this hit, signed with ("===")
                # if yes, start new event
                # if no, count hit to the current event

                if dat[line-1].startswith(key_event):

                    # exclude process for very first event
                    if startv == False:
                        startv = True
                        iter += 1

                    else:
                        # count the number of events in this run and reset counting
                        if DUT == True:
                            fill(iter,counts)
                            DUT = False
                        iter = 1
                else:
                    # count as hit in the event
                    iter += 1
    return counts



#######################---MAIN_CODE   MAIN_CODE   MAIN_CODE   MAIN_CODE---#############################

# search in the compressed folder for files (sorted alphabetically/numerically?)

for i in sorted(os.listdir(data_path)):

    # events stored in the .txt files beginning with "Cosmics"
    if i.startswith("Cosmics"):
        path = os.path.join(data_path,i)

        # split the path to get the Runnumber
        cosmic_run, file_type = i.split("_")
        runnumber = cosmic_run.split("smics")[1]
        RUN = "Run_"+runnumber

        # fill the counted events in the container
        count, Seq = counter(path)
        seq.append(Seq)
        pE.append(count)
        run.append(RUN)
        hits_plane.append(hitnum_plane(path))
        dut.append(DUT_events(path))


        Run_000639
        Run_000733