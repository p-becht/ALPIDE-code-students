#!/bin/bash

FILE=$1

root -l <<EOF
gErrorIgnore = kError;
TFile* f = new TFile("${FILE}.root", "RECREATE")
TTree* t = new TTree("NoiseOccupancyDebug", "NoiseOccupancyDebug")
t->ReadFile("${FILE}", "AlmostFull/C:BufferDepth/l:tEventId/l:TimeStamp/l:TrigType/l:StrobeCount/l:ChiBusy/l:DAQbusy/l:ExtTrig/l:EventSize/l:ChipStrobeCount/l:ChipBunchCount/l:timeDistDAQboard/l:timeDistChip/l",'\t')
f->Write()
f->Close()
EOF

cat ${FILE} | cut -f4 | sort -n | uniq -c >> ${FILE}_DAQtimestamps.txt

