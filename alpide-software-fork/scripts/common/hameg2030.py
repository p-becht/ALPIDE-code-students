#!/usr/bin/env python

#
# WARNING: RTS/CTS should be used, as otherwise the PSU might not process
#          every command
#

import serial
import sys
import time

def trip(sour):
    # check compliance
    tripped = False
    for c in range(3):
        sour.write("INST OUT%d\n" % (c+1))
        sour.write("FUSE:TRIP?\n")
        if (int(sour.readline())!=0):
            sys.stderr.write("Channel %d tripped\n" % (c+1))
            tripped = True
    return tripped

def ramp(sour, v, nsteps):
    for step in range(0, nsteps+1):
        sour.write("INST OUT1\n")
        sour.write("SOUR:VOLT %f\n" % (float(v[0])*step/nsteps))
        sour.write("INST OUT2\n")
        sour.write("SOUR:VOLT %f\n" % (float(v[1])*step/nsteps))
        sour.write("INST OUT3\n")
        sour.write("SOUR:VOLT %f\n" % (float(v[2])*step/nsteps))
        time.sleep(0.1);

def rampFromTo(sour, v, nsteps):
    for step in range(0, nsteps+1):
        sour.write("INST OUT3\n")
        sour.write("SOUR:VOLT %f\n" %
                   (float(v[0]+(float(v[1]-float(v[0]))*step/nsteps))))
        time.sleep(0.1);


def powerDown(sour):
    sour.write("INST OUT1\n")
    sour.write("OUTP OFF\n")
    sour.write("INST OUT2\n")
    sour.write("OUTP OFF\n")
    sour.write("INST OUT3\n")
    sour.write("OUTP OFF\n")

def measureCurr(sour):
    val=([0.0, 0.0, 0.0])
    for c in range(3):
        sour.write("INST OUT%d\n" % (c+1))
        sour.write("MEAS:CURR?\n")
        val[c] = float(sour.readline())
    print "%0.4fA\t%0.4fA\t%0.4fA" % ( val[0], val[1], val[2])

def init2030(hameg, i_max):
    hameg.write("*IDN?\n")
    idn = hameg.readline()
    if not ("HAMEG" and "HMP2030") in idn:
        sys.stderr.write("WRONG DEVICE: %s" % idn)
        return
    #print idn
    #print "maximum current: %f %f %f" % i_max
    hameg.write("*RST\n")
    # activate digital fuse and set outputs to 0V
    # CH1
    hameg.write("INST OUT1\n");
    hameg.write("FUSE:LINK 2\n")
    hameg.write("FUSE:LINK 3\n")
    hameg.write("FUSE:DEL 100\n")
    hameg.write("FUSE on\n")
    hameg.write("SOUR:VOLT 5.0\n")
    hameg.write("SOUR:CURR %f\n" % i_max[0])
    hameg.write("OUTP ON\n")
    # CH2
    hameg.write("INST OUT2\n");
    hameg.write("FUSE:LINK 1\n")
    hameg.write("FUSE:LINK 3\n")
    hameg.write("FUSE:DEL 100\n")
    hameg.write("FUSE on\n")
    hameg.write("SOUR:VOLT 0.0\n")
    hameg.write("SOUR:CURR %f\n" % i_max[1])
    hameg.write("OUTP OFF\n")
    # CH3
    hameg.write("INST OUT3\n");
    hameg.write("FUSE:LINK 1\n")
    hameg.write("FUSE:LINK 2\n")
    hameg.write("FUSE:DEL 100\n")
    hameg.write("FUSE on\n")
    hameg.write("SOUR:VOLT 0.0\n")
    hameg.write("SOUR:CURR %f\n" % i_max[2])
    hameg.write("OUTP ON\n")

def main():
    # set up serial port
    dev=sys.argv[1]
    mode=int(sys.argv[2]) if len(sys.argv)>=3 else -1;
    sour=serial.Serial(dev, 9600, rtscts=True);

    # switch mode
    if mode==0:
        # prepare the voltage source
        init2030(sour, (float(sys.argv[3]), float(sys.argv[4]), float(sys.argv[5])))
    elif mode==1:
        # set a voltage value
        sour.write("INST OUT%d\n" % int(sys.argv[3]))
        sour.write("SOUR:VOLT %f\n" % float(sys.argv[4]))
    elif mode==2:
        ramp(sour, (float(sys.argv[3]), float(sys.argv[4]), float(sys.argv[5])), int(sys.argv[6]))
    elif mode==3:
        rampFromTo(sour, (float(sys.argv[3]), float(sys.argv[4])), int(sys.argv[5]))
    elif mode==4:
        # measure current and check whether it is in range
        return trip(sour)
    elif mode==5:
        measureCurr(sour)
    else:
        powerDown(sour)

## execute the main
if __name__ == "__main__":
    sys.exit(main())
