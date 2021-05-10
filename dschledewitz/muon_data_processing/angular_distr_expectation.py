#!/usr/bin/python

# loading packages
import sys
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import os
import tempfile
import subprocess


#angles = np.array([90, 50.1944, 30.9637, 21.8014, 16.6992, 13.4957, 11.3099, 0])
angles = np.array([90, 30.9638, 16.6992, 11.3099, 8.5308, 6.8428, 5.7106, 0])
radian = angles*2*np.pi/360
#intensit = [0.07812, 0.1486, 0.06728, 0.02618, 0.01044, 0.004055, 0.005013, 0]
intensit = [0.202778, 0.096772, 0.022202, 0.006841, 0.002465, 0.000909, 0.001056,0]
results_d = np.array([2.70812528e-03, 6.59204309e-04, 2.87728413e-04, 1.61231336e-04, 8.62297043e-05, 6.10805891e-05, 5.71673851e-05])
#results_d = np.loadtxt("muon_data_processing/csv/mean_rate_multi_plane_events.csv", delimiter=",", skiprows=1, usecols=(2))
results = np.array([0.77014841, 0.10774474, 0.0229592,  0.0069235,  0.00242199, 0.0010189, 0.00099113])
#results = np.loadtxt("muon_data_processing/csv/mean_rate_multi_plane_events.csv", delimiter=",", skiprows=1, usecols=(1))
print(results)
plane = np.arange(1, 8)

intensity = np.zeros(7)
intensity_d = np.zeros(7)
ranges = []
d_ranges = []
for i in range(len(angles)-1):
    # define angular range
    ranges.append((angles[i] + angles[i+1])/2)
    d_ranges.append((angles[i] - angles[i+1])/2)
    # define intensity for angular range
    intensity[i]    = intensit[i]#/0.00758*0.0004
    intensity_d[i]  = intensit[i]/0.00758*0.0004
    # reduce intensity to flux (1/multipl., 1/area)
    intensity[i]    = intensity[i]/(8-plane[i])/4.5
    intensity_d[i] =intensity_d[i]/(8-plane[i])/4.5
    results[i]      =   results[i]/(8-plane[i])/4.5
    results_d[i]    = results_d[i]/(8-plane[i])/4.5

    # calculate the absolute vertical integral intensity
    intensity[i]    = intensity[i]*2/np.pi/(np.cos(radian[i+1])**4-np.cos(radian[i])**4)
    intensity_d[i]= intensity_d[i]*2/np.pi/(np.cos(radian[i+1])**4-np.cos(radian[i])**4)
    results[i]      =   results[i]*2/np.pi/(np.cos(radian[i+1])**4-np.cos(radian[i])**4)
    results_d[i]    = results_d[i]*2/np.pi/(np.cos(radian[i+1])**4-np.cos(radian[i])**4)
    #integrate over range to get angular distr (cos²)
    intensity[i]    = intensity[i]*1/2*(radian[i]-radian[i+1]+1/2*(np.sin(2*radian[i])-np.sin(2*radian[i+1])))/(radian[i]-radian[i+1])
    intensity_d[i] =intensity_d[i]*1/2*(radian[i]-radian[i+1]+1/2*(np.sin(2*radian[i])-np.sin(2*radian[i+1])))/(radian[i]-radian[i+1])
    results[i]      =   results[i]*1/2*(radian[i]-radian[i+1]+1/2*(np.sin(2*radian[i])-np.sin(2*radian[i+1])))/(radian[i]-radian[i+1])
    results_d[i]    = results_d[i]*1/2*(radian[i]-radian[i+1]+1/2*(np.sin(2*radian[i])-np.sin(2*radian[i+1])))/(radian[i]-radian[i+1])
    #print(intensity[i])
print(results)
n = ["7-p-e", "6-p-e", "5-p-e", "4-p-e", "3-p-e", "2-p-e", "1-p-e"]
cos_x = np.linspace(0,90,1000)
cos_y = np.cos(cos_x*2*np.pi/360)**2*7.6

plt.figure(figsize=(9, 5.8))
plt.xlim(0, 90)
plt.ylim(0,8)
#plt.yscale("log")
plt.grid(which="both", axis="both")
# plt.errorbar(ranges, 1000*intensity, xerr=d_ranges, yerr=1000*intensity_d, fmt='k',
#              elinewidth=1.5, lw=0, capsize=3, capthick=2, label= "expected distribution")
# plt.errorbar(ranges[1:], 1000*intensity[1:], xerr=d_ranges[1:], yerr=1000*intensity_d[1:], fmt='k',
#              elinewidth=1.5, lw=0, capsize=3, capthick=2, label= "expected distribution")
# plt.errorbar(ranges[1:], 1000*results[1:], xerr=d_ranges[1:], yerr=1000*results_d[1:], fmt='r',
#               elinewidth=1.5, lw=0, capsize=3, capthick=2, label= "measured distribution")
plt.errorbar(cos_x,cos_y,fmt='b', label= "$cos^2(\Theta)$-distribution")

# for i, txt in enumerate(n):
#     plt.annotate(txt, (ranges[i], 1000*intensity[i]+np.array([0.2,0.4,0.4,0.3,0.4,0.7,0.45])[i]), color = "red")#, textcoords = ("offset points","offset points"))

plt.xlabel("Zenith angle [°]", fontsize=18)
plt.ylabel("Muon flux $ [10^{-3} \: s^{-1} \: cm^{-2}] $ ", fontsize=18)
plt.tick_params(axis='both', labelsize=18)
#plt.title("Expected angular distribution", fontsize=20)
# plt.legend(fontsize=18)
#plt.savefig("/home/david/Desktop/figures/compare_distribution.png", dpi=300)
#plt.savefig("/home/david/Desktop/Bachelor_images/expected_distribution.png", dpi=300)
plt.savefig("/home/david/Desktop/Bachelor_images/presentation_cos2.png", dpi=300)
# plt.show()
