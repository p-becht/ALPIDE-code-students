import numpy as np
import matplotlib.pyplot as plt

x = np.random.rand(1000)

print(x)
plt.hist(x,10)
plt.xlim(0,5)
plt.show()

