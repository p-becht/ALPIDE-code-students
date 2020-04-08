#!/usr/bin/python

# loading packages
import sys
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import os
import tempfile
import subprocess

#import the datasheet,where all test-configurations are given
RN,BBV,VCASN,ITHR = np.loadtxt("Sheet.csv",delimiter=",",usecols=(0,1,2,3),skiprows=1,unpack=True)
VCASN2 = VCASN +12

#definition, which allows to replace the value of a key-parameter
def replace_key(filename, key, value):
    #read the file and make a copy where we will change the values	
    with open(filename, 'r') as f_in, tempfile.NamedTemporaryFile(
            'w', dir=os.path.dirname(filename), delete=False) as f_out:
	#go through the lines of the original file
        for line in f_in.readlines():
	    #look for the line where the key-parameter is placed
            if line.startswith(key):
		#change the parameter, by splitting the line and replace the last part by the wished value
                line = " ".join((line.split(' ')[0],'{}'.format(value)))
	    # overwrite the line in the copy
            f_out.write(line)
                                  
    # remove original file
    os.unlink(filename)
 
    # rename new file to the original
    os.rename(f_out.name, filename)

# execute the replacement for all parameters we want to change 
for i in range (len(RN)):
    replace_key("Config.cfg", "ITHR ",str(int(ITHR[i]))+"\n" )
    replace_key("Config.cfg", "VCASN ",str(int(VCASN[i]))+"\n" )
    replace_key("Config.cfg", "VCASN2 ",str(int(VCASN2[i]))+"\n" )
    #run the test with args: THRESHHOLD MASKSTAGES DAC-start-value DAC-stop-value
    subprocess.call(["./runtest", "THRESHOLD", "164", "0", "50"])

#if call does not work, try this:
#import shlex
#pro = subprocess.Popen(shlex.split("./runtest THRESHOLD 164 0 50"), stdout= subprocess.PIPE)
