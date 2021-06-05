import numpy as np
import matplotlib.pyplot as plt

plt.style.use('bmh')
conversion_to_mm = True

desy_683_x = np.array([26.49,26.,30.7,20.93,19.26,-24.43])
ddesy_683_x = np.array([1.56,2.28,2.8,3.41,3.98,4.61])

desy_683_y = np.array([-49.55,17.34,21.66,20.59,-55.17,-44.41])
ddesy_683_y = np.array([1.57,2.42,2.7,3.22,3.71,4.25])

desy_288_x = np.array([10.89,6.06,-0.41,-25.89,-28.06,-79.91])
ddesy_288_x = np.array([2.34,2.7,3.09,3.57,4.09,4.61])

desy_288_y = np.array([-46.93,11.77,6.92,8.,-34.12,-19.14])
ddesy_288_y = np.array([2.45,2.88,3.21,3.5,4.06,4.48])

if (conversion_to_mm == True):
    desy_288_x, desy_288_y = 29e-3*desy_288_x, 29e-3*desy_288_y
    ddesy_288_x, ddesy_288_y = 29e-3*ddesy_288_x, 29e-3*ddesy_288_y
    desy_683_x, desy_683_y = 29e-3*desy_683_x, 29e-3*desy_683_y
    ddesy_683_x, ddesy_683_y = 29e-3*ddesy_683_x, 29e-3*ddesy_683_y

adiff_x = desy_683_x - desy_288_x
dadiff_x = np.sqrt(ddesy_683_x**2+ddesy_288_x**2)
adiff_y = desy_683_y - desy_288_y
dadiff_y = np.sqrt(ddesy_683_y**2+ddesy_288_y**2)

x = 2+np.arange(6)
plt.figure(figsize=(9,4))
plt.errorbar(x, adiff_x, yerr = dadiff_x, capsize=3,elinewidth = 2, label="X")
plt.errorbar(x, adiff_y, yerr = dadiff_y, capsize=3,elinewidth = 2, label="Y")
plt.hlines(0,min(x),max(x), linestyle='--',color='grey',linewidth=1)
plt.xticks(2+np.arange(0,6))
plt.xlabel("Plane")
if (conversion_to_mm == True):
    plt.ylabel("Offset in [mm] between 2019 and 2020")
else:
    plt.ylabel("Offset in [px] between 2019 and 2020")
plt.legend()
plt.show()
