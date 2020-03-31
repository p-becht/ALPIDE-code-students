# SCRIPT FOR EDIITNG A CONFIG FILE

# IMPORT LIBRARIES

import numpy as np
import subprocess
import tempfile
import os

# Specify, which file to edit:
cfg = "Config.cfg"
deli = " "

# CHOOSE VALUES
# ITHR = [40, 50, 60, 70]
# VCASN = [47, 50, 53, 56]
# VCASN2 = []

RN,BBV,VCASN,ITHR = np.loadtxt("Sheet.csv",delimiter=",",usecols=(0,1,2,3),skiprows=1,unpack=True)
VCASN2 = []

# set VCASN2 to VCASN +12
for i in range(len(VCASN)): VCASN2.append(VCASN[i]+12)

# Function to replace some value of a key
def replace_key(filename,key,value,delimiter):
    # The option "open" returns a stream
    # copy information to a temporary file, and change values in there
    # we have to set delete to False, so that we can rename the tempfile later
    # and overwrite our Config file
    with open(filename, 'r') as f_in, tempfile.NamedTemporaryFile('w',delete=False) as f_out:
        # find line where value has to be changed
        for line in f_in:
            if line.startswith(key):
                # Doing some python String magic to change "key a" to "key b"
                line = delimiter.join((line.split(delimiter)[0],value+"\n"))
            # Write current line into the tempfile
            f_out.write(line)

    # Rename the tempfile and overwrite Config
    os.rename(f_out.name, cfg)

# Iterate over arrays, we have to convert values to strings to edit Config file
for i in range(len(VCASN)):
    replace_key(cfg,"VCASN", str(int(VCASN[i])), deli)
    replace_key(cfg,"VCASN2", str(int(VCASN2[i])), deli)
    replace_key(cfg,"ITHR", str(int(ITHR[i])), deli)
        
    # Start threshold scan
    # subprocess.call(["./testscript.sh"])
    subprocess.call(["./runtest", "THRESHOLD", "10", "0", "50"])
