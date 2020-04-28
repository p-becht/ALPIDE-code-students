#!/usr/bin/python3.6

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

#Function to extract Values in the right order
def ExtractValues(VCASN_in,ITHR_in,array):
    output = np.ndarray((len(VCASN_in),len(ITHR_in)))
    for i in range(len(VCASN_in)):
        for j in range(len(ITHR_in)):
            output[i,j] = array[(VCASN == VCASN_in[i]) & (ITHR == ITHR_in[j])]
    return output

#Load 0bb Data
VCASN, ITHR, Threshold = np.loadtxt("../04-22_Threshold_Evaluation/bb0_output.csv", skiprows=1, usecols=(1,2,3), delimiter=",", unpack=True)
VCASN_0 = getValues(VCASN)
ITHR_0 = getValues(ITHR)
TRSH = ExtractValues(VCASN_0,ITHR_0,Threshold)

# NoiseOcc data with 100000 Triggers
VCASN, ITHR, Noise, Noise_Err = np.loadtxt("0bb_output.csv", skiprows=1, usecols=(1,2,4,5), delimiter=",", unpack=True)
VCASN_0_Noise = getValues(VCASN)
ITHR_0_Noise = getValues(ITHR)
NOISE = ExtractValues(VCASN_0_Noise,ITHR_0_Noise,Noise)
NOISE_ERR = ExtractValues(VCASN_0_Noise,ITHR_0_Noise,Noise_Err)

#Check for mismatched Elements
for i in range(len(VCASN_0)):
    if (VCASN_0[i] - VCASN_0_Noise[i]) == 0: continue
    print("Error, mismatched element in VCASN detected")
for i in range(len(ITHR_0)):
    if (ITHR_0[i] - ITHR_0_Noise[i]) == 0: continue
    print("Error, mismatched element in ITHR detected")

#TODO Extract one value each from threshold and Noise and fit them NOICELY together
xaxis = []
yaxis = []
for i in range(len
