#bin/bash

### check input
if [ ! "$#" -eq 1 ]
then
    echo "wrong number of parameter!"
    echo "please provide:"
    echo "1) path to measurement folder"
    exit 1
else
    MEAS_DIR=$1
    echo $MEAS_DIR
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
  .L FitThresholds.C++
EOF

cd $DATA_DIR

####################################################################################
### single file analysis
# loop over temperatures, VBB, VCASN, ITHR

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
                echo "************************************************************" 
                echo "VCASN: $VCASN,    ITHR: $ITHR " 
                echo "TEMP: $TEMP,      VBB: $VBB " 
                echo "DATA DIR found"
                echo "************************************************************"
                echo "************************************************************" >> $LOG
                echo "VCASN: $VCASN,    ITHR: $ITHR " >> $LOG 
                echo "TEMP: $TEMP,      VBB: $VBB "  >> $LOG
                echo "************************************************************" >> $LOG
               
                DATA_FILE=`ls -1 $ITHR_DIR/ThresholdScan_*.dat | tail -n1`
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
  .x FitThresholds.C+g("$DATA_FILE", true, $ITHR, $VCASN, true) 
EOF
                wait
    
                cd $DATA_DIR
    
            done
        done
    done
done

rm $DATA_DIR/Thresholds.root
hadd $DATA_DIR/Thresholds.root `find $DATA_DIR -name "Thresholds_*.root"`







