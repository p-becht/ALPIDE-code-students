#!/usr/bin/python

# loading packages
import sys
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import os
import tempfile
import subprocess
import csv

### import the the data files
# Therefore seperate the configs from the Noise_occ data and build a pair for every timestamp
data_path = os.path.join(os.path.split(os.path.dirname(__file__))[0],"data","noise_scan_CHIP8_VBB3")

## import the files and get the information out of them

# arrays for storing the information we want in the csv: VCASN, VCASN2, ITHR, #triggers from the cfg file
# and from #hits (fakehits) from the NoiseOcc file
VCASN = []
VCASN2= []
ITHR  = []
NTRIG = []
NHITS = []


###### defenition to get the value of a specific key word of a file
def get_value(filename, key):
    # open file
    file = open(filename, "r")
    # read the lines
    for line in file.readlines():
        # find the line with the given keyword
        if line.startswith(key):
            # get the value of this parameter
            value = line.split()[1]
            return value



####### def for calculating the Fakehitrate
## calc by: number of hits in all tested(documented) pixels divided by 
## the number of tested pixles(length of the array) times the number of test loops (NTRIG)

# crating a container for the fakehitrate
FHR = []
#error, calculated by sqrt(hits)/(NTRIG*number of pixels) <- intrinsic noise in the production of e
d_FHR = []

def fake_rate(hits, Ntrig):
    # for every config: calculate the rate
    for i in range(len(hits)):
        # test if the array is 0-dimensional, if size = 1:  calculate without the len()
        if hits[i].size == 1:
            FHR.append(hits[i]/Ntrig[i])
            d_FHR.append(np.sqrt(hits[i])/Ntrig[i])
            continue
        #calculating the FakeHitRate for every configuration
        FHR.append(sum(hits[i])/(Ntrig[i]*len(hits[i])))
        d_FHR.append(np.sqrt(sum(hits[i]))/(Ntrig[i]*len(hits[i])))




################ MAIN CODE #####################################
# search in the noise_occ_folder for files
for i in sorted(os.listdir(data_path)):
    # Config files start with "ScanConfig -> we search for them"
    if i.startswith("ScanConfig"):
        # reading out the data we want and put them into the according arrays, created at the beginning
        path = os.path.join(data_path,i)
        VCASN.append(int(get_value(path,"VCASN")))
        VCASN2.append(int(get_value(path,"VCASN2")))
        ITHR.append(int(get_value(path, "ITHR")))
        NTRIG.append(int(get_value(path, "NTRIGGERS")))

    # Data for the #hits is stored in the .dat files beginning with "Noise"
    elif i.startswith("Noise"):
        path = os.path.join(data_path,i)
        # since we dont need the adress of the pixel, we will only import the #hits
        NHITS.append(np.loadtxt(path ,delimiter=" ",usecols=(2), dtype= int))


### calculate the fakehit rate
fake_rate(NHITS, NTRIG)

###creating a csv file
with open("FakeHitRate.csv","w", newline="") as f:
    ## writing the data we need: the configuration parameters and the Fakehitrate with error
    #creating the the header
    header = [" VCASN", " VCASN2", " ITHR", " NTRIGGERS", " FHR", " Error_FHR"]
    writing = csv.DictWriter(f, fieldnames= header)

    # write the header in the file
    writing.writeheader()
    # put in the values
    for j in range(len(VCASN)):
        writing.writerow({" VCASN" : VCASN[j], " VCASN2" : VCASN2[j], " ITHR" : ITHR[j],
         " NTRIGGERS" : NTRIG[j], " FHR" : FHR[j], " Error_FHR" : d_FHR[j]})
