#!/bin/bash

PSU_DEV="/dev/ttyHAMEG0"

VOLT_STEP=-0.05

PSU_CURR1=1.0
PSU_CURR2=0.1
PSU_CURR3=0.020

PSU_VOLT1=5.0
PSU_VOLT2=0.0
PSU_VOLT3=1.5

#CHIP_ID=$1
OUT_DIR=$1

HOME_DIR=`pwd`
SOFTWARE_DIR=$HOME_DIR/../pALPIDEfs-software
DATA_DIR=$OUT_DIR/VBB_Threshold
echo "$DATA_DIR"
LOG=$DATA_DIR/run.log
TEMP=$DATA_DIR/temp
DATA_FILE=$DATA_DIR/backbias_threshold.dat

if [ ! -d "$DATA_DIR" ]; then
    mkdir -p $DATA_DIR
fi



#$HOME_DIR/common/hameg2030.py $PSU_DEV 0 $PSU_CURR1 $PSU_CURR2 $PSU_CURR3 2>&1
#sleep 1
$HOME_DIR/common/hameg2030.py $PSU_DEV 1 3 $PSU_VOLT3
sleep 1
cd $HOME_DIR/../fx3
./program.sh
cd $SOFTWARE_DIR
cp Config_template_0V.cfg Config.cfg 

#for i in {3.0..0..${VOLT_STEP}};
for i in `seq 1.5 -0.05 0.0`
do
  beforei=`awk "BEGIN {print $i+0.05}"`
  if [ "$i" != "1.5" ]; then
      $HOME_DIR/common/hameg2030.py $PSU_DEV 1 3 $beforei $i 10
  fi
  sleep 2

  timeout -k 68 60 ./runTest POWERON | tee -a $TEMP
  cat $TEMP >> $LOG
  if [ $? -eq 139 ] && [ -f "$DATA_FILE" ]; then
    TIME=`date +%d%m%Y_%H%M%S`
    WRITE_DATA="$TIME;$i;;;;;;;;;;"
    echo "$WRITE_DATA" >> $DATA_FILE  
    exit 139
  elif [ $? -eq 139 ]; then
    echo "TIME;HAMEGCURR;BACK_BIAS_CURR;TEMPERATURE_PREINIT;DIGITAL_PREINIT;OUTPUT_PREINIT;ANALOG_PREINIT;TEMPERATURE_INIT;DIGITAL_INIT;OUTPUT_INIT;ANALOG_INIT;" >> $DATA_FILE
    TIME=`date +%d%m%Y_%H%M%S`
    WRITE_DATA="$TIME;$i;;;;;;;;;;"
    echo "$WRITE_DATA" >> $DATA_FILE  
    exit 139    
  fi
  LINECOUNT=0
  sleep 1
  while read -r line;
  do
    TIME=`date +%Y%m%d_%H%M%S`
    TEMPCURRENT=$(echo "$line" | awk '/Read ADC/ {print}' | awk -F'=' '{print $2}' | awk '{print $1}')
    TEMPCURR_REMOVESPACE="$(echo -e "${TEMPCURRENT}" | tr -d '[[:space:]]')"
    if [ -n "$TEMPCURR_REMOVESPACE" ]; then
      echo $LINECOUNT
      echo $TEMPCURR_REMOVESPACE
      if [ "$LINECOUNT" -eq 0 ]; then
        TEMPERATURE_PREINIT=$TEMPCURR_REMOVESPACE
        LINECOUNT=$(($LINECOUNT+1))
      elif [ "$LINECOUNT" -eq 1 ]; then
        DIGITAL_PREINIT=$TEMPCURR_REMOVESPACE
        LINECOUNT=$(($LINECOUNT+1))
      elif [ "$LINECOUNT" -eq 2 ]; then
        OUTPUT_PREINIT=$TEMPCURR_REMOVESPACE
        LINECOUNT=$(($LINECOUNT+1))
      elif [ "$LINECOUNT" -eq 3 ]; then
        ANALOG_PREINIT=$TEMPCURR_REMOVESPACE
        LINECOUNT=$(($LINECOUNT+1))
      elif [ "$LINECOUNT" -eq 4 ]; then
        TEMPERATURE_INIT=$TEMPCURR_REMOVESPACE
        LINECOUNT=$(($LINECOUNT+1))
      elif [ "$LINECOUNT" -eq 5 ]; then
        DIGITAL_INIT=$TEMPCURR_REMOVESPACE
        LINECOUNT=$(($LINECOUNT+1))
      elif [ "$LINECOUNT" -eq 6 ]; then
        OUTPUT_INIT=$TEMPCURR_REMOVESPACE
        LINECOUNT=$(($LINECOUNT+1))
      elif [ "$LINECOUNT" -eq 7 ]; then
        ANALOG_INIT=$TEMPCURR_REMOVESPACE
        LINECOUNT=$(($LINECOUNT+1))
      fi
    fi
    HAMEG_CURR=`${HOME_DIR}/common/hameg2030.py $PSU_DEV 5 | awk '{print $1}' | sed 's/A//'`
    HAMEG_VBBCURR=`${HOME_DIR}/common/hameg2030.py $PSU_DEV 5 | awk '{print $3}' | sed 's/A//'`
  done < $TEMP

  if [ -f $TEMP ]; then
    rm $TEMP
  fi
  WRITE_DATA="$TIME;$i;$HAMEG_CURR;$HAMEG_VBBCURR;$TEMPERATURE_PREINIT;$DIGITAL_PREINIT;$OUTPUT_PREINIT;$ANALOG_PREINIT;$TEMPERATURE_INIT;$DIGITAL_INIT;$OUTPUT_INIT;$ANALOG_INIT;"

  if [ -f "$DATA_FILE" ]; then
   echo "$WRITE_DATA" >> $DATA_FILE
  else
    echo "TIME;HAMEGCURRENT5V;BACK_BIASCURR;TEMPERATURE_PREINIT;DIGITAL_PREINIT;OUTPUT_PREINIT;ANALOG_PREINIT;TEMPERATURE_INIT;DIGITAL_INIT;OUTPUT_INIT;ANALOG_INIT;" >> $DATA_FILE
    echo "$WRITE_DATA" >> $DATA_FILE
  fi
done

timestamp=`date +%s`
echo "$timestamp" >> $DATA_DIR/timestamp.dat
