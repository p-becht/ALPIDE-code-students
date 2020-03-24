#!/bin/bash

# PSU file descriptor
PSU_DEV=${PSU_DEV-'/dev/ttyHAMEG0'}

# power down hameg
./hameg2030.py $PSU_DEV 7 2>&1
exit 1







