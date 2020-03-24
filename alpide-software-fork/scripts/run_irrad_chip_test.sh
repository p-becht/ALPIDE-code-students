#!/bin/bash

######################################################################################
#
# script for running measurements after TID irradiation
# written by jacobus - j.w.van.hoorne@cern.ch
#
######################################################################################

if [ -z "$1" ]
then
  echo "Usage: run_irrad_chip_test.sh <CHIPID>"
  exit 0
fi
CHIP_ID=$1

VBB=0
if [ $CHIP_ID == "W7b-9" ]
then 
    echo $CHIP_ID
    VBB=3.0
elif [ $CHIP_ID == "W7b-11" ]
then 
    echo $CHIP_ID
    VBB=3.0
elif [ $CHIP_ID == "W1b-4" ]
then 
    echo $CHIP_ID
    VBB=3.0
elif [ $CHIP_ID == "W7b-26" ]
then 
    echo $CHIP_ID
    VBB=3.0
elif [ $CHIP_ID == "W7b-36" ]
then 
    echo $CHIP_ID
    VBB=0.0
elif [ $CHIP_ID == "W7b-38" ]
then 
    echo $CHIP_ID
    VBB=0.0
elif [ $CHIP_ID == "W1b-7" ]
then 
    echo $CHIP_ID
    VBB=6.0
elif [ $CHIP_ID == "W7b-4" ]
then 
    echo $CHIP_ID
    VBB=6.0
elif [ $CHIP_ID == "W7b-6" ]
then 
    echo $CHIP_ID
    VBB=6.0
elif [ $CHIP_ID == "W7b-14" ]
then 
    echo $CHIP_ID
    VBB=6.0
elif [ $CHIP_ID == "W1b-6" ]
then 
    echo $CHIP_ID
    VBB=6.0
elif [ $CHIP_ID == "W1b-31" ]
then 
    echo $CHIP_ID
    VBB=6.0
elif [ $CHIP_ID == "W7b-5" ]
then 
    echo $CHIP_ID
    VBB=6.0
elif [ $CHIP_ID == "W4b-33" ]
then 
    echo $CHIP_ID
    VBB=6.0
elif [ $CHIP_ID == "W4b-34" ]
then 
    echo $CHIP_ID
    VBB=6.0
elif [ $CHIP_ID == "W9b-1" ]
then 
    echo $CHIP_ID
    VBB=6.0
elif [ $CHIP_ID == "W9b-5" ]
then 
    echo $CHIP_ID
    VBB=6.0
else
    echo "chip id not found, please check!"
    exit 1
fi



#################################################################
### measurement config
#################################################################


##########################################

# measurement home dir
HOME_DIR=`pwd`
# dir with pALPIDEfs software
SOFTWARE_DIR=$HOME_DIR/../pALPIDEfs-software/
ANA_DIR=$SOFTWARE_DIR/../analysis/
COMMON_DIR=$HOME_DIR/common/

# basic command
CMD=$SOFTWARE_DIR/runTest

##### hameg and back bias

# PSU file descriptor
PSU_DEV=${PSU_DEV-'/dev/ttyHAMEG0'}
# maximum supply current to the boards
PSU_CURR1=1.0    # positive
PSU_CURR2=0.1    #
PSU_CURR3=0.020  # vbb

PSU_VOLT1=5.0
PSU_VOLT2=0.0
PSU_VOLT3=0.0

# initialise all power supplies
$HOME_DIR/common/hameg2030.py $PSU_DEV 0 $PSU_CURR1 $PSU_CURR2 $PSU_CURR3 2>&1
sleep 1

# measure the current
$HOME_DIR/common/hameg2030.py $PSU_DEV 4 2>&1
if [ ${PIPESTATUS[0]} -eq 1 ]
then
    echo "back-bias current too high, stopping measurement"  2>&1
    $HOME_DIR/common/hameg2030.py $PSU_DEV 7 2>&1
    exit 1
fi

##### program fx3
FX3_DIR=$SOFTWARE_DIR/../fx3
cd $FX3_DIR
is_programmed=1
i_try=0
max_tries=5
while [ ! $is_programmed -eq 0 ] 
do
    sleep 1
    echo "programming fx3..."
    ./download_fx3 -t RAM -i SlaveFifoSync.img
    is_programmed=$?

    let i_try=i_try+1
    if [ $i_try -ge $max_tries ]
    then
        echo "max number of tries reached, please reboot daqboard"
        exit 3
    fi
done
wait
sleep 2



cd $HOME_DIR


#$CMD THRESHOLD 164 0 50 2>&1 | tee -a threshold.txt
#
#$CMD POWEROFF

#################################################################
### create folder structure and parameter lists
#################################################################
DATE_TIME=`date +%Y%m%d_%H%M%S`

# create folder structure and write list files
OUT_FOLDER=${CHIP_ID}_${DATE_TIME}
#DATA_DIR=$ANA_DIR/data/$OUT_FOLDER
DATA_DIR=$SOFTWARE_DIR/Data/$OUT_FOLDER
if [ ! -d "$DATA_DIR" ]
then
    mkdir $DATA_DIR 
fi
THR_DIR=$DATA_DIR/Threshold
if [ ! -d "$THR_DIR" ]
then
    mkdir $THR_DIR 
fi
FHR_DIR=$DATA_DIR/Fhr
if [ ! -d "$FHR_DIR" ]
then
    mkdir $FHR_DIR 
fi
DAC_DIR=$DATA_DIR/ScanDAC
if [ ! -d "$DAC_DIR" ]
then
    mkdir $DAC_DIR 
fi



LOG=$DATA_DIR/run.log
if [ -f "$LOG" ]
then
    rm $LOG
fi


#################################################################
### loop over parameters
#################################################################
cd $SOFTWARE_DIR

#cd $HOME_DIR
$HOME_DIR/common/hameg2030.py $PSU_DEV 3 0.0 $VBB 5 2>&1 | tee -a $LOG
# measure the current
$HOME_DIR/common/hameg2030.py $PSU_DEV 4 2>&1 | tee -a $LOG
if [ ${PIPESTATUS[0]} -eq 1 ]
then
    echo "back-bias current too high, stopping measurement"
    ./hameg2030.py $PSU_DEV 7 2>&1 | tee -a $LOG
    exit 1
fi

# change config
if (( "$(echo "${VBB} == 0.0" | bc -l)" )) 
then 
    cp Config_template_0V.cfg Config.cfg 
elif (( "$(echo "${VBB} == 3.0" | bc -l)" )) 
then
    cp Config_template_3V.cfg Config.cfg 
elif (( "$(echo "${VBB} == 6.0" | bc -l)" )) 
then
    cp Config_template_6V.cfg Config.cfg 
else 
    echo "config for VBB not found, please check!!"
    exit 1
fi


# threshold measurement
cd $SOFTWARE_DIR
sleep 5
#./runTest THRESHOLD 164 0 57 2>&1 | tee -a $LOG
./runTest THRESHOLD 164 0 75 2>&1 | tee -a $LOG
cd $SOFTWARE_DIR/Data
RUN_DATA_FILE=`ls -tr1 Threshold*.dat | tail -1`
RUN_CFG_FILE=`ls -tr1 ScanConfig*.cfg | tail -1`
echo "" >> $SOFTWARE_DIR/Data/$RUN_CFG_FILE
echo "CHIP $CHIP_ID" >> $SOFTWARE_DIR/Data/$RUN_CFG_FILE
echo "VBB  $VBB" >> $SOFTWARE_DIR/Data/$RUN_CFG_FILE
# copy data files into folder structure
mv $SOFTWARE_DIR/Data/$RUN_CFG_FILE $THR_DIR
mv $SOFTWARE_DIR/Data/$RUN_DATA_FILE $THR_DIR
cd $THR_DIR
date +%s > timestamp.dat
wait

# fhr measurement
cd $SOFTWARE_DIR
sleep 5
#./runTest NOISEOCC 200000 2>&1 | tee -a $LOG
./runTest NOISEOCC 200000 2>&1 | tee -a $LOG
cd $SOFTWARE_DIR/Data
RUN_DATA_FILE=`ls -tr1 NoiseOccupancy*.dat | tail -1`
RUN_CFG_FILE=`ls -tr1 ScanConfig*.cfg | tail -1`
echo "" >> $SOFTWARE_DIR/Data/$RUN_CFG_FILE
echo "CHIP $CHIP_ID" >> $SOFTWARE_DIR/Data/$RUN_CFG_FILE
echo "VBB  $VBB" >> $SOFTWARE_DIR/Data/$RUN_CFG_FILE
# copy data files into folder structure
mv $SOFTWARE_DIR/Data/$RUN_CFG_FILE $FHR_DIR
mv $SOFTWARE_DIR/Data/$RUN_DATA_FILE $FHR_DIR
cd $FHR_DIR
date +%s > timestamp.dat
wait

# dac scans
cd $SOFTWARE_DIR
sleep 5
./runTest SCANDACS 2>&1 | tee -a $LOG
cd $SOFTWARE_DIR/Data
IBIAS_FILE=`ls -tr1 IDACIBIAS.dat | tail -1`
IDB_FILE=`ls -tr1 IDACIDB.dat | tail -1`
ITHR_FILE=`ls -tr1 IDACITHR.dat | tail -1`
IRESET_FILE=`ls -tr1 IDACIRESET.dat | tail -1`
IAUX2_FILE=`ls -tr1 IDACIAUX2.dat | tail -1`
#IREF_FILE=`ls -tr1 IDACIREF.dat | tail -1`
#IREFBUF_FILE=`ls -tr1 IDACIREFBUF.dat | tail -1`

VCASP_FILE=`ls -tr1 VDACVCASP.dat | tail -1`
VCASN_FILE=`ls -tr1 VDACVCASN.dat | tail -1`
VCASN2_FILE=`ls -tr1 VDACVCASN2.dat | tail -1`
VTEMP_FILE=`ls -tr1 VDACVTEMP.dat | tail -1`
VCLIP_FILE=`ls -tr1 VDACVCLIP.dat | tail -1`
VRESETD_FILE=`ls -tr1 VDACVRESETD.dat | tail -1`
VRESETP_FILE=`ls -tr1 VDACVRESETP.dat | tail -1`
VPULSEL_FILE=`ls -tr1 VDACVPULSEL.dat | tail -1`
VPULSEH_FILE=`ls -tr1 VDACVPULSEH.dat | tail -1`

mv $SOFTWARE_DIR/Data/$IBIAS_FILE   $DAC_DIR
mv $SOFTWARE_DIR/Data/$IDB_FILE     $DAC_DIR
mv $SOFTWARE_DIR/Data/$ITHR_FILE    $DAC_DIR
mv $SOFTWARE_DIR/Data/$IRESET_FILE  $DAC_DIR
mv $SOFTWARE_DIR/Data/$IAUX2_FILE   $DAC_DIR
#mv $SOFTWARE_DIR/Data/$IREF_FILE    $DAC_DIR
#mv $SOFTWARE_DIR/Data/$IREFBUF_FILE $DAC_DIR

mv $SOFTWARE_DIR/Data/$VCASP_FILE     $DAC_DIR
mv $SOFTWARE_DIR/Data/$VCASN_FILE     $DAC_DIR
mv $SOFTWARE_DIR/Data/$VCASN2_FILE    $DAC_DIR
mv $SOFTWARE_DIR/Data/$VTEMP_FILE     $DAC_DIR
mv $SOFTWARE_DIR/Data/$VCLIP_FILE     $DAC_DIR
mv $SOFTWARE_DIR/Data/$VRESETD_FILE   $DAC_DIR
mv $SOFTWARE_DIR/Data/$VRESETP_FILE   $DAC_DIR
mv $SOFTWARE_DIR/Data/$VPULSEL_FILE   $DAC_DIR
mv $SOFTWARE_DIR/Data/$VPULSEH_FILE   $DAC_DIR

cd $DAC_DIR
date +%s > timestamp.dat

cd $HOME_DIR
./run_backbias_threshold.sh $DATA_DIR 


# power off DAQ board 
$HOME_DIR/common/hameg2030.py $PSU_DEV 7 




