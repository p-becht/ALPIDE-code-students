#!/usr/bin/env python_32bit

import visa
import sys
import array
import time

def set_arbitrary(pulse, wffile):

    freq_pulser=int(wffile.split("fclk")[1].split("Hz")[0])
    print "input file: ", wffile
    print "freq_pulser: ", str(freq_pulser)

    # set up pulser to emulate random trigger
    wave = array.array('H')
    n = 0 
    # loadwaform from file
    f = open(wffile, 'r')
    for line in f:
        wave.append( 16384 - int(line) )
        n = n+1
    f.close()

    print n

    pulse.write("*RST")
    pulse.write("*CLS")
    # save wavefrom to internal memory
    #pulse.write("TRACE:DATA EMEMORY,#" + str(len(str(n*2))) \
    #                + str(n*2) + wave.tostring() )
    pulse.write_binary_values("TRACE:DATA EMEMORY,", wave, datatype='H', is_big_endian=True )
    pulse.write("TRACE:COPY USER1,EMEM")
    time.sleep(5)

    pulse.write("FUNCTION USER1")

    # set up trigger
    pulse.write("TRIG:SOUR EXT")
    pulse.write(":CONT ON")

    # set up channel 1
    pulse.write("VOLT:AMPLITUDE 3.00")
    pulse.write("VOLT:OFFSET 1.50")

    # set repitition frequency of entire waveform
    pulse.write("FREQUENCY %fHz" % (freq_pulser/float(n)) )
    pulse.write("OUTPUT ON")

def visa_open(rm):
    p=rm.open_resource("USB0::0x0699::0x0345::C020275::INSTR")
    return p

def main(wffile):
    rm = visa.ResourceManager()
    p=visa_open(rm)
    set_arbitrary(p, wffile)

if __name__ == "__main__":
    if len(sys.argv)>1:
        wffile = str(sys.argv[1])
        main(wffile)
    else:
        main("waveforms/waveform_rate10000Hz_busy50us_fclk5000000Hz.txt")


