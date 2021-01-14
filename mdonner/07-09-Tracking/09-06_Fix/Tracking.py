import numpy as np
import Data_Unfitted
import matplotlib.pyplot as plt
from mpl_toolkits import mplot3d
from mpl_toolkits.mplot3d import proj3d

exclude_planes = [7]
old_testbeam = False

#####################    DEFINE PLANE POSITION   ######################## {{{

offset = np.ndarray((2,7))

# Define Reference Planes
offset[0][1], offset[1][1] = 10.89, -46.93
offset[0][2], offset[1][2] = 6.06, 11.77
offset[0][3], offset[1][3] = -0.41, 6.92
offset[0][4], offset[1][4] = -25.89, 8.00
# Plane 5 and 6 seem to be more misaligned than the rest. (Peak in chi2)
offset[0][5], offset[1][5] = -28.06, -34.12
offset[0][6], offset[1][6] = -79.91, -19.14

if (old_testbeam == True):
    offset[0][1], offset[1][1] = 26.49, -49.55
    offset[0][2], offset[1][2] = 26.0, 17.34
    offset[0][3], offset[1][3] = 30.7, 21.66
    offset[0][4], offset[1][4] = 20.93, 20.59
    offset[0][5], offset[1][5] = 19.26, -55.17
    offset[0][6], offset[1][6] = -24.43, -44.41

# }}}

#####################        FIT LINEAR TRACK       ######################## {{{
print('Performing Fit...')

# Import hit data
hit_data = Data_Unfitted.hit_data
N = len(hit_data) # Number of Tracks

for track in range(N):
    
    # Number of planes belonging to the track
    number_of_planes = hit_data[track]["Number_of_planes"]
    
    # Now check if there are planes to be excluded for the tracking algorithm
    for plane in range(7):
        if (hit_data[track][plane]["XC"] != -1) and (plane in exclude_planes):
            number_of_planes -= 1

    Fit_Data = np.ndarray((number_of_planes,3))
    std_hit = np.ndarray((number_of_planes))

    # Create a counter for the Fit Array
    counter = 0
    
    for plane in range(7):
        if (hit_data[track][plane]["XC"] == -1): continue
        hit_data[track][plane]["XC"]+=offset[0][plane]
        hit_data[track][plane]["YC"]+=offset[1][plane]
        if (hit_data[track][plane]["XC"] == -1) or (plane in exclude_planes):
            continue
        Fit_Data[counter][0] = hit_data[track][plane]["XC"]
        Fit_Data[counter][1] = hit_data[track][plane]["YC"]
        Fit_Data[counter][2] = plane*1024*2/3
        std_hit[counter] = hit_data[track][plane]["sig"]
        counter+=1
    
    # Fitting Algorithm:
    datamean = Fit_Data.mean(axis=0)
    uu, dd, vv = np.linalg.svd(Fit_Data - datamean)
    linepts = vv[0] * np.mgrid[-2500:2500:2j][:,np.newaxis]
    linepts += datamean

    # Two Points Define the Fitted Track
    x1 = linepts[0]
    x2 = linepts[1]

    # Calculate shortest distance of a hit to the track
    d = []
    for hit in range(number_of_planes):
        x0 = Fit_Data[hit]
        dist = np.sqrt( ( (np.linalg.norm(x1-x0)**2*np.linalg.norm(x2-x1)**2)
            - (np.dot((x1-x0),(x2-x1)))**2 ) / np.linalg.norm(x2-x1)**2 )
        d.append(dist)

    # Actually, take the residual, not the shortest distance TODO finish this
    d = []
    for hit in range(number_of_planes):
        x0 = Fit_Data[hit]
        # Solve for the point in plane of Track z = a+mb
        lbda = (x0[2] - x2[2])/(x2-x1)[2] # m = (z-a)/(b)
        xz = x2+lbda*(x2-x1) # find point in axis that lies in the same plane

        # Proof that it works {{{
        #fig = plt.figure(figsize=(5,5))
        #ax = plt.axes(projection='3d')
        #x_track = [x1[0],x2[0]]
        #y_track = [x1[1],x2[1]]
        #z_track = [x1[2],x2[2]]
        #ax.plot(x_track,y_track,z_track)
        #ax.scatter(x0[0],x0[1],x0[2])
        #ax.scatter(xz[0],xz[1],xz[2])

        #x = np.arange(0,1025,512)
        #y = np.arange(0,1024,512)
        #X, Y = np.meshgrid(x,y)
        #Z = np.ndarray((len(y),len(x)))
        #Z.fill(0)
        ## Draw the planes
        #for plane in range(7):
        #    Z.fill(plane*1024*2/3)
        #    ax.plot_surface(X,Y,Z,alpha=.1,color='black')
        #plt.show() }}}

    hit_data[track]["Track_Point_1"], hit_data[track]["Track_Point_2"] = [],[]
    for i in range(3):
        hit_data[track]["Track_Point_1"].append(x1[i])
        hit_data[track]["Track_Point_2"].append(x2[i])

    # From there, calculate chi2 to determine the goodness of the fit
    chi2 = 0
    for entry in range(len(d)):
        chi2 += d[entry]**2/std_hit[entry]

    hit_data[track]["chi2"] = chi2

# Export Data

fx = open("Data_Fitted.py","w")
fx.write("hit_data = "+str(hit_data))
fx.close

# }}}
