#!/usr/bin/python
import numpy as np
import matplotlib.pyplot as plt
import csv


# load data, seperate by runsize later, seconds: 480,864,1006
datas = np.loadtxt("muon_data_processing/csv/n-event-plane-data.csv", delimiter=",", skiprows=1, usecols=(1,2,3,4,5,6,7),dtype=int)
run_numbers = np.loadtxt("muon_data_processing/csv/n-event-plane-data.csv", delimiter=",", skiprows=1, usecols=(0), dtype=str)
# DUT_data
datas_DUT = np.loadtxt("muon_data_processing/csv/DUT_n-event-plane-data.csv", delimiter=",", skiprows=1, usecols=(1,2,3,4,5,6,7),dtype=int)

# theorietical data
intensit = [0.4651, 0.3317, 0.1526, 0.06826, 0.03156, 0.01408, 0.005013, 0]
intensity = []
for i in range(len(intensit)-1):
    intensity.append(intensit[i]-intensit[i+1])

# split the path to get the Runnumber, to seperate by run size(<416<606)
run=np.zeros(len(run_numbers))
for i in range(len(run_numbers)):
    run[i] = run_numbers[i].split("_")[1]
    run[i] = int(run[i])
plane = np.arange(1, 8)

# normalize all runs, to be able to compare them
data=np.zeros((len(run_numbers),7))
data_DUT=np.zeros((len(run_numbers),7))
for i in range(len(run_numbers)):

    if int(run[i]) < 416:
        data[i] = datas[i]/480
        data_DUT[i] = datas_DUT[i]/480
    elif int(run[i])<606:
        data[i] = datas[i]/864
        data_DUT[i] = datas_DUT[i]/864
    else:
        data[i] = datas[i]/1006
        data_DUT[i] = datas_DUT[i]/1006

# calculate mean
def Mean(val,d_val):
    mean = np.mean(val)
    stat= np.sqrt(1/(val.size*(val.size-1))*np.sum((mean-val)**2)) #statistischer Fehler
    sys = np.mean(d_val) #systematischer Fehler
    #print("Meanvalue:" , mean, " +- ", stat, "stat. +- ", sys, "sys")
    return mean, stat, sys

mean = np.zeros((2,7)) #first data, DUT
d_mean = np.zeros((2,7))
for i in range(7):
    mean[0,i], d_mean[0,i], sys_1 = Mean(data[:,i],0) # n-plane-events
    mean[1,i], d_mean[1,i], sys_1 = Mean(data_DUT[:,i],0) # DUT_events

# plotting
fig, (ax1,ax2) = plt.subplots(1,2,figsize=(14, 10))
ax1.set_yscale("log")
ax1.grid(which="both", axis="both")
ax1.errorbar(plane, mean[0], yerr=d_mean[0], xerr=0.5, fmt='r',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "Measured mean rate")
ax1.errorbar(plane, intensity, xerr=0.5, fmt='k',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "Expected mean rate")
ax1.errorbar(plane, mean[1], yerr=d_mean[1], xerr=0.5, fmt='b',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "Mean rate including DUT")
ax1.set_xlabel("Number of traversed planes")
ax1.set_ylabel("Mean rate $[1/s]$")
ax1.set_title("Mean rate of measured and expected multi-plane-events")
ax1.legend()

errr = mean[0]/mean[1]*np.sqrt((d_mean[0]/mean[0])**2+(d_mean[1]/mean[1])**2)
ax2.errorbar(plane, mean[0]/mean[1], yerr= errr, xerr=0.5, fmt='r',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "Ratio of total to including DUT n-p-e")
ax2.errorbar(plane, (8-plane)/np.array([1,2,3,4,3,2,1]), xerr=0.5, fmt='k',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "Ratio of combinations total to incl. DUT n-p-e")
ax2.grid(which="both", axis="both")
ax2.set_xlabel("Number of traversed plane")
ax2.set_ylabel("Ratio n-p-e total/(incl. DUT)")
ax2.set_title("Ratio of total n-plane-events to including DUT n-plane-events")
ax2.legend()
plt.show()
