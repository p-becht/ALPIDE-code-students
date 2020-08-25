import numpy as np
import os
import matplotlib.pyplot as plt

path = "R08_3/"

for filename in os.listdir(path):
    fil = path+str(filename)
    if filename.endswith(".npy"):
        data = np.load(fil)
        tmp = []
        for i in range(len(data[0])):
            if not np.isnan(data[0][i]):
                tmp.append(data[0][i])
        npdata = np.ndarray((len(tmp)))
        print("THERE ARE {} VALUES".format(len(tmp)))
        for i in range(len(npdata)):
            npdata[i] = tmp[i]
        print("Average Threshold for run {}: {} +- {}".format(filename,
            np.mean(npdata), np.std(npdata)))
    else: continue
    #print("continue? [y/n]")
    #stdin = str(input())
    #if stdin == "y": continue
    #else: break
