import numpy as np
import Data_Unfitted

plottracks = 3
exclude_planes = [5,6]

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
        if (hit_data[track][plane]["XC"] == -1) or (plane in exclude_planes):
            continue
        Fit_Data[counter][0] = hit_data[track][plane]["XC"]+offset[0][plane]
        Fit_Data[counter][1] = hit_data[track][plane]["YC"]+offset[1][plane]
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
