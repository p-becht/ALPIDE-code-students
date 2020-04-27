#!/usr/bin/env/ python3.5

import numpy as np
import matplotlib.pyplot as plt

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

def (VCASN_ITHR):

#Load 0bb Data
vCASN, ITHR, TRSH = np.loadtxt("../0bb_output.csv", skiprows=1, usecols=(1,2,3), delimiter=",", unpack=True)
VCASN_0 = getValues(VCASN)
ITHR_0 = getValues(ITHR)

##### Implement sorting algorithm #####
Threshold = np.ndarray((len(VCASN_0),len(ITHR_0)))
for i in range(len(VCASN_0)):
    for j in range(len(ITHR_0)):
        Threshold[i,j] = TRSH[(VCASN == VCASN_0[i]) & (ITHR == ITHR_0[j])]
#######################################


