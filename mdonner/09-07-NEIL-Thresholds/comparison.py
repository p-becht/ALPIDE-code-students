#!/usr/bin/env python3.5
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import sys

plt.style.use('bmh')

csv = sys.argv[1]
csv2 = sys.argv[2]
RUN = sys.argv[3]
#Import data
VCASN, ITHR, TRSH, TRSH_Err = np.loadtxt(csv, skiprows=1, usecols=(2,3,4,5), delimiter=",", unpack=True)
VCASN2, ITHR2, TRSH2, TRSH2_Err = np.loadtxt(csv2, skiprows=1, usecols=(2,3,4,5), delimiter=",", unpack=True)

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
VCASN2_0 = getValues(VCASN2)
ITHR2_0 = getValues(ITHR2)

#######################################
Threshold2 = np.ndarray((len(ITHR2_0),len(VCASN2_0)))
for i in range(len(ITHR2_0)):
    for j in range(len(VCASN2_0)):
        Threshold2[i,j] = TRSH2[(ITHR2 == ITHR2_0[i]) & (VCASN2 == VCASN2_0[j])]
#######################################

#####  Do the same for the Error  #####
Threshold_Error2 = np.ndarray((len(ITHR2_0),len(VCASN2_0)))
for i in range(len(ITHR2_0)):
    for j in range(len(VCASN2_0)):
        Threshold_Error2[i,j] = TRSH2_Err[(ITHR2 == ITHR2_0[i]) & (VCASN2 == VCASN2_0[j])]
#######################################

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
                linewidth=.5,capsize=2,label='ITHR = '+str(ITHR_0[i]),zorder=0)
    else:
        ax.scatter(VCASN_0,Threshold[i],marker='.',color='black',zorder=1)
        ax.plot(VCASN_0,Threshold[i],linewidth=1,label='ITHR = '+str(ITHR_0[i]),zorder=0)
for i in range(len(ITHR2_0)):
    if (print_errors == "y"):
        ax.scatter(VCASN2_0,Threshold2[i],marker='.',s=5,color='blue',zorder=1)
        ax.errorbar(VCASN2_0,Threshold2[i],Threshold_Error2[i],
                linewidth=.5,capsize=2,zorder=0)
    else:
        ax.scatter(VCASN2_0,Threshold2[i],marker='.',color='blue',zorder=1)
        ax.plot(VCASN2_0,Threshold2[i],linewidth=1,zorder=0)
ax.scatter(VCASN2_0,Threshold2[0],marker='.',s=5,color='blue',zorder=1,label=csv2)
ax.scatter(VCASN_0,Threshold[0],marker='.',s=5,color='black',zorder=1,label=csv)

plt.legend()
plt.savefig("Graph.png",dpi=300)

plt.style.use('classic')
