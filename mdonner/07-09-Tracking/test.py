import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits import mplot3d

x = [1,5,-1]
y = [1,2,0.5]
z = [1,3,0]

xx = [0,3]
yy = [0,0]
zz = [1.5,3]

xxx = [0,0]
yyy = [0,3]
zzz = [1.5,7.5]

fig = plt.figure()
ax = mplot3d.Axes3D(fig)

ax.scatter3D(x,y,z,alpha=.7,color='black')
ax.plot(x,y,z,linewidth=.5)
ax.scatter3D(xx,yy,zz,alpha=.7,color='black')
ax.plot(xx,yy,zz,linewidth=.5)
ax.scatter3D(xxx,yyy,zzz,alpha=.7,color='black')
ax.plot(xxx,yyy,zzz,linewidth=.5)

ax.set_xlim=(0,10)
ax.set_ylim=(0,10)
plt.zlim=(0,10)

plt.show()
