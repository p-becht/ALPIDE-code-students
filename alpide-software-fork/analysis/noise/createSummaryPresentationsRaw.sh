#!/bin/bash

if [ ! "$#" -eq 3 ]
then
    echo "wrong number of parameter!"
    echo "please provide:"
    echo "1) path to data folder"
    echo "2) comparison selection: 0->noise, 1->threshold"
    echo "3) flag to create also noise hitmap summary"
    exit 1
else
    DATA_DIR=$1
    echo $DATA_DIR
    COMP_SELECT=$2
    echo $COMP_SELECT
    CREATE_HITMAP=$3
    echo $3
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
    COMP_DIR=$DATA_DIR/comparisons_noise/
elif [ $COMP_SELECT -eq 1 ] # thresholds
then 
    COMP_DIR=$DATA_DIR/comparisons_threshold/
fi
if [ ! -d "$COMP_DIR" ]
then
    "comparison dir not found! please check!"
    exit 3
fi


####################################################################################
### summary documents
####################################################################################

### chip to chip variation
VS_CHIP_DIR=$COMP_DIR/vsChips/
cp $HOME_DIR/presentation_template/cpcg.sty $VS_CHIP_DIR

cd $HOME_DIR

for SEC in 0 1 2 3
do
    root -b -l <<EOF 
      .L SummaryPresentationRaw.C+g
      create_summary_Chips("$COMP_DIR", $SEC, $COMP_SELECT)
EOF
done
# create pdf documents
cd $VS_CHIP_DIR 
for texfile in `ls -1 *.tex`
do
    pdflatex $texfile
done
# .. and again
for texfile in `ls -1 *.tex`
do
    pdflatex $texfile
done


### vbb dependency
cd $HOME_DIR
VS_VBB_DIR=$COMP_DIR/vsVBB/
cp $HOME_DIR/presentation_template/cpcg.sty $VS_VBB_DIR

for SEC in 0 1 2 3
do
    root -b -l <<EOF 
      .L SummaryPresentationRaw.C+g
      create_summary_VBB("$COMP_DIR", $SEC, $COMP_SELECT)
EOF
done
# create pdf documents
cd $VS_VBB_DIR 
for texfile in `ls -1 *.tex`
do
    pdflatex $texfile
done
# .. and again
for texfile in `ls -1 *.tex`
do
    pdflatex $texfile
done


### ithr dependency
cd $HOME_DIR
VS_ITHR_DIR=$COMP_DIR/vsITHR/
cp $HOME_DIR/presentation_template/cpcg.sty $VS_ITHR_DIR

for SEC in 0 1 2 3
do
    root -b -l <<EOF 
      .L SummaryPresentationRaw.C+g
      create_summary_ITHR("$COMP_DIR", $SEC, $COMP_SELECT)
EOF
done
# create pdf documents
cd $VS_ITHR_DIR 
for texfile in `ls -1 *.tex`
do
    pdflatex $texfile
done
# .. and again
for texfile in `ls -1 *.tex`
do
    pdflatex $texfile
done


### temp dependency
cd $HOME_DIR
VS_TEMP_DIR=$COMP_DIR/vsTemp/
cp $HOME_DIR/presentation_template/cpcg.sty $VS_TEMP_DIR

for SEC in 0 1 2 3
do
    root -b -l <<EOF 
      .L SummaryPresentationRaw.C+g
      create_summary_Temp("$COMP_DIR", $SEC, $COMP_SELECT)
EOF
done
# create pdf documents
cd $VS_TEMP_DIR 
for texfile in `ls -1 *.tex`
do
    pdflatex $texfile
done
# .. and again
for texfile in `ls -1 *.tex`
do
    pdflatex $texfile
done


### irradiation dependency
cd $HOME_DIR
VS_IRRAD_DIR=$COMP_DIR/vsIrrad/
cp $HOME_DIR/presentation_template/cpcg.sty $VS_IRRAD_DIR

for SEC in 0 1 2 3
do
    root -b -l <<EOF 
      .L SummaryPresentationRaw.C+g
      create_summary_Irrad("$COMP_DIR", $SEC, $COMP_SELECT)
EOF
done
# create pdf documents
cd $VS_IRRAD_DIR 
for texfile in `ls -1 *.tex`
do
    pdflatex $texfile
done
# .. and again
for texfile in `ls -1 *.tex`
do
    pdflatex $texfile
done



####################################################################################
### hitmap summary
####################################################################################

if [ $CREATE_HITMAP -eq 1 ]
then 
    cd $HOME_DIR
    HITMAP_DIR=$COMP_DIR/hitmap
    cp $HOME_DIR/presentation_template/cpcg.sty $HITMAP_DIR

    root -b -l <<EOF 
      .L SummaryPresentationRaw.C+g
      create_summary_Hitmaps("$DATA_DIR")
EOF

    # create pdf documents
    cd $HITMAP_DIR
    for texfile in `ls -1 *.tex`
    do
        pdflatex $texfile
    done
    # .. and again
    for texfile in `ls -1 *.tex`
    do
        pdflatex $texfile
    done
fi




