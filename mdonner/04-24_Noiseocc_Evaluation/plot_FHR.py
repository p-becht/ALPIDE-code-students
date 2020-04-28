#!/usr/bin/python3.5

import numpy as np
import matplotlib.pyplot as plt

csv_Threshold_0 = "../04-22_Threshold_Evaluation/bb0_output.csv"
csv_Noise_0 = "0bb_output.csv"
csv_Threshold_3 = "../04-22_Threshold_Evaluation/bb3_output.csv"
csv_Noise_3 = "3bb_output.csv"

# {{{ Functions

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
    output = np.ndarray((len(VCASN_in)*len(ITHR_in)))
    for i in range(len(VCASN_in)):
        for j in range(len(ITHR_in)):
            output[j+len(ITHR_in)*i] = array[(VCASN == VCASN_in[i]) & (ITHR == ITHR_in[j])]
    return output

#Function to check for mismatched Elements between Arrays
def mismatch(VCASN_1, VCASN_2, ITHR_1, ITHR_2):
    for i in range(len(VCASN_1)):
        if (VCASN_1[i] - VCASN_2[i]) == 0: continue
        print("Error, mismatched element in VCASN detected")
    for i in range(len(ITHR_1)):
        if (ITHR_1[i] - ITHR_2[i]) == 0: continue
        print("Error, mismatched element in ITHR detected")

# }}}

# {{{ Data

#Load 0bb Threshold Data
VCASN, ITHR, Threshold = np.loadtxt(csv_Threshold_0, skiprows=1, usecols=(1,2,3), delimiter=",", unpack=True)
VCASN_0 = getValues(VCASN)
ITHR_0 = getValues(ITHR)
TRSH_0 = ExtractValues(VCASN_0,ITHR_0,Threshold)

#Load 0bb Noiseocc Data
VCASN, ITHR, Ntriggers, Noise, Noise_Err = np.loadtxt(csv_Noise_0, skiprows=1, usecols=(1,2,3,4,5), delimiter=",", unpack=True)
VCASN_0_Noise = getValues(VCASN)
ITHR_0_Noise = getValues(ITHR)
NOISE_0 = ExtractValues(VCASN_0_Noise,ITHR_0_Noise,Noise)
NOISE_0_ERR = ExtractValues(VCASN_0_Noise,ITHR_0_Noise,Noise_Err)

#Check for mismatched values
mismatch(VCASN_0, VCASN_0_Noise, ITHR_0, ITHR_0_Noise)

#Load 3bb Threshold Data
VCASN, ITHR, Threshold = np.loadtxt(csv_Threshold_3, skiprows=1, usecols=(1,2,3), delimiter=",", unpack=True)
VCASN_3 = getValues(VCASN)
ITHR_3 = getValues(ITHR)
TRSH_3 = ExtractValues(VCASN_3,ITHR_3,Threshold)

#Load 3bb NoiseOcc Data
VCASN, ITHR, Ntriggers, Noise, Noise_Err = np.loadtxt(csv_Noise_3, skiprows=1, usecols=(1,2,3,4,5), delimiter=",", unpack=True)
VCASN_3_Noise = getValues(VCASN)
ITHR_3_Noise = getValues(ITHR)
NOISE_3 = ExtractValues(VCASN_3_Noise,ITHR_3_Noise,Noise)
NOISE_3_ERR = ExtractValues(VCASN_3_Noise,ITHR_3_Noise,Noise_Err)

#Check for mismatched values
mismatch(VCASN_0, VCASN_0_Noise, ITHR_0, ITHR_0_Noise)

# }}}

# {{{ Plot 

# Plot section #################################################################
senlim = min(NOISE_3)
x_val_0 = [x for x in TRSH_0]
y_val_0 = [y for y in NOISE_0]
y_err_0 = [y for y in NOISE_0_ERR]
x_val_3 = [x for x in TRSH_3]
y_val_3 = [y for y in NOISE_3]
y_err_3 = [y for y in NOISE_3_ERR]

plt.figure(figsize=(12,10))

#Plot 0bb 
plt.subplot(211)

plt.errorbar(x_val_0, y_val_0, xerr=None, yerr=y_err_0, fmt='.',capsize=3, c='red')

plt.yscale("log")
plt.xlim(1.5,26.5)
plt.ylim(1e-11,1e-4)
plt.hlines(senlim, min(x_val_0), max(x_val_0), linewidth=1, linestyle="--")
plt.text(22.5, 2*senlim, "Sensitivity Limit", fontsize=12)

plt.xlabel("Threshold in DAC Values")
plt.ylabel("Fake hit rate")
plt.title("V_BB = 0 V")

#Plot 3bb 
plt.subplot(212)

plt.errorbar(x_val_3, y_val_3, xerr=None, yerr=y_err_3, fmt='.' ,capsize=3, c='red')

plt.yscale("log")
plt.xlim(1.5,26.5)
plt.ylim(1e-11,1e-4)
plt.hlines(senlim, min(x_val_0), max(x_val_0), linewidth=1, linestyle="--")
plt.text(22.5, 2*senlim, "Sensitivity Limit", fontsize=12)

plt.xlabel("Threshold in DAC Values")
plt.ylabel("Fake hit rate")
plt.title("V_BB = 3 V")
plt.subplots_adjust(hspace=0.3)
plt.savefig("Fake_Hit_Rate.pdf")

# }}}
