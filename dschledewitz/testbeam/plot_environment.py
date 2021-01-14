#!/usr/bin/python

# loading packages
import sys
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import os
import tempfile
import subprocess
import datetime
import matplotlib.dates as md
from math import log,exp

#path to environmental data
data_path = "/home/david/Documents/testbeam/eudaq2/bin/Env_data"


#######################---CONTAINER   CONTAINER   CONTAINER   CONTAINER---#############################
RUN = []
HUMIDITY = []
PRESSURE = []#np.array([])
TEMPERATURE = []# np.array([])
TIME =[] #np.array([])
SECONDS = []
TEMPERATURE_DAQ =[[],[],[],[],[],[],[]]# np.array([])
TIME_DAQ = [[],[],[],[],[],[],[]]# np.array([])
SECONDS_DAQ = [[],[],[],[],[],[],[]]
#######################---PRE-DEFINITIONS   PRE-DEFINITIONS   PRE-DEFINITIONS---#############################

def convert_time(Ctime):
    day,ctime = Ctime.split("T")
    h,m,s = ctime.split(":")
    seconds = int(day)*86400+int(h)*3600+int(m)*60+int(s)
    return seconds

def convert_time_day(Ctime):
    day,ctime = Ctime.split("T")
    h,m,s = ctime.split(":")
    h,m,s = int(h),int(m),int(s)
    final_time = datetime.datetime(year=2020,month=8,day=int(day),hour=h, minute=m, second=s)
    #final_time = datetime.time(hour=h, minute=m, second=s)
    #final_time = final_time.isoformat(timespec='auto')
    return final_time

#def to correct temperature
def correct_t(temp):
    TR = 298.15
    RT = 10000
    B = 3940
    t = temp + 271.15
    # print(t)
    r = RT * exp(B/TR-B/t)
    # print(r)
    v = 1.8/(1+r/5100)
    #print(v)
    # v *= 4096*1.8/3.3
    # v *= 3.3/4096*1.8------------> equal to 1.8²
    #v *= 1.8*1.8 
    #print(v)
    r=5100*(1.8-v)/v
    B = 3650
    #print(r)
    t=B*TR/(B+TR*log(r/RT))
    return t-273.15
    


#definition to get the data out of the txt
def extract(filename):

    # initilize container
    humidity = np.array([])
    pressure = np.array([])
    temperature = np.array([])
    time = np.array([])
    seconds = np.array([])
    temperature_daq = [[],[],[],[],[],[],[]]
    time_daq = [[],[],[],[],[],[],[]]
    seconds_daq = [[],[],[],[],[],[],[]]

    # Keys
    event   = "</Description>\n"
    run     = "run"

    # open file
    file = open(filename, "r")

    # create array
    dat = file.readlines()

    # read the lines
    for line in range(len(dat)):
        #first line
        if dat[line].startswith(run):
            Run= dat[line].split(run)[1]
            Run = Run.split("_")[0]

        # check wich kind of status we have
        elif dat[line].endswith(event):
            status = dat[line].split("<Description>")[1]
            status = status.split("_status</Description>")[0]
            
            #go into PTH (external sensor)
            if status.startswith("PTH"):

                #get Humidity, Pressure, Temperature and time
                humidity = np.append(humidity,float((dat[line+1].split("=")[1]).split(" ")[0]))
                pressure = np.append(pressure,float((dat[line+2].split("=")[1]).split(" ")[0]))
                temperature = np.append(temperature,float((dat[line+3].split("=")[1]).split(" ")[0]))
                time = np.append(time,convert_time_day((dat[line+4].split("=2020-08-2")[1]).split(".")[0]))
                seconds = np.append(seconds,convert_time((dat[line+4].split("=2020-08-2")[1]).split(".")[0]))

            #get DAQ-Board information 
            if status.startswith("ALPIDE"):
                plane = int(status.split("_")[2])
                #select tmeperature and time for each plane
                temperature_daq[plane].append(correct_t(float((dat[line+1].split("=")[1]).split(" ")[0])))
                time_daq[plane].append(convert_time_day((dat[line+2].split("=2020-08-2")[1]).split(".")[0]))
                seconds_daq[plane].append(convert_time((dat[line+2].split("=2020-08-2")[1]).split(".")[0]))

                # temperature_daq[plane] = np.append(temperature[plane],float((dat[line+1].split("=")[1]).split(" ")[0]))
                # time_daq[plane] = np.append(time_daq[plane],convert_time_day((dat[line+2].split("=2020-08-2")[1]).split(".")[0]))
                # seconds_daq[plane] = np.append(seconds_daq[plane],convert_time((dat[line+2].split("=2020-08-2")[1]).split(".")[0]))

    return Run, humidity, pressure, temperature, time, temperature_daq, time_daq, seconds, seconds_daq

#######################---MAIN_CODE   MAIN_CODE   MAIN_CODE   MAIN_CODE---#############################

# search in the compressed folder for files (sorted alphabetically/numerically?)

for i in sorted(os.listdir(data_path)):

    # events stored in the .txt files beginning with "Cosmics"
    if i.startswith("EnvironmentLog_2"):
        path = os.path.join(data_path, i)

        # get all informations we need, in the structure: [[run1],[run2],...]
        Run, humidity, pressure, temperature, time, temperature_daq, time_daq, seconds, seconds_daq = extract(path)
        
        # RUN = np.append(RUN,Run)
        # HUMIDITY = np.append(HUMIDITY, humidity)
        # PRESSURE = np.append(PRESSURE, pressure)
        # TEMPERATURE = np.append(TEMPERATURE, temperature)
        # TIME = np.append(TIME, time)
        # TEMPERATURE_DAQ = np.append(TEMPERATURE_DAQ, temperature_daq)
        # TIME_DAQ = np.append(TIME_DAQ, time_daq)
        RUN.append(Run)
        HUMIDITY.append(humidity)
        PRESSURE.append(pressure)
        TEMPERATURE.append(temperature)
        TIME.append(time)
        SECONDS.append(seconds)
        for i in range(7):
            TEMPERATURE_DAQ[i].append(temperature_daq[i])
            TIME_DAQ[i].append(time_daq[i])
            SECONDS_DAQ[i].append(seconds_daq[i])
        
## Plot the data
fig, ax = plt.subplots(1, 1, figsize = (10,6))
for i in range(len(TIME)):
    ax.plot(TIME[i], TEMPERATURE[i],color="red", lw=1, zorder= 1)
ax.plot(TIME[1], TEMPERATURE[1],color="red", lw=1, zorder= 1, label= "Dracal-sensor")
#line to distinguish between days
night = np.array([datetime.datetime(year=2020,month=8,day=7,hour=0),datetime.datetime(year=2020,month=8,day=8,hour=0)])
dates = np.array(["27.08.2020, 00:00","28.08.2020, 00:00"])
for i in range(len(night)):

    ax.axvline(x= night[i],lw=1, color="grey",alpha=0.5, zorder=0)#, label="27.08.2020, 00:00")
    ax.text(night[i]+datetime.timedelta(minutes=20), 24, dates[i],color = "grey",fontsize=6, alpha = 0.7, rotation=90, va='center', zorder=0)

# Set time format and the interval of ticks (every 15 minutes)
xformatter = md.DateFormatter('%H:%M')
xlocator = md.HourLocator( interval= 2)

## Set xtick labels to appear every 15 minutes
ax.xaxis.set_major_locator(xlocator)

## Format xtick labels as HH:MM
plt.gcf().axes[0].xaxis.set_major_formatter(xformatter)

# for i in range(1):    
#     plt.plot(SECONDS[1], TEMPERATURE[1], lw=1)
plt.gcf().autofmt_xdate()
plt.xlabel("Time [s]")
plt.ylabel("Temperature [°C]")
plt.title("Environmental Log: Temperatur from sensor", fontsize=20)
# plt.legend()
#plt.show()


colors = ["green","blue","yellow","black","pink","cyan","purple", "lightgreen"]
## Plot the data
#fig, ax1 = plt.subplots(1, 1, figsize = (10,6))
for i in range(len(TIME_DAQ)):
    ax.plot(TIME_DAQ[i][1], TEMPERATURE_DAQ[i][1], color= colors[i], lw=1, zorder= 1, label = "DAQ-board plane "+str(i))
    for j in range(len(TIME_DAQ[i])):
        ax.plot(TIME_DAQ[i][j], TEMPERATURE_DAQ[i][j], color= colors[i], lw=1, zorder= 1)
#line to distinguish between days
night = np.array([datetime.datetime(year=2020,month=8,day=7,hour=0),datetime.datetime(year=2020,month=8,day=8,hour=0)])
dates = np.array(["27.08.2020, 00:00","28.08.2020, 00:00"])
for i in range(len(night)):

    ax.axvline(x= night[i],lw=1, color="grey",alpha=0.5, zorder=0)#, label="27.08.2020, 00:00")
    ax.text(night[i]+datetime.timedelta(minutes=20), 24, dates[i],color = "grey",fontsize=6, alpha = 0.7, rotation=90, va='center', zorder=0)

# Set time format and the interval of ticks (every 15 minutes)
xformatter = md.DateFormatter('%H:%M')
xlocator = md.HourLocator( interval= 2)

## Set xtick labels to appear every 15 minutes
ax.xaxis.set_major_locator(xlocator)

## Format xtick labels as HH:MM
plt.gcf().axes[0].xaxis.set_major_formatter(xformatter)

# for i in range(1):    
#     plt.plot(SECONDS[1], TEMPERATURE[1], lw=1)
plt.gcf().autofmt_xdate()
plt.xlabel("Time [s]")
plt.ylabel("Temperature [°C]")
plt.title("Environmental Log: Temperature", fontsize=20)
plt.legend()
plt.show()



##############################################'PRESSURE##################################################

## Plot the data
fig, ax = plt.subplots(1, 1, figsize = (10,6))
for i in range(len(TIME)):
    ax.plot(TIME[i], PRESSURE[i],color="red", lw=1, zorder= 1)
# ax.plot(TIME[1], TEMPERATURE[1],color="red", lw=1, zorder= 1, label= "Dracal-sensor")
#line to distinguish between days
night = np.array([datetime.datetime(year=2020,month=8,day=7,hour=0),datetime.datetime(year=2020,month=8,day=8,hour=0)])
dates = np.array(["27.08.2020, 00:00","28.08.2020, 00:00"])
for i in range(len(night)):

    ax.axvline(x= night[i],lw=1, color="grey",alpha=0.5, zorder=0)#, label="27.08.2020, 00:00")
    ax.text(night[i]+datetime.timedelta(minutes=20), 996, dates[i],color = "grey",fontsize=6, alpha = 0.7, rotation=90, va='center', zorder=0)

# Set time format and the interval of ticks (every 15 minutes)
xformatter = md.DateFormatter('%H:%M')
xlocator = md.HourLocator( interval= 2)

## Set xtick labels to appear every 15 minutes
ax.xaxis.set_major_locator(xlocator)

## Format xtick labels as HH:MM
plt.gcf().axes[0].xaxis.set_major_formatter(xformatter)

# for i in range(1):    
#     plt.plot(SECONDS[1], TEMPERATURE[1], lw=1)
plt.gcf().autofmt_xdate()
plt.xlabel("Time [s]")
plt.ylabel("Temperature [mbar]")
plt.title("Environmental Log: Pressure from sensor", fontsize=20)


# plt.figure(figsize=(12, 10))
# #plt.grid(which="both", axis="both")
# for i in range(len(PRESSURE)):
#     plt.plot(TIME[i], PRESSURE[i], 'g-', lw=1)
# plt.xlabel("Time [s]")
# plt.ylabel("Pressure [mbar]")
# plt.title("Environmental Log: Pressure from Dracal sensor", fontsize=20)
# plt.legend()


# plt.figure(figsize=(12, 10))
#plt.grid(which="both", axis="both")
# for i in range(len(HUMIDITY)):
#     plt.plot(TIME[i], HUMIDITY[i], 'g-', lw=1)
# plt.xlabel("Time [s]")
# plt.ylabel("Relative humidity [%]")
# plt.title("Environmental Log: Humidity from Dracal sensor", fontsize=20)
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