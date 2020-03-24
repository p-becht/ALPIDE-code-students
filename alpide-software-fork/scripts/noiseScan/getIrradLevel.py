#!/usr/bin/env python

import os
import sys


if len(sys.argv)-1 != 1:
    print "number of arguments not correct! please provide: "
    print "1) chip name in format : W<wafer number>-<chip number>"
    print -1
    sys.exit(1)

else:
    chip_name = sys.argv[1]


irrad_level = {'W1-25':0, \
               'W2-31':0, \
               'W9-16':0, \
               'W9-38':0, \
               'W5-21':1e13, \
               'W5-25':1e13, \
               'W6-6' :1e13, \
               'W8-20':1e13, \
               'W8-23':2.5e12, \
               'W9-14':2.5e12, \
               'W9-15':2.5e12
              }

if chip_name in irrad_level.keys():
    print irrad_level[chip_name]
else:
    print -1













