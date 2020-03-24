#!/bin/bash
###################################################################################################
### Configuration
###################################################################################################

VBB_LIST=( 0.0 3.0 6.0 ) # in V
I_THR_LIST=( 51 100 ) # in DAC
V_CASN_LIST=( 57 105 135 ) # in DAC
# how many V_CASN values to process per back-bias voltage
V_CASN_PER_VBB=$(echo "${#V_CASN_LIST[@]} / ${#VBB_LIST[@]}" | bc)

MODE_LIST=( A B ) # chip readout mode
STROBEB_LIST=( 4 0 ) # continuous integration: 0,
                     # different from 0: multiple of 25ns clock cycles

TRG_FREQ_LIST=( 100 200 500 1000 2000 5000 10000 20000 50000 100000 ) # in Hz
TRG_TRAIN_LENGTH=( 100 ) # number of consecutive triggers

N_EVTS=50000 # number of events per setting


HAMEG=/dev/ttyHAMEG0

###################################################################################################


###################################################################################################
# setup environment / load functions
ROOT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd ) # determine where this script is located
source ${ROOT_DIR}/functions.sh

###################################################################################################
### determine 'run conditions'
eval $(determine_run_conditions)

###################################################################################################
### execution

if [ "$#" -lt 1 ]
then
    # ask for chip ID
    echo "Please enter the name of the chip followed by [ENTER]:"
    read CHIPNAME
else
    CHIPNAME=$1
fi


# is ROOT available?
check_root

# create output folder
FULLPATH=${ROOT_DIR}/../Data/rate_dependence_${DATE}_${GIT_INFO}${SUFFIX}
create_output_folder $FULLPATH

LOGFILE=${FULLPATH}/log.txt

# store the chip name
touch ${FULLPATH}/${CHIPNAME}

# store git diff
store_git_diff $FULLPATH >> $LOGFILE 2>&1

DATAPATH=${FULLPATH}/data
mkdir $DATAPATH

###################################################################################################
### initialise all power supplies
###################################################################################################

${ROOT_DIR}/pulser.py -1.0 # deactivate pulser which could lead to an input signal above VDDA and VDDD

${ROOT_DIR}/hameg2030.py ${HAMEG} 0 ${VBB_LIST[0]}

###################################################################################################
#### start the measurment itself
###################################################################################################
i_VBB=0
for VBB in "${VBB_LIST[@]}"
do
    # set the back-bias voltage
    echo "setting back-bias voltage to ${VBB}V" | tee -a $LOGFILE
    ${ROOT_DIR}/hameg2030.py ${HAMEG} 1 3 ${VBB}
    VBB_OLD=${VBB}
    sleep 1
    ${ROOT_DIR}/hameg2030.py ${HAMEG} 2
    if [ $? -eq 1 ]
    then
        echo "back-bias current too high, stopping measurement"
        ${ROOT_DIR}/hameg2030.py ${HAMEG} 4
        exit 1
    fi

    for I_THR in "${I_THR_LIST[@]}"
    do
        echo "I_THR="${I_THR}"DAC" | tee -a $LOGFILE
        for i_V_CASN in $(seq 0 $((${V_CASN_PER_VBB} - 1)))
        #for V_CASN in "${V_CASN_LIST[@]}"
        do
            V_CASN=${V_CASN_LIST[$(( i_VBB*${V_CASN_PER_VBB} + i_V_CASN ))]}
            echo "V_CASN="${V_CASN}"DAC" | tee -a $LOGFILE
            for MODE in "${MODE_LIST[@]}"
            do
                echo "READOUT MODE=$MODE" | tee -a $LOGFILE

                for TRG_FREQ in "${TRG_FREQ_LIST[@]}"
                do
                    echo "TRG_FREQ="${TRG_FREQ}"Hz" | tee -a $LOGFILE
                    ${ROOT_DIR}/pulser.py 1 ${TRG_FREQ} ${TRG_TRAIN_LENGTH}

                    for STROBEB in "${STROBEB_LIST[@]}"
                    do
                        if [ "${STROBEB}" -eq 0 ]
                        then
                            STROBEB=$(bc <<< "40000000/${TRG_FREQ}-10" )
                            echo "Continuous integration mode, STROBEB="$(bc <<< "${STROBEB}*25")"ns" | tee -a $LOGFILE
                        else
                            echo "Fixed length STROBEB="$(bc <<< "${STROBEB}*25")"ns" | tee -a $LOGFILE
                        fi

                        ###########################################################################
                        ### write config file
                        ###########################################################################
                        cd ${ROOT_DIR}
                        cp Config_tmpl.cfg Config.cfg
                        sed -i -e s/#ITHR/ITHR/g Config.cfg
                        sed -i -e 's/ithrTmp/'${I_THR}'/g' Config.cfg
                        sed -i -e s/#VCASN/VCASN/g Config.cfg
                        sed -i -e 's/vcasnTmp/'${V_CASN}'/g' Config.cfg
                        sed -i -e s/#VCASN2/VCASN2/g Config.cfg
                        sed -i -e 's/vcasn2Tmp/'$((${V_CASN}+7))'/g' Config.cfg

                        sed -i -e s/#READOUTMODE/READOUTMODE/g Config.cfg
                        sed -i -e 's/readoutmodeTmp/'${MODE}'/g' Config.cfg

                        sed -i -e s/#STROBEBLENGTHSTANDARD/STROBELENGTHSTANDARD/g Config.cfg
                        sed -i -e s/#STROBEBLENGTHDIGITAL/STROBELENGTHDIGITAL/g Config.cfg
                        sed -i -e s/#STROBEBLENGTHSOURCE/STROBELENGTHSOURCE/g Config.cfg
                        sed -i -e 's/strobebTmp/'${STROBEB}'/g' Config.cfg

                        sed -i -e s/#PULSELENGTHDIGITAL/PULSELENGTHDIGITAL/g Config.cfg
                        sed -i -e 's/pulselengthTmp/'$(bc <<< "${STROBEB} - 10")'/g' Config.cfg


                        ## VCASN2, STROBEBLENGTH, READOUTMODE,
                        #
                        #
                        mv Config.cfg ../



                        CURR_DATAPATH=$(printf "$DATAPATH/VBB-%0.1f/ITHR%0.3d/VCASN%0.3d/MODE%c/TRG_FREQ%0.6d/STROBEB%0.6d" \
                                               ${VBB} ${V_RST} ${I_THR} ${V_CASN} ${MODE} ${TRG_FREQ} ${STROBEB})
                        SUBFOLDER=${CURR_DATAPATH}/FHR
                        mkdir -p ${SUBFOLDER}
                        cp ../Config.cfg ${SUBFOLDER}

                        # take the data

                        # process it

                        ###########################################################################
                        ### start the acquisition
                        ###########################################################################

                        ###########################################################################
                        ### start the analysis
                        ###########################################################################
                    done
                done
            done
        done
    done
    let i_VBB+=1
done

###################################################################################################
### power down everything
###################################################################################################
${ROOT_DIR}/hameg2030.py ${HAMEG} 6 # turn off the PSU

### cleanup
#git checkout ${ROOT_DIR}/Config.cfg
