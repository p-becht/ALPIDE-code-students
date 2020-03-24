#!/bin/bash

if [ ! "$#" -eq 2 ]
then
    echo "wrong number of parameter!"
    echo "please provide:"
    echo "1) path to data folder"
    echo "2) comparison selection: 0->noise, 1->threshold"
    exit 1
else
    DATA_DIR=$1
    echo $DATA_DIR
    COMP_SELECT=$2
    echo $COMP_SELECT
fi

HOME_DIR=`pwd`

# create folder structure and write list files
if [ ! -d "$DATA_DIR" ]
then
    "data dir not found! please check!"
    exit 3
fi

if [ $COMP_SELECT -eq 0 ] # noise
then 
    COMP_DIR=$DATA_DIR/comparisons_noise/summary_plots/
elif [ $COMP_SELECT -eq 1 ] || [ $COMP_SELECT -eq 2 ] # thresholds
then 
    COMP_DIR=$DATA_DIR/comparisons_threshold/summary_plots/
fi
if [ ! -d "$COMP_DIR" ]
then
    "comparison dir not found! please check!"
    exit 3
fi


####################################################################################
### summary documents
####################################################################################


### irradiation dependency
cd $HOME_DIR
VS_IRRAD_DIR=$COMP_DIR/vsIrrad/
cp $HOME_DIR/presentation_template/cpcg.sty $VS_IRRAD_DIR

root -b -l <<EOF 
      .L SummaryPresentation.C+g
      create_summary_Irrad("$COMP_DIR", $COMP_SELECT)
EOF
# create pdf documents
cd $VS_IRRAD_DIR 
#for texfile in `ls -1 vsIrrad_Summary.tex`
for texfile in `ls -1 vsIrrad*.tex`
do
    pdflatex $texfile
done
# .. and again
#for texfile in `ls -1 vsIrrad_Summary.tex`
for texfile in `ls -1 vsIrrad*.tex`
do
    pdflatex $texfile
done



