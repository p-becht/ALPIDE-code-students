#!/usr/bin/python3.5
import numpy as np
import matplotlib.pyplot as plt

plt.style.use('bmh')

data = np.loadtxt("output.csv", delimiter=",", skiprows=1, usecols=(1,2,3,4,5,6,7),dtype=int)
run_numbers = np.loadtxt("output.csv", delimiter=",", skiprows=1, usecols=(0), dtype=str)

# Count amount of runs
N = len(data)

total_events = np.ndarray((N))
positioning = np.ndarray((N))
for i in range(N):
    total_events[i]=sum(data[i])
    positioning[i]=int(run_numbers[i][7:])
# Plot runs
plt.figure(figsize=(12,4))
print("Plotting...")
#for i in range(N):
#    run_number = 'run '+str(int(run_numbers[i][7:]))
#    total_events = sum(data[i])
#    positioning = int(run_numbers[i][7:])
plt.scatter(positioning, total_events, c=total_events/1200, marker='.', cmap='plasma')

plt.xlabel('Run number')
plt.xticks(np.arange(380,780,20))
plt.ylabel('# of events')
plt.savefig('rate.png',dpi=200)
print("File Written to runs.png")
