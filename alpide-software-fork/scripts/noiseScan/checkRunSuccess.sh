#!/bin/bash

if [ "$#" -lt 1 ]
then
    # ask for chip ID
    echo "Please enter the name data directory, followed by [ENTER]:"
    read DATA_DIR
else
    DATA_DIR=$1
fi

#################################################################
#################################################################

CHIP_FILE=$DATA_DIR/chip_name.txt
CHIP=`cat $CHIP_FILE`
echo $CHIP
IRRAD_FILE=$DATA_DIR/irrad_level.txt
TEMP_FILE=$DATA_DIR/temperature.txt
RATE_FILE=$DATA_DIR/rate_list.txt
BUSY_FILE=$DATA_DIR/busy_list.txt
ITHR_FILE=$DATA_DIR/ithr_list.txt
VBB_FILE=$DATA_DIR/vbb_list.txt

for TEMP in `cat $TEMP_FILE`
do
    TEMP_DIR=$DATA_DIR/$(printf "TEMP-%0.1f" ${TEMP})
    if [ ! -d "$TEMP_DIR" ]
    then
        echo "VBB DIR not found!"
        exit 1
    fi

    for VBB in `cat $VBB_FILE`
    do
        VBB_DIR=$TEMP_DIR/$(printf "VBB-%0.1f" ${VBB}) 
        if [ ! -d "$VBB_DIR" ]
        then
            echo "VBB DIR not found!"
            exit 1
        fi

        VCASN_FILE=$VBB_DIR/vcasn_list.txt

        for VCASN in `cat $VCASN_FILE`
        do
            
            VCASN_DIR=$VBB_DIR/VCASN$VCASN
            if [ ! -d "$VCASN_DIR" ]
            then
                echo "VCASN DIR not found!"
                exit 1
            fi

            for ITHR in `cat $ITHR_FILE`
            do
                ITHR_DIR=$VCASN_DIR/ITHR$ITHR
                if [ ! -d "$ITHR_DIR" ]
                then
                    echo "ITHR DIR not found!"
                    exit 1
                fi

                ITHR_CFG=`cat $ITHR_DIR/ThresholdScan_*.cfg | grep ITHR | awk -F' ' '{print $2;}'`
                VCASN_CFG=`cat $ITHR_DIR/ThresholdScan_*.cfg | grep VCASN | awk -F' ' '{print $2;}'`
                
                CFG_FILE=$ITHR_DIR/Threshold*.cfg
                echo $CFG_FILE
                #echo " " >> $CFG_FILE
                echo "VBB: $VBB" >> $CFG_FILE
                
                if [ $ITHR_CFG -ne $ITHR ]
                then 
                    "ITHR do not match"
                    echo $ITHR_DIR
                    echo "$ITHR     $ITHR_CFG"
                    exit 1
                fi
                if [ $VCASN_CFG -ne $VCASN ]
                then 
                    "VCASN do not match"
                    echo $ITHR_DIR
                    echo "$VCASN     $VCASN_CFG"
                    exit 1
                fi

                for RATE in `cat $RATE_FILE`
                do
                    RATE_DIR=$ITHR_DIR/RATE$RATE
                    if [ ! -d "$RATE_DIR" ]
                    then
                        echo $RATE_DIR
                        echo "RATE DIR not found!"
                        exit 1
                    fi

                    for BUSY in `cat $BUSY_FILE`
                    do
                        BUSY_DIR=$RATE_DIR/BUSY$BUSY
                        if [ ! -d "$BUSY_DIR" ]
                        then
                            echo "BUSY DIR not found!"
                            exit 1
                        fi

                        ITHR_CFG=`cat $BUSY_DIR/Noise*.cfg | grep ITHR | awk -F' ' '{print $2;}'`
                        VCASN_CFG=`cat $BUSY_DIR/Noise*.cfg | grep VCASN | awk -F' ' '{print $2;}'`

                        if [ $ITHR_CFG -ne $ITHR ]
                        then 
                            "ITHR do not match"
                            echo $BUSY_DIR
                            echo "$ITHR     $ITHR_CFG"
                            exit 1
                        fi
                        if [ $VCASN_CFG -ne $VCASN ]
                        then 
                            "VCASN do not match"
                            echo $BUSY_DIR
                            echo "$VCASN     $VCASN_CFG"
                            exit 1
                        fi

                        #CFG_FILE=$BUSY_DIR/Noise*.cfg
                        #echo $CFG_FILE
                        #echo " " >> $CFG_FILE
                        #echo "CHIP: $CHIP" >> $CFG_FILE
                        #echo "VBB: $VBB" >> $CFG_FILE

                    done
                done
            done
        done
    done
done




