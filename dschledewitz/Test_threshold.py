#!/usr/bin/python

import sys
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import os
import tempfile
import subprocess
 
RN,BBV,VCASN,ITHR = np.loadtxt("Sheet.csv",delimiter=",",usecols=(0,1,2,3),skiprows=1,unpack=True)
VCASN2 = VCASN +12
 
def replace_key(filename, key, value):
    with open(filename, 'r') as f_in, tempfile.NamedTemporaryFile(
            'w', dir=os.path.dirname(filename), delete=False) as f_out:
        for line in f_in.readlines():
            if line.startswith(key):
                line = "\t".join((line.split('\t')[0],'{}'.format(value)))
            f_out.write(line)
                                  
    # remove old version
    os.unlink(filename)
 
    # rename new version
    os.rename(f_out.name, filename)
 
for i in range (len(RN)):
    replace_key("Config1.cfg", "ITHR\t",str(int(ITHR[i]))+"\n" )
    replace_key("Config1.cfg", "VCASN\t",str(int(VCASN[i]))+"\n" )
    replace_key("Config1.cfg", "VCASN2\t",str(int(VCASN2[i]))+"\n" )
    subprocess.call(["./test_shell.sh"])
