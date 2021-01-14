#! /usr/bin/python

from itertools import takewhile
import re
import numpy as np
import matplotlib.pyplot as plt
import sys
from pprint import pprint
from scipy.optimize import curve_fit
from mpl_toolkits import mplot3d
from mpl_toolkits.mplot3d import Axes3D

# sys.argv[1]
#txt_file = "/home/david/ALPIDE-code-students/dschledewitz/muon_data_processing/desy19.txt" 
txt_file = "/home/david/ALPIDE-code-students/data/compressed/Cosmics000639_Compressed.txt"


def eventData(data, event):
    marker = f"=== {event} ==="
    if marker not in data:
        return []
    start = data.index(marker)+1
    print(data[start-1])
    return list(takewhile(lambda d: not d.startswith("==="), data[start:]))


def planeData(data, plane):
    marker = f"--- pALPIDEfs_{plane} ---"
    if marker not in data:
        return []
    start = data.index(marker)+1
    return list(takewhile(lambda d: not d.replace("=", "-").startswith("---"), data[start:]))


def selectXandY(line):
    list_line = line.split(',')
    col = re.findall(r'\b\d+\b', list_line[0])
    row = re.findall(r'\b\d+\b', list_line[1])
    return list(zip(col, row))

# calculate mean


def Mean(val, d_val):
    if isinstance(val, int):
        return 0,0
    elif val.size == 1:
        return int(val), 0  # ,0
    else:
        mean = np.mean(val)
        stat = np.sqrt(1/(val.size*(val.size-1)) *
                    np.sum((mean-val)**2))  # statistischer Fehler
        # sys = np.mean(d_val) #systematischer Fehler
        # print("Meanvalue:" , mean, " +- ", stat, "stat. +- ", sys, "sys")
        return mean, stat  # , sys



# ev_num = np.array(np.array([31304, 2231009, 2332345, 4920656, 6549643, 9520660]),
 # np.array([1826261, 2936620, 6626458]), np.array([2290604, 7915917]),
  # np.array([1480603, 1880592, 3186234, 6871459, 8197996])) #4,5,6,7pe
ev_num = np.array([1480603, 1880592, 3186234, 6871459, 8197996])  # 7pe
#ev_num = np.array([2290604, 7915917])  # 6pe
#ev_num = np.array([1826261, 2936620, 6626458]) #5pe
#ev_num = np.array([6549643])#np.array([31304, 2231009, 2332345, 4920656, 6549643, 9520660]) #4pe

# correction
x = np.array([0., -10.70464573,  -6.25092376,   0.19635326,  25.26630419, 27.30091044,  78.18635686])
x_cut = np.array([0.0, -10.942751693937705, -6.107853151401792, 0.3392396058722253, 25.787629617440512, 27.90264139955528, 79.67674933762731])
d_x = np.array([5.0753646042158405, 5.87477832494781, 5.755816368216302, 5.995301277503309, 5.941687387178227, 6.3436679805375995, 6.370528043707604])
y = np.array([0., 45.15699466, -10.91364125,  -6.02800581,  -6.77937338, 33.55199672,  19.33957541])
y_cut = np.array([0.0, 46.93523623413028, -11.63021732446464, -6.644735635251222, -7.586362208978847, 34.72339574790476, 19.851017367665627])
d_y = np.array([3.5754407942229207, 4.222249682772156, 4.001147068181429, 4.2116401173182245, 4.1758062264516, 4.519796135420165, 4.4523349048317495])
#desy19
#ev_num = np.array([168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178])
All_x = []
All_y = []
fig, (ax2,ax1) = plt.subplots(1,2,figsize=(18, 10))
ax1.set_xlim([0, 1024])
ax1.set_ylim([0, 512])
ax1.set_xlabel("x-axis [pixels]", fontsize=18)
ax1.set_ylabel("y-axis [pixels]", fontsize=18)
ax1.set_title("Aligned cosmic tracks", fontsize=24)
ax2.set_xlim([0, 1024])
ax2.set_ylim([0, 512])
ax2.set_xlabel("x-axis [pixels]", fontsize=18)
ax2.set_ylabel("y-axis [pixels]", fontsize=18)
ax2.set_title("Unaligned cosmic tracks", fontsize=24)
# plt.set_title("Run_000639")
for e in range(len(ev_num)):
    with open(txt_file) as file:
        data = file.read().split('\n')
        # str(re.findall(r'\b\d+\b', data[int(sys.argv[2])-4])[0])   # adapted for a quick checkup; cat 'file' | grep -n 'pALPIDE_' ; this outputs txt line number; go back 4 lines once an event line is identified to get event number
        event_number = ev_num[e]
        event = eventData(data, event_number)
        # pprint (event)
        # print ()

        all_x = np.array([])
        all_y = np.array([])
        all_c_x = np.array([])
        all_c_y = np.array([])
        missing = []
        for i in range(0, 7):
            # print(i)
            x_coor = np.array([])
            y_coor = np.array([])
            for line in planeData(event, i):
                # print(selectXandY(line))
                for pair in selectXandY(line):
                    x_coor = np.append(x_coor, int(pair[0]))
                    print(i)
                    print("x; "+str(int(pair[0])))
                    print("x; "+str(int(pair[1])))
                    y_coor = np.append(y_coor, int(pair[1]))
            if x_coor.size != 0:
                mean_x, d_mean_x = Mean(x_coor, 0)
                mean_y, d_mean_y = Mean(y_coor, 0)
                mean_x = mean_x
                mean_y = mean_y
                all_x = np.append(all_x, mean_x)
                all_y = np.append(all_y, mean_y)
                all_c_x = np.append(all_c_x, mean_x-x[i])#x_cut[i]
                all_c_y = np.append(all_c_y, mean_y-y[i])#y_cut[i]
            else:
                missing.append(i)
        ax2.plot(all_x, all_y,"o-", label="Event "+ str(event_number)+", missing plane(s): "+str(missing))
        ax1.plot((all_c_x),(all_c_y),"o-")#, label="Event "+ str(event_number)+", missing plane(s): "+str(missing))
        ax2.plot(all_x[0], all_y[0],"ro-")
        ax1.plot((all_c_x[0]),(all_c_y[0]),"ro-")
        
        All_x.append(all_c_x)
        All_y.append(all_c_y)
########################################### LEGENDS HERE    LEGENDS HERE    LEGENDS HERE    LEGENDS HERE    ####################################
#ax1.legend()
#plt.legend()
plt.savefig("/home/david/ALPIDE-code-students/dschledewitz/muon_data_processing/images/presentation/track_visual.png", dpi=600)
#plt.show()

# fitting
def lin(x, a, b):

    return a * x +b

# create array without DUT for tracking
REF = np.zeros((len(All_x),2,6))
for i in range(len(All_x)):
    for j in range(0,3):
        REF[i,0,j] = All_x[i][j]
        REF[i,1,j] = All_y[i][j]
    for j in range(3,6):
        REF[i,0,j] = All_x[i][j+1]
        REF[i,1,j] = All_y[i][j+1]

for i in range(len(All_x)):
    fit, err = curve_fit(lin,REF[i,0],REF[i,1])
    x = np.linspace(np.min((REF[i,0,0], REF[i,0,5]))-5,np.max((REF[i,0,0], REF[i,0,5]))+5, 100)
    y = fit[0] * x + fit[1]
    print(fit)
    print(np.sqrt(np.diag(err)))
    ax1.plot(x,y)
plt.show()



# create array without DUT for tracking
# track = np.array([0,1,2,4,5,6])

# #fitting
# # def lin(x, a, b):

# #     return a * x[0] + b * x[1] + c * x[2] +d

# fig = plt.figure()
# ax = plt.axes(projection='3d')
# #ax = fig.add_subplot(111, projection='3d')

# # Data for a three-dimensional line
# #x = np.linspace(0,1024,2048)
# #y = -(fit1[0] * x +fit1[2])/fit1[1]
# z = np.linspace(0, 6, 100)#fit1[0] * x + fit1[1] * y + fit1[2]

# # print(x)
# # print(y)
# # print(z)
# print(fit)
# print(np.sqrt(np.diag(err)))
# ax.plot3D(x, y, z)
# ax.plot3D(REF[0,0], REF[0,1], track, "o-")
#Axes3D.plot(x, y,z)
#plt.show()




"""[289, 343, 342, 343]
[455, 181, 181, 182]

[181, 181, 182, 182, 323, 324]
[191, 192, 191, 192, 465, 465]

[612, 613, 613, 612, 853]
[100, 100, 101, 101, 347]"""



"""def lin(x, a, b, c):

    return a * x[0] + b * x[1] + c

# for 2d: x[1] = -(a * x[0] +c)/b

# create array without DUT for tracking
track = np.array([0,1,2,4,5,6])
REF = np.zeros((len(All_x),2,6))
for i in range(len(All_x)):
    for j in range(0,3):
        REF[i,0,j] = All_x[i][j]
        REF[i,1,j] = All_y[i][j]
    for j in range(3,6):
        REF[i,0,j] = All_x[i][j+1]
        REF[i,1,j] = All_y[i][j+1]

fit, err = curve_fit(lin,REF[0],track)
x = np.linspace(0, 1024, 2048)
y = -(fit[0] * x +fit[2])/fit[1]

ax1.plot(x,y)
plt.show()"""