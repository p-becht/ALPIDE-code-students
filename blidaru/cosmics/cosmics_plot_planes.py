#! /usr/bin/python3

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


with open(txt_file) as file:
    data = file.read().split('\n')
    event_number = str(re.findall(r'\b\d+\b', data[int(sys.argv[2])-4])[0])   # adapted for a quick checkup; cat 'file' | grep -n 'pALPIDE_' ; this outputs txt line number; go back 4 lines once an event line is identified to get event number
    event = eventData(data,event_number)
    pprint (event)
    print ()
    for i in range(0,7):
        print(i)
        im=np.zeros((512,1024))
        for line in planeData(event,i): 
            print(selectXandY(line))
            for pair in selectXandY(line):
                im[int(pair[1]),int(pair[0])] =1
        plt.figure(i)
        plt.xlim([500,775])
        plt.ylim([150,375])
        plt.imshow(im)
        plt.show()