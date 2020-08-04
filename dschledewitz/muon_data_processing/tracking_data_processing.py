#!/usr/bin/python

# loading packages
import sys
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import os
import tempfile
import subprocess
import csv
from scipy.optimize import curve_fit

# we want to import the the txt files
# import the files and get the information out of them

# get the path to read the files, U HAVE TO BE IN "dschledewitz"
data_path = os.path.join(os.path.split(os.path.split(os.path.dirname(__file__))[
                         0])[0], "data","compressed")

#######################---CONTAINER   CONTAINER   CONTAINER   CONTAINER---#############################
X_coordinates = []
Y_coordinates = []
Event = []
Eventsize = []
run = []
Mean_x = []
Mean_y = []
DMean_x = []
DMean_y = []

#alignement:desy2020
x_align = np.loadtxt("muon_data_processing/csv/alignement_data.csv", delimiter=",", skiprows=1, usecols=(1),dtype=float)
#np.array([0., -10.70464573,  -6.25092376,   0.19635326,  25.26630419, 27.30091044,  78.18635686])
d_x_align =  np.loadtxt("muon_data_processing/csv/alignement_data.csv", delimiter=",", skiprows=1, usecols=(3),dtype=float)
y_align = np.loadtxt("muon_data_processing/csv/alignement_data.csv", delimiter=",", skiprows=1, usecols=(4),dtype=float) 
#np.array([0., 45.15699466, -10.91364125,  -6.02800581,  -6.77937338, 33.55199672,  19.33957541])
d_y_align = np.loadtxt("muon_data_processing/csv/alignement_data.csv", delimiter=",", skiprows=1, usecols=(6),dtype=float)
#######################---PRE-DEFINITIONS   PRE-DEFINITIONS   PRE-DEFINITIONS---#############################

# MEAN
def Mean(val, d_val):
    if val.size == 1:
        return int(val), 0  # ,0
    else:
        mean = np.mean(val)
        stat = np.sqrt(1/(val.size*(val.size-1)) *
                    np.sum((mean-val)**2))  # statistischer Fehler
        # sys = np.mean(d_val) #systematischer Fehler
        # print("Meanvalue:" , mean, " +- ", stat, "stat. +- ", sys, "sys")
        return mean, stat  # , sys

#CHISQUARED
#Chi-quadrat-test p=x, n=y, dn=d_y, 
def chi(x,y,dy):
    chi2_=np.sum((lin(x,*fit)-y)**2/dy**2)
    dof=len(x)-2 #dof:degrees of freedom, Freiheitsgrad  - zahl der variablen
    chi2_red=chi2_/dof
    #print("chi2=", chi2_)
    #print("chi2_red=",chi2_red)
    return chi2_red


# goodness
def R2(y,y_fit):
    # residual sum of squares
    ss_res = np.sum((y - y_fit) ** 2)

    # total sum of squares
    ss_tot = np.sum((y - np.mean(y)) ** 2)

    # r-squared
    r2 = 1 - (ss_res / ss_tot)
    return r2

# defenition to count the number of n-plane-events and save the sequence
def counter(filename):

    # initilize container
    # global event
    # global eventsize
    # global x_coordinates
    # global y_coordinates

    x_coordinates = []
    y_coordinates = []
    event = []
    eventsize = []

    # Keys
    key_event = "==="
    key_num = "--- pALPIDEfs_"

    # open file
    file = open(filename, "r")

    # create array
    dat = file.readlines()

    # starting variable
    startv = False

    # read the lines
    for line in range(len(dat)):
        # skip first 10 lines
        if line < 10:
            iter = 0
            x_coor = [[], [], [], [], [], [], []]
            y_coor = [[], [], [], [], [], [], []]
        # elif line > 1000:
        #     return event, eventsize, x_coordinates, y_coordinates

        else:
            # looking for hits, signed with keyword ("--- pALPIDEfs_")
            if dat[line].startswith(key_num):
                # check, if event started with this hit, signed with ("===")
                # if yes, start new event
                # if no, count hit to the current event
                if dat[line-1].startswith(key_event):
                    # exclude process for very first event
                    if startv == False:
                        startv = True
                        iter += 1
                        # get the number of the plane for this event
                        plane_n = int(
                            (dat[line].split("_")[1]).split(" ")[0])+1
                        # note event_number
                        event_num = int((dat[line-1].split(" ")[1]))
                        # x coordinates
                        k = 1
                        while dat[line+k].startswith("Pixel"):
                            x_coor[plane_n -
                                    1].append(int((dat[line+k].split(" ")[1]).split(",")[0]))
                            y_coor[plane_n -
                                    1].append(int((dat[line+k].split(" ")[2]).split(",")[0]))
                            k += 1

                    else:
                        # count the number of events in this run and reset counting
########################## NUMBER OF PLANES ##############################################################
########################## NUMBER OF PLANES ##############################################################
########################## NUMBER OF PLANES ##############################################################
########################## NUMBER OF PLANES ##############################################################

                        if iter >= 4:
                            event.append(event_num)
                            eventsize.append(iter)
                            x_coordinates.append(x_coor)
                            y_coordinates.append(y_coor)

                        event_num = int((dat[line-1].split(" ")[1]))

                        # get the number of the plane for this event
                        plane_n = int(
                            (dat[line].split("_")[1]).split(" ")[0])+1
                        # note event_number
                        event_num = int((dat[line-1].split(" ")[1]))
                        # x coordinates
                        k = 1
                        x_coor = [[], [], [], [], [], [], []]
                        y_coor = [[], [], [], [], [], [], []]
                        iter = 1

                        while dat[line+k].startswith("Pixel") and line+k<len(dat)-1:
                            x_coor[plane_n -
                                    1].append(int((dat[line+k].split(" ")[1]).split(",")[0]))
                            y_coor[plane_n -
                                    1].append(int((dat[line+k].split(" ")[2]).split(",")[0]))
                            k += 1


                else:
                    # count as hit in the event
                    iter += 1
                    # get the number of the plane for this event
                    plane_n = int((dat[line].split("_")[1]).split(" ")[0])+1
                    # x coordinates
                    k = 1
                    while dat[line+k].startswith("Pixel") and line+k<len(dat)-1:
                        x_coor[plane_n -
                                1].append(int((dat[line+k].split(" ")[1]).split(",")[0]))
                        y_coor[plane_n -
                                1].append(int((dat[line+k].split(" ")[2]).split(",")[0]))
                        k += 1

    return event, eventsize, x_coordinates, y_coordinates

# definition to calculate the mean out of the pixel position
def mean_position_align(x_coordinates, y_coordinates):

    # container
    mean_x = []
    mean_y = []
    d_mean_x = []
    d_mean_y = []
    
    good_event = True

    for i in range(len(x_coordinates)):
        m_x = [[],[],[],[],[],[],[]] #np.array([])
        m_y = [[],[],[],[],[],[],[]] #np.array([])
        dm_x = [[],[],[],[],[],[],[]]
        dm_y = [[],[],[],[],[],[],[]]
        for j in range(0, 7):
            # get the pixel coordinates of one event and calculate the mean
            if np.array(x_coordinates[i][j]).size != 0:
                x,dx = Mean(np.array(x_coordinates[i][j]),0)
                y,dy = Mean(np.array(y_coordinates[i][j]),0)
                if dx == 0:
                    dx = 1/np.sqrt(12)
                if dy == 0:
                    dy = 1/np.sqrt(12)
                m_x[j] = x-x_align[j]
                m_y[j] = y-y_align[j]
                dm_x[j] = np.sqrt(dx**2 + d_x_align[j]**2)
                dm_y[j] = np.sqrt(dy**2 + d_y_align[j]**2)
                if dx > 15 or dy > 15:
                    good_event = False
        # take hole event to container
        if good_event == False:
            good_event = True
            continue
        mean_x.append(m_x)
        mean_y.append(m_y)
        d_mean_x.append(dm_x)
        d_mean_y.append(dm_y)
        #print(i)
    return mean_x, d_mean_x, mean_y, d_mean_y
#######################---MAIN_CODE   MAIN_CODE   MAIN_CODE   MAIN_CODE---#############################

# search in the compressed folder for files (sorted alphabetically/numerically?)

for i in sorted(os.listdir(data_path)):

    # events stored in the .txt files beginning with "Cosmics"
    if i.startswith("Cosmics000"):
        path = os.path.join(data_path, i)

        # split the path to get the Runnumber
        cosmic_run, file_type = i.split("_")
        runnumber = cosmic_run.split("smics")[1]
        RUN = "Run_"+runnumber

        # fill the counted events in the container
        event, eventsize, x_coordinates, y_coordinates = counter(path)
        mean_x, d_mean_x, mean_y, d_mean_y = mean_position_align(x_coordinates, y_coordinates)
        Event.append(event)
        Eventsize.append(eventsize)
        run.append(RUN)
        X_coordinates.append(x_coordinates)
        Y_coordinates.append(y_coordinates)
        Mean_x.append(mean_x)
        Mean_y.append(mean_y)
        DMean_x.append(d_mean_x)
        DMean_y.append(d_mean_y)

#print(Mean_x[325])
#print(DMean_x[325])
#print(run[325])


########### fitting process

#from scipy.optimize import curve_fit
def lin(x, a, b):

    return a * x +b
Phi =  np.array([])
Phi1 =  np.array([])
Chi =  np.array([])
Chi1 = np.array([])
it = 0
chi11,chi12,chi21,chi22 = 0,20,20,1000
phi11,phi12,phi21,phi22 = 24,26,26,28#0,2,2,4
fig, (ax1,ax2) = plt.subplots(1,2,figsize=(14, 10))
ax1.set_xlim([0, 1024])
ax1.set_ylim([0, 512])
ax2.set_xlim([0, 1024])
ax2.set_ylim([0, 512])
ax1.set_xlabel("x-axis [pixels]")
ax2.set_xlabel("x-axis [pixels]")
ax1.set_ylabel("y-axis [pixels]")
ax2.set_ylabel("y-axis [pixels]")
ax1.set_title("Selection of aligned cosmic events")
#ax2.set_title("Selection of aligned cosmic events with fits")
#ax2.set_title("Fitted tracks")
fig2, (ax3,ax4) = plt.subplots(1,2,figsize=(14, 10))
ax1.set_xlim([0, 1024])
ax1.set_ylim([0, 512])
ax2.set_xlim([0, 1024])
ax2.set_ylim([0, 512])
ax1.set_xlabel("x-axis [pixels]")
ax2.set_xlabel("x-axis [pixels]")
ax1.set_ylabel("y-axis [pixels]")
ax2.set_ylabel("y-axis [pixels]")
ax1.set_title("Selection of aligned cosmic events with $\chi^2_{red}$<= "+str(chi12))


# select an event
for i in range(len(Mean_x)):
    for j in range(len(Mean_x[i])):
        x,y,dy,z = np.array([]),np.array([]),np.array([]),np.array([])
        for k in range(len(Mean_y[i][j])):
            # check if plane k in event j in run i is not empty
            if np.array(Mean_y[i][j][k]).size != 0:
                # get the coordinate information
                y = np.append(y, Mean_y[i][j][k])
                x = np.append(x, Mean_x[i][j][k])
                dy = np.append(dy, DMean_y[i][j][k])
                z = np.append(z,k)
        # linear fit on the track data
        fit, err = curve_fit(lin,x,y, sigma = dy)
        
        # get the corresponding y value for the fit (CHISQUARE TEST)
        y_fit = x*fit[0] + fit[1]

#           transform pixel and plane coordinates to meter and calculate the zenith angle
        d_x = (x[-1]-x[0])*29.24e-6
        d_y = (y[-1]-y[0])*26.88e-6
        d_z = (z[-1]-z[0])*2e-2
        d_t = np.sqrt(d_x**2 + d_y**2)

        #print(R2(np.array(Mean_x[i][j]),y_fit))
        #if (np.sqrt(np.diag(err)[0])/fit[0]*100) <5 and (np.sqrt(np.diag(err)[1])/fit[1]*100) <5:
            #print("yes: "+ str(R2(y,y_fit)))
            #pass
            #ax2.plot(Mean_x[i][j],Mean_y[i][j])
        # if R2(y,y_fit)>0.8 and R2(y,y_fit)<0.85:
        #     print(R2(y,y_fit))
        #     ax2.plot(Mean_x[i][j],Mean_y[i][j])
        # if R2(y,y_fit)>0.8:
            # print("good")
            # print(R2(y,y_fit))
            # ax1.plot(x,y)
            # ax1.plot(x,y_fit,"r")
            # d_x = (x[-1]-x[0])*29.24e-6
            # d_y = (y[-1]-y[0])*26.88e-6
            # d_z = (z[-1]-z[0])*2e-2
            # d_t = np.sqrt(d_x**2 + d_y**2)
            # phi = np.arctan(d_t/d_z)*360/(2*np.pi)
            # Phi = np.append(Phi,phi) 
        
        #chi_ = chi(x,y,dy)
        #Chi = np.append(Chi,chi_)
        # chi square
        if (chi(x,y,dy))>chi11 and (chi(x,y,dy))<chi12:
            #print("good")
            #print(R2(y,y_fit))
            chi_ = chi(x,y,dy)
            Chi = np.append(Chi,chi_)
            Chi1 = np.append(Chi1,chi_)
            ax1.plot(x[0],y[0],"ro")
            ax1.plot(x,y, label = "$\chi^2_{red}$= "+str(int(chi_))+", "+str(int(len(x)))+"pE")
            #ax1.plot(x,y_fit,"r")
            #it += 1
            #print("yeay"+str(it))
            
            
            phi = np.arctan(d_t/d_z)*360/(2*np.pi)
            Phi = np.append(Phi,phi)
            if phi > phi11 and phi < phi12:
                ax3.plot(x[0],y[0],"ro")
                ax3.plot(x,y)
        



        if (chi(x,y,dy))>chi21 and (chi(x,y,dy))<chi22:
           # ax2.plot(x,y)
        #else:
            #it += 1
            #print("bad")
            #print(R2(y,y_fit)

        #else:
            chi_ = chi(x,y,dy)
            ax2.plot(x[0],y[0],"ro")
            ax2.plot(x,y, label = "$\chi^2_{red}$= "+str(int(chi_))+", "+str(int(len(x)))+"pE")
            # chi_ = chi(x,y,dy)
            # Chi = np.append(Chi,chi_)
            chi1 = chi(x,y,dy)
            Chi1 = np.append(Chi1,chi1)
            phi = np.arctan(d_t/d_z)*360/(2*np.pi)
            if phi > phi21 and phi < phi22:
                ax4.plot(x[0],y[0],"ro")
                ax4.plot(x,y)
        phi1 = np.arctan(d_t/d_z)*360/(2*np.pi)
        Phi1 = np.append(Phi1,phi1) 
        

            #ax2.plot(x,y_fit,"r")
        #     print(R2(y,y_fit))
            #print("fuck"+str(it))
    #x = np.linspace(np.min((REF[i,0,0], REF[i,0,5]))-5,np.max((REF[i,0,0], REF[i,0,5]))+5, 100)
    #y = fit[0] * x + fit[1]
        # print(fit)
        # print(np.sqrt(np.diag(err)))
        #print(np.sqrt(np.diag(err))/fit*100)
# ax2.plot(x,y_fit,"green",label="good fit, R>0.8")# $\chi^2$<1000")
# ax2.plot(x,y_fit,"r", label= "bad fit")
ax1.set_title("Selection of aligned cosmic events, $\chi^2_{red}$ in range "+str(chi11)+" to "+str(chi12))
ax2.set_title("Selection of aligned cosmic events, $\chi^2_{red}$ in range "+str(chi21)+" to "+str(chi22))
ax3.set_title("Selection of aligned cosmic events, $\phi$ in range "+str(phi11)+" to "+str(phi12))
ax4.set_title("Selection of aligned cosmic events, $\phi$ in range "+str(phi21)+" to "+str(phi22))
#ax1.legend()
#ax2.legend()
plt.show()
# HISTOGRAM 
plt.hist(Phi1, label="All tracks")
plt.hist(Phi, label="Selected tracks, $\chi^2_{red}$ in range "+str(chi11)+" to "+str(chi12))
plt.title("Angular distribution of 4 or more plane events")
plt.xlabel("Angle[°]")
plt.ylabel("Counts")
plt.legend()
plt.show()

plt.hist(Chi1, bins=50, label="Selected tracks, $\chi^2_{red}$ in range "+str(chi11)+" to "+str(chi12)+" and "+str(chi21)+" to "+str(chi22))
plt.hist(Chi,range=(0,1000), bins=50, label="Selected tracks, $\chi^2_{red}$ in range "+str(chi11)+" to "+str(chi12))
plt.title("Chi-squared distribution of 4 and more plane events")
plt.xlabel("$Chi^2_{red}$")
plt.ylabel("Counts")
plt.legend()
plt.show()

"""1479 misaligned events of in total 3731############yeay2252 for chi_1000"""


# # 3 d plotting and fitting

# select an event
# for i in range(len(Mean_x)):
#     for j in range(len(Mean_x[i])):
#         data = np.array([],[],[])
#         for k in range(len(Mean_y[i][j])):
#             # check if plane k in event j in run i is not empty
#             if np.array(Mean_y[i][j][k]).size != 0:
#                 # get the coordinate information
#                 data[1] = np.append(data[1], Mean_y[i][j][k])
#                 data[0] = np.append(data[0], Mean_x[i][j][k])
#                 #dy = np.append(dy, DMean_y[i][j][k])
#                 data[2] = np.append(data[2],k)
#             data[0] *= 29.24e-6
#             data[1] *= 26.88e-6
#             data[2] *= 2e-2
#             #mean pos of the hits
#             datamean = data.mean(axis=0)

#             uu, dd, vv = np.linalg.svd(data - datamean)
            
            