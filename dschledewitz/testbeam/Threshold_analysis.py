#!/usr/bin/python

# loading packages
import sys
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import os
import tempfile
import subprocess


### import the data arrays, with threshold data
data_path = os.path.join(os.path.split(os.path.dirname(__file__))[0],"testbeam","thr_data")
chipname = ["R02_0","REF0_3"]
#container
CHIP = np.array([])
BB = np.array([])
VCASN = np.array([])
ITHR = np.array([])
MEAN = np.array([])
SIG = np.array([])


#def mean
def Mean(val):

    mean = np.mean(val)
    stat = np.sqrt(1/(val.size*(val.size-1)) *
                np.sum((mean-val)**2))  # statistischer Fehler
    return mean, stat  



# loop over all files we got
for c in range(len(chipname)):
    # identify chip

    chip_path = os.path.join(data_path,chipname[c])
    print(chip_path)
    #extract chipname and BB
    chip = chipname[c].split("_")[0]
    CHIP = np.append(CHIP,chip)
    VBB = chipname[c].split("_")[1]
    BB = np.append(BB,VBB)
    
    for i in sorted(os.listdir(chip_path)):
        #select the data files
        if i.startswith("Thresholds"):    
            #extract VCASN and ITHR and 
            path = os.path.join(chip_path,i)



            # split the path to get the Runnumber
            vcasn = i.split("_")[1]
            VCASN = np.append(VCASN,vcasn)
            

            ithr = i.split("_")[2]
            ithr = ithr.split(".")[0]
            ITHR = np.append(ITHR,ithr)


            data = np.load(path)
            #calculate mean Threshold
            Threshs = np.array([])
            Threshs = np.append(Threshs,data[0])
            Threshs = np.append(Threshs,data[64])
            Threshs = np.append(Threshs,data[128])
            Threshs = np.append(Threshs,data[256])
            print(Threshs[4000:4010])
            #remove nan
            
            # for k in range(len(Threshs)):
            #     if np.isnan(Threshs[k]):
            #         Threshs[k] = 0

            Threshs_fin = np.array([])
            for k in range(len(Threshs)):
                if np.isnan(Threshs[k]):
                    
                    continue
                    #Threshs = np.array([0,0])
                if Threshs[k]>100:
                    
                    continue
                if Threshs[k]<100:#wieso kommen Thr >2000 durch?
                    Threshs_fin = np.append(Threshs_fin,k)
            #print(len(Threshs_fin))
            if len(Threshs_fin) == 0:
                mean,d_mean = 0,0
                MEAN = np.append(MEAN,mean)
                SIG = np.append(SIG,d_mean)
                continue

            
            

            mean, d_mean = Mean(Threshs_fin)
            print(mean)
            MEAN = np.append(MEAN,mean)
            SIG = np.append(SIG,d_mean)





RN = np.arange(0,len(MEAN))         
### build the figure so that nothing will truncated
plt.figure(figsize=(12,10))
plt.subplots_adjust(bottom=0.28,top=0.95)
### set the parameters as x-axis-values
#x_ax_values= np.array([VCASN,ITHR])   # made a matrix of all parameter information we want to display
#x_ax_values = np.swapaxes(x_ax_values,0,1)        # swap axis, so we get a parameter boundle in every row we can identify with the x axis value
#x_ax_values.astype(int)
#plt.xticks(RN,x_ax_values)                        # replace RN with our parameters as axis-values
### rotate the presentation of the x-axis-values to ensure they wont overlap
#plt.xticks(rotation=90)
### plotting
plt.errorbar(RN, MEAN, yerr=SIG, fmt='.k')
#plt.plot(RN,MEAN,marker=".",linewidth=0)
plt.xlabel("parameters: [Backbias. VCASN. VCASN2. ITHR]")
#plt.ylabel("Calculated mean threshold [DAC]")
#plt.title("Threshold for various parameter settings and Backbias of 0V",fontsize=20)
plt.show()