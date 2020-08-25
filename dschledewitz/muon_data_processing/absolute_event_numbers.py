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

# split the path to get the Runnumber, to seperate by run size(<416<606)
run=np.zeros(len(run_numbers))
for i in range(len(run_numbers)):
    run[i] = run_numbers[i].split("_")[1]
    run[i] = int(run[i])
plane = np.arange(1, 8)

sum_n_planes= np.zeros(7)
sum_hits_p_plane = np.zeros(7)

# summ all events 
for i in range(7):
    sum_n_planes[i] = sum(datas[:,i]) # n-plane-events
    sum_hits_p_plane[i] = sum(datas_hits[:,i]) # total_hits


# holes: sum them
sum_hole = np.zeros(9)
sum_hole_dut = np.zeros(9)
# summ all events 
for i in range(9):
    sum_hole[i] = sum(datas_holes[:,i]) # holes
    sum_hole_dut[i] = sum(datas_holes_dut[:,i]) # holes with dut
sum_holes = np.array([0,sum_hole[0],sum(sum_hole[1:3]), sum(sum_hole[3:6]), sum(sum_hole[6:8]),sum_hole[8],0])
sum_holes_dut = np.array([0,sum_hole_dut[0],sum(sum_hole_dut[1:3]), sum(sum_hole_dut[3:6]),
 sum(sum_hole_dut[6:8]),sum_hole_dut[8],0])



##################---PLOTTING   PLOTTING   PLOTTING---######################
fig, (ax1,ax2) = plt.subplots(1,2,figsize=(14, 10))
ax1.set_yscale("log")
ax1.grid(which="both", axis="both")
ax1.errorbar(plane[:3], sum_n_planes[:3], xerr=0.5, fmt='k',elinewidth=1.5, lw=0, capsize=3, capthick=1.5)
for i in range(3,7):
    ax1.errorbar(plane[i], sum_n_planes[i], xerr=0.5, fmt='r',elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= str(i+1)+" planes= %s"%(int(sum_n_planes[i])))
ax1.set_xlabel("Number of traversed planes")
ax1.set_ylabel("Counts")
ax1.set_title("Total number of measured multi-plane-events")
ax1.legend()

ax2.errorbar(plane, sum_hits_p_plane, xerr=0.5, fmt='k', elinewidth=1.5, lw=0, capsize=3, capthick=1.5)
ax2.grid(which="both", axis="both")
ax2.set_xlabel("Traversed plane")
ax2.set_ylabel("Counts")
ax2.set_title("Total number of measured hits per plane")
plt.show()

###holes
fig, (ax1,ax2) = plt.subplots(1,2,figsize=(14, 10))
ax1.set_yscale("log")
ax1.grid(which="both", axis="both")
ax1.errorbar(plane, sum_n_planes, xerr=0.5, fmt='k',
 elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label = "Total number of hits")
ax1.errorbar(plane, sum_holes, xerr= 0.5, fmt='b',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "Events with one ore more holes")
ax1.errorbar(plane, sum_holes_dut, xerr=0.5, fmt='green',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "Events with holes including the DUT")
ax1.set_xlabel("Number of traversed planes")
ax1.set_ylabel("Counts")
ax1.set_title("Considering events with holes", fontsize=20)
ax1.legend()

ax2.grid(which="both", axis="both")
ax2.errorbar(plane[1:6], sum_holes[1:6]/sum_n_planes[1:6], xerr= 0.5, fmt='b',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "Events with one ore more holes")
ax2.errorbar(plane[1:6], sum_holes_dut[1:6]/sum_n_planes[1:6], xerr=0.5, fmt='green',
             elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label= "Events with holes including the DUT")
ax2.set_xlabel("Number of traversed planes")
ax2.set_ylabel("Ratio hole/total events")
ax2.set_title("Ratio of total evetns to events with holes", fontsize=20)
ax2.legend()
plt.show()