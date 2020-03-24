#!/bin/bash
###################################################################################################
#
# Turn ON hameg
#
###################################################################################################

###################################################################################################
### default configuration
#
# only used for parameters which are not in the config.sh or if the config.sh cannot be loaded

# back-bias voltages to be scanned (specified as positive values!)
#VBB_LIST=($(seq 8 -1.0 0))
#VBB_LIST=( 1.0 )
#VBB_LIST=( 6.0 1.0 )
#VBB_LIST=( 6.0 1.0 0.0 )
#VBB_LIST=( 6.0 3.0 1.0 )

# PSU file descriptor
PSU_DEV=${PSU_DEV-'/dev/ttyHAMEG4CH0'}
# maximum supply current to the proximity board in A
PSU_CURR1=1.0    # positive
PSU_CURR2=0.001  # vbb
PSU_CURR3=0.0  # 
PSU_CURR4=0.0  # positive

PSU_VOLT1=5.0
PSU_VOLT2=0.0
PSU_VOLT3=0.0
PSU_VOLT4=0.0


# initialise all power supplies
./hameg2030.py $PSU_DEV 0 $PSU_CURR1 $PSU_CURR2 $PSU_CURR3 $PSU_CURR4 2>&1
sleep 1

# ramp the main power supply
echo "ramping the main power supply" 2>&1
./hameg2030.py $PSU_DEV 2 $PSU_VOLT1 $PSU_VOLT2 $PSU_VOLT3 $PSU_VOLT4 10  2>&1
sleep 1
# measure the current
./hameg2030.py $PSU_DEV 4 2>&1
if [ ${PIPESTATUS[0]} -eq 1 ]
then
    echo "back-bias current too high, stopping measurement"  2>&1
    ./hameg2030.py $PSU_DEV 7 2>&1
    exit 1
fi

./hameg2030.py $PSU_DEV 6 3 2>&1
wait
./hameg2030.py $PSU_DEV 6 4 2>&1
wait



## set the first back-bias voltage
#echo "setting first back-bias voltage" 2>&1
#VBB_OLD=0.0
#./hameg2030.py $PSU_DEV  3 $VBB_OLD ${VBB_LIST[0]} 100 2>&1
#VBB_OLD=${VBB_LIST[0]}
#
#sleep 1
## measure the current
#./hameg2030.py $PSU_DEV 4 2>&1
#if [ ${PIPESTATUS[0]} -eq 1 ]
#then
#    echo "back-bias current too high, stopping measurement" 2>&1
#    ./hameg2030.py $PSU_DEV 7 2>&1
#    exit 1
#fi




