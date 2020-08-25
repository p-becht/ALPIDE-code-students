#! /usr/bin/python

from itertools import takewhile
import re 
import numpy as np
import matplotlib.pyplot as plt
import sys
from pprint import pprint

txt_file = sys.argv[1] 

def eventData(data,event):
    marker = f"=== {event} ==="
    if marker not in data: return []  
    start = data.index(marker)+1
    print (data[start-1])
    return list(takewhile(lambda d:not d.startswith("==="),data[start:]))  

def planeData(data,plane):
    marker = f"--- pALPIDEfs_{plane} ---"
    if marker not in data: return []
    start = data.index(marker)+1
    return list(takewhile(lambda d:not d.replace("=","-").startswith("---"),data[start:]))
    
def selectXandY(line): 
    list_line = line.split(',')
    col = re.findall(r'\b\d+\b', list_line[0])
    row = re.findall(r'\b\d+\b', list_line[1])
    return list(zip(col,row))


# ev_num = np.array([1480603, 1880592, 3186234, 6871459, 8197996]) #7pe
ev_num = np.array([2290604, 7915917]) #6pe
# ev_num = np.array([1826261, 2936620, 6626458]) #5pe
for e in range(len(ev_num)):
    with open(txt_file) as file:
        data = file.read().split('\n')
        event_number = ev_num[e] #str(re.findall(r'\b\d+\b', data[int(sys.argv[2])-4])[0])   # adapted for a quick checkup; cat 'file' | grep -n 'pALPIDE_' ; this outputs txt line number; go back 4 lines once an event line is identified to get event number
        event = eventData(data,event_number)
        pprint (event)
        print ()
        fig, axs = plt.subplots(2,4, figsize=(14, 12))
        fig.subplots_adjust(hspace = .5, wspace=.15, left=0.05, right= 0.95)
        axs = axs.ravel()
        for i in range(0,7):
            print(i)
            im=np.zeros((512,1024))
            for line in planeData(event,i): 
                print(selectXandY(line))
                for pair in selectXandY(line):
                    im[int(pair[1]),int(pair[0])] =1
                    print(int(pair[1]))
                    for t in range(1,5):
                        for g in range(1,5):
                            if (int(pair[1])+t < 512) and (int(pair[0])+g < 1024) and (int(pair[1])-t > 0) and (int(pair[0])-g > 0):
                                im[int(pair[1])+t,int(pair[0])+g] =1
                                im[int(pair[1])+t,int(pair[0])-g] =1
                                im[int(pair[1])-t,int(pair[0])+g] =1
                                im[int(pair[1])-t,int(pair[0])-g] =1
            #axs[i].set_xlim([500,775])
            #axs[i].set_ylim([50,375])
            axs[i].set_xlim([0,1024])
            axs[i].set_ylim([0,512])
            axs[i].set_title("pALPIDEfs_" + str(i))
            axs[i].imshow(im)
        axs[7].axis("off")
        fig.suptitle("Run_000639: event " + str(ev_num[e]))
        plt.show()
