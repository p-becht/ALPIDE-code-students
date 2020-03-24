#bin/bash

### check input
if [ ! "$#" -eq 3 ]
then
    echo "wrong number of parameter!"
    echo "please provide:"
    echo "1) path to measurement folder"
    echo "2) number of pixels to exclude for noise occupancy calculation and comparison plots"
    echo "3) flag for summary creation: 0->no, 1->yes"
    exit 1
else
    MEAS_DIR=$1
    echo $MEAS_DIR
    N_PIX_EXCL=$2
    echo $2
    CREATE_SUMMARY=$3
    echo $3
fi

HOME_DIR=`pwd`

# dir with pALPIDEfs analysis software
SOFTWARE_DIR=$HOME_DIR/../

### read list files for recreating measurement folder structure 
DATA_DIR=$MEAS_DIR
if [ ! -d "$DATA_DIR" ]
then
    "data dir not found! please check!"
    exit 3
fi
LOG=$DATA_DIR/analysis.log


TEMP_FILE=$DATA_DIR/temperature.txt
if [ ! -f "$TEMP_FILE" ]
then
    "temp file not found! please check!"
    exit 3
fi
VBB_FILE=$DATA_DIR/vbb_list.txt
if [ ! -f "$VBB_FILE" ]
then
    "vbb file not found! please check!"
    exit 3
fi
# VCASN file in VBB folders!
ITHR_FILE=$DATA_DIR/ithr_list.txt
if [ ! -f "$ITHR_FILE" ]
then
    "ithr file not found! please check!"
    exit 3
fi
RATE_FILE=$DATA_DIR/rate_list.txt
if [ ! -f "$RATE_FILE" ]
then
    "rate file not found! please check!"
    exit 3
fi
BUSY_FILE=$DATA_DIR/busy_list.txt
if [ ! -f "$BUSY_FILE" ]
then
    "busy file not found! please check!"
    exit 3
fi


# for information
IRRAD_FILE=$DATA_DIR/irrad_level.txt
if [ ! -f "$IRRAD_FILE" ]
then
    "irradiation file not found! please check!"
    exit 3
fi
CHIP_FILE=$DATA_DIR/chip_name.txt
if [ ! -f "$CHIP_FILE" ]
then
    "chip file not found! please check!"
    exit 3
fi


### compile raw data analysis macro
cd $SOFTWARE_DIR
root -b -l <<EOF >> $LOG 
  .L classes/helpers.cpp+g
  .L pALPIDEfsNoiseOccupancy.C++
EOF

cd $DATA_DIR

####################################################################################
### single file analysis
# loop over temperatures, VBB, VCASN, ITHR, rates, busys

for TEMP in `cat $TEMP_FILE`
do # temp loop
    TEMP_DIR=$DATA_DIR/$(printf "TEMP-%0.1f" ${TEMP}) 
    if [ ! -d "$TEMP_DIR" ]
    then
        echo $TEMP
        "temp dir not found! please check!"
        exit 3
    fi

    for VBB in `cat $VBB_FILE`
    do # vbb loop
        VBB_DIR=$TEMP_DIR/$(printf "VBB-%0.1f" ${VBB}) 
        if [ ! -d "$VBB_DIR" ]
        then
            echo $VBB
            "vbb dir not found! please check!"
            exit 3
        fi
        VCASN_FILE=$VBB_DIR/vcasn_list.txt
        if [ ! -f "$VCASN_FILE" ]
        then
            "vcasn file not found! please check!"
            exit 3
        fi
        for VCASN in `cat $VCASN_FILE`
        do # vcasn loop
            VCASN_DIR=$VBB_DIR/VCASN$VCASN
            if [ ! -d "$VCASN_DIR" ]
            then
                echo $VCASN
                "vcasn dir not found! please check!"
                exit 3
            fi
            for ITHR in `cat $ITHR_FILE`
            do # ithr loop
                ITHR_DIR=$VCASN_DIR/ITHR$ITHR
                if [ ! -d "$ITHR_DIR" ]
                then
                    echo $ITHR
                    "ithr dir not found! please check!"
                    exit 3
                fi
                for RATE in `cat $RATE_FILE`
                do # rate loop
                    RATE_DIR=$ITHR_DIR/RATE$RATE
                    if [ ! -d "$RATE_DIR" ]
                    then
                        echo $RATE
                        "rate dir not found! please check!"
                        exit 3
                    fi
                    for BUSY in `cat $BUSY_FILE`
                    do # busy loop
                        BUSY_DIR=$RATE_DIR/BUSY$BUSY
                        if [ ! -d "$BUSY_DIR" ]
                        then
                            echo $BUSY
                            "busy dir not found! please check!"
                            exit 3
                        fi
                        echo "************************************************************" 
                        echo "RATE: $RATE Hz,   BUSY: $BUSY us"
                        echo "VCASN: $VCASN,    ITHR: $ITHR " 
                        echo "TEMP: $TEMP,      VBB: $VBB " 
                        echo "DATA DIR found"
                        echo "************************************************************"
                        echo "************************************************************" >> $LOG
                        echo "RATE: $RATE Hz,   BUSY: $BUSY us" >> $LOG
                        echo "VCASN: $VCASN,    ITHR: $ITHR " >> $LOG 
                        echo "TEMP: $TEMP,      VBB: $VBB "  >> $LOG
                        echo "************************************************************" >> $LOG
               
                        DATA_FILE=`ls -1 $BUSY_DIR/NoiseOccupancyScan_*.dat | tail -n1`
                        if [ ! -f "$DATA_FILE" ]
                        then
                            echo $DATA_FILE
                            "data file not found! please check!"
                            exit 3
                        fi
    
                        cd $SOFTWARE_DIR
    
                        # analyize file
                        root -b -l <<EOF >> $LOG 
  .L classes/helpers.cpp+g
  .x pALPIDEfsNoiseOccupancy.C+g("$DATA_FILE", 120000) 
EOF
                        wait
    
                        cd $DATA_DIR
    
                    done
                done
            done
        done
    done
done

rm $DATA_DIR/NoiseOccupancy.root
hadd $DATA_DIR/NoiseOccupancy.root `find $DATA_DIR -name "NoiseOccupancy.root"`





# create comparison plots and summary document
####################################################################################
if [ $CREATE_SUMMARY -eq 1 ]
then
    cd $HOME_DIR
    ./noiseVsTrgRateSummary.sh $DATA_DIR $N_PIX_EXCL

fi



