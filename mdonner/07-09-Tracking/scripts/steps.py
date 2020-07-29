#!/usr/bin/env python3.5
import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
from scipy.special import erf
import sys

plt.style.use('bmh')

xaxis=np.arange(49)
filename="ThresholdScan_200323_131753.dat"

graphic_output_single = False
graphic_output_landau = True

def step_func(x,a,b):
    return 1/(1+np.exp(a*(x-b)))

def gerf(x,mu,sig):
    return 1/2*(1+erf((x-mu)/(np.sqrt(2)*sig)))

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
thresholds_scipy = []
rmss_scipy = []
with open(filename) as f:
    prevr = prevc = -1
    for line in f:
        r,c,dv,hits = [int(i) for i in line.strip().split()]
        if not (prevr==r and prevc==c):
            #Once one set is done, fit
            if (prevr>=0 and prevc>=0):
                m,s=scurve_fit(steps,50)
                thresholds.append(m)
                rmss.append(s)
                #write from dictionary to array
                y = np.ndarray((49))
                for i in range(1,50):
                    y[i-1] = steps[i]/50
                #Fit Function to data #################################
                p0 = [18,0.5]
                popt, pcov = curve_fit(gerf, xaxis, y, method='dogbox',
                        bounds=([0., 0.],[100., 10.]))
                thresholds_scipy.append(popt[0])
                rmss_scipy.append(popt[1])
                ####################################### PLOTTING
                if graphic_output_single:
                    plt.figure()
                    plt.scatter(xaxis,y, label="Data",marker='.')
                    x = np.linspace(0,49,500)
                    plt.plot(x, gerf(x,*popt),label="Scipy Fit")
                    plt.plot(x, np.heaviside(x-m,0.5),label="Quick Fit")
                    plt.legend()
            steps={}
        else:
            steps[dv]=hits
        prevr=r
        prevc=c

        #if c == 256: break print(thresholds)
    print(np.mean(thresholds))
    print(np.mean(rmss))
    print(np.mean(thresholds_scipy))
    print(np.mean(rmss_scipy))

if graphic_output_landau:
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

    plt.figure()
    plt.xlabel('Threshold in DAC')
    plt.ylabel('Frequency')
    plt.title('Whole ALPIDE')
    plt.hist(thresholds_scipy,range=(0,30), bins=30)
    plt.show()

    plt.figure()
    plt.xlabel('RMS in DAC')
    plt.ylabel('Frequency')
    plt.title('Whole ALPIDE')
    plt.hist(rmss_scipy,range=(0,1), bins=50)
    plt.show()
