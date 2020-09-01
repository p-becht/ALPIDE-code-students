#!/usr/bin/env python3.5
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import pandas as pd
import sys

plt.style.use('bmh')

csv = sys.argv[1]
RUN = sys.argv[2]
Vrange=[45,67]
Trange=[0.79,39.98]
#Import data
VCASN, ITHR, TRSH, TRSH_Err = np.loadtxt(csv, skiprows=1, usecols=(1,2,3,4), delimiter=",", unpack=True)

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

#######################################
Threshold= np.ndarray((len(ITHR_0),len(VCASN_0)))
for i in range(len(ITHR_0)):
    for j in range(len(VCASN_0)):
        Threshold[i,j] = TRSH[(ITHR == ITHR_0[i]) & (VCASN == VCASN_0[j])]
#######################################

#####  Do the same for the Error  #####
Threshold_Error = np.ndarray((len(ITHR_0),len(VCASN_0)))
for i in range(len(ITHR_0)):
    for j in range(len(VCASN_0)):
        Threshold_Error[i,j] = TRSH_Err[(ITHR == ITHR_0[i]) & (VCASN == VCASN_0[j])]
#######################################

fig = plt.figure()
ax = fig.add_subplot(111)
plt.title("Threshold in Electrons for Run {}".format(RUN))
plt.xlabel("VCASN")
plt.ylabel("THRESHOLD [electrons]")

print("Include Errors? [y,n]")
print_errors = str(input())

for i in range(len(ITHR_0)):
    if (print_errors == "y"):
        ax.scatter(VCASN_0,Threshold[i],marker='.',s=5,color='black',zorder=1)
        ax.errorbar(VCASN_0,Threshold[i],Threshold_Error[i],
                linewidth=1,capsize=2,label='ITHR = '+str(ITHR_0[i]),zorder=0)
    else:
        ax.scatter(VCASN_0,Threshold[i],marker='.',color='black',zorder=1)
        ax.plot(VCASN_0,Threshold[i],label='ITHR = '+str(ITHR_0[i]),zorder=0)

ax.add_patch(
    patches.Rectangle(
    (Vrange[0], Trange[0]), # (x,y)
    Vrange[1]-Vrange[0], # width
    Trange[1]-Trange[0], # height
    # You can add rotation as well with 'angle'
    alpha=0.3, facecolor="red", edgecolor="black", linewidth=3, linestyle='solid'
    ))

plt.legend()
plt.savefig("Graph.png")

plt.style.use('classic')
