#!/usr/bin/env python_32bit

import visa
import sys
import array

def output_off(pulse):
    pulse.write("OUTPUT OFF")

def visa_open(rm):
    p = rm.open_resource("USB0::0x0699::0x0345::C020275::INSTR")
    return p

def main():
    rm = visa.ResourceManager()
    p=visa_open(rm)
    output_off(p)

if __name__ == "__main__":
    main()


