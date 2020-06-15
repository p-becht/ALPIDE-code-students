#!/usr/bin/python

# loading packages
import sys
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import os
import tempfile
import subprocess


angles = [90, 50.1944, 30.9637, 21.8014, 16.6992, 13.4957, 11.3099, 0]
intensit = [0.4651, 0.3317, 0.1526, 0.06826, 0.03156, 0.01408, 0.005013,0]
plane = np.arange(1, 8)

intensity = []
ranges = []
d_ranges = []
for i in range(len(angles)-1):
    ranges.append((angles[i] + angles[i+1])/2)
    d_ranges.append((angles[i] - angles[i+1])/2)
    intensity.append(intensit[i]-intensit[i+1])

plt.figure(figsize=(12, 10))
# plt.subplots_adjust(bottom=0.28,top=0.95)
# plotting
plt.xlim(90, 0)
plt.yscale("log")
plt.grid(which="both", axis="both")
plt.errorbar(ranges, intensity, xerr=d_ranges, fmt='k',
             elinewidth=1.5, lw=0, capsize=4, capthick=2)
# plt.plot(RN,MEAN,marker=".",linewidth=0)
plt.xlabel("zenith angle ranges [°]")
plt.ylabel("Expected intensity $[1/s]$")
plt.title("Expectation for measurable angular distribution", fontsize=20)
plt.show()
