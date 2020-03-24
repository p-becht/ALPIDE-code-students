#!/usr/bin/env python

import usbtmc
import sys
import array
import time

def init_pulse(pulse, freq, cycles):
    pulse.write("*RST")
    #print str(pulse.ask("*IDN?"))
    pulse.write("TRIG:SOUR TIM")
    pulse.write("TRIG:TIM 100 ms")
    pulse.write(":VOLT:LOW  0.0")
    pulse.write(":VOLT:HIGH 3.3")
    pulse.write(":FUNC PULSE")
    pulse.write(":BURS:STAT ON")
    pulse.write(":BURS:NCYC %d" % cycles)
    pulse.write(":PULS:PER %e" % (1./freq))  # distance between bursts
    pulse.write(":PULS:WIDT 1E-7") # width of the pulse
    print "Frequency %0.3f kHz / Period %0.3f us" % ( freq/1000., 1./freq*1.e6)


def output_on(pulse):
    pulse.write("OUTPUT ON")


def output_off(pulse):
    pulse.write("*RST")
    pulse.write("OUTPUT OFF")


def usbtmc_open():
    #p=usbtmc.Instrument("USB0::0x0699::0x0345::C020275::INSTR")
    #p=usbtmc.Instrument("USB0::0x0699::0x0345::C022401::INSTR")
    #p=usbtmc.Instrument("USB0::0x0699::0x0345::C022766::INSTR")
    p=usbtmc.Instrument(0x699,0x345)
    return p

def main(turnOn, period, cycles):
    p=usbtmc_open()
    time.sleep(1)
    #print "setting up pulser"
    if turnOn:
        init_pulse(p, period, cycles)
        output_on(p)
    else:
        output_off(p)
    #print "pulser setting done."


# parameters periode
if __name__ == "__main__":
    try:
        if len(sys.argv)>=4 and (int(sys.argv[1])==1):
            freq = float(sys.argv[2])
            cycles = int(sys.argv[3])
            main(True, freq, cycles)
        else:
            main(False, float(0.), float(0.))
        exit(0)
    except:
        exit(1)
