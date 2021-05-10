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

# container for rate of one-plane events
one_p_e = [0,0,0,0,0,0,0]

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
def fill(num,container,plane=10):
    if num == 1:
        container[0] += 1
        if plane != 10:
            one_p_e[plane-1] += 1
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
                        # note event_number
                        event_num = int((dat[line-1].split(" ")[1]))

                    else:
                        # count the number of events in this run and reset counting
                        fill(iter,counts,plane_n)

                        # select eventnumber, for seven plane events
                        # if iter == 7:
                        #     print("seven plane event: "+str(event_num))
                        # if iter == 6:
                        #     print("six plane event: "+str(event_num))
                        # if iter == 5:
                        #     print("five plane event: "+str(event_num))
                        #if iter == 4:
                            #print(", "+str(event_num))
                        event_num = int((dat[line-1].split(" ")[1]))

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
    if i.startswith("Cosmics00"): #Cosmics000639
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
print(one_p_e)
#######################---SEQUENCING   SEQUENCING   SEQUENCING---###############################

# # print(np.array([1,4,5])==1)
# ### count the holes total, incl.DUT
# hole_1 = []
# hole_1_dut = []
# hole_2 = []
# hole_2_dut = []
# hole_3 = []
# hole_3_dut = []
# #go into a run 
# for j in range(len(run)):
#     holes=[[0,0,0,0,0,0],[0,0,0,0,0,0],[0,0,0,0,0,0],
#     [0,0,0,0,0,0],[0,0,0,0,0,0],[0,0,0,0,0,0]]
#     # go into a container of sequences with equal length
#     for k in range(len(seq[j])):
#         # check one event for holes
#         for i in range(len(seq[j][k])):
#             if seq[j][k][i]:

#                 # check for holes, if no holes: np.diff=[1,1,...,1]-> sum(np.diff(([j,k,i]))>1)= sum of holes
#                 num_holes = sum(np.diff((seq[j][k][i]))>1)
#                 # if at least one hole is there
#                 if num_holes > 0:
#                     if num_holes == 1:
#                         if sum(np.array(seq[j][k][i]) == 4) ==1:
#                             holes[3][k] += 1
#                         holes[0][k] += 1
#                     elif num_holes == 2:
#                         if sum(np.array(seq[j][k][i]) == 4) ==1:
#                             holes[4][k] += 1
#                         holes[1][k] += 1
#                     elif num_holes == 3:
#                         print(seq[j][k][i])
#                         if sum(np.array(seq[j][k][i]) == 4) ==1:
#                             holes[5][k] += 1
#                         holes[2][k] += 1
#     hole_1.append(holes[0])
#     hole_1_dut.append(holes[3])
#     hole_2.append(holes[1])
#     hole_2_dut.append(holes[4])
#     hole_3.append(holes[2])
#     hole_3_dut.append(holes[5])
    

# #######################---CSV   CSV   CSV   CSV---#############################


# ###creating a csv file
# with open("muon_data_processing/csv/n-event-plane-data.csv","w", newline="") as f:

#     #creating the the header
#     header = ["RunNumber"," pE_1", " pE_2", " pE_3", " pE_4", " pE_5", " pE_6", " pE_7"]
#     writing = csv.DictWriter(f, fieldnames= header)

#     # write the header in the file
#     writing.writeheader()
    
#     # put in the values
#     for j in range(len(run)):
#         writing.writerow({"RunNumber" : run[j]," pE_1" : pE[j][0], " pE_2" : pE[j][1], " pE_3" : pE[j][2],
#          " pE_4" : pE[j][3], " pE_5" : pE[j][4], " pE_6" : pE[j][5], " pE_7" : pE[j][6]})

# ###creating a csv file
# with open("muon_data_processing/csv/hits_per_plane.csv","w", newline="") as f:

#     #creating the the header
#     header = ["RunNumber"," hpP_1", " hpP_2", " hpP_3", " hpP_4", " hpP_5", " hpP_6", " hpP_7", " total_N"]
#     writing = csv.DictWriter(f, fieldnames= header)

#     # write the header in the file
#     writing.writeheader()
    
#     # put in the values
#     for j in range(len(run)):
#         writing.writerow({"RunNumber" : run[j]," hpP_1" : hits_plane[j][0], " hpP_2" : hits_plane[j][1],
#          " hpP_3" : hits_plane[j][2]," hpP_4" : hits_plane[j][3], " hpP_5" : hits_plane[j][4], " hpP_6" : hits_plane[j][5],
#           " hpP_7" : hits_plane[j][6], " total_N" : sum(hits_plane[j])})

# # csv file
# with open("muon_data_processing/csv/DUT_n-event-plane-data.csv","w", newline="") as f:

#     #creating the the header
#     header = ["RunNumber"," pE_1", " pE_2", " pE_3", " pE_4", " pE_5", " pE_6", " pE_7"]
#     writing = csv.DictWriter(f, fieldnames= header)

#     # write the header in the file
#     writing.writeheader()
    
#     # put in the values
#     for j in range(len(run)):
#         writing.writerow({"RunNumber" : run[j]," pE_1" : dut[j][0], " pE_2" : dut[j][1],
#          " pE_3" : dut[j][2], " pE_4" : dut[j][3], " pE_5" : dut[j][4], " pE_6" : dut[j][5],
#           " pE_7" : dut[j][6]})


# ###creating a csv file SEQUENCE
# with open("muon_data_processing/csv/event_HOLES-data.csv","w", newline="") as f:

#     #creating the the header
#     header = ["RunNumber"," s2_h1", " s3_h1", " s3_h2", " s4_h1", " s4_h2", " s4_h3",
#      " s5_h1", " s5_h2", " s6_h1"]
#     writing = csv.DictWriter(f, fieldnames= header)

#     # write the header in the file
#     writing.writeheader()
    
#     # put in the values
#     for j in range(len(run)):
#         writing.writerow({"RunNumber" : run[j]," s2_h1" : hole_1[j][0], " s3_h1" : hole_1[j][1],
#          " s3_h2" : hole_2[j][1], " s4_h1" : hole_1[j][2], " s4_h2" : hole_2[j][2],
#           " s4_h3" : hole_3[j][5], " s5_h1" : hole_1[j][3], " s5_h2" : hole_2[j][3],
#            " s6_h1" : hole_1[j][4]})

# ###creating a csv file SEQUENCE DUT
# with open("muon_data_processing/csv/event_HOLES_DUT-data.csv","w", newline="") as f:

#     #creating the the header
#     header = ["RunNumber"," s2_h1", " s3_h1", " s3_h2", " s4_h1", " s4_h2", " s4_h3",
#      " s5_h1", " s5_h2", " s6_h1"]
#     writing = csv.DictWriter(f, fieldnames= header)

#     # write the header in the file
#     writing.writeheader()
    
#     # put in the values
#     for j in range(len(run)):
#         writing.writerow({"RunNumber" : run[j]," s2_h1" : hole_1_dut[j][0], " s3_h1" : hole_1_dut[j][1],
#          " s3_h2" : hole_2_dut[j][1], " s4_h1" : hole_1_dut[j][2], " s4_h2" : hole_2_dut[j][2],
#           " s4_h3" : hole_3_dut[j][5], " s5_h1" : hole_1_dut[j][3], " s5_h2" : hole_2_dut[j][3],
#            " s6_h1" : hole_1_dut[j][4]})
