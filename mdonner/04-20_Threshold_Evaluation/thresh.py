#!/usr/bin/env python3.5

import sys
import matplotlib
import matplotlib.pyplot as plt
import numpy as np

graphical_output=False

def scurve_fit(steps, ninj):
    dvs=sorted(steps.keys())
    m=0
    s=0
    den=0
    for dv1,dv2 in zip(dvs[:-1],dvs[1:]):
        ddv=dv2-dv1
        mdv=0.5*(dv2+dv1)
        n1=1.0*steps[dv1]/ninj
        n2=1.0*steps[dv2]/ninj
        dn=n2-n1
        den+=dn/ddv
        m+=mdv*dn/ddv
        s+=mdv**2*dn/ddv
    if den>0:
        if s>m*m:
            s=(s-m*m)**0.5
        m/=den
        s/=den
    return m,s

thresholds = []
rmss = []

with open(sys.argv[1]) as f:
    pr = pc = -1
    for line in f:
        r,c,dv,hits = [int(i) for i in line.strip().split()]
        if not (pr==r and pc==c):
            if pr>0 and pc>0:
                m,s=scurve_fit(steps,50)
                thresholds.append(m)
                rmss.append(s)
            steps = {}
        else:
            steps[dv] = hits
        pr=r
        pc=c
print(np.mean(thresholds))

if graphical_output:
    plt.figure()
    plt.xlabel('Threshold in DAC')
    plt.ylabel('Frequency')
    plt.title('Whole ALPIDE')
    plt.hist(thresholds,range=(0,30), bins=30)
    plt.show()

    plt.figure()
    plt.xlabel('RMS in DAC')
    plt.ylabel('Frequency')
    plt.title('Whole ALPIDE')
    plt.hist(rmss,range=(0,1), bins=50)
    plt.show()
