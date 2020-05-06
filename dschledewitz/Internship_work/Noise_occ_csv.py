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

### we want to import the the data files
# Therefore seperately read the configs and the Noise_occ data and build collect the needed data
# for every timestamp pair
# import the files and get the information out of them 

# get the path to read the files
data_path = os.path.join(os.path.split(os.path.split(os.path.dirname(__file__))[0])[0],"blidaru","telescope_data","Data","bb0_noise")
data_path_2 = os.path.join(os.path.split(os.path.split(os.path.dirname(__file__))[0])[0],"blidaru","telescope_data","Data","bb3_noise")
# create arrays for storing the information we want in the csv: VCASN, VCASN2, ITHR, #triggers from the cfg file
# from the .dat files we need the number of hits
VCASN = []
VCASN2= []
ITHR  = []
NTRIG = []
NHITS = []
BB    = []


###### defenition to get the value of a specific keyword from a file
def get_value(filename, key):

    # open file
    file = open(filename, "r")

    # read the lines
    for line in file.readlines():

        # find the line with the given keyword
        if line.startswith(key):

            # get the value corresponding to the keyword
            value = line.split()[1]
            return value



####### def for calculating the Fakehitrate

# creating a container for the fakehitrate (FHR)
FHR = []
# error of FHR
d_FHR = []

def fake_rate(hits, Ntrig):

    # test if the array is 0-dimensional, if size = 1:  calculate without the len()
    if (hits.size == 1):

        #FHR, if only one event in this configuration
        FHR.append(hits/(Ntrig*524288))

        #corresponding error
        d_FHR.append(np.sqrt(hits)/(Ntrig*524288))
      
    else:

        #calculating the FakeHitRate for every configuration (assumption that one pixel would fire once)
        FHR.append(sum(hits)/(Ntrig*524288))

        #corresponding error
        d_FHR.append(np.sqrt(sum(hits))/(Ntrig*524288))

## calc by: number of hits in all tested(documented) pixels divided by 
## the number of tested pixles(all=524288) times the number of test loops (NTRIG)

## error calculated by sqrt(hits)/(NTRIG*number of pixels) <- intrinsic noise in the production of e




################ MAIN CODE #####################################
#######BB=0V

### search in the noise_occ_folder for files
# we need a container for the .dat files to test later , if there exists a .dat file to every .cfg file
Dat = []

for i in sorted(os.listdir(data_path)):

    # Data for the #hits is stored in the .dat files beginning with "NoiseOcc"
    if i.startswith("NoiseOcc"):
        path = os.path.join(data_path,i)

        # since we dont need the adress of the pixel, we will only import the #hits
        NHITS.append(np.loadtxt(path ,delimiter=" ",usecols=(2), dtype= int))

        ### extract the timestamp
        # Therefore split the path to get the timestamp
        file_type, date, time_ext = i.split("_")   # in time_ext still the.cfg need to be cut off
        time = time_ext.split(".")[0]

        #create the timestamp with both time and Hit data
        timestamp = date + "_" + time
        
        # contain the .dat files to compare them later on
        Dat.append(timestamp)

### now search for the config files and compare them to the .dat
for i in sorted(os.listdir(data_path)):

    # Config files start with "ScanConfig" -> search for them
    if i.startswith("ScanConfig"):
        
        #define the whole filepath
        path = os.path.join(data_path,i)

        # reading out the data we want and put them into the according arrays, created at the beginning
        VCASN.append(int(get_value(path,"VCASN")))
        VCASN2.append(int(get_value(path,"VCASN2")))
        ITHR.append(int(get_value(path, "ITHR")))
        BB.append(int(0))
        
        #we need the number of triggers also for the fakehit rate as varaible, so we define one here
        num_triggers = int(100000)#get_value(path, "NTRIGGERS"))   ###no trigger in new config file
        NTRIG.append(num_triggers)

        ### test, if there is also a coresponding NoiseOcc file
        # Therefore split the path to get the timestamp
        file_type, date, time_ext = i.split("_")   # in time_ext still the.cfg need to be cut off
        time = time_ext.split(".")[0]

        #create the timestamp:
        timestamp = date + "_" + time
        

        ## testing if a corresponding noiseocc file excists:
        # if yes: calculate the fakehitrate
        if timestamp in Dat:

            #use the index of the corresponding timestamp to find the right Hit data
            Hit_data = NHITS[Dat.index(timestamp)]
            fake_rate(Hit_data,num_triggers)
        
        #if no: assume the sensitvity limit = 1.907e-11
        else:
            #FHR_sensitivity limit
            FHR.append(1/(num_triggers*524288))#

            #d_FHR_sensitivity limit
            d_FHR.append(1/(num_triggers*524288))




######################################################################################BB=3V
for i in sorted(os.listdir(data_path_2)):

    # Data for the #hits is stored in the .dat files beginning with "NoiseOcc"
    if i.startswith("NoiseOcc"):
        path = os.path.join(data_path_2,i)

        # since we dont need the adress of the pixel, we will only import the #hits
        NHITS.append(np.loadtxt(path ,delimiter=" ",usecols=(2), dtype= int))

        ### extract the timestamp
        # Therefore split the path to get the timestamp
        file_type, date, time_ext = i.split("_")   # in time_ext still the.cfg need to be cut off
        time = time_ext.split(".")[0]

        #create the timestamp with both time and Hit data
        timestamp = date + "_" + time
        
        # contain the .dat files to compare them later on
        Dat.append(timestamp)

### now search for the config files and compare them to the .dat
for i in sorted(os.listdir(data_path_2)):

    # Config files start with "ScanConfig" -> search for them
    if i.startswith("ScanConfig"):
        
        #define the whole filepath
        path = os.path.join(data_path_2,i)

        # reading out the data we want and put them into the according arrays, created at the beginning
        VCASN.append(int(get_value(path,"VCASN")))
        VCASN2.append(int(get_value(path,"VCASN2")))
        ITHR.append(int(get_value(path, "ITHR")))
        BB.append(int(3))
        
        #we need the number of triggers also for the fakehit rate as varaible, so we define one here
        num_triggers = int(100000)#get_value(path, "NTRIGGERS"))   ###no trigger in new config file
        NTRIG.append(num_triggers)

        ### test, if there is also a coresponding NoiseOcc file
        # Therefore split the path to get the timestamp
        file_type, date, time_ext = i.split("_")   # in time_ext still the.cfg need to be cut off
        time = time_ext.split(".")[0]

        #create the timestamp:
        timestamp = date + "_" + time
        

        ## testing if a corresponding noiseocc file excists:
        # if yes: calculate the fakehitrate
        if timestamp in Dat:

            #use the index of the corresponding timestamp to find the right Hit data
            Hit_data = NHITS[Dat.index(timestamp)]
            fake_rate(Hit_data,num_triggers)
        
        #if no: assume the sensitvity limit = 1.907e-11
        else:
            #FHR_sensitivity limit
            FHR.append(1/(num_triggers*524288))#

            #d_FHR_sensitivity limit
            d_FHR.append(1/(num_triggers*524288))


       

###creating a csv file
with open("FakeHitRate.csv","w", newline="") as f:

    ## writing the data we need: the configuration parameters and the Fakehitrate with error
    #creating the the header
    header = ["BB"," VCASN", " VCASN2", " ITHR", " NTRIGGERS", " FHR", " Error_FHR"]
    writing = csv.DictWriter(f, fieldnames= header)

    # write the header in the file
    writing.writeheader()
    
    # put in the values
    for j in range(len(VCASN)):
        writing.writerow({"BB" : BB[j]," VCASN" : VCASN[j], " VCASN2" : VCASN2[j], " ITHR" : ITHR[j],
         " NTRIGGERS" : NTRIG[j], " FHR" : FHR[j], " Error_FHR" : d_FHR[j]})
