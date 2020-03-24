#!/bin/bash
#
SW_NAME="pAAnalysis"
SW_VERSION="master"
#
export PA_ROOT=YOUR_OPT_PATH/${SW_NAME}/${SW_VERSION}
#
export PATH=${PA_ROOT}/bin:${PATH}
export LD_LIBRARY_PATH=${PA_ROOT}/lib:${LD_LIBRARY_PATH}
