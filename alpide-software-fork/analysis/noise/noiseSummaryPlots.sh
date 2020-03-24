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
#CHIP_LIST=( "W1-25" "W2-31" "W9-16" "W9-38" "W6-6" "W5-25" "W5-21")
CHIP_LIST=( "W2-31" "W9-16" "W9-38" "W6-6" "W5-25" "W5-21" )
#IRRAD_LIST=( "0" "2.5e+12" "1e+13" )
IRRAD_LIST=( "0" "1e+13" )

#TEMP_LIST=( 12 )
#VBB_LIST=( 0 )
#ITHR_LIST=( 10 )
#CHIP_LIST=("W2-31" )



####################################################################################
### create the graphs
cd $HOME_DIR

root -b -l <<EOF
     .L NoiseSummary.C+g
     create_noise_graphs_vsTemp("$DATA_DIR", "run_list.txt")
     create_noise_graphs_vsITHR("$DATA_DIR", "run_list.txt")
EOF



COMP_DIR=$DATA_DIR/comparisons_noise/summary_plots/
cd $COMP_DIR


cd $HOME_DIR
####################################################################################
### comparisons for same chip, same VCASN, VBB,  different Sectors
if [ ! -d $COMP_DIR/vsSector ]
then
    mkdir $COMP_DIR/vsSector
fi
IRRAD_LEVEL=0

for CHIP in "${CHIP_LIST[@]}"
do
    CHIP_DIR=$COMP_DIR/vsSector/$CHIP
    if [ ! -d $CHIP_DIR ] 
    then
        mkdir $CHIP_DIR
    fi
    IRRAD_LEVEL=`$MEASSCRIPT_DIR/getIrradLevel.py $CHIP | tail -n1` 
    if [ $IRRAD_LEVEL -eq -1 ]
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
        
        #CHIP_VBB_DIR=$CHIP_DIR/sec_$SEC
        #if [ ! -d $CHIP_SEC_DIR ]
        #then
        #    mkdir $CHIP_SEC_DIR
        #fi 

    
        for ITHR in "${ITHR_LIST[@]}"
        do
    
            root -b -l <<EOF
            .L NoiseSummary.C+g
            compare_vsTemp_Sectors("$DATA_DIR", "$CHIP", $IRRAD_LEVEL, $VBB, $VCASN, $ITHR, 10000, 50)
EOF
            OUT_FILE="$COMP_DIR/${CHIP}_vsSec_VBB${VBB}.0_VCASN${VCASN}_ITHR${ITHR}.pdf"
            if [ -f $OUT_FILE ]
            then
                #mv $OUT_FILE $CHIP_SEC_DIR
                mv $OUT_FILE $CHIP_DIR
            fi 
        done
    done
done


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
    if [ $IRRAD_LEVEL -eq -1 ]
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
    
        for SEC in 0 1 2 3 
        do
    
            CHIP_SEC_DIR=$CHIP_DIR/sec_$SEC
            if [ ! -d $CHIP_SEC_DIR ]
            then
                mkdir $CHIP_SEC_DIR
            fi 
            root -b -l <<EOF
            .L NoiseSummary.C+g
            compare_vsTemp_ITHR("$DATA_DIR", "$CHIP", $IRRAD_LEVEL, $VBB, $VCASN, 10000, 50, $SEC)
EOF
            OUT_FILE="$COMP_DIR/${CHIP}_vsITHR_VBB${VBB}.0_VCASN${VCASN}_sec$SEC.pdf"
            if [ -f $OUT_FILE ]
            then
                mv $OUT_FILE $CHIP_SEC_DIR
            fi 
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
    if [ $IRRAD_LEVEL -eq -1 ]
    then
        echo $IRRAD_LEVEL
        echo $CHIP
        echo "chip name or irradiation level of chip not found, please check!"
        exit 1
    fi

    for ITHR in "${ITHR_LIST[@]}"
    do

        for SEC in 0 1 2 3 
        do

            CHIP_SEC_DIR=$CHIP_DIR/sec_$SEC
            if [ ! -d $CHIP_SEC_DIR ]
            then
                mkdir $CHIP_SEC_DIR
            fi
            root -b -l <<EOF
            .L NoiseSummary.C+g
            compare_vsTemp_VBB("$DATA_DIR", "$CHIP", $IRRAD_LEVEL, $ITHR, 10000, 50, $SEC)
EOF
            OUT_FILE="$COMP_DIR/${CHIP}_vsVBB_ITHR${ITHR}_sec$SEC.pdf"
            if [ -f $OUT_FILE ]
            then
                mv $OUT_FILE $CHIP_SEC_DIR
            fi
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

                SEC_DIR=$IRRAD_DIR/sec_$SEC
                if [ ! -d $SEC_DIR ]
                then
                    mkdir $SEC_DIR
                fi 
                root -b -l <<EOF
                .L NoiseSummary.C+g
                compare_vsTemp_Chips("$DATA_DIR", $IRRAD_LEVEL, $VBB, $VCASN, $ITHR, 10000, 50, $SEC)
EOF
                OUT_FILE="$COMP_DIR/vsChips_$(printf "IRRAD%1.1e" ${IRRAD_LEVEL})_VBB${VBB}.0_VCASN${VCASN}_ITHR${ITHR}_sec${SEC}.pdf"
                if [ -f $OUT_FILE ]
                then
                    mv $OUT_FILE $SEC_DIR
                fi

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

            SEC_DIR=$COMP_DIR/vsIrrad/sec_$SEC
            if [ ! -d $SEC_DIR ]
            then
                mkdir $SEC_DIR
            fi 
            root -b -l <<EOF
            .L NoiseSummary.C+g
            compare_vsTemp_Irrad("$DATA_DIR", $VBB, $VCASN, $ITHR, 10000, 50, $SEC)
EOF
            OUT_FILE="$COMP_DIR/vsIrrad_VBB${VBB}.0_VCASN${VCASN}_ITHR${ITHR}_sec${SEC}.pdf"
            if [ -f $OUT_FILE ]
            then
                mv $OUT_FILE $SEC_DIR
            fi

        done
    done
done





