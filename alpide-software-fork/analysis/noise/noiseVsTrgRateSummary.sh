#!/bin/bash

if [ ! "$#" -eq 2 ]
then
    echo "wrong number of parameter!"
    echo "please provide:"
    echo "1) path to measurement folder"
    echo "2) number of pixels to exclude for noise occupancy calculation"
    exit 1
else
    MEAS_DIR=$1
    echo $MEAS_DIR
    N_PIX_EXCL=$2
    echo $2
fi

HOME_DIR=`pwd`

# create folder structure and write list files
DATA_DIR=$MEAS_DIR
if [ ! -d "$DATA_DIR" ]
then
    "data dir not found! please check!"
    exit 3
fi
LOG=$DATA_DIR/summary.log


####################################################################################
### comparison plots
####################################################################################
# graphs of noise occupancy vs rate or busy
cd $HOME_DIR
root -b -l <<EOF 
  .L NoiseVsRateComparison.C+g
  create_graphs("$DATA_DIR", $N_PIX_EXCL) 
EOF

# comparison plots
ITHR_FILE=$DATA_DIR/ithr_list.txt
if [ ! -f "$ITHR_FILE" ]
then
    "ithr file not found! please check!"
    exit 3
fi
VBB_FILE=$DATA_DIR/vbb_list.txt
if [ ! -f "$VBB_FILE" ]
then
    "vbb file not found! please check!"
    exit 3
fi

PLOT_DIR=$DATA_DIR/plots
if [ ! -d "$PLOT_DIR" ]
then
    mkdir $PLOT_DIR
fi

for VBB in `cat $VBB_FILE`
do
    VBB_DIR=$DATA_DIR/$(printf "VBB-%0.1f" ${VBB}) 
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
    do
        VCASN_DIR=$VBB_DIR/VCASN$VCASN
        if [ ! -d "$VCASN_DIR" ]
        then
            echo $VCASN
            "vcasn dir not found! please check!"
            exit 3
        fi
        for ITHR in `cat $ITHR_FILE`
        do
            ITHR_DIR=$VCASN_DIR/ITHR$ITHR
            if [ ! -d "$ITHR_DIR" ]
            then
                echo $ITHR
                "ithr dir not found! please check!"
                exit 3
            fi
            for SEC in 0 1 2 3
            do

# Bool_t compare_graphs(TString path_results, TString type, Float_t vbb, Int_t vcasn, Int_t ithr, Int_t sec, Int_t n_excl_pix, Bool_t save_plot) 
root -b -l <<EOF 
  .L NoiseVsRateComparison.C+g
  compare_graphs("$DATA_DIR", "vsRate", $VBB, $VCASN, $ITHR, $SEC, $N_PIX_EXCL, 1) 
  compare_graphs("$DATA_DIR", "vsBusy", $VBB, $VCASN, $ITHR, $SEC, $N_PIX_EXCL, 1) 
EOF

            done # sec
        done # ITHR
    done # VCASN
done # VBB






####################################################################################
### create summary presentation sector by sector
SUM_DIR=$DATA_DIR/comparisons
if [ ! -d "$SUM_DIR" ]
then
    mkdir $SUM_DIR
fi

cp $HOME_DIR/presentation_template/cpcg.sty $SUM_DIR

for SEC in 0 1 2 3
do
    # Bool_t create_summary(TString path_results, Int_t sec, Int_t n_excl_pix) 
    root -b -l <<EOF 
      .L SummaryPresentation.C+g
      create_summary_RateBusy("$DATA_DIR", $SEC, $N_PIX_EXCL)
EOF

done

# create pdf documents
cd $SUM_DIR 
for texfile in `ls -1 *.tex`
do
    pdflatex $texfile
done
# .. and again
for texfile in `ls -1 *.tex`
do
    pdflatex $texfile
done







