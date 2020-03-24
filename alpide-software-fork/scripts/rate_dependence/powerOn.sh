#!/bin/bash
#
### powerOn.sh
#
### Configuration
HAMEG=/dev/ttyHAMEG1


### Initialisation
CURR_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd ) # determine where this script is located

### Execution
${CURR_DIR}/hameg2030.py ${HAMEG} 5 # power off
sleep 2
${CURR_DIR}/hameg2030.py ${HAMEG} 0 3.0 # power on, Vbb=-3V
sleep 1
${CURR_DIR}/hameg2030.py ${HAMEG} 2 # tripped?
${CURR_DIR}/hameg2030.py ${HAMEG} 3 # currents?
