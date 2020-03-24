#bin/bash

if [ ! "$#" -eq 1 ]
then
    echo "wrong number of parameter!"
    echo "please provide:"
    echo "1) path to data directory"
    exit 1
else
    DATA_DIR=$1
    echo $DATA_DIR
fi
if [ ! -d "$DATA_DIR" ]
then
    "data dir not found! please check!"
    exit 3
fi

HOME_DIR=`pwd`

# dir with pALPIDEfs analysis software
#SOFTWARE_DIR=$( readlink -f $HOME_DIR/../ )
SOFTWARE_DIR=$HOME_DIR/../
cd $SOFTWARE_DIR
SOFTWARE_DIR=`pwd`
echo $SOFTWARE_DIR
#MEASSCRIPT_DIR=$( readlink -f $HOME_DIR/../../pALPIDEfs-software/noiseScan/ )
MEASSCRIPT_DIR=$HOME_DIR/../../pALPIDEfs-software/noiseScan/
cd $MEASSCRIPT_DIR
MEASSCRIPT_DIR=`pwd`
echo $MEASSCRIPT_DIR


cd $DATA_DIR
# create run list
# TODO uncomment!
#ls -d1 NoiseVsRate* > run_list.txt

# loop over rates, busys, VBB, VCASN, ITHR
####################################################################################
TEMP_LIST=( 12 20 28 36 )
VBB_LIST=( 0 3 )
ITHR_LIST=( 10 20 30 51 70 )
VCASN_LIST=( 57 135 )
#CHIP_LIST=( "W1-25" "W2-31" "W9-16" "W9-38" "W6-6" "W5-25" "W5-21" )
CHIP_LIST=( "W2-31" "W9-16" "W9-38" "W6-6" "W5-25" "W5-21" )
#IRRAD_LIST=( "0" "2.5e+12" "1e+13" )
IRRAD_LIST=( "0" "1e+13" )


#TEMP_LIST=( 12 )
#VBB_LIST=( 0 )
#ITHR_LIST=( 10 )
#CHIP_LIST=("W2-31" )


COMP_DIR=$DATA_DIR/comparisons_threshold
cd $COMP_DIR



cd $HOME_DIR

####################################################################################
### comparisons for same chip, same VCASN, VBB,  different ITHR
if [ ! -d $COMP_DIR/vsITHR ]
then
    mkdir $COMP_DIR/vsITHR
fi
IRRAD_LEVEL=0

for CHIP in "${CHIP_LIST[@]}"
do
    CHIP_DIR=$COMP_DIR/vsITHR/$CHIP
    if [ ! -d $CHIP_DIR ] 
    then
        mkdir $CHIP_DIR
    fi
    IRRAD_LEVEL=`$MEASSCRIPT_DIR/getIrradLevel.py $CHIP | tail -n1` 
    cmp=`echo ${IRRAD_LEVEL} | sed -e 's/[eE]+*/\\*10\\^/'` 
    if (( $(echo "if (${cmp} > -1) 1 else -1" | bc) == -1 ))
    then
        echo $IRRAD_LEVEL
        echo $CHIP
        echo "chip name or irradiation level of chip not found, please check!"
        exit 1
    fi

    for TEMP in "${TEMP_LIST[@]}"
    do
        CHIP_TEMP_DIR=$CHIP_DIR/$(printf "TEMP-%0.1f" ${TEMP})
        if [ ! -d $CHIP_TEMP_DIR ]
        then
            mkdir $CHIP_TEMP_DIR
        fi 
        for VBB in "${VBB_LIST[@]}"
        do
            if [ $VBB -eq 0 ]
            then 
                VCASN=57
            elif [ $VBB -eq 3 ]
            then
                VCASN=135
            fi
    
            for SEC in 0 1 2 3 
            do
    
                CHIP_SEC_DIR=$CHIP_TEMP_DIR/sec_$SEC
                if [ ! -d $CHIP_SEC_DIR ]
                then
                    mkdir $CHIP_SEC_DIR
                fi 
                root -b -l <<EOF
                .L ThresholdComparison.C+g
                compare_settings("$DATA_DIR", "run_list.txt", "$CHIP", $TEMP, $IRRAD_LEVEL, "ITHR", $VBB, $VCASN, $SEC)
EOF
                OUT_FILE="$COMP_DIR/${CHIP}_vsITHR_TEMP${TEMP}.0_VBB${VBB}.0_VCASN${VCASN}_sec$SEC.pdf"
                if [ -f $OUT_FILE ]
                then
                    mv $OUT_FILE $CHIP_SEC_DIR
                fi 
            done
        done
    done
done


####################################################################################
### comparisons for same chip, same VCASN, ITHR,  different VBB
if [ ! -d $COMP_DIR/vsVBB ]
then
    mkdir $COMP_DIR/vsVBB
fi
for CHIP in "${CHIP_LIST[@]}"
do
    CHIP_DIR=$COMP_DIR/vsVBB/$CHIP
    if [ ! -d $CHIP_DIR ] 
    then
        mkdir $CHIP_DIR
    fi
    IRRAD_LEVEL=`$MEASSCRIPT_DIR/getIrradLevel.py $CHIP | tail -n1` 
    cmp=`echo ${IRRAD_LEVEL} | sed -e 's/[eE]+*/\\*10\\^/'` 
    if (( $(echo "if (${cmp} > -1) 1 else -1" | bc) == -1 ))
    then
        echo $IRRAD_LEVEL
        echo $CHIP
        echo "chip name or irradiation level of chip not found, please check!"
        exit 1
    fi

    for TEMP in "${TEMP_LIST[@]}"
    do
        CHIP_TEMP_DIR=$CHIP_DIR/$(printf "TEMP-%0.1f" ${TEMP})
        if [ ! -d $CHIP_TEMP_DIR ]
        then
            mkdir $CHIP_TEMP_DIR
        fi
        for ITHR in "${ITHR_LIST[@]}"
        do

            for SEC in 0 1 2 3 
            do

                CHIP_SEC_DIR=$CHIP_TEMP_DIR/sec_$SEC
                if [ ! -d $CHIP_SEC_DIR ]
                then
                    mkdir $CHIP_SEC_DIR
                fi
                root -b -l <<EOF
                .L ThresholdComparison.C+g
                compare_vbb("$DATA_DIR", "run_list.txt", "$CHIP", $TEMP, $IRRAD_LEVEL, $ITHR, $SEC)
EOF
                OUT_FILE="$COMP_DIR/${CHIP}_vsVBB_TEMP${TEMP}.0_ITHR${ITHR}_sec$SEC.pdf"
                if [ -f$OUT_FILE ]
                then
                    mv $OUT_FILE $CHIP_SEC_DIR
                fi
            done
        done
    done
done




####################################################################################
### comparisons for same VBB, VCASN, ITHR,  different TEMP
if [ ! -d $COMP_DIR/vsTemp ]
then
    mkdir $COMP_DIR/vsTemp
fi

for CHIP in "${CHIP_LIST[@]}"
do
    CHIP_DIR=$COMP_DIR/vsTemp/$CHIP
    if [ ! -d $CHIP_DIR ]
    then
        mkdir $CHIP_DIR
    fi 

    IRRAD_LEVEL=`$MEASSCRIPT_DIR/getIrradLevel.py $CHIP | tail -n1` 
    cmp=`echo ${IRRAD_LEVEL} | sed -e 's/[eE]+*/\\*10\\^/'` 
    if (( $(echo "if (${cmp} > -1) 1 else -1" | bc) == -1 ))
    then
        echo $IRRAD_LEVEL
        echo $CHIP
        echo "chip name or irradiation level of chip not found, please check!"
        exit 1
    fi

    for VBB in "${VBB_LIST[@]}"
    do
        if [ $VBB -eq 0 ]
        then 
            VCASN=57
        elif [ $VBB -eq 3 ]
        then
            VCASN=135
        fi
        CHIP_VBB_DIR=$CHIP_DIR/$(printf "VBB-%0.1f" ${VBB})
        if [ ! -d $CHIP_VBB_DIR ]
        then
            mkdir $CHIP_VBB_DIR
        fi

        for ITHR in "${ITHR_LIST[@]}"
        do

           for SEC in 0 1 2 3 
           do

                SEC_DIR=$CHIP_VBB_DIR/sec_$SEC
                if [ ! -d $SEC_DIR ]
                then
                    mkdir $SEC_DIR
                fi

                root -b -l <<EOF
                .L ThresholdComparison.C+g
                compare_temperature("$DATA_DIR", "run_list.txt", "$CHIP", $IRRAD_LEVEL, $VBB, $VCASN, $ITHR, $SEC)
EOF
                OUT_FILE="$COMP_DIR/${CHIP}_vsTemp_VBB${VBB}.0_VCASN${VCASN}_ITHR${ITHR}_sec${SEC}.pdf"
                if [ -f  ]
                then 
                    mv $OUT_FILE $SEC_DIR
                fi

            done
        done
    done
done



####################################################################################
### comparisons for same VBB, VCASN, ITHR,  different CHIPS
if [ ! -d $COMP_DIR/vsChips ]
then
    mkdir $COMP_DIR/vsChips
fi

for IRRAD_LEVEL in "${IRRAD_LIST[@]}"
do
    IRRAD_DIR=$COMP_DIR/vsChips/$(printf "IRRAD-%2.1e" ${IRRAD_LEVEL})
    if [ ! -d $IRRAD_DIR ]
    then
        mkdir $IRRAD_DIR
    fi
    for TEMP in "${TEMP_LIST[@]}"
    do
    
        TEMP_DIR=$IRRAD_DIR/$(printf "TEMP-%0.1f" ${TEMP})
        if [ ! -d $TEMP_DIR ]
        then
            mkdir $TEMP_DIR
        fi
        for ITHR in "${ITHR_LIST[@]}"
        do
            for VBB in "${VBB_LIST[@]}"
            do
                if [ $VBB -eq 0 ]
                then 
                    VCASN=57
                elif [ $VBB -eq 3 ]
                then
                    VCASN=135
                fi
        
                for SEC in 0 1 2 3 
                do
        
                    SEC_DIR=$TEMP_DIR/sec_$SEC
                    if [ ! -d $SEC_DIR ]
                    then
                        mkdir $SEC_DIR
                    fi 
                    root -b -l <<EOF
                    .L ThresholdComparison.C+g
                    compare_chips("$DATA_DIR", "run_list.txt", $TEMP, $IRRAD_LEVEL, $VBB, $VCASN, $ITHR, $SEC)
EOF
                   OUT_FILE="$COMP_DIR/vsChips_$(printf "IRRAD%1.1e" ${IRRAD_LEVEL})_TEMP${TEMP}.0_VBB${VBB}.0_VCASN${VCASN}_ITHR${ITHR}_sec${SEC}.pdf"
                   if [ -f $OUT_FILE ]
                   then
                       mv $OUT_FILE $SEC_DIR
                   fi
               done
           done
       done
    done
done


####################################################################################
### comparisons for same VBB, VCASN, ITHR,  different IRRADIATION LEVELS
if [ ! -d $COMP_DIR/vsIrrad ]
then
    mkdir $COMP_DIR/vsIrrad
fi


for TEMP in "${TEMP_LIST[@]}"
do

    TEMP_DIR=$COMP_DIR/vsIrrad/$(printf "TEMP-%0.1f" ${TEMP})
    if [ ! -d $TEMP_DIR ]
    then
        mkdir $TEMP_DIR
    fi

    for ITHR in "${ITHR_LIST[@]}"
    do
        for VBB in "${VBB_LIST[@]}"
        do
            if [ $VBB -eq 0 ]
            then 
                VCASN=57
            elif [ $VBB -eq 3 ]
            then
                VCASN=135
            fi

            for SEC in 0 1 2 3 
            do

                SEC_DIR=$TEMP_DIR/sec_$SEC
                if [ ! -d $SEC_DIR ]
                then
                    mkdir $SEC_DIR
                fi 
                root -b -l <<EOF
                .L ThresholdComparison.C+g
                compare_irradiation("$DATA_DIR", "run_list.txt", $TEMP, $VBB, $VCASN, $ITHR, $SEC)
EOF

                OUT_FILE="$COMP_DIR/vsIrrad_TEMP${TEMP}.0_VBB${VBB}.0_VCASN${VCASN}_ITHR${ITHR}_sec${SEC}.pdf"
                if [ -f $OUT_FILE ]
                then
                    mv $OUT_FILE $SEC_DIR
                fi

            done
        done
    done
done




