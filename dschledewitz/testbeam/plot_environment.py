#!/usr/bin/python

# loading packages
import sys
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import os
import tempfile
import subprocess

#path to environmental data
data_path = "/home/david/Documents/testbeam/eudaq2/bin/Env_data"


# time=np.array([1,2,3,4,5,6,7,8,9])
# temperature_1 = np.array([17,18,18,21,20,19,20,18,19])
# temperature_2 = np.array([17,19,23,21,17,19,23,18,12])

# plt.figure(figsize=(12, 10))
# plt.grid(which="both", axis="both")
# plt.plot(time, temperature_1, 'g-',marker="o", markersize=1.5, lw=1, label="Temperature 1")

# plt.plot(time, temperature_2, 'b-',marker="o", markersize=1.5, lw=1, label="Temperature 2")

# plt.xlabel("Time [s]")
# plt.ylabel("Temperature [°C]")
# plt.title("Environmental Log", fontsize=20)
# plt.legend()

# pressure_1 = np.array([996,997,997,998,997,998,997,997,997])
# pressure_2 = np.array([993,993,993,994,994,995,996,998,998])

# plt.figure(figsize=(12, 10))
# plt.grid(which="both", axis="both")
# plt.plot(time, pressure_1, 'g-',marker="o", markersize=1.5, lw=1, label="Pressure 1")

# plt.plot(time, pressure_2, 'b-',marker="o", markersize=1.5, lw=1, label="Pressure 2")

# plt.xlabel("Time [s]")
# plt.ylabel("Pressure [mbar]")
# plt.title("Environmental Log", fontsize=20)
# plt.legend()


# rel_hum_1 = np.array([96,97,97,98,97,98,97,97,97])
# rel_hum_2 = np.array([93,93,93,94,94,95,96,98,98])

# plt.figure(figsize=(12, 10))
# plt.grid(which="both", axis="both")
# plt.plot(time, rel_hum_1, 'g-',marker="o", markersize=1.5, lw=1, label="Relative humidity 1")

# plt.plot(time, rel_hum_2, 'b-',marker="o", markersize=1.5, lw=1, label="Relative humidity 2")

# plt.xlabel("Time [s]")
# plt.ylabel("Relative humidity [%]")
# plt.title("Environmental Log", fontsize=20)
# plt.legend()
# plt.show()


# from mpl_toolkits.axes_grid1 import host_subplot
# import mpl_toolkits.axisartist as AA

# plt.figure(figsize=(12, 10))
# host = host_subplot(111, axes_class=AA.Axes)
# plt.subplots_adjust(right=0.75)

# par1 = host.twinx()
# par2 = host.twinx()

# offset = 60
# new_fixed_axis = par2.get_grid_helper().new_fixed_axis
# par2.axis["right"] = new_fixed_axis(loc="right", axes=par2,
#                                         offset=(offset, 0))

# par2.axis["right"].toggle(all=True)
# par1.axis["right"].toggle(all=True)

# host.set_xlim(1,9)
# host.set_ylim(15,25)

# host.grid(which="both", axis="both")
# plt.title("Environmental Log")
# host.set_xlabel("Time [s]")
# host.set_ylabel("Temperature [°C]")
# par1.set_ylabel("Pressure [mbar]")
# par2.set_ylabel("Relative humidity [%]")

# p1, = host.plot(time, temperature_1,"green", label="Temperature")
# p2, = par1.plot(time, pressure_1, "red", label="Pressure")
# p3, = par2.plot(time, rel_hum_2,"blue", label="Humidity")


# par1.set_ylim(990, 1000)
# par2.set_ylim(90,100)


# host.axis["left"].label.set_color(p1.get_color())
# par1.axis["right"].label.set_color(p2.get_color())
# par2.axis["right"].label.set_color(p3.get_color())



# host.legend()
# #plt.draw()
# plt.show()

# plt.errorbar(planas, Diff_y[0], yerr=Dsys_diff_y[0], xerr=0.5, fmt='blue',
#              elinewidth=1.5, lw=0, capsize=3, capthick=1.5, label="y-axis")