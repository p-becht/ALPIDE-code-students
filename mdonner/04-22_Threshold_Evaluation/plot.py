#!/usr/bin/env python3.5
import numpy as np
import matplotlib.pyplot as plt
import sys

csv = sys.argv[1]
#Import data
VCASN, ITHR, TRSH = np.loadtxt(csv, skiprows=1, usecols=(1,2,3), delimiter=",", unpack=True)

#These arrays list entries multiple times. What will be beneficial for this
#Kind of plot is, to have ITHR and VCASN as X and Y axis respectively,
#While having all entries for the Threshold in one array. For that,
#the values need to be sorted!

#The same values of VCASN and ITHR appear multiple times in the csv. Extract them
def getValues(array):
    #Create a temporary list
    temp = []
    #Write each unique entry into the temporary list
    for i in array:
        if i in temp: continue
        else: temp.append(i)
    #Since the array of values in this case is quite small, we can use temp.sort
    temp.sort()
    output = np.ndarray((len(temp)),dtype=int)
    for i in range(len(temp)):
        output[i] = int(temp[i])
    return output

VCASN_0 = getValues(VCASN)
ITHR_0 = getValues(ITHR)

##### Implement sorting algorithm #####
Threshold = np.ndarray((len(VCASN_0),len(ITHR_0)))
for i in range(len(VCASN_0)):
    for j in range(len(ITHR_0)):
        Threshold[i,j] = TRSH[(VCASN == VCASN_0[i]) & (ITHR == ITHR_0[j])]
#######################################

for png in ["Heatmap.png","Heatmap_soft.png"]:
    
    #Plotting
    fig, ax = plt.subplots()
    im = ax.imshow(Threshold)

    ##Show all ticks and label them correctly
    ax.set_xticks(np.arange(len(ITHR_0)))
    ax.set_yticks(np.arange(len(VCASN_0)))
    ax.set_xticklabels(ITHR_0)
    ax.set_yticklabels(VCASN_0)
    ax.set_xlabel("ITRH")
    ax.set_ylabel("VCASN")

    for i in range(len(VCASN_0)):
        for j in range(len(ITHR_0)):
            text = ax.text(j,i,round(Threshold[i,j],1),ha="center", va="center", color="w")
    ax.set_title("Threshold in DAC values")

    plt.savefig(png)

    plt.style.use('classic')
