#!/usr/bin/env python3.5
import numpy as np
import sys

path = sys.argv[1]
NTRIG = int(sys.argv[2])

def fakehitrate(hits,triggers):
    FHR = hits/(triggers*524288)
    dFHR = 1/(np.sqrt(hits)*(triggers*524288))
    print(FHR)
    print(dFHR)
    return FHR, dFHR

NHITS = []
with open(sys.argv[1]) as f:
    for line in f:
        doublecol, address, nhits = [int(i) for i in line.strip().split()]
        NHITS.append(nhits)

fakehitrate(sum(NHITS),NTRIG)

