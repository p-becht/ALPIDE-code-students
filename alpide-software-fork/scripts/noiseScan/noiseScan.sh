#/bin/bash

###################################################################################################
#
# code written by jacobus
#
# Complete Noise Scan: temperature, vbb, vcasn, ithr, rate, busy
#
###################################################################################################
NOTIFICATION_MAIL=${NOTIFICATION_MAIL-"jvanhoor@cern.ch"}

if [ "$#" -lt 1 ]
then
    # ask for chip ID
    echo "Please enter the name of the chip, followed by [ENTER]:"
    read CHIPNAME
else
    CHIPNAME=$1
fi

IRRAD_LEVEL=`./getIrradLevel.py $CHIPNAME | tail -n1`
cmp=`echo ${IRRAD_LEVEL} | sed -e 's/[eE]+*/\\*10\\^/'`
if (( $(echo "if (${cmp} > -1) 1 else -1" | bc) == -1 ))
then
    echo "chip name or irradiation level of chip not found, please check!"
    exit 1
fi

#if [ "$#" -lt 2 ]
#then
#    echo "Please enter the chip temperature, followed by [ENTER]:"
#    echo "If temperature not controlled enter -1, followed by [ENTER]:"
#    read TEMP
#else
#    TEMP=$2
#fi


#################################################################
### measurement config
#################################################################

# measure threshold as well
MEAS_THR=1

# measurement home dir
HOME_DIR=`pwd`
# dir with pALPIDEfs software
SOFTWARE_DIR=$HOME_DIR/../
ANA_DIR=$SOFTWARE_DIR/../analysis/noise/

##### hameg and back bias
N_VBB=2
VBB_LIST=( 0.0 3.0 )

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

# switch off channel 3,4; TODO: improve!
./hameg2030.py $PSU_DEV 6 3 2>&1
wait
./hameg2030.py $PSU_DEV 6 4 2>&1
wait

##### VCASN
# TODO different VCASN values for VBB
N_VCASN=1
declare -A VCASN_LIST
VCASN_LIST[0,0]=57
VCASN_LIST[1,0]=135
##### ITHR
ITHR_LIST=( 10 20 30 51 70 )
#ITHR_LIST=( 20 51 )

##### RATE ANB BUSY
#RATE_LIST=( 50000 10000 1000 ) 
#BUSY_LIST=( 1 10 20 50 100 ) 
RATE_LIST=( 10000 ) 
BUSY_LIST=( 50 ) 

CLK_FREQ=5000000 # 10MHz 

##### temperature list
TEMP_LIST=(12 20 28)

# program fx3
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

#################################################################
### create folder structure and parameter lists
#################################################################

# create folder structure and write list files
OUT_FOLDER=NoiseVsRate_`date +"%y%m%d_%H%M"`
DATA_DIR=$ANA_DIR/data/$OUT_FOLDER
if [ ! -d "$DATA_DIR" ]
then
    mkdir $DATA_DIR 
fi
LOG=$DATA_DIR/run.log

touch $DATA_DIR/pALPIDEfs_$CHIPNAME
CHIP_FILE=$DATA_DIR/chip_name.txt
if [ -f "$CHIP_FILE" ]
then
    rm $CHIP_FILE
fi
echo "$CHIPNAME" >> $CHIP_FILE
IRRAD_FILE=$DATA_DIR/irrad_level.txt
if [ -f "$IRRAD_FILE" ]
then
    rm $IRRAD_FILE
fi
echo "$IRRAD_LEVEL" >> $IRRAD_FILE

TEMP_FILE=$DATA_DIR/temperature.txt
if [ -f "$TEMP_FILE" ]
then
    rm $TEMP_FILE
fi
for TEMP in "${TEMP_LIST[@]}"
do
    echo "$TEMP" >> $TEMP_FILE
done

RATE_FILE=$DATA_DIR/rate_list.txt
if [ -f "$RATE_FILE" ]
then
    rm $RATE_FILE
fi
for RATE in "${RATE_LIST[@]}"
do
    echo $RATE >> $RATE_FILE
done
BUSY_FILE=$DATA_DIR/busy_list.txt
if [ -f "$BUSY_FILE" ]
then
    rm $BUSY_FILE
fi
for BUSY in "${BUSY_LIST[@]}"
do
    echo $BUSY >> $BUSY_FILE
done
ITHR_FILE=$DATA_DIR/ithr_list.txt
if [ -f "$ITHR_FILE" ]
then
    rm $ITHR_FILE
fi
for ITHR in "${ITHR_LIST[@]}"
do
    echo $ITHR >> $ITHR_FILE
done
VBB_FILE=$DATA_DIR/vbb_list.txt
if [ -f "$VBB_FILE" ]
then
    rm $VBB_FILE
fi
for VBB in "${VBB_LIST[@]}"
do
    echo $VBB >> $VBB_FILE
done


#################################################################
### loop over temp, rates, busys, VBB, VCASN, ITHR
#################################################################
VBB=0.0
VBB_OLD=0.0
VCASN=0.0

for TEMP in "${TEMP_LIST[@]}"
do
    TEMP_DIR=$DATA_DIR/$(printf "TEMP-%0.1f" ${TEMP})
    if [ ! -d "$TEMP_DIR" ]
    then
        mkdir $TEMP_DIR
    fi
    # set temperature of chiller
    cd $HOME_DIR
    ./huber.py 1 $TEMP >> $LOG
    # check if it is reached 
    temp_is_set=0
    while [ $temp_is_set -eq 0 ]
    do
        sleep 10
        ./huber.py 3 
        temp_is_set=$? 
    done
    # wait some time to let the chip accomodate
    sleep 60
    ./huber.py 3 >> $LOG 

    #for VBB in "${VBB_LIST[@]}"
    for ((i_vbb=0;i_vbb<$N_VBB;i_vbb++))
    do
        VBB_OLD=$VBB
        VBB=${VBB_LIST[${i_vbb}]}
        VBB_DIR=$TEMP_DIR/$(printf "VBB-%0.1f" ${VBB}) 
        if [ ! -d "$VBB_DIR" ]
        then
            mkdir $VBB_DIR
        fi
        # TODO include hameg
        cd $HOME_DIR
        ./hameg2030.py $PSU_DEV 3 $VBB_OLD $VBB 5 >> $LOG
        # measure the current
        ./hameg2030.py $PSU_DEV 4 >> $LOG
        if [ ${PIPESTATUS[0]} -eq 1 ]
        then
            echo "back-bias current too high, stopping measurement"
            ./hameg2030.py $PSU_DEV 7  >> $LOG
            exit 1
        fi

        VCASN_FILE=$VBB_DIR/vcasn_list.txt
        if [ -f "$VCASN_FILE" ]
        then
            rm $VCASN_FILE
        fi
        #for VCASN in "${VCASN_LIST[@]}"
        for ((i_vcasn=0;i_vcasn<$N_VCASN;i_vcasn++))
        do
            VCASN=${VCASN_LIST[${i_vbb},${i_vcasn}]}
            echo $VCASN >> $VCASN_FILE
            
            VCASN_DIR=$VBB_DIR/VCASN$VCASN
            if [ ! -d "$VCASN_DIR" ]
            then
                mkdir $VCASN_DIR
            fi
            for ITHR in "${ITHR_LIST[@]}"
            do
                ITHR_DIR=$VCASN_DIR/ITHR$ITHR
                if [ ! -d "$ITHR_DIR" ]
                then
                    mkdir $ITHR_DIR
                fi
    
                # measure thresholds if needed
                if [ $MEAS_THR -eq 1 ]
                then 
                    # switch off pulser output    
                    cd $HOME_DIR
                    ./pulser_off.py >> $LOG
                   
                   # reset daqboard needed to go from packet based to event based
                    cd $SOFTWARE_DIR
                   ./runTest SOFTRESET 10 >> $LOG
                   sleep 5
    
                   # reprogram fx3
                   cd $FX3_DIR
                   ./program.sh >> $LOG
    
                   # threshold measurement
                   cd $SOFTWARE_DIR
                   sleep 4
                   ./runTest THRESHOLD 160 0 50 ${VCASN} ${ITHR} 64 >> $LOG
                   #./ThresholdScanTemperature.sh $CHIPNAME $TEMP
                   cd $SOFTWARE_DIR/Data
                   RUN_DATA_FILE=`ls -tr1 Threshold*.dat | tail -1`
                   RUN_CFG_FILE=`ls -tr1 Threshold*.cfg | tail -1`
                   echo "" >> $SOFTWARE_DIR/Data/$RUN_CFG_FILE
                   echo "CHIP: $CHIPNAME" >> $SOFTWARE_DIR/Data/$RUN_CFG_FILE
                   echo "TEMP: $TEMP" >> $SOFTWARE_DIR/Data/$RUN_CFG_FILE
                   echo "VBB:  $VBB" >> $SOFTWARE_DIR/Data/$RUN_CFG_FILE
                   # copy data files into folder structure
                   cp $SOFTWARE_DIR/Data/$RUN_CFG_FILE $ITHR_DIR
                   cp $SOFTWARE_DIR/Data/$RUN_DATA_FILE $ITHR_DIR
                   wait
                   sleep 1

                   # reset daqboard needed to go from packet based to event based
                    cd $SOFTWARE_DIR
                   ./runTest SOFTRESET 10 >> $LOG
                   sleep 5
    
                    # reprogram fx3
                    cd $FX3_DIR
                    ./program.sh >> $LOG
                fi
    
                # switch on pulser output    
                cd $HOME_DIR
                ./pulser_on.py >> $LOG
    
                for RATE in "${RATE_LIST[@]}"
                do
                    RATE_DIR=$ITHR_DIR/RATE$RATE
                    if [ ! -d "$RATE_DIR" ]
                    then
                        mkdir $RATE_DIR
                    fi
                    for BUSY in "${BUSY_LIST[@]}"
                    do
                        BUSY_DIR=$RATE_DIR/BUSY$BUSY
                        if [ ! -d "$BUSY_DIR" ]
                        then
                            mkdir $BUSY_DIR
                        fi
                        echo "************************************************************" 
                        echo "RATE: $RATE Hz,   BUSY: $BUSY us"
                        echo "VCASN: $VCASN,    ITHR: $ITHR " 
                        echo "TEMP: $TEMP,      VBB:  $VBB"
                        echo "************************************************************"
                        echo "************************************************************" >> $LOG
                        echo "RATE: $RATE Hz,   BUSY: $BUSY us" >> $LOG
                        echo "VCASN: $VCASN,    ITHR: $ITHR " >> $LOG 
                        echo "TEMP: $TEMP,      VBB:  $VBB" >> $LOG
                        echo "************************************************************" >> $LOG
                
                        cd $HOME_DIR
                
                        # generate waveform
                        root -b -l <<EOF >> $LOG 
      .x GenerateWaveform.C+g($RATE, $BUSY, $CLK_FREQ)
EOF
                        wait
                        # program function generator
                        WAVE_FORM="$HOME_DIR/waveforms/waveform_rate${RATE}Hz_busy${BUSY}us_fclk${CLK_FREQ}Hz.txt"
                        echo $WAVE_FORM >> $LOG
                        ./pulse_waveform.py $WAVE_FORM  >> $LOG 
                        wait
                        sleep 5
            
                        echo "************************************************************"
    
                        cd $SOFTWARE_DIR
                        # change config
                        cp Config_template.cfg Config.cfg 
                        sed -i -e s/#VCASN/VCASN/g Config.cfg
                        sed -i -e 's/vcasnTmp/'${VCASN}'/g' Config.cfg
                        sed -i -e s/#ITHR/ITHR/g Config.cfg
                        sed -i -e 's/ithrTmp/'${ITHR}'/g' Config.cfg
            
                        # start measurement
                        ./runTest RATETEST 120000 10000  >> $LOG 
                        wait
                        
                        # add info to run .cfg file 
                        cd $SOFTWARE_DIR/Data
                        RUN_DATA_FILE=`ls -tr1 *.dat | tail -1`
                        RUN_CFG_FILE=`ls -tr1 *.cfg | tail -1`
                        echo "" >> $SOFTWARE_DIR/Data/$RUN_CFG_FILE
                        #echo "# Function generator settings: " >> $SOFTWARE_DIR/Data/$RUN_CFG_FILE
                        echo "RATE: $RATE Hz" >> $SOFTWARE_DIR/Data/$RUN_CFG_FILE
                        echo "BUSY: $BUSY us" >> $SOFTWARE_DIR/Data/$RUN_CFG_FILE
                        echo "GENERATOR CLK_FREQ: $CLK_FREQ Hz" >> $SOFTWARE_DIR/Data/$RUN_CFG_FILE
                        echo "" >> $SOFTWARE_DIR/Data/$RUN_CFG_FILE
                        echo "CHIP: $CHIPNAME" >> $SOFTWARE_DIR/Data/$RUN_CFG_FILE
                        echo "TEMP: $TEMP" >> $SOFTWARE_DIR/Data/$RUN_CFG_FILE
                        echo "VBB:  $VBB" >> $SOFTWARE_DIR/Data/$RUN_CFG_FILE
           
                        # copy data files into folder structure
                        echo $BUSY_DIR
                        cp $SOFTWARE_DIR/Data/$RUN_CFG_FILE $BUSY_DIR
                        cp $SOFTWARE_DIR/Data/$RUN_DATA_FILE $BUSY_DIR
                        cp $WAVE_FORM $BUSY_DIR
                        wait
    
    
                    done
                done
            done
        done
    done
done


cd $HOME_DIR
# power off chip
./hameg2030.py $PSU_DEV 7 
# power off cooling
./huber.py 4
# pulser output off
./pulser_off.py

echo "finished noiseScan.sh" | mail -s "finished noiseScan.sh" $NOTIFICATION_MAIL



