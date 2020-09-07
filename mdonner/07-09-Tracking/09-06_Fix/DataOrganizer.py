# A more optimised version of 3d.py. A script that takes .txt files as input
# and organizes corry's textwriter output into a python object which much more
# compressed information. It contains all the information about planes hits
# and clusters. Syntax:
# hit_data[track][plane_number]["X"/"Y"/"XC"/"YC"/"sig"]

import os
from tqdm import tqdm
import numpy as np

# Specify Directory containing Cosmic Data, and the minimum n of hits used per track #
path="/home/maurice/Documents/Bachelor/ALPIDE-code-students/mdonner/06-16-Analysis/Data/"
min_hits_on_track = 5
################################################################################

print("Initializing...")

hit_data = []
tmp_hit_data = []
total_planes, counter = 0, 0
for i in range(7):
    tmp_hit_data.append({})
    tmp_hit_data[i]["X"] = []
    tmp_hit_data[i]["Y"] = []

print("Searching for all {} plane events...".format(min_hits_on_track))

for filename in os.listdir(path):

    if filename.endswith(".txt"):

        print("Analyzing {}...".format(filename))

        f = open(path+filename)
        for line in f.readlines():

            # Temporarily Save EventID and create dictionary for Pixel data
            if '===' in line.split():

                # If planes are empty, fill them with some value
                for i in range(7):
                    if not tmp_hit_data[i]["X"]: 
                        tmp_hit_data[i]["X"].append(-1)
                        tmp_hit_data[i]["Y"].append(-1)

                # If enough planes are hit, write data, and reset
                if (total_planes >= min_hits_on_track): 

                    # First create a new Dictionary for each Event
                    hit_data.append({})

                    # Write EventID information and number of planes hit
                    hit_data[counter]["EventID"] = EventID
                    hit_data[counter]["Number_of_planes"] = total_planes
                    
                    # Write information PER PLANE
                    for i in range(7):
                        hit_data[counter][i] = {}
                        hit_data[counter][i]["X"] = tmp_hit_data[i]["X"]
                        hit_data[counter][i]["Y"] = tmp_hit_data[i]["Y"]

                    counter+=1

                # Reset all Variables for next event
                EventID = line.split()[1]
                total_planes = 0
                tmp_hit_data = []
                for plane_number in range(7):
                    tmp_hit_data.append({})
                    tmp_hit_data[plane_number]["X"] = []
                    tmp_hit_data[plane_number]["Y"] = []

            # Plane information
            if '---' in line.split():

                plane_number = int(line.split()[1][-1:])
                total_planes+=1

            # Pixel (Cluster) Information
            if 'Pixel' in line.split():
                tmp_hit_data[plane_number]["X"].append(int(line.split()[1][:-1]))
                tmp_hit_data[plane_number]["Y"].append(int(line.split()[2][:-1]))

###################### CALCULATE CLUSTER POSITION ##############################

number_of_tracks = len(hit_data)
print("number_of_tracks found: {}".format(number_of_tracks))

print("Calculating Cluster positions")

with tqdm(total=number_of_tracks) as pbar:
    for track in range(number_of_tracks):
        for plane in range(7):

            # Calculate the cluster position. XC stands for X position of Cluster
            Cluster_X = np.mean(hit_data[track][plane]["X"])
            Cluster_Y = np.mean(hit_data[track][plane]["Y"])
            hit_data[track][plane]["XC"] = np.mean(hit_data[track][plane]["X"])
            hit_data[track][plane]["YC"] = np.mean(hit_data[track][plane]["X"])
            
            # Calculate the Cluster Spread
            sdev = np.sqrt(np.std(hit_data[track][plane]["X"])**2+
                    np.std(hit_data[track][plane]["Y"])**2)
            if sdev == 0: sdev = 0.5

            # Round values
            sdev = round(sdev,2)
            Cluster_X = round(Cluster_X,2)
            Cluster_Y = round(Cluster_Y,2)

            # Write to Dictionary
            hit_data[track][plane]["XC"] = Cluster_X
            hit_data[track][plane]["YC"] = Cluster_Y
            hit_data[track][plane]["sig"] = sdev

        pbar.update(1)

# Export information into a callable python object
Filename = "Track_Data_"+str(min_hits_on_track)+".py"
with open(Filename, 'w') as f:
    f.write("hit_data = ")
    f.write("%s" % hit_data)
