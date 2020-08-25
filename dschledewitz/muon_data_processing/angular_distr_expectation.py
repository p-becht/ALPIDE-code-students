#!/usr/bin/python

# loading packages
import sys
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import os
import tempfile
import subprocess


angles = np.array([90, 50.1944, 30.9637, 21.8014, 16.6992, 13.4957, 11.3099, 0])
radian = angles*2*np.pi/360
intensit = [0.07812, 0.1486, 0.06728, 0.02618, 0.01044, 0.004055, 0.005013, 0]
results_d = np.loadtxt("muon_data_processing/csv/mean_rate_multi_plane_events.csv", delimiter=",", skiprows=1, usecols=(2))
results = np.loadtxt("muon_data_processing/csv/mean_rate_multi_plane_events.csv", delimiter=",", skiprows=1, usecols=(1))
print(results)
plane = np.arange(1, 8)

intensity = np.zeros(7)
ranges = []
d_ranges = []
for i in range(len(angles)-1):
    # define angular range
    ranges.append((angles[i] + angles[i+1])/2)
    d_ranges.append((angles[i] - angles[i+1])/2)
    # define intensity for angular range
    intensity[i] = intensit[i]#/0.0094*0.003
    # reduce intensity to flux (1/multipl., 1/area)
    intensity[i] = intensity[i]/(8-plane[i])/4.5
    results[i] = results[i]/(8-plane[i])/4.5
    results_d[i] = results_d[i]/(8-plane[i])/4.5
    # calculate the integral vertical flux
    intensity[i] = intensity[i]*2/np.pi/(np.cos(radian[i+1])**4-np.cos(radian[i])**4)
    results[i] = results[i]*2/np.pi/(np.cos(radian[i+1])**4-np.cos(radian[i])**4)
    results_d[i] = results_d[i]*2/np.pi/(np.cos(radian[i+1])**4-np.cos(radian[i])**4)
    #integrate over range to get angular distr
    intensity[i] = intensity[i]*1/2*(radian[i]-radian[i+1]+1/2*(np.sin(2*radian[i])-np.sin(2*radian[i+1])))/(radian[i]-radian[i+1])
    results[i] = results[i]*1/2*(radian[i]-radian[i+1]+1/2*(np.sin(2*radian[i])-np.sin(2*radian[i+1])))/(radian[i]-radian[i+1])
    results_d[i] = results_d[i]*1/2*(radian[i]-radian[i+1]+1/2*(np.sin(2*radian[i])-np.sin(2*radian[i+1])))/(radian[i]-radian[i+1])
    #print(intensity[i])
print(results)



plt.figure(figsize=(12, 10))
plt.xlim(0, 90)
plt.yscale("log")
plt.grid(which="both", axis="both")
plt.errorbar(ranges, intensity, xerr=d_ranges, fmt='k',
             elinewidth=1.5, lw=0, capsize=3, capthick=2, label= "expected distribution")
# plt.errorbar(ranges, results, xerr=d_ranges, yerr=results_d, fmt='r',
#              elinewidth=1.5, lw=0, capsize=3, capthick=2, label= "measured distribution")
# plt.plot(RN,MEAN,marker=".",linewidth=0)
plt.xlabel("zenith angle ranges [°]")
plt.ylabel("rate $[1/s]$")
plt.title("Expected angular distribution", fontsize=20)
plt.legend()
plt.show()
