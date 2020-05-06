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
#datafile = os.path.join(os.path.split(os.path.split(os.path.dirname(__file__))[0])[0],"mdonner","04-22_Threshold_Evaluation","bb0_output.csv")
#datafile2 = os.path.join(os.path.split(os.path.split(os.path.dirname(__file__))[0])[0],"mdonner","04-22_Threshold_Evaluation","bb3_output.csv")
datafile = os.path.join(os.path.split(os.path.dirname(__file__))[0],"Internship_work","plotting_Thr=0V.csv")
datafile2= os.path.join(os.path.split(os.path.dirname(__file__))[0],"Internship_work","plotting_Thr=3V.csv")
VCASN,ITHR,MEAN,ERR = np.loadtxt( datafile ,delimiter=",",usecols=(1,2,3,4),skiprows=1,unpack=True)
vcasn,ithr,mean,err = np.loadtxt( datafile2 ,delimiter=",",usecols=(1,2,3,4),skiprows=31,unpack=True) 
print(mean,err)
#####################xAPPNG SHIT
##first csv has only BB = 0, need length of the doc
BB     = VCASN*0
bb     = vcasn*0+3

#runnumber:
RN = np.arange(0,len(BB))
rn = np.arange(0,len(bb))

### THRESH is the resulting Treshold for the given parameters calculated with tresh.py
VCASN2 = VCASN +12
vcasn2 = vcasn +12



####################################################plot for BB=0V
### build the figure so that nothing will truncated
plt.figure(figsize=(12,10))
plt.subplots_adjust(bottom=0.28,top=0.95)
### set the parameters as x-axis-values
x_ax_values= np.array([BB,VCASN,VCASN2,ITHR])    # made a matrix of all parameter information we want to display
x_ax_values = np.swapaxes(x_ax_values,0,1)        # swap axis, so we get a parameter boundle in every row we can identify with the x axis value
x_ax_values.astype(int)
plt.xticks(RN,x_ax_values)                        # replace RN with our parameters as axis-values
### rotate the presentation of the x-axis-values to ensure they wont overlap
plt.xticks(rotation=90)
### plotting
plt.errorbar(RN, MEAN, yerr=ERR, fmt='.k')
#plt.plot(RN,MEAN,marker=".",linewidth=0)
plt.xlabel("parameters: [Backbias. VCASN. VCASN2. ITHR]")
plt.ylabel("Calculated mean threshold [DAC]")
plt.title("Threshold for various parameter settings and Backbias of 0V",fontsize=20)
plt.show()

####################################################plot for BB=3V
### build the figure so that nothing will truncated
plt.figure(figsize=(12,10))
plt.subplots_adjust(bottom=0.28,top=0.95,left=0.07,right=0.98)
### set the parameters as x-axis-values
x_ax_values= np.array([bb,vcasn,vcasn2,ithr])    # made a matrix of all parameter information we want to display
x_ax_values = np.swapaxes(x_ax_values,0,1)        # swap axis, so we get a parameter boundle in every row we can identify with the x axis value
x_ax_values.astype(int)
plt.xticks(rn,x_ax_values)                        # replace RN with our parameters as axis-values
### rotate the presentation of the x-axis-values to ensure they wont overlap
plt.xticks(rotation=90)
### plotting
plt.errorbar(rn, mean, yerr=err, fmt='.k')
#plt.plot(rn,mean,marker=".",linewidth=0)
plt.xlabel("parameters: [Backbias. VCASN. VCASN2. ITHR]")
plt.ylabel("Calculated mean threshold [DAC]")
plt.title("Threshold for various parameter settingsand Backbias of 3V",fontsize=20)
plt.show()