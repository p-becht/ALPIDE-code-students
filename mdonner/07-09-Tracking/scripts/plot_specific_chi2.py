import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits import mplot3d
import os
from tqdm import tqdm
# Import Hit data for a minimum of five/six/seven plane events
import minfive

hit_data = minfive.hit_data

min_hits_on_track = 7
usetracks = 1600
plottracks = 1
fit_tracks = True
plot_specific_chi2 = True
plot_chi2 = 290252
skip_tracks = 1
connect_tracks = False
accurate_scale = False
show_animation = False
print_chi2 = True

Fit_lines = {}
chisquared= []

################# Import Plane position from 2020 Testbeam data ############ {{{
offset = np.ndarray((2,7))
offset[0][0] = offset[1][0] = 0
offset[0][1], offset[1][1] = 10.89, -46.93
offset[0][2], offset[1][2] = 6.06, 11.77
offset[0][3], offset[1][3] = -0.41, 6.92
offset[0][4], offset[1][4] = -25.89, 8.00
offset[0][5], offset[1][5] = -28.06, -34.12
offset[0][6], offset[1][6] = -79.91, -19.14
# }}}

#####################        PLOT/FIT SECTION       ######################## {{{

# Plotting Tracks after Alignment
if accurate_scale: figx, figy = 5, 5
else: figx, figy = 5, 7
fig = plt.figure(figsize=(figx,figy))
ax = plt.axes(projection='3d')
ax._axis3don = False

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
# }}}

#####################        FIT LINEAR TRACK       ######################## {{{

# Number of tracks is defined in usetracks
if fit_tracks:
    print('Performing Fit...')
    chi2 = []
    if (usetracks > len(hit_data[0]["X"])): usetracks = len(hit_data[0]["X"])
    with tqdm(total=usetracks) as pbar:
        track_counter_plot = track_counter_skip = 0
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
            if (chi2 >= 999) and (print_chi2 == True):
                print("chi2 = {} encountered in {}. Event ID: {}".format(
                    chi2,track,hit_data["ID"][track]))
            #if chi2 <= 200:
            chisquared.append(chi2)

            if (track_counter_plot < plottracks) and (number_of_planes == min_hits_on_track):
                # Plot a track with a specific chi2
                if plot_specific_chi2 == True:
                    minchi2 = plot_chi2-plot_chi2/10
                    maxchi2 = plot_chi2+plot_chi2/10
                    if (chi2 <= maxchi2) and (chi2 >= minchi2):
                        if (track_counter_skip >= skip_tracks): 
                            ax.plot3D(points_x,points_y,points_z, linewidth=.5)
                            print("Plotted Event {} with chi2 = {}".format(
                                hit_data["ID"][track], chi2))
                            x, y, z = [], [], []
                            for plane in range(7):
                                if (hit_data[plane]["X"][track] == -1):
                                    continue
                                x.append(hit_data[plane]["X"][track]+offset[0][plane])
                                y.append(hit_data[plane]["Y"][track]+offset[1][plane])
                                z.append(plane*1024*2/3)

                                if connect_tracks:
                                    ax.plot(x,y,z, linewidth=.5)
                                ax.scatter3D(x,y,z,alpha=0.7,color='black',marker='.')
                            track_counter_plot+=1
                        track_counter_skip+=1

                else:
                    if (track_counter_skip >= skip_tracks):
                        ax.plot3D(points_x,points_y,points_z, linewidth=.5)
                        x, y, z = [], [], []
                        for plane in range(7):
                            if (hit_data[plane]["X"][track] == -1):
                                continue
                            x.append(hit_data[plane]["X"][track]+offset[0][plane])
                            y.append(hit_data[plane]["Y"][track]+offset[1][plane])
                            z.append(plane*1024*2/3)

                            if connect_tracks:
                                ax.plot(x,y,z, linewidth=.5)
                            ax.scatter3D(x,y,z,alpha=0.7,color='black',marker='.')
                        track_counter_plot+=1
                    track_counter_skip+=1

            pbar.update(1)
# }}}

if show_animation:
    for angle in range(0, 360):
        ax.view_init(30, angle)
        plt.draw()
        plt.pause(.001)

plt.tight_layout()
ax.view_init(10,60)
plt.savefig('../example_{}.png'.format(str(plot_chi2)))
plt.show()

# }}}
