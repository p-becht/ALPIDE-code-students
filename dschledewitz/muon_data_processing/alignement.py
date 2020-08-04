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

# we want to import the the txt files
# import the files and get the information out of them

# get the path to read the files, U HAVE TO BE IN "dschledewitz"
data_path = os.path.join(os.path.split(os.path.split(os.path.dirname(__file__))[
                         0])[0], "dschledewitz", "muon_data_processing")

#######################---CONTAINER   CONTAINER   CONTAINER   CONTAINER---#############################
X_coordinates = []
Y_coordinates = []
Event = []
Eventsize = []
run = []
Diff_x = []
D_diff_x = []
Dsys_diff_x = []
Diff_y = []
D_diff_y = []
Dsys_diff_y = []
#######################---PRE-DEFINITIONS   PRE-DEFINITIONS   PRE-DEFINITIONS---#############################


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

# definition for filling data in the arrays


def fill(num, container):
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

# MEAN
def Mean(val, d_val):
    if val.size == 1:
        return int(val), 0   ,0
    else:
        mean = np.mean(val)
        stat = np.sqrt(1/(val.size*(val.size-1)) *
                    np.sum((mean-val)**2))  # statistischer Fehler
        sys = np.mean(d_val) #systematischer Fehler
        #print("Meanvalue:" , mean, " +- ", stat, "stat. +- ", sys, "sys")
        return mean, stat   , sys

# defenition to count the number of n-plane-events and save the sequence
def counter(filename):

    # initilize container
    # global event
    # global eventsize
    # global x_coordinates
    # global y_coordinates

    x_coordinates = []
    y_coordinates = []
    event = []
    eventsize = []

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
        # skip first 10 lines
        if line < 10:
            iter = 0
            x_coor = [[], [], [], [], [], [], []]
            y_coor = [[], [], [], [], [], [], []]
        # elif line > 1000:
        #     return event, eventsize, x_coordinates, y_coordinates

        else:
            # looking for hits, signed with keyword ("--- pALPIDEfs_")
            if dat[line].startswith(key_num):
                # check, if event started with this hit, signed with ("===")
                # if yes, start new event
                # if no, count hit to the current event
                if dat[line-3].startswith(key_event):
                    # exclude process for very first event
                    if startv == False:
                        startv = True
                        iter += 1
                        # get the number of the plane for this event
                        plane_n = int(
                            (dat[line].split("_")[1]).split(" ")[0])+1
                        # note event_number
                        event_num = int((dat[line-3].split(" ")[1]))
                        # x coordinates
                        k = 1
                        while dat[line+k].startswith("Pixel"):
                            x_coor[plane_n -
                                    1].append(int((dat[line+k].split(" ")[1]).split(",")[0]))
                            y_coor[plane_n -
                                    1].append(int((dat[line+k].split(" ")[2]).split(",")[0]))
                            k += 1

                    else:
                        # count the number of events in this run and reset counting
                        if iter >= 7:
                            event.append(event_num)
                            eventsize.append(iter)
                            x_coordinates.append(x_coor)
                            y_coordinates.append(y_coor)

                        event_num = int((dat[line-3].split(" ")[1]))

                        # get the number of the plane for this event
                        plane_n = int(
                            (dat[line].split("_")[1]).split(" ")[0])+1
                        # note event_number
                        event_num = int((dat[line-3].split(" ")[1]))
                        # x coordinates
                        k = 1
                        x_coor = [[], [], [], [], [], [], []]
                        y_coor = [[], [], [], [], [], [], []]
                        iter = 1

                        while dat[line+k].startswith("Pixel"):
                            x_coor[plane_n -
                                    1].append(int((dat[line+k].split(" ")[1]).split(",")[0]))
                            y_coor[plane_n -
                                    1].append(int((dat[line+k].split(" ")[2]).split(",")[0]))
                            k += 1


                else:
                    # count as hit in the event
                    iter += 1
                    # get the number of the plane for this event
                    plane_n = int((dat[line].split("_")[1]).split(" ")[0])+1
                    # x coordinates
                    k = 1
                    while dat[line+k].startswith("Pixel"):
                        x_coor[plane_n -
                                1].append(int((dat[line+k].split(" ")[1]).split(",")[0]))
                        y_coor[plane_n -
                                1].append(int((dat[line+k].split(" ")[2]).split(",")[0]))
                        k += 1

    return event, eventsize, x_coordinates, y_coordinates


# definition to find the offset of the planes
def align(x_coordinates, y_coordinates):

    # container
    mean_x = [] #np.array([])
    mean_y = [] #np.array([])
    d_mean_x = [] #np.array([])
    d_mean_y = [] #np.array([]
    diff_x = [] #np.array([])
    diff_y = [] #np.array([])
    d_diff_x = [] #np.array([])
    d_diff_y = [] #np.array([])
    mean_diff_x = [] #np.array([])
    mean_diff_y = [] #np.array([])
    mean_d_diff_x = [] #np.array([])
    mean_d_diff_y = [] #np.array([])
    mean_dsys_diff_x = [] #np.array([])
    mean_dsys_diff_y = [] #np.array([])

    p = 0
    for i in range(len(x_coordinates)):
        X = np.array([])
        dX = np.array([])
        Y = np.array([])
        dY = np.array([])
        good_event = True
        for j in range(0, 7):
            # get the pixel coordinates of one event and calculate the mean
            x,dx,dxs = Mean(np.array(x_coordinates[i][j]),0)
            y,dy,dys = Mean(np.array(y_coordinates[i][j]),0)
            # cutting off bad events(multiple uncorrelated events)
            if dx > 15 or dy > 15:
                good_event = False
            else:
                X = np.append(X,x)
                dX = np.append(dX,dx)
                Y = np.append(Y,y)
                dY = np.append(dY,dy)
        # take hole event to container
        if good_event == False:
            p += 1
            continue
        mean_x.append(X)
        d_mean_x.append(dX)
        mean_y.append(Y)
        d_mean_y.append(dY)

    #for i in range(len(x_coordinates)):
        # calculate shift relative to first plane
        shift_X = np.array([])
        shift_dX = np.array([])
        shift_Y = np.array([])
        shift_dY = np.array([])
        for j in range(0,7):
            shift_X = np.append(shift_X,mean_x[i-p][j]-mean_x[i-p][0])
            shift_dX = np.append(shift_dX,np.sqrt(d_mean_x[i-p][j]**2+d_mean_x[i-p][0]**2))
            shift_Y = np.append(shift_Y,mean_y[i-p][j]-mean_y[i-p][0])
            shift_dY = np.append(shift_dY,np.sqrt(d_mean_y[i-p][j]**2+d_mean_y[i-p][0]**2))
        diff_x.append(shift_X)
        diff_y.append(shift_Y)
        d_diff_x.append(shift_dX)
        d_diff_y.append(shift_dY)
    #print(diff_y)

    # now take mean of the shift to calculate the final alignement
    for i in range(0,7):
        final_x, d_final_x, dsys_final_x = Mean(np.array(diff_x)[:,i], np.array(d_diff_x)[:,i])
        final_y, d_final_y, dsys_final_y = Mean(np.array(diff_y)[:,i], np.array(d_diff_y)[:,i])

        mean_diff_x.append(final_x)
        mean_d_diff_x.append(d_final_x)
        mean_diff_y.append(final_y)
        mean_d_diff_y.append(d_final_y)
        mean_dsys_diff_x.append(dsys_final_x)
        mean_dsys_diff_y.append(dsys_final_y)
    return mean_diff_x, mean_d_diff_x, mean_dsys_diff_x, mean_diff_y, mean_d_diff_y, mean_dsys_diff_y

#######################---MAIN_CODE   MAIN_CODE   MAIN_CODE   MAIN_CODE---#############################

# search in the compressed folder for files (sorted alphabetically/numerically?)

for i in sorted(os.listdir(data_path)):

    # events stored in the .txt files beginning with "Cosmics"
    if i.startswith("desy2"):
        path = os.path.join(data_path, i)

        # split the path to get the Runnumber
        Run = i.split("y")[1]  # i.split("_")
        RUN = "Run_"+Run

        # fill the counted events in the container
        event, eventsize, x_coordinates, y_coordinates = counter(path)
        diff_x, d_diff_x, dsys_diff_x, diff_y, d_diff_y, dsys_diff_y = align(x_coordinates, y_coordinates)

        Event.append(event)
        Eventsize.append(eventsize)
        run.append(RUN)
        X_coordinates.append(x_coordinates)
        Y_coordinates.append(y_coordinates)
        Diff_x.append(diff_x)
        D_diff_x.append(d_diff_x)
        Dsys_diff_x.append(dsys_diff_x)
        Diff_y.append(diff_y)
        D_diff_y.append(d_diff_y)
        Dsys_diff_y.append(dsys_diff_y)

print(Diff_x)
print(D_diff_x)
print(Dsys_diff_x)
print(Diff_y)
print(D_diff_y)
print(Dsys_diff_y)

planas = np.arange(1,8)
plt.figure(figsize=(12, 10))
plt.grid(which="both", axis="both")
plt.errorbar(planas, Diff_x[0], yerr=D_diff_x[0], xerr=0.5, fmt='green',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label="x-axis")
plt.errorbar(planas, Diff_y[0], yerr=D_diff_y[0], xerr=0.5, fmt='blue',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label="y-axis")
plt.xlabel("plane")
plt.ylabel("X-shift [pixels]")
plt.title("X-axis-shift relative to plane 1", fontsize=20)
plt.legend()
plt.show()

plt.figure(figsize=(12, 10))
plt.grid(which="both", axis="both")
plt.errorbar(planas, Diff_x[0], yerr=Dsys_diff_x[0], xerr=0.5, fmt='green',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label="x-axis")
plt.errorbar(planas, Diff_y[0], yerr=Dsys_diff_y[0], xerr=0.5, fmt='blue',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label="y-axis")
plt.xlabel("plane")
plt.ylabel("Y-shift [pixels]")
plt.title("Y-axis-shift relative to plane 1", fontsize=20)
plt.legend()
plt.show()






#######################---CSV   CSV   CSV   CSV   CSV   CSV   CSV   CSV---#############################

planas = np.arange(1,8)
###creating a csv file
with open("muon_data_processing/csv/alignement_data.csv","w", newline="") as f:

    #creating the the header
    header = ["Planenumber"," x", " dx", " dsysx", " y", " dy", " dsysy"]
    writing = csv.DictWriter(f, fieldnames= header)

    # write the header in the file
    writing.writeheader()
    
    # put in the values
    for j in range(len(planas)):
        writing.writerow({"Planenumber" : planas[j]," x" : Diff_x[0][j],
         " dx" : D_diff_x[0][j], " dsysx" : Dsys_diff_x[0][j],
         " y" : Diff_y[0][j], " dy" : D_diff_y[0][j], " dsysy" : Dsys_diff_y[0][j]})