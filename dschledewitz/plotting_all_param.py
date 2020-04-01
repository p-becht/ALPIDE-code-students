#!/usr/bin/python

# loading packages
import sys
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import os
import tempfile
import subprocess

############# how to execute thresh.py for every Threshold data set automaticly?
############# we need to exec. thresh.py for every parameterset and join the result to the csv file
###### maybe one can search in the scanconfig files for the right parameterset and use the
###### timestamp to find the corresponding Threshhold file and then put this one in ?
##sounds complicated, but dont have a better idea yet

#import the datasheet,where all test-configurations are given
datafile = "Sheet.csv"
RN,BBV,VCASN,ITHR = np.loadtxt(datafile ,delimiter=",",usecols=(0,1,2,3),skiprows=1,unpack=True)
#THRESH is the resulting Treshold for the given parameters calculated with tresh.py
VCASN2 = VCASN +12

plt.figure(figsize=(20,20))
x_ax_values= np.array([BBV,VCASN,VCASN2,ITHR])
x_ax_values = np.swapaxes(x_ax_values,0,1)
#print(x_ax_values)
x_ax_values.astype(int)
plt.xticks(RN,x_ax_values)
plt.xticks(rotation=80)
plt.plot(RN,ITHR,marker=".",linewidth=0)
plt.xlabel("parameters: [Backbias. VCASN. VCASN2. ITHR]")
plt.ylabel("Calculated mean threshold")
plt.show()

