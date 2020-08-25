#!/usr/bin/python
import numpy as np
import matplotlib.pyplot as plt

plt.style.use('bmh')

data = np.loadtxt("muon_data_processing/n-event-plane-data.csv", delimiter=",", skiprows=1, usecols=(1,2,3,4,5,6,7),dtype=int)
run_numbers = np.loadtxt("muon_data_processing/n-event-plane-data.csv", delimiter=",", skiprows=1, usecols=(0), dtype=str)

# X axis
x = np.arange(1,8,1)

# Count amount of runs
N = len(data)

barwidth = 0.5 # Make sure all the bars only take up this space

# Plot runs in bar diagram
print("Plotting...")
for i in range(N):
    #STR run number
    #run_number = run_numbers[i]
    #INT run number
    run_number = 'run '+str(int(run_numbers[i][7:]))
    run = data[i]
    positioning = x+(i*barwidth/N)-barwidth*(N-1)/(2*N)
    plt.bar(positioning, run, width=barwidth/N, label=run_number)

plt.xlabel('# of planes hit')
plt.ylabel('# of events')
plt.legend()
plt.yscale('log')
plt.savefig('rate.png',dpi=600)
print("File Written to rate.png")
