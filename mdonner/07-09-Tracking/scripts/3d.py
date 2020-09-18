import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits import mplot3d
import os
from tqdm import tqdm
import minseven

#####################   User input section   ###################################
# Specify Directory containing Cosmic Data
path="/home/maurice/Documents/Bachelor/ALPIDE-code-students/mdonner/06-16-Analysis/Data/"
# This is where i stored some testbeam data
# path="/home/maurice/Documents/Bachelor/Desy_Data/desy2019/" 

min_hits_on_track = 7
# Define the number of tracks used for tracking, and plotting
usetracks = 1000000
plottracks = 3
connect_tracks = False
fit_tracks = True
accurate_scale = False
show_animation = True
search_txt = False

print_eventid = False
use_reference_for_align = True
#################################################################################

#####################          CODE SECTION         ######################## {{{

# Data Structure: Dictionary -> 7 Dictionaries (Planes) -> 2 Arrays (X and Y)
print("Initializing...")
hit_data = {}
hit_data["ID"] = []
for plane_number in range(7):
    hit_data[plane_number] = {}
    hit_data[plane_number]["X"] = []
    hit_data[plane_number]["Y"] = []
    hit_data[plane_number]["s"] = []
pixel_x_mean, pixel_y_mean = [], []
total_planes, stopcounter, Event_ID = 0,0,0
# Temporary
Hits = {}
for plane_number in range(7):
    Hits[plane_number] = {}
    Hits[plane_number]["X"] = []
    Hits[plane_number]["Y"] = []
    Hits[plane_number]["s"] = []
Fit_lines = {}
chisquared = []

# Add a progress bar, since the next loop will take quite long
print("Searching for all {} plane events...".format(min_hits_on_track))

if search_txt:
    with tqdm(total=usetracks) as pbar:

        for filename in os.listdir(path):
            
            if filename.endswith(".txt"):

                if (stopcounter == usetracks): break

                f = open(path+filename)


                for line in f.readlines():

                    # Temporarily Save EventID and create dictionary for Pixel data
                    if '===' in line.split():

                        if (stopcounter == usetracks): break

                        # If buffer is non-empty, write and delete
                        if pixel_x_mean:
                            Hits[int(plane_number)]["X"].append(np.mean(pixel_x_mean))
                            Hits[int(plane_number)]["Y"].append(np.mean(pixel_y_mean))
                            sdev = np.sqrt(np.std(pixel_x_mean)**2+np.std(pixel_y_mean)**2)
                            Hits[int(plane_number)]["s"].append(sdev)
                            #print(filename, " -> ",EventID," -> ",Hits[int(plane_number)]["s"])
                            pixel_x_mean = []
                            pixel_y_mean = []

                        # If planes are empty, fill them with some value
                        for i in range(7):
                            if not Hits[i]["X"]: 
                                Hits[i]["X"].append(-1)
                                Hits[i]["Y"].append(-1)
                                Hits[i]["s"].append(-1)

                        # If enough planes are hit, write data, and reset
                        if (total_planes >= min_hits_on_track): 
                            # Only use real cosmics, not randomly spaced hits
                            if (np.mean(Hits[i]["s"]) < 10): # and EventID == '6258990':
                                hit_data["ID"].append(EventID)
                                for i in range(7):
                                    hit_data[i]["X"].extend(Hits[i]["X"])
                                    hit_data[i]["Y"].extend(Hits[i]["Y"])
                                    hit_data[i]["s"].extend(Hits[i]["s"])
                                if print_eventid:
                                    print("Found {} plane event in file {} at {}".format(
                                        min_hits_on_track, filename, EventID))
                                # Stop if requested amount of tracks have been added
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
                            Hits[plane_number]["s"] = []

                    # Plane information
                    if '---' in line.split():

                        if (stopcounter == usetracks): break

                        if pixel_x_mean: # If buffer is non-empty, write and delete
                            Hits[int(plane_number)]["X"].append(np.mean(pixel_x_mean))
                            Hits[int(plane_number)]["Y"].append(np.mean(pixel_y_mean))
                            sdev = np.sqrt(np.std(pixel_x_mean)**2+np.std(pixel_y_mean)**2)
                            Hits[int(plane_number)]["s"].append(sdev)
                            pixel_x_mean = []
                            pixel_y_mean = []

                        plane_number = line.split()[1][-1:]

                        total_planes+=1

                    # Pixel (Cluster) Information
                    if 'Pixel' in line.split():
                        pixel_x_mean.append(int(line.split()[1][:-1]))
                        pixel_y_mean.append(int(line.split()[2][:-1]))

else:
    hit_data = minseven.hit_data
    hit_data["chi2"] = []

# }}}

#####################        TO PREVENT ERRORS      ######################## {{{
test_entries = len(hit_data[0]["X"])
for i in range(7):
    if (len(hit_data[i]["X"]) != test_entries):
        print("Error! Array has mismatched size")

if (plottracks > 315) or (plottracks == -1): plottracks = 315
elif (plottracks > usetracks): plottracks = usetracks

#}}}

#####################    CALCULATE PLANE POSITION   ######################## {{{
offset = np.ndarray((2,7))

# Plane 0 will be our Reference plane
offset[0][0] = offset[1][0] = 0

# Either use a reference for aligning, or align with Testbeam data:

if use_reference_for_align: #This is from 2019 Testbeam data
    offset[0][1], offset[1][1] = 13.74, -36.65
    offset[0][2], offset[1][2] = 25.4, 16.19
    offset[0][3], offset[1][3] = 30.15, 21.23
    offset[0][4], offset[1][4] = 20.29, 20.06
    offset[0][5], offset[1][5] = 20.65, -52.77
    offset[0][6], offset[1][6] = -22.33, -41.59

if use_reference_for_align: #This is from 2020 Testbeam data (more accurate)
    offset[0][1], offset[1][1] = 10.89, -46.93
    offset[0][2], offset[1][2] = 6.06, 11.77
    offset[0][3], offset[1][3] = -0.41, 6.92
    offset[0][4], offset[1][4] = -25.89, 8.00
    offset[0][5], offset[1][5] = -28.06, -34.12
    offset[0][6], offset[1][6] = -79.91, -19.14

else:
    print("Aligning planes...")
    # Count number of events
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
        offset[0][i+1] = np.mean(xdiff[i+1])
        offset[1][i+1] = np.mean(ydiff[i+1])

        # We mask all values outside of an interval of 20 around mean, to avoid
        # considering scattered particles for alignment
        x_m = np.ma.masked_outside(xdiff[i+1], offset[0][i+1]-20, offset[0][i+1]+20)
        y_m = np.ma.masked_outside(ydiff[i+1], offset[1][i+1]-20, offset[1][i+1]+20)
        print("Masked {} hits".format(np.sum(x_m.mask)+np.sum(y_m.mask)))
        print("Plane {} position: X {} +- {} Y {} +- {}".format(i,
            round(np.mean(x_m),2),round(np.std(x_m),2),
            round(np.mean(y_m),2),round(np.std(y_m),2)))
# }}}

#####################        PLOT/FIT SECTION       ######################## {{{

# Plotting Tracks after Alignment
if accurate_scale: figx, figy = 5, 5
else: figx, figy = 5, 7
fig = plt.figure(figsize=(figx,figy))
ax = plt.axes(projection='3d')
ax._axis3don = False
#xscale, yscale, zscale = 1,1,1.5
#scale = np.diag([xscale,yscale,zscale,1.0])
#scale = scale*(1.0/scale.max())
#def short_proj():
#    return np.dot(mplot3d.axes3d.Axes3D.get_proj(ax),scale)
#ax.get_proj = short_proj

if accurate_scale:
    max_scale = 6*1024*2/3
    ax.set_xlim3d(-max_scale/2+512,max_scale/2+512)
    ax.set_ylim3d(-max_scale/2+256,max_scale/2+256)
    ax.set_zlim3d(0,max_scale)
else:
    ax.set_xlim3d(0,1023)
    ax.set_ylim3d(0,511)

# Draw ALPIDE
x = np.arange(0, 1025, 512)
y = np.arange(0, 1024, 512)
X, Y = np.meshgrid(x,y)
Z = np.ndarray((len(y),len(x)))
Z.fill(0)
for plane in range(7):
    Z.fill(plane*1024*2/3)
    ax.plot_surface(X,Y,Z,alpha=0.1,color='black')

# Plot Hit information
for i in range(plottracks):
    x, y, z = [], [], []
    for plane in range(7):
        if (hit_data[plane]["X"][i] == -1):
            continue
        x.append(hit_data[plane]["X"][i]+offset[0][plane])
        y.append(hit_data[plane]["Y"][i]+offset[1][plane])
        z.append(plane*1024*2/3)

    # Connect dots for Better visibility of Tracks
    if connect_tracks:
        ax.plot(x,y,z, linewidth=.5)
    ax.scatter3D(x,y,z,alpha=0.7,color='black',marker='.')

#####################        FIT LINEAR TRACK       ######################## {{{

# Number of tracks is defined in usetracks
if fit_tracks:
    print('Performing Fit...')
    if (usetracks > len(hit_data[0]["X"])): usetracks = len(hit_data[0]["X"])
    with tqdm(total=usetracks) as pbar:
        for track in range(usetracks):

            #Count number of planes belonging to the track
            number_of_planes = 0
            for plane in range(7):
                if (hit_data[plane]["X"][track] == -1):
                    continue #This skips over empty planes
                number_of_planes +=1
            
            Fit_Data = np.ndarray((number_of_planes,3))
            std_hit = np.ndarray((number_of_planes))

            # Create a counter for the Fit_Data Array
            counter = 0
            # Write Coordinates of Track to be fit in a new array with different format
            for hit in range(7):
                if (hit_data[hit]["X"][track] == -1):
                    continue
                Fit_Data[counter][0] = hit_data[hit]["X"][track]+offset[0][hit]
                Fit_Data[counter][1] = hit_data[hit]["Y"][track]+offset[1][hit]
                Fit_Data[counter][2] = hit*1024*2/3
                std_hit[counter] = hit_data[hit]["s"][track]
                counter += 1

            # Fitting Algorithm:
            datamean = Fit_Data.mean(axis=0)
            uu, dd, vv = np.linalg.svd(Fit_Data - datamean)
            linepts = vv[0] * np.mgrid[-2500:2500:2j][:,np.newaxis]
            linepts += datamean

            # Plot Line Fit
            points_x = [linepts[0][0],linepts[1][0]]
            points_y = [linepts[0][1],linepts[1][1]]
            points_z = [linepts[0][2],linepts[1][2]]
            if (track < plottracks): ax.plot3D(points_x,points_y,points_z, linewidth=.5)

            # Line through two points
            Fit_lines[track] = linepts
            x1 = linepts[0]
            x2 = linepts[1]

            d = []
            for hit in range(number_of_planes):
                x0 = Fit_Data[hit]
                # Calculation of residual of point x to fit
                res = np.sqrt( ( (np.linalg.norm(x1-x0)**2*np.linalg.norm(x2-x1)**2)
                    - (np.dot((x1-x0),(x2-x1)))**2 )  / np.linalg.norm(x2-x1)**2 )
                d.append(res)

            # append sum of chi2
            chi2 = 0
            for entry in range(len(d)):
                # Make sure we dont divide by 0 by taking minimal std of .5 pix
                if (std_hit[entry] <= 0.5): std_hit[entry] = 0.5
                chi2+= d[entry]**2/(std_hit[entry])
            #if (chi2 >= 100000):
            #    print("chi2 = {} encountered in {}. Event ID: {}".format(
            #        chi2,track,hit_data["ID"][track]))
            #if chi2 <= 200:
            if (number_of_planes == 6) and (chi2 <=200):
                chisquared.append(chi2)
            
            # Add chi2 to the data for felicitas
            hit_data["chi2"].append(chi2)
            print(len(hit_data["chi2"]))
            pbar.update(1)
# }}}

# Export dict
fx = open("gtffeli567.py","w")
fx.write(str(hit_data))
fx.close()

#plt.figure()
#plt.title("Distribution of chi2 for {} {} hit events".format(usetracks,min_hits_on_track))
#plt.xlabel("chi2")
#plt.ylabel("frequency")
#plt.hist(chisquared,50)
#plt.xlim(0,150)
#plt.ylim(0,35)

if show_animation:
    for angle in range(0, 1080):
        ax.view_init(30, angle)
        plt.draw()
        plt.pause(.001)

plt.show()

# }}}
