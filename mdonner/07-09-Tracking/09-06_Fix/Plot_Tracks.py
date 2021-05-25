import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits import mplot3d
import Data_Fitted
import pylab
from mpl_toolkits.mplot3d import proj3d

hit_data = Data_Fitted.hit_data

############################## USER INPUT ######################################
plottracks = [0]
connect_dots = True
plot_tracks = True
print_chi2 = False
min_number_of_planes = 3
################################################################################


figx, figy = 5, 5
xlim, ylim = 1023, 511

fig = plt.figure(figsize=(figx,figy))
# Create 3d Axes
ax = plt.axes(projection='3d')
ax._axis3don = False # ... but dont plot them
ax.set_box_aspect((3,3,3)) # For aspect ratio

# Define Axes Lenghts (Dont change this, not worth)
ax.set_xlim3d(0,xlim)
ax.set_ylim3d(0,ylim)
ax.set_zlim3d(0,(6*1024)/2)
x = np.arange(0,1025,512)
y = np.arange(0,1024,512)
X, Y = np.meshgrid(x,y)
Z = np.ndarray((len(y),len(x)))
Z.fill(0)

print('Plotting...')

# Draw the planes
for plane in range(7):
    Z.fill(plane*1024*2/3)
    ax.plot_surface(X,Y,Z,alpha=.1,color='black')

# Plot Hits
for i in plottracks:

    if (hit_data[i]['Number_of_planes'] < min_number_of_planes): continue

    x_data, y_data, z_data = [], [], []
    counter = 0

    for plane in range(7):

        # Make sure that empty hits arent plotted
        if (hit_data[i][plane]["XC"] == -1):
            continue

        # Put everything else into plottable arrays
        x_data.append(hit_data[i][plane]["XC"])
        y_data.append(hit_data[i][plane]["YC"])
        z_data.append(plane*1024*2/3)

        ax.scatter3D(x_data,y_data,z_data,alpha=.7,color='black',marker='.')

        # OPTIONAL: Connect dots for Better visibility of Tracks
        if connect_dots:
            ax.plot(x_data,y_data,z_data, linewidth=.5)#, color='grey')

        # OPTIONAL: Plot tracks associated to events
        if plot_tracks:
            x_track = [hit_data[i]['Track_Point_1'][0],hit_data[i]['Track_Point_2'][0]]
            y_track = [hit_data[i]['Track_Point_1'][1],hit_data[i]['Track_Point_2'][1]]
            z_track = [hit_data[i]['Track_Point_1'][2],hit_data[i]['Track_Point_2'][2]]
            ax.plot(x_track,y_track,z_track,linewidth=.5)

        # OPTIONAL: Print the chi2 value for the track
        if print_chi2 and (counter == 0):
            x2, y2, _ = proj3d.proj_transform(
                    hit_data[i]['Track_Point_2'][0],
                    hit_data[i]['Track_Point_2'][1],
                    hit_data[i]['Track_Point_2'][2],ax.get_proj())
            counter +=1
            label = pylab.annotate(str(round(hit_data[i]['chi2'])),
            xy = (x2, y2), xytext = (-20, 20),
            textcoords = 'offset points', ha = 'right', va = 'bottom',
            bbox = dict(boxstyle = 'round,pad=0.5', fc = 'yellow', alpha = 0.5),
            arrowprops = dict(arrowstyle = '->', connectionstyle = 'arc3,rad=0'))
plt.show()
