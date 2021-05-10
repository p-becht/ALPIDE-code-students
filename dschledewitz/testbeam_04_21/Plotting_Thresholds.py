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
L0_Bottom= os.path.join(os.path.split(os.path.dirname(__file__))[0],"testbeam_04_21","python","ref-thr-L0_BOTTOM_vbb0.csv")
L0_Top= os.path.join(os.path.split(os.path.dirname(__file__))[0],"testbeam_04_21","python","ref-thr-L0_TOP_vbb0.csv")
L1_Top= os.path.join(os.path.split(os.path.dirname(__file__))[0],"testbeam_04_21","python","ref-thr-L1_TOP_vbb0.csv")
L2_Top= os.path.join(os.path.split(os.path.dirname(__file__))[0],"testbeam_04_21","python","ref-thr-L2_TOP_vbb0.csv")

vcasn0b,ithr0b,mean0b,err0b = np.loadtxt( L0_Bottom ,delimiter=",",usecols=(2,3,4,5),skiprows=1,unpack=True)
vcasn0t,ithr0t,mean0t,err0t = np.loadtxt( L0_Top ,delimiter=",",usecols=(2,3,4,5),skiprows=1,unpack=True)
vcasn1t,ithr1t,mean1t,err1t = np.loadtxt( L1_Top ,delimiter=",",usecols=(2,3,4,5),skiprows=1,unpack=True)
vcasn2t,ithr2t,mean2t,err2t = np.loadtxt( L2_Top ,delimiter=",",usecols=(2,3,4,5),skiprows=1,unpack=True)
vcasn,ithr,mean,err = vcasn0b,ithr0b,mean0b,err0b
name = ["L0_Bottom","L0_Top","L1_Top","L2_Top",]

##first csv has only BB = 0, need length of the doc
# BB     = VCASN*0
# bb     = vcasn*0+3

#runnumber:
# RN = np.arange(0,len(BB))
# rn = np.arange(0,len(bb))

### THRESH is the resulting Treshold for the given parameters calculated with tresh.py
# VCASN2 = VCASN +12
# vcasn2 = vcasn +12



##
####################################################plot for BB=0V
### build the figure so that nothing will truncated
plt.figure(figsize=(12,6))
# plt.subplots_adjust(bottom=0.28,top=0.95,left=0.07,right=0.98)
# vcasn0b,ithr0b,mean0b,err0b 
# vcasn0t,ithr0t,mean0t,err0t 
# vcasn1t,ithr1t,mean1t,err1t
# vcasn2t,ithr2t,mean2t,err2t

THR = 60#np.array([40,50,60])
print(ithr2t,vcasn2t)
# for j in len(THR):
a= False
b= False
d= False
e= False

for i in range(len(vcasn0b)):
    if ithr0b[i] == THR and a == False:
        plt.errorbar( vcasn0b[i], mean0b[i], fmt=".k",
            elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= name[0])
        # print(a)
        a = True
    elif ithr0b[i] == THR:
        plt.errorbar( vcasn0b[i], mean0b[i], fmt=".k",
            elinewidth=1.5, lw=0, capsize=3, capthick=1.5)
for i in range(len(vcasn0t)):
    if ithr0t[i] == THR and b == False:
        plt.errorbar( vcasn0t[i], mean0t[i], fmt=".b",
            elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= name[1])
        b = True
    elif ithr0t[i] == THR:
        plt.errorbar( vcasn0t[i], mean0t[i], fmt=".b",
            elinewidth=1.5, lw=0, capsize=3, capthick=1.5)
for i in range(len(vcasn1t)):
    if ithr1t[i] == THR and d == False:
        plt.errorbar( vcasn1t[i], mean1t[i], fmt=".r",
            elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= name[2])
        d = True
    elif ithr1t[i] == THR:
        plt.errorbar( vcasn1t[i], mean1t[i], fmt=".r",
            elinewidth=1.5, lw=0, capsize=3, capthick=1.5)
for i in range(len(vcasn2t)):
    if ithr2t[i] == THR and e == False:
        plt.errorbar( vcasn2t[i], mean2t[i], fmt=".y",
        elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= name[3])
        e = True
    elif ithr2t[i] == THR:
        plt.errorbar( vcasn2t[i], mean2t[i], fmt=".y",
            elinewidth=1.5, lw=0, capsize=3, capthick=1.5)

    # if ithr0b[i] == 50:
    #     plt.errorbar( vcasn0b[i], mean0b[i], yerr=err0b[i], fmt=".b",
    #         elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "ITHR = 50")
    # if ithr0b[i] == 60:
    #     plt.errorbar( vcasn0b[i], mean0b[i], yerr=err0b[i], fmt=".r",
    #         elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "ITHR = 60")

#plt.plot(rn,mean,marker=".",linewidth=0)
#plt.xlabel("parameters: [Backbias. VCASN. VCASN2. ITHR]")
plt.xlabel("VCASN", fontsize=18)
plt.ylabel("Mean threshold [DAC]", fontsize=18)
plt.tick_params(axis='both', labelsize=18)
plt.title("Threshold for ITHR = 60",fontsize=20)
plt.legend(fontsize=18)

plt.savefig("/home/david/ALPIDE-code-students/dschledewitz/testbeam_04_21/thresholdsWWWWWITHR60.png", dpi=300)
plt.show()

# plt.figure(figsize=(14, 10))
# plt.yscale("log")
# plt.grid(which="both", axis="both")
# plt.errorbar(plane, mean[0], yerr=d_mean[0], xerr=0.5, fmt='r',
#              elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "measured mean rate")
# plt.errorbar(plane, intensity, xerr=0.5, yerr=theory_error, fmt='k',
#              elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "expected mean rate")
# #ax1.errorbar(plane, intensity, xerr=0.5, fmt='k',
#              #elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "expected rate")
# plt.xlabel("Number of traversed planes", fontsize=18)
# plt.ylabel("mean rate $[1/s]$", fontsize=18)
# #plt.title("Mean rate of expected and measured multi-plane-events", fontsize=24)
# plt.tick_params(axis='both', labelsize=18)
# plt.legend(fontsize=18)