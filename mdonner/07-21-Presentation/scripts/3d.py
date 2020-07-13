import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits import mplot3d
import os
from tqdm import tqdm

#####################   User input section   ###################################
# Specify Directory containing the Data
path="/home/maurice/Documents/Bachelor/ALPIDE-code-students/mdonner/06-16-Analysis/Data/"
path="/home/maurice/Documents/Bachelor/Desy_Data/desy2019/" # This is where i stored some testbeam data

min_hits_on_track = 7   # Only accept tracks that hit all planes (for now)
usetracks = 5000
plottracks = 10

print_eventid = False
use_reference_for_align = False

################################################################################

# Data Structure: Dictionary -> 7 Dictionaries (Planes) -> 2 Arrays (X and Y)
print("Initializing...")
hit_data = {}
for plane_number in range(7):
    hit_data[plane_number] = {}
    hit_data[plane_number]["X"] = []
    hit_data[plane_number]["Y"] = []
pixel_x_mean, pixel_y_mean = [], []
total_planes, stopcounter, Event_ID = 0,0,0

for filename in os.listdir(path):
    
    if filename.endswith(".txt"):

        if (stopcounter == usetracks): break

        f = open(path+filename)
        print("Searching for all {} plane events in {}...".format(
            min_hits_on_track, filename))

        # Add a progress bar, since the next loop will take quite long
        with tqdm(total=usetracks) as pbar:

            for line in f.readlines():

                # Temporarily Save EventID and create dictionary for Pixel data
                if '===' in line.split():

                    if (stopcounter == usetracks): break

                    # If buffer is non-empty, write and delete
                    if pixel_x_mean:
                        Hits[int(plane_number)]["X"].append(np.mean(pixel_x_mean))
                        Hits[int(plane_number)]["Y"].append(np.mean(pixel_y_mean))
                        pixel_x_mean = []
                        pixel_y_mean = []

                    # If enough planes are hit, write data, and reset
                    if (total_planes >= min_hits_on_track):
                        if print_eventid:
                            print("Found {} plane event in file {} at {}".format(
                                min_hits_on_track, filename, EventID))
                        for i in range(7):
                            hit_data[i]["X"].extend(Hits[i]["X"])
                            hit_data[i]["Y"].extend(Hits[i]["Y"])
                        # Stop if requested amount of tracks have been plotted
                        stopcounter+=1
                        pbar.update(1)
                        if (stopcounter == usetracks): break

                    # Reset all Variables for next event
                    EventID = line.split()[1]
                    total_planes = 0
                    Hits = {}
                    for plane_number in range(7):
                        Hits[plane_number] = {}
                        Hits[plane_number]["X"] = []
                        Hits[plane_number]["Y"] = []

                # Plane information
                if '---' in line.split():

                    if (stopcounter == usetracks): break

                    if pixel_x_mean: # If buffer is non-empty, write and delete
                        Hits[int(plane_number)]["X"].append(np.mean(pixel_x_mean))
                        Hits[int(plane_number)]["Y"].append(np.mean(pixel_y_mean))
                        pixel_x_mean = []
                        pixel_y_mean = []
                    plane_number = line.split()[1][-1:]
                    total_planes+=1

                # Pixel (Cluster) Information
                if 'Pixel' in line.split():
                    pixel_x_mean.append(int(line.split()[1][:-1]))
                    pixel_y_mean.append(int(line.split()[2][:-1]))

##########################   TO PREVENT ERRORS   ###############################
test_entries = len(hit_data[0]["X"])
for i in range(7):
    if (len(hit_data[i]["X"]) != test_entries):
        print("Error! Array has mismatched size")

if (plottracks > 315) or (plottracks == -1): plottracks = 315
elif (plottracks > usetracks): plottracks = usetracks

######################   CALCULATE PLANE POSITION   ############################
offset = np.ndarray((2,6))

if use_reference_for_align: #This is from 2019 Testbeam data
    offset[0][0], offset[1][0] = 13.74, -36.65
    offset[0][1], offset[1][1] = 25.4, 16.19
    offset[0][2], offset[1][2] = 30.15, 21.23
    offset[0][3], offset[1][3] = 20.29, 20.06
    offset[0][4], offset[1][4] = 20.65, -52.77
    offset[0][5], offset[1][5] = -22.33, -41.59

else:
    print("Aligning planes...")
    N = len(hit_data[0]["Y"])
    # Initialize a dict, to append the individual offsets to
    xdiff, ydiff = {}, {}
    for i in range(6):
        xdiff[i+1] = []
        ydiff[i+1] = []

    for i in range(N):
        # Use plane 0 as a reference, and align every other plane
        Reference = np.array([hit_data[0]["X"][i], hit_data[0]["Y"][i]])
        for j in range(6):
            xdiff[j+1].append(Reference[0] - hit_data[j+1]["X"][i])
            ydiff[j+1].append(Reference[1] - hit_data[j+1]["Y"][i])

    # Now calculate the mean of the offset of each hit per plane
    for i in range(6):
        offset[0][i] = np.mean(xdiff[i+1])
        offset[1][i] = np.mean(ydiff[i+1])

        # We mask all values outside of an interval of 20 around mean, to avoid
        # considering scattered particles for alignment
        x_m = np.ma.masked_outside(xdiff[i+1], offset[0][i]-20, offset[0][i]+20)
        y_m = np.ma.masked_outside(ydiff[i+1], offset[1][i]-20, offset[1][i]+20)
        print("Masked {} hits".format(np.sum(x_m.mask)+np.sum(y_m.mask)))
        print("Plane {} position: X {} +- {} Y {} +- {}".format(i,
            round(np.mean(x_m),2),round(np.std(x_m),2),
            round(np.mean(y_m),2),round(np.std(y_m),2)))

###########################   PLOTTING SECTION   ########################### {{{
print("\n{} Tracks Found, Plotting {} Tracks...".format(stopcounter, plottracks))
x1, y1, z1 = np.ndarray((plottracks)), np.ndarray((plottracks)), np.ndarray((plottracks))
x2, y2, z2 = np.ndarray((plottracks)), np.ndarray((plottracks)), np.ndarray((plottracks))
x3, y3, z3 = np.ndarray((plottracks)), np.ndarray((plottracks)), np.ndarray((plottracks))
x4, y4, z4 = np.ndarray((plottracks)), np.ndarray((plottracks)), np.ndarray((plottracks))
x5, y5, z5 = np.ndarray((plottracks)), np.ndarray((plottracks)), np.ndarray((plottracks))
x6, y6, z6 = np.ndarray((plottracks)), np.ndarray((plottracks)), np.ndarray((plottracks))
x7, y7, z7 = np.ndarray((plottracks)), np.ndarray((plottracks)), np.ndarray((plottracks))
for i in range(plottracks):
    x1[i] = hit_data[0]["X"][i]
    y1[i] = hit_data[0]["Y"][i]
    z1[i] = 0
    x2[i] = hit_data[1]["X"][i]
    y2[i] = hit_data[1]["Y"][i]
    z2[i] = 1
    x3[i] = hit_data[2]["X"][i]
    y3[i] = hit_data[2]["Y"][i]
    z3[i] = 2
    x4[i] = hit_data[3]["X"][i]
    y4[i] = hit_data[3]["Y"][i]
    z4[i] = 3
    x5[i] = hit_data[4]["X"][i]
    y5[i] = hit_data[4]["Y"][i]
    z5[i] = 4
    x6[i] = hit_data[5]["X"][i]
    y6[i] = hit_data[5]["Y"][i]
    z6[i] = 5
    x7[i] = hit_data[6]["X"][i]
    y7[i] = hit_data[6]["Y"][i]
    z7[i] = 6
fig = plt.figure(figsize=(6,8))
ax = plt.axes(projection='3d')
plt.tight_layout()
ax.view_init(0,30)
ax.scatter3D(x1,y1,z1)
ax.scatter3D(x2,y2,z2)
ax.scatter3D(x3,y3,z3)
ax.scatter3D(x4,y4,z4)
ax.scatter3D(x5,y5,z5)
ax.scatter3D(x6,y6,z6)
ax.scatter3D(x7,y7,z7)
ax.set_xlim3d(0,1023)
ax.set_ylim3d(0,511)
plt.savefig("Before.png")
fig = plt.figure(figsize=(6,8))
ax = plt.axes(projection='3d')
plt.tight_layout()
ax.view_init(0,30)
ax.scatter3D(x1,y1,z1)
ax.scatter3D(x2+offset[0][0],y2+offset[1][0],z2)
ax.scatter3D(x3+offset[0][1],y3+offset[1][1],z3)
ax.scatter3D(x4+offset[0][2],y4+offset[1][2],z4)
ax.scatter3D(x5+offset[0][3],y5+offset[1][3],z5)
ax.scatter3D(x6+offset[0][4],y6+offset[1][4],z6)
ax.scatter3D(x7+offset[0][5],y7+offset[1][5],z7)
ax.set_xlim3d(0,1023)
ax.set_ylim3d(0,511)
plt.savefig("After.png")

plt.figure()
plt.ylabel("Frequency")
plt.xlabel("x Offset in Pixel")
plt.text(-40,980,"sigma="+str(np.std(x_m)))
plt.hist(x_m,bins=20)
plt.savefig("../../07-09-Tracking/Afterimasked.png")

#plt.tight_layout()
plt.show()

# }}}
