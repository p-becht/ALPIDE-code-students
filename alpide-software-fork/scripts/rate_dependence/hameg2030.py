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

def powerDown(sour):
    sour.write("INST OUT1\n")
    sour.write("OUTP OFF\n")
    sour.write("INST OUT2\n")
    sour.write("OUTP OFF\n")
    sour.write("INST OUT3\n")
    sour.write("OUTP OFF\n")

def measureCurr(sour):
    valc=([0.0, 0.0, 0.0])
    valv=([0.0, 0.0, 0.0])
    for c in range(3):
        sour.write("INST OUT%d\n" % (c+1))
        sour.write("MEAS:CURR?\n")
        valc[c] = float(sour.readline())
        sour.write("INST OUT%d\n" % (c+1))
        sour.write("MEAS:VOLT?\n")
        valv[c] = float(sour.readline())
    print "%0.2fV\t%0.2fV\t%0.2fV" % ( valv[0], valv[1], valv[2])
    print "%0.4fA\t%0.4fA\t%0.4fA" % ( valc[0], valc[1], valc[2])

def powerOn(hameg, vbb):
    hameg.write("*IDN?\n")
    idn = hameg.readline()
    if not ("HAMEG" and "HMP2030") in idn:
        sys.stderr.write("WRONG DEVICE: %s" % idn)
        return
    #print idn
    hameg.write("*RST\n")
    # activate digital fuse and set outputs
    # CH1 (DAQ board 5V)
    hameg.write("INST OUT1\n");
    hameg.write("FUSE:LINK 2\n")
    hameg.write("FUSE:LINK 3\n")
    hameg.write("FUSE:DEL 100\n")
    hameg.write("FUSE on\n")
    hameg.write("SOUR:VOLT 5.0\n")
    hameg.write("SOUR:CURR 0.8\n")
    hameg.write("OUTP ON\n")
    # CH2 (Auxilliary 3.3V)
    hameg.write("INST OUT2\n");
    hameg.write("FUSE:LINK 1\n")
    hameg.write("FUSE:LINK 3\n")
    hameg.write("FUSE:DEL 100\n")
    hameg.write("FUSE on\n")
    hameg.write("SOUR:VOLT 3.3\n")
    hameg.write("SOUR:CURR 0.1\n");
    hameg.write("OUTP ON\n")
    # CH3 (reverse substrate bias)
    hameg.write("INST OUT3\n");
    hameg.write("FUSE:LINK 1\n")
    hameg.write("FUSE:LINK 2\n")
    hameg.write("FUSE:DEL 100\n")
    hameg.write("FUSE on\n")
    hameg.write("SOUR:VOLT %f\n" % vbb)
    hameg.write("SOUR:CURR 0.002\n")
    hameg.write("OUTP ON\n")
    time.sleep(1);

def main():
    # set up serial port
    dev=sys.argv[1]
    mode=int(sys.argv[2]) if len(sys.argv)>=3 else -1
    sour=serial.Serial(dev, 9600, rtscts=True);

    # switch mode
    if mode==0:
        # prepare the voltage source
        vbb = float(sys.argv[3]) if len(sys.argv)>=4 else float(0.)
        print "Reverse substrate voltage Vbb: %0.1f V" % vbb
        powerOn(sour, vbb)
    elif mode==1:
        # set a voltage value
        sour.write("INST OUT%d\n" % int(sys.argv[3]))
        sour.write("SOUR:VOLT %f\n" % float(sys.argv[4]))
    elif mode==2:
        # measure current and check whether it is in range
        return trip(sour)
    elif mode==3:
        measureCurr(sour)
    else:
        powerDown(sour)

## execute the main
if __name__ == "__main__":
    sys.exit(main())
