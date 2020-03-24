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


irrad_level = {'W1-13':0, \
               'W6-3':0, \
               'W1-16':1.7e13, \
               'W1-21':1.7e13
              }

if chip_name in irrad_level.keys():
    print irrad_level[chip_name]
else:
    print -1













