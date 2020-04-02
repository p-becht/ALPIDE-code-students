#!/usr/bin/python

# loading packages
import sys
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import os
import tempfile
import subprocess


### import the datasheet,where all test-configurations are given
datafile = os.path.join(os.path.split(os.path.dirname(__file__))[0],"data","thresh_scan_CHIP8_VBB3","threshold_scan_results.txt")
RN,BBV,VCASN,ITHR,MEAN = np.loadtxt(datafile ,delimiter=" ",usecols=(0,2,3,4,6),skiprows=1,unpack=True)
### THRESH is the resulting Treshold for the given parameters calculated with tresh.py
VCASN2 = VCASN +12

### build the figure so that nothing will truncated
plt.figure(figsize=(12,10))
plt.subplots_adjust(bottom=0.28,top=0.95)
### set the parameters as x-axis-values
x_ax_values= np.array([BBV,VCASN,VCASN2,ITHR])    # made a matrix of all parameter information we want to display
x_ax_values = np.swapaxes(x_ax_values,0,1)        # swap axis, so we get a parameter boundle in every row we can identify with the x axis value
x_ax_values.astype(int)
plt.xticks(RN,x_ax_values)                        # replace RN with our parameters as axis-values
### rotate the presentation of the x-axis-values to ensure they wont overlap
plt.xticks(rotation=90)
### plotting
plt.plot(RN,MEAN,marker=".",linewidth=0)
plt.xlabel("parameters: [Backbias. VCASN. VCASN2. ITHR]")
plt.ylabel("Calculated mean threshold [DAC]")
plt.title("Threshold for various parameter settings")
plt.show()

