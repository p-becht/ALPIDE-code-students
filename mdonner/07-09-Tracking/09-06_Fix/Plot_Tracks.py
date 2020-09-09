import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits import mplot3d
import Fitted_Data

hit_data = Fitted_Data.hit_data

print('Plotting...')

figx, figy = 8, 5
xlim, ylim = 1023, 511

fig = plt.figure(figsize=(figx,figy))
# Create 3d Axes
ax = plt.axes(projection='3d')
# ... but dont plot them
ax._axis3don = False

# Define Axes Lenghts (Dont change this, not worth)
ax.set_xlim3d(0,xlim)
ax.set_ylim3d(0,ylim)
x = np.arange(0,1025,512)
y = np.arange(0,1024,512)
X, Y = np.meshgrid(x,y)
Z = np.ndarray((len(y),len(x)))
Z.fill(0)

# Draw the planes
for plane in range(7):
    Z.fill(plane*1024*2/3)
    ax.plot_surface(X,Y,Z,alpha=.1,color='black')

# Plot Hits
for i in range(plottracks):

    x, y, z = [], [], []

    for plane in range(7):

        # Make sure that empty hits dont be plotted
        if (hit_data[i][plane]["X"] == -1):
            continue
        # Put everything else into plottable arrays
        x.append(hit_data[i][plane]["XC"]+offset[0][plane])
        y.append(hit_data[i][plane]["YC"]+offset[0][plane])
        z.append(plane*1024*2/3) #TODO Make sure this is to scale, seems fishy

        ax.scatter3D(x,y,z,alpha=.7,color='black',marker='.')
