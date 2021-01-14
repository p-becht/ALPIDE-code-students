import numpy as np
import matplotlib.pyplot as plt

plt.style.use('bmh')

desy_683_x = np.array([26.49,26.,30.7,20.93,19.26,-24.43])
ddesy_683_x = np.array([1.56,2.28,2.8,3.41,3.98,4.61])

desy_683_y = np.array([-49.55,17.34,21.66,20.59,-55.17,-44.41])
ddesy_683_y = np.array([1.57,2.42,2.7,3.22,3.71,4.25])

corry_683_x = np.array([25.6,25.4,30.2,20.5,19.,-24.5])+0.7

corry_683_y  = np.array([-45.3,16.,20.5,19.8,-49.3,-39.])+0.1

x = 1+np.arange(6)
plt.figure(figsize=(9,4))
plt.errorbar(x, desy_683_x, yerr = ddesy_683_x, capsize=3, label="Translation in X",c='C0',zorder=0)
plt.errorbar(x, desy_683_y, yerr = ddesy_683_y, capsize=3, label="Translation in Y",c='C4',zorder=0)
plt.scatter(x, corry_683_x, marker='.',label="Corryvreckan", c='black',zorder=10)
plt.scatter(x, corry_683_y, marker='.',c='black',zorder=10)
plt.title("Comparison of our Alignment (translation) to Corryvreckan Alignment")
plt.xlabel("Plane")
plt.ylabel("Position [Pixel]")
plt.legend()
plt.savefig("Corry.png")
plt.show()
