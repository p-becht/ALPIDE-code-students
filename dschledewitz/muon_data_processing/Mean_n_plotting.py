#!/usr/bin/python
import numpy as np
import matplotlib.pyplot as plt
import csv


# load data, seperated by runsize, seconds: 480,864,1006
datas = np.loadtxt("muon_data_processing/csv/n-event-plane-data.csv", delimiter=",", skiprows=1, usecols=(1,2,3,4,5,6,7),dtype=int)
run_numbers = np.loadtxt("muon_data_processing/csv/n-event-plane-data.csv", delimiter=",", skiprows=1, usecols=(0), dtype=str)
#load number of hits per plane
datas_hits = np.loadtxt("muon_data_processing/csv/hits_per_plane.csv", delimiter=",", skiprows=1, usecols=(1,2,3,4,5,6,7),dtype=int)
total_hits = np.loadtxt("muon_data_processing/csv/hits_per_plane.csv", delimiter=",", skiprows=1, usecols=(8),dtype=int)
# load holes
datas_holes = np.loadtxt("muon_data_processing/csv/event_HOLES-data.csv", delimiter=",", skiprows=1, usecols=(1,2,3,4,5,6,7,8,9),dtype=int)
datas_holes_dut = np.loadtxt("muon_data_processing/csv/event_HOLES_DUT-data.csv", delimiter=",", skiprows=1, usecols=(1,2,3,4,5,6,7,8,9),dtype=int)



# theorietical data
intensity = [0.07812, 0.1486, 0.06728, 0.02618, 0.01044, 0.004055, 0.005013]


# split the path to get the Runnumber, to seperate by run size(<416<606)
run=np.zeros(len(run_numbers))
for i in range(len(run_numbers)):
    run[i] = run_numbers[i].split("_")[1]
    run[i] = int(run[i])
plane = np.arange(1, 8)

# normalize all runs, to be able to compare them
data=np.zeros((len(run_numbers),7))
data_hits=np.zeros((len(run_numbers),7))
data_holes=np.zeros((len(run_numbers),9))
data_holes_dut=np.zeros((len(run_numbers),9))
for i in range(len(run_numbers)):

    if int(run[i]) < 416:
        data[i] = datas[i]/480
        data_hits[i] = datas_hits[i]/480
        data_holes[i] = datas_holes[i]/480
        data_holes_dut[i] = datas_holes_dut[i]/480
    elif int(run[i])<606:
        data[i] = datas[i]/862
        data_hits[i] = datas_hits[i]/862
        data_holes[i] = datas_holes[i]/862
        data_holes_dut[i] = datas_holes_dut[i]/862
    else:
        data[i] = datas[i]/1006
        data_hits[i] = datas_hits[i]/1006
        data_holes[i] = datas_holes[i]/1006
        data_holes_dut[i] = datas_holes_dut[i]/1006

# calculate mean
def Mean(val,d_val):
    if isinstance(val, int):
        return 0,0,0
    else:
        mean = np.mean(val)
        stat= np.sqrt(1/(val.size*(val.size-1))*np.sum((mean-val)**2)) #statistischer Fehler
        sys = np.mean(d_val) #systematischer Fehler
        #print("Meanvalue:" , mean, " +- ", stat, "stat. +- ", sys, "sys")
        return mean, stat, sys


mean = np.zeros((2,7)) #first data, second dada_hits
d_mean = np.zeros((2,7))
for i in range(7):
    mean[0,i], d_mean[0,i], sys_1 = Mean(data[:,i],0) # n-plane-events
    mean[1,i], d_mean[1,i], sys_1 = Mean(data_hits[:,i],0) # total_hits

# holes
#differentiate different hole numbers
hole_1 = np.array([0, data_holes[:,0], data_holes[:,1], data_holes[:,3], data_holes[:,6], data_holes[:,8], 0])
hole_2 = np.array([0, 0, data_holes[:,2], data_holes[:,4], data_holes[:,7],0, 0])
hole_3 = np.array([0, 0, 0, data_holes[:,5], 0, 0, 0])
hole_1_dut = np.array([0, data_holes_dut[:,0], data_holes_dut[:,1], data_holes_dut[:,3], data_holes_dut[:,6], data_holes_dut[:,8], 0])
hole_2_dut = np.array([0, 0, data_holes_dut[:,2], data_holes_dut[:,4], data_holes_dut[:,7],0, 0])
hole_3_dut = np.array([0, 0, 0, data_holes_dut[:,5], 0, 0, 0])

# bring the events in the right shape
data_holes_sum = np.zeros((len(data_holes),8))
data_holes_dut_sum = np.zeros((len(data_holes),8))
for i in range(len(data_holes)):
    data_holes_sum[i,2] = data_holes[i,1]+data_holes[i,2]
    data_holes_sum[i,5] = data_holes[i,3]+data_holes[i,4]+data_holes[i,5]
    data_holes_sum[i,7] = data_holes[i,6]+data_holes[i,7]
    data_holes_dut_sum[i,2] = data_holes_dut[i,1]+data_holes_dut[i,2]
    data_holes_dut_sum[i,5] = data_holes_dut[i,3]+data_holes_dut[i,4]+data_holes_dut[i,5]
    data_holes_dut_sum[i,7] = data_holes_dut[i,6]+data_holes_dut[i,7]


data_hole = np.array([0, data_holes[:,0], data_holes_sum[:,2], data_holes_sum[:,5],
 data_holes_sum[:,7], data_holes[:,8], 0])
data_hole_dut = np.array([0, data_holes_dut[:,0], data_holes_dut_sum[:,2], data_holes_dut_sum[:,5],
 data_holes_dut_sum[:,7], data_holes_dut[:,8], 0])

mean_holes = np.zeros(7)
mean_holes_i = np.zeros((3,7))
mean_holes_dut = np.zeros(7)
mean_holes_i_dut = np.zeros((3,7))
d_mean_holes = np.zeros(7)
d_mean_holes_i = np.zeros((3,7))
d_mean_holes_dut = np.zeros(7)
d_mean_holes_i_dut = np.zeros((3,7))
for i in range(1,6):
    mean_holes[i], d_mean_holes[i], sys_1 = Mean(data_hole[i],0) # holes
    mean_holes_dut[i], d_mean_holes_dut[i], sys_1 = Mean(data_hole_dut[i],0) # holes with dut
    # mean of 1, 2 and 3 holes in a event
    mean_holes_i[0,i], d_mean_holes_i[0,i], sys_1 = Mean(hole_1[i],0)
    mean_holes_i[1,i], d_mean_holes_i[1,i], sys_1 = Mean(hole_2[i],0)
    mean_holes_i[2,i], d_mean_holes_i[2,i], sys_1 = Mean(hole_3[i],0)
    mean_holes_i_dut[0,i], d_mean_holes_i_dut[0,i], sys_1 = Mean(hole_1_dut[i],0)
    mean_holes_i_dut[1,i], d_mean_holes_i_dut[1,i], sys_1 = Mean(hole_2_dut[i],0)
    mean_holes_i_dut[2,i], d_mean_holes_i_dut[2,i], sys_1 = Mean(hole_3_dut[i],0)

#################---HOLE CORRECTION   HOLE CORRECTION   HOLE CORRECTION---#############################
# mean[0,i]= mean[0,i]- #holes[i]
# #already excluding three hole events, since they dont exist
corr_mean = np.array([mean[0,0],
 mean[0,1] - mean_holes_i[0,1],
 mean[0,2] - mean_holes_i[0,2] - mean_holes_i[1,2], 
 mean[0,3] - mean_holes_i[0,3] - mean_holes_i[1,3],
 mean[0,4] - mean_holes_i[0,4] - mean_holes_i[1,4], 
 mean[0,5] - mean_holes_i[0,5], 
 mean[0,6]
 ])

d_corr_mean = np.array([d_mean[0,0],
 np.sqrt(d_mean[0,1]**2 - d_mean_holes_i[0,1]**2),
 np.sqrt(d_mean[0,2]**2 - d_mean_holes_i[0,2]**2 - d_mean_holes_i[1,2]**2), 
 np.sqrt(d_mean[0,3]**2 - d_mean_holes_i[0,3]**2 - d_mean_holes_i[1,3]**2),
 np.sqrt(d_mean[0,4]**2 - d_mean_holes_i[0,4]**2 - d_mean_holes_i[1,4]**2), 
 np.sqrt(d_mean[0,5]**2 - d_mean_holes_i[0,5]**2), 
 d_mean[0,6]
 ])



# plotting
plt.figure(figsize=(12, 10))
plt.yscale("log")
plt.grid(which="both", axis="both")
plt.errorbar(plane, intensity, xerr=0.5, fmt='k',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5)
#ax1.errorbar(plane, intensity, xerr=0.5, fmt='k',
             #elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "expected rate")
plt.xlabel("number of traversed planes")
plt.ylabel("mean rate $[1/s]$")
plt.title("Mean rate of expected multi-plane-events", fontsize=20)
plt.show()
# plotting
fig, (ax1,ax2) = plt.subplots(1,2,figsize=(14, 10))
ax1.set_yscale("log")
ax1.grid(which="both", axis="both")
ax1.errorbar(plane, mean[0], yerr=d_mean[0], xerr=0.5, fmt='r',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "measured mean rate")
ax1.errorbar(plane, intensity, xerr=0.5, yerr= np.array(intensity)*0.05, fmt='k',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "expected rate")
# plt.plot(RN,MEAN,marker=".",linewidth=0)
ax1.set_xlabel("number of traversed planes")
ax1.set_ylabel("mean rate $[1/s]$")
ax1.set_title("Mean rate of expected and measured multi-plane-events")#, fontsize=20)
ax1.legend()

errr_0 = intensity/mean[0]*np.sqrt((d_mean[0]/mean[0])**2+(0.05)**2)
ax2.errorbar(plane, intensity/mean[0], yerr= errr_0, xerr=0.5, fmt='k',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5)
ax2.grid(which="both", axis="both")
ax2.set_xlabel("number of traversed planes")
ax2.set_ylabel("Ratio expected/measured rate")
ax2.set_title("Ratio of expected and measured multi-plane-events and ex to DUT")
plt.show()


# plotting HOLES
fig, (ax1,ax2) = plt.subplots(1,2,figsize=(14, 10))
ax1.set_yscale("log")
ax1.grid(which="both", axis="both")
ax1.errorbar(plane, mean[0], yerr=d_mean[0], xerr=0.5, fmt='k',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "Measured mean rate")
ax1.errorbar(plane, mean_holes, yerr= d_mean_holes, xerr=0.5, fmt='b',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "Mean rate of holes")
ax1.errorbar(plane, mean_holes_dut, yerr= d_mean_holes_dut, xerr=0.5, fmt='green',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "Mean rate of holes-incl. DUT")

ax1.set_xlabel("number of traversed planes")
ax1.set_ylabel("mean rate $[1/s]$")
ax1.set_title("Mean rate of measured multi-plane-events, considering holes")
ax1.legend()

errr_holes = mean_holes/mean[0]*np.sqrt((d_mean[0]/mean[0])**2+(d_mean_holes/mean_holes)**2)
errr_holes_dut = mean_holes_dut/mean[0]*np.sqrt((d_mean[0]/mean[0])**2+(d_mean_holes_dut/mean_holes_dut)**2)
ax2.errorbar(plane, mean_holes/mean[0], yerr= errr_holes, xerr=0.5, fmt='b',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "All holes")
ax2.errorbar(plane, mean_holes_dut/mean[0], yerr= errr_holes_dut, xerr=0.5, fmt='green',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "holes, including DUT")
ax2.grid(which="both", axis="both")
ax2.set_xlabel("number of traversed planes")
ax2.set_ylabel("Ratio holes/all measured events")
ax2.set_title("Ratio of total evetns to events with holes")
ax2.legend()
plt.show()


# plotting MORE ON HOLES
fig, (ax1,ax2) = plt.subplots(1,2,figsize=(14, 10))
# ax1.set_yscale("log")
ax1.grid(which="both", axis="both")
ax1.errorbar(plane[1:6], mean_holes[1:6], yerr=d_mean_holes[1:6], xerr=0.5, fmt='k',
             elinewidth=1.5, lw=0, capsize=6, capthick=1.5, label= "Total rate of holes")
ax1.errorbar(plane[1:6], mean_holes_i[0,1:6], yerr= d_mean_holes_i[0,1:6], xerr=0.5, fmt='r',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "Rate of 1 hole events")
ax1.errorbar(plane[1:6], mean_holes_i[1,1:6], yerr= d_mean_holes_i[1,1:6] , xerr=0.5, fmt='b',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "Rate of 2 hole events")
ax1.errorbar(plane[1:6], mean_holes_i[2,1:6], yerr= d_mean_holes_i[2,1:6] , xerr=0.5, fmt='green',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "Rate of 3 hole events")

ax1.set_xlabel("number of traversed planes")
ax1.set_ylabel("mean rate $[1/s]$")
ax1.set_title("Mean rate of holes")
ax1.legend()

errr_holes_1 = mean_holes_i_dut[0]/mean_holes_i[0]*np.sqrt((d_mean_holes_i[0]/mean_holes_i[0])**2+
 (d_mean_holes_i_dut[0]/mean_holes_i_dut[0])**2)
errr_holes_2 = mean_holes_i_dut[1]/mean_holes_i[1]*np.sqrt((d_mean_holes_i[1]/mean_holes_i[1])**2+
 (d_mean_holes_i_dut[1]/mean_holes_i_dut[1])**2)
errr_holes_3 = mean_holes_i_dut[2]/mean_holes_i[2]*np.sqrt((d_mean_holes_i[2]/mean_holes_i[2])**2+
 (d_mean_holes_i_dut[2]/mean_holes_i_dut[2])**2)

ax2.errorbar(plane[1:6], mean_holes_i_dut[0,1:6]/mean_holes_i[0,1:6], yerr= errr_holes_1[1:6], xerr=0.5, fmt='r',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "Ration of 1 hole events")
ax2.errorbar(plane[2:5], mean_holes_i_dut[1,2:5]/mean_holes_i[1,2:5], yerr= errr_holes_2[2:5], xerr=0.5, fmt='b',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "Ration of 2 hole events")
ax2.errorbar(plane[3], mean_holes_i_dut[2,3]/mean_holes_i[2,3], yerr= errr_holes_3[3], xerr=0.5, fmt='green',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "Ration of 3 hole events")             
ax2.grid(which="both", axis="both")
ax2.set_xlabel("number of traversed planes")
ax2.set_ylabel("Ratio of (incl. DUT)/all of  certain holenumber")
ax2.set_title("Ratio of holes per event to holes including DUT")
ax2.legend()
plt.show()


################## HOLE CORRECTION ####################

plt.figure(figsize=(12, 10))
plt.yscale("log")
plt.grid(which="both", axis="both")
# plt.errorbar(plane, intensity,yerr=np.array(intensity)*0.05 , xerr=0.5, fmt='k',
#               elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "Expected rate")
plt.errorbar(plane, mean[0], xerr=0.5, yerr=d_mean[0], fmt='r',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "Uncorrected rate")
plt.errorbar(plane, corr_mean, xerr=0.5, yerr=d_corr_mean, fmt='b',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "Corrected rate")
plt.xlabel("znumber of traversed planes")
plt.ylabel("Mean rate $[1/s]$")
plt.title("Mean rate correction by visible holes", fontsize=20)
plt.legend()
plt.show()


# plotting_TOTAL_HITS
fig, (ax1,ax2) = plt.subplots(1,2,figsize=(12, 10))
ax1.grid(which="both", axis="both")
ax1.errorbar(plane, mean[1], yerr=d_mean[1], xerr=0.5, fmt='k',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5)
# plt.errorbar(plane, mean[1], yerr=d_mean[1], xerr=0.5, fmt='r',
#              elinewidth=1.5, lw=0, capsize=3, capthick=1.5)
ax1.set_xlabel("Traversed plane")
ax1.set_ylabel("Mean rate $[1/s]$")
ax1.set_title("Mean rate measured hits per plane")#, fontsize=20)

ax2.grid(which="both", axis="both")

errr = mean[1]/mean[1,3]*np.sqrt((d_mean[1]/mean[1])**2+(d_mean[1,3]/mean[1,3])**2)
ax2.errorbar(plane, mean[1]/mean[1,3], yerr= errr, xerr=0.5, fmt='k',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5)
ax2.set_xlabel("Traversed plane")
ax2.set_ylabel("Ratio")
ax2.set_title("Ratio of hits per plane according to DUT")#, fontsize=20)
plt.show()

###creating a csv file
with open("muon_data_processing/csv/mean_rate_multi_plane_events.csv","w", newline="") as f:

    #creating the the header
    header = ["Plane","  mean", "  error"]
    writing = csv.DictWriter(f, fieldnames= header)

    # write the header in the file
    writing.writeheader()
    
    # put in the values
    for j in range(len(mean[0])):
        writing.writerow({"Plane" : j+1,"  mean" : mean[0,j], "  error" : d_mean[0,j]})

###creating a csv file
# with open("muon_data_processing/csv/hole_rates","w", newline="") as f:

#     #creating the the header
#     header = ["s2_h1","  mean", "  error"]
#     writing = csv.DictWriter(f, fieldnames= header)

#     # write the header in the file
#     writing.writeheader()
    
#     # put in the values
#     for j in range(len(mean[0])):
#         writing.writerow({"Plane" : j+1,"  mean" : mean[0,j], "  error" : d_mean[0,j]})