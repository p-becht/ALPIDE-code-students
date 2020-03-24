//
//  chiptests.cpp
//  pALPIDEfs-software
//
//  Created by Markus Keil on 24/06/14.
//  Copyright (c) 2014 Markus Keil. All rights reserved.
//

#include "chiptests.h"
#include "TConfig.h"
#include <stdio.h>
#include <cstring>
#include <cmath>
#include <unistd.h>
#include <iostream>
#include <fstream>

int ThresholdData   [512][1024][256];
int ThresholdPoints [256];
const int MAXPOINTS = 256;
char OutputPath     [100];
char Suffix         [100];
int USB_id = 0;

double PrepTime;
double MaskTime;
double PulseTime;
double PulseReadTime;
double DecodeTime;
double TotalTime;
double ReadTime;
double SignalTime;

void ClearThresholdData() {
  for (int icol = 0; icol < 512; icol ++) {
    for (int iaddr = 0; iaddr < 1024; iaddr ++) {
      for (int istep = 0; istep < MAXPOINTS; istep ++) {
        ThresholdData[icol][iaddr][istep] = 0;
      }
    }
  }
}


void ResetClocks() {
  PrepTime   = 0;
  MaskTime   = 0;
  PulseTime  = 0;
  PulseReadTime  = 0;
  DecodeTime = 0;
  TotalTime  = 0;
  ReadTime   = 0;
  SignalTime = 0;
}


void PrintClocks() {
  std::cout << std::endl << "Timing information:" << std::endl;
  std::cout << "Total time:   " << TotalTime << " seconds."  << std::endl;
  std::cout << "Masking:      " << MaskTime << " seconds."  << std::endl;
  std::cout << "PrepInj:      " << PrepTime << " seconds."  << std::endl;
  std::cout << "PulseAndRead: " << PulseReadTime << " seconds."  << std::endl;
  std::cout << "ReadEvent:    " << ReadTime << " seconds."  << std::endl;
  std::cout << "Pulse:        " << PulseTime << " seconds."  << std::endl;
  std::cout << "SendSignal:   " << SignalTime << " seconds."  << std::endl;
  std::cout << "Decoding:     " << DecodeTime << " seconds."  << std::endl << std::endl;
}

void SetDAC (TpAlpidefs *myAlpide, int ADAC, int AValue) {
  if (AValue == -1) {
    TChipConfig *cc = fConfig->GetChipConfig(USB_id);
    int DACValue = cc->GetDACValue(myAlpide->GetDACName(ADAC));
    // use value from config
    //std::cout << "Setting DAC " << ADAC << " to " << DACValue << std::endl;

    myAlpide->SetDAC (ADAC, fConfig->GetChipConfig(USB_id)->GetDACValue(myAlpide->GetDACName(ADAC)));


    //fConfig->GetDACValue (GetDACName(i));
  }
  else {
    // use AValue and write to config
    myAlpide->SetDAC (ADAC, AValue);
    fConfig->GetChipConfig(USB_id)->SetDACValue(myAlpide->GetDACName(ADAC), AValue);
  }
}


bool FifoTest(TpAlpidefs *myAlpide) {
  bool Result = true;
  int  Address;

  std::cout << "Doing FifoTest, Fifo size= " << myAlpide->GetDataFifoSize() << std::endl << std::endl;

  for (int ireg = 0; ireg < 32; ireg ++) {
    std::cout << "Region " << ireg << std::endl;
    for (int iword = 0; iword < myAlpide->GetDataFifoSize(); iword ++) {
      if (myAlpide->GetConfig()->ChipType == DUT_PALPIDEFS3) {
        TpAlpidefs3 *myAlpide3 = (TpAlpidefs3*) myAlpide;
	      Result &= myAlpide3->TestFifo(ireg, iword);
	    }
      else if (myAlpide->GetConfig()->ChipType == DUT_PALPIDEFS4) {
        TpAlpidefs4 *myAlpide4 = (TpAlpidefs4*) myAlpide;
	      Result &= myAlpide4->TestFifo(ireg, iword);
	    }
      else {
        Address = TpAlpidefs::REG_DATAFIFOBASE | (ireg << 11) | (iword);
        Result &= myAlpide->TestRegister(Address);
	    }
    }
  }
  if (Result) std::cout << "FifoTest passed." << std::endl;
  else std::cout << "FifoTest failed." << std::endl;
  return Result;
}


void PrepareMaskStageCluster (TpAlpidefs *myAlpide, TAlpidePulseType APulseType, int AMaskStage, std::vector<TPixHit> ACluster) {
  int FirstRegion = 0;
  int LastRegion  = 31;

  if (AMaskStage >= 1024 * 16) {
    std::cout << "PrepareMaskStage, Warning: Mask stage too big, doing nothing" << std::endl;
    return;
  }

  // Mask and unselect all pixels and disable all columns
  myAlpide->SetChipMode      (MODE_ALPIDE_CONFIG);
  myAlpide->SetMaskAllPixels (true);
  myAlpide->SetMaskAllPixels (false, PR_PULSEENABLE);

  for (int ireg = 0; ireg < 32; ireg ++ ) {
    myAlpide->SetDisableAllColumns(ireg, true);
  }
    
  // now calculate pixel corresponding to mask stage
  // unmask and select this pixel in each region

  int DColOffset    = AMaskStage / 1024;
  int AddressOffset = AMaskStage % 1024;

  for (int ipix = 0; ipix < ACluster.size(); ipix ++) {
    for (int ireg = FirstRegion; ireg <= LastRegion; ireg ++) {
      int DCol    = ACluster.at(ipix).doublecol + DColOffset;
      int Address = ACluster.at(ipix).address   + AddressOffset;
      if ((DCol >= 16) || (Address >= 1024)) continue;
      myAlpide->SetDisableColumn     (ireg, DCol, false);
      myAlpide->SetMaskSinglePixel   (ireg, DCol, Address, false);
      myAlpide->SetInjectSinglePixel (ireg, DCol, Address, true, APulseType, false);
    }
  }

}


void PrepareMaskStageFast(TpAlpidefs *myAlpide, TAlpidePulseType APulseType, int AMaskStage, int nPixels) {
}


// Will unmask and select one pixel per region.
// The pixels are calculated from the number of the mask stage

void PrepareMaskStage(TpAlpidefs *myAlpide, TAlpidePulseType APulseType, int AMaskStage, int nPixels) {
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  int FirstRegion = 0;
  int LastRegion  = 31;

  if (AMaskStage >= 512 * 32 / nPixels) {
    std::cout << "PrepareMaskStage, Warning: Mask stage too big, doing nothing" << std::endl;
    return;
  }

  // Mask and unselect all pixels and disable all columns, except the ones in ARegion
  myAlpide->SetChipMode      (MODE_ALPIDE_CONFIG);
  myAlpide->SetMaskAllPixels (true);
  myAlpide->SetMaskAllPixels (false, PR_PULSEENABLE);

  for (int ireg = 0; ireg < 32; ireg ++ ) {
    myAlpide->SetDisableAllColumns(ireg, true);
  }
    
  // now calculate pixel corresponding to mask stage
  // unmask and select this pixel in each region
  if (nPixels == 1) { // default, use Felix's algorithm
    int tmp = AMaskStage + 764*(AMaskStage/4)+4*(AMaskStage/256);
    tmp = tmp % (16*1024); // maximum is total number of pixels per region
    int DCol    = tmp / 1024;
    int Address = tmp % 1024;
    for (int ireg = FirstRegion; ireg <= LastRegion; ireg ++) {
      myAlpide->SetDisableColumn    (ireg, DCol, false);
      myAlpide->SetMaskSinglePixel  (ireg, DCol, Address, false);
      myAlpide->SetInjectSinglePixel(ireg, DCol, Address, true, APulseType, false);
    }
  }
  else { // old algorithm
    for (int ireg = FirstRegion; ireg <= LastRegion; ireg ++) {
      for (int pixel = 0; pixel < nPixels; pixel++) {
        int tmp = AMaskStage * nPixels + pixel;
        int DCol    = tmp / 1024;
        int Address = tmp % 1024;

        myAlpide->SetDisableColumn     (ireg, DCol, false);
        myAlpide->SetMaskSinglePixel   (ireg, DCol, Address, false);
        myAlpide->SetInjectSinglePixel (ireg, DCol, Address, true, APulseType, false);
      }
    }
  }
        
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  duration<double> TimeSpan = duration_cast<duration<double>>(t2 - t1);
  MaskTime += TimeSpan.count();
}


void DumpHitDataToFile (const char *fName, std::vector<TPixHit> hits, int inner, int outer, bool Recreate) {
  FILE *fp;
  if (Recreate) fp = fopen(fName, "w");
  else          fp = fopen(fName, "a");

  fprintf (fp,"%d %d %d %d %d\n", outer, inner, -1, -1, -1); // Add header line to have record also of points with 0 hits
  for (int ihit = 0; ihit < hits.size(); ihit++) {
    fprintf(fp, "%d %d %d %d %d\n", outer,
            inner,
            hits.at(ihit).region,
            hits.at(ihit).doublecol,
            hits.at(ihit).address);
  }
  fclose(fp);
}


void WriteDataToFile (const char *fName, int ASteps, bool Recreate) {
  FILE *fp;
  bool  HasData;
  if (Recreate) fp = fopen(fName, "w");
  else          fp = fopen(fName, "a");

  for (int icol = 0; icol < 512; icol ++) {
    for (int iaddr = 0; iaddr < 1024; iaddr ++) {
      HasData = false;     // check if pixel has seen hits at all, otherwise don't write it
      for (int istep = 0; istep < ASteps; istep ++) {
        if (ThresholdData[icol][iaddr][istep] > 0) HasData = true;
      }
      if (HasData) {
        for (int istep = 0; istep < ASteps; istep ++) {
          if (ASteps > 1) {
            fprintf(fp, "%d %d %d %d\n", icol, iaddr, ThresholdPoints[istep], ThresholdData[icol][iaddr][istep]);
          }
          else {
            fprintf(fp, "%d %d %d\n", icol, iaddr, ThresholdData[icol][iaddr][istep]);
          }

        }
      }
    }
  }
  fclose (fp);
}


void DigitalInject (TTestSetup *ts, TpAlpidefs *myAlpide, int NEvts) {
  std::vector <TPixHit> Hits;
  bool	     PacketBased = false; // False  Disabled    True Enabled
  if (myAlpide->GetChipType() == DUT_PALPIDEFS2) {
    TpAlpidefs2 *myAlpide2 = (TpAlpidefs2*) myAlpide;
    if ((myAlpide2->GetChipId()== 16) && (myAlpide2->GetPreviousChipId() == 17)) {
      TDAQBoard2* myDAQBoard2 = (TDAQBoard2*)ts->GetDAQBoard(USB_id);
      TestEvent(myDAQBoard2);
      myDAQBoard2->SetEmulationMode(0x2);
      myAlpide2  ->SetChipMasterFPGASlave(true);
      std::cout << "Chip in master mode" << std::endl;
    }
    else if ((myAlpide2->GetChipId() == 17) && (myAlpide2->GetPreviousChipId() == 16)) {
      TDAQBoard2* myDAQBoard2 = (TDAQBoard2*)ts->GetDAQBoard(USB_id);
      myDAQBoard2->SetEmulationMode(0x1);
      std::cout << "Chip in slave mode" << std::endl;
    }
  }
  //    ts->PrepareDigitalInjection (USB_id);
  ts->PrepareDigitalNew (USB_id, NEvts, PacketBased);
  Hits.clear();

    
    /*
     // Caterina 
     // Debug for Current Profile - modification for ALPIDE3
     // Perifery Control Register
     myAlpide-> WriteRegister (0x1, 0x21); // Matrix 20 MHz; Readout Triggered Mode; Clusterin disabled
     //myAlpide-> WriteRegister (0x1, 0x25); // Matrix 20 MHz; Readout Triggered Mode; Clusterin enabled
     
     
     
     // MEB slices scan -- FROMU Configuration Register // Caterina
     myAlpide-> WriteRegister (0x4, 0x0); // enable Slice 1 - 2 - 3 ---> yes here we see the bias - modulo3
     //myAlpide-> WriteRegister (0x4, 0x6); //  enable Slice 1
     //myAlpide-> WriteRegister (0x4, 0x4); //  enable Slice 1 - 2
     //myAlpide-> WriteRegister (0x4, 0x2); //  enable Slice 1 - 3 ---> yes here we see the bias - modulo2
     //myAlpide-> WriteRegister (0x4, 0x5); //  enable Slice 2
     //myAlpide-> WriteRegister (0x4, 0x1); //  enable Slice 2 - 3 ---> yes here we see the bias - modulo2
     //myAlpide-> WriteRegister (0x4, 0x3); //  enable Slice 3 ---> yes here we see the bias (stable)
     
     
     // Disable regions
     //myAlpide-> WriteRegister (0x2, 0x1); // Disable region 0 // added Caterina
     //myAlpide-> WriteRegister (0x2, 0x2); // Disable region 1 // added Caterina
     
     */
              
  ts->PulseAndReadEvent (USB_id, fConfigGeneral->PulselengthDigital, &Hits,NEvts); 
  /*for (int ievt = 0; ievt < NEvts; ievt++) {
    //myAlpide->Pulse                 (fConfigGeneral->PulselengthDigital);
    //  ts      ->TriggerAndReadEvent (USB_id, &Hits);
    ts->PulseAndReadEvent (USB_id, fConfigGeneral->PulselengthDigital, &Hits, 1);
  }*/

  for (int ihit = 0; ihit < Hits.size(); ihit ++) {
    ThresholdData[Hits.at(ihit).doublecol + Hits.at(ihit).region * 16][Hits.at(ihit).address][0] ++;
  }
  // Read End Trigger Word
  if (ts->GetDAQBoard(0)->GetFirmwareDate() >= 0x7E0912 && PacketBased == true){
      unsigned char  data_buf[1024*100];
      int            Length;
      ts->GetDAQBoard(0)->ReadChipEvent(data_buf, &Length, sizeof(data_buf));
      }
}


void AnalogueInject(TTestSetup *ts, TpAlpidefs *myAlpide, int ACharge, int NEvts) {
  std::vector <TPixHit> Hits;
  const int             PulseMode = 2;

  ts->PrepareAnalogueInjection (USB_id, ACharge, PulseMode, NEvts);

  Hits.clear();

  ts->PulseAndReadEvent(USB_id, fConfigGeneral->PulselengthAnalogue, &Hits, NEvts);

  for (int ihit = 0; ihit < Hits.size(); ihit ++) {
    ThresholdData[Hits.at(ihit).doublecol + Hits.at(ihit).region * 16][Hits.at(ihit).address][0] ++;
  }
  ts->FinaliseAnalogueInjection (USB_id, PulseMode);
}


bool ReadSelfTriggered (TTestSetup           *ts,
                        TpAlpidefs           *myAlpide,
                        int                   AStrobeLength,
                        int                   AStrobeBLength,
                        int                   AReadoutDelay,
                        int                   NEvts,
                        int                  *NHits,
                        std::vector<TPixHit> *Hits,
                        TAlpideMode           AReadoutMode,
                        bool                  verbose,
                        bool                  WriteHitsToFile) {

  char  fName[100];
  FILE *fp = 0;
  sprintf(fName, "%s/RawHits_%s.dat", OutputPath, Suffix);
  bool PacketBased = false;
  int BusyDuration =0; 
  int TriggerMode =1; // internal 
  int NumTrigger = -1; 
  int Delay =0x4;
  int MaxTrig=0xfffd; // Register limit 
  
  *NHits = 0;
  // Trigger and read NEvents evts
  if (ts->GetDAQBoard(0)->GetFirmwareDate() <  0x7E0912){
      ts->PrepareEmptyReadout  (USB_id, AStrobeLength, PacketBased);
      }
  else {
     ts->GetDAQBoard(0)->ConfigureReadout (1, true, PacketBased);
     if (NEvts <= MaxTrig) NumTrigger=NEvts;
     else {NumTrigger = MaxTrig;}
     ts->GetDAQBoard(0)->ConfigureTrigger (BusyDuration, AStrobeLength, TriggerMode, NumTrigger, Delay);
     ts->GetDAQBoard(0)->WriteRegister(0x505, 0); // Pulse delay register
     }    
  myAlpide->PrepareReadout       (AStrobeBLength, AReadoutDelay, AReadoutMode);

  if (WriteHitsToFile) {
    fp = fopen (fName, "a");
  }
  if (ts->GetDAQBoard(0)->GetFirmwareDate() >=  0x7E0912) ts->GetDAQBoard(0)->StartTrigger();
  int count_evt =0;
  for (int ievt = 0; ievt < NEvts; ievt ++) {
    int NLastEvents = Hits->size();
    if (verbose && (NEvts >= 100) && (!(ievt % (NEvts / 100)))) {
      std::cout << "ReadSelfTriggered: Triggering event " << ievt << " / " << NEvts << std::endl;
    }
    // Set number of trigger
    if (ts->GetDAQBoard(0)->GetFirmwareDate() >=  0x7E0912){
        if (count_evt == MaxTrig){
            count_evt=0; 
	    NumTrigger = NEvts - ievt;
	    if (NumTrigger > MaxTrig) NumTrigger=MaxTrig;
	    ts->GetDAQBoard(0)->ConfigureTrigger (BusyDuration, AStrobeLength, TriggerMode, NumTrigger, Delay);
	    ts->GetDAQBoard(0)->StartTrigger();
	    }
	} 
    if (! ts->TriggerAndReadEvent(USB_id, Hits)) return false;
    if (WriteHitsToFile) {
      for (int i = NLastEvents; i < Hits->size(); i++ ) {
        int Column = myAlpide->AddressToColumn (Hits->at(i).region, Hits->at(i).doublecol, Hits->at(i).address);
        int Row    = myAlpide->AddressToRow    (Hits->at(i).region, Hits->at(i).doublecol, Hits->at(i).address);
        if (fp) fprintf(fp, "%d %d %d\n", ievt, Column, Row);
      }
    }

    if (!(ievt %1000)) {
      *NHits += (int)Hits->size();
      for (int ihit = 0; ihit < Hits->size(); ihit ++) {
        ThresholdData[Hits->at(ihit).doublecol + Hits->at(ihit).region * 16][Hits->at(ihit).address][0] ++;
      }
      Hits->clear();
    }
   if (ts->GetDAQBoard(0)->GetFirmwareDate() >=  0x7E0912) count_evt++; 
  }

  if (WriteHitsToFile) fclose(fp);
  if (ts->GetDAQBoard(0)->GetFirmwareDate() >=  0x7E0912) ts->GetDAQBoard(0)->StopTrigger();
  if (ts->GetDAQBoard(0)->GetFirmwareDate() >= 0x7E0912 && PacketBased == true){
      unsigned char  data_buf[1024*100];
      int            Length;
      ts->GetDAQBoard(0)->ReadChipEvent(data_buf, &Length, sizeof(data_buf));
      }
  return true;
}


void DigitalScan(TTestSetup *ts, TpAlpidefs *myAlpide, int ANEvents, int NMaskStages, int nPixels)
{
  char fName[100];

  ClearThresholdData();

  for (int istage = 0; istage < NMaskStages; istage ++) {
    if (!(istage %100)) std::cout << "DigitalScan: Mask stage " << istage << std::endl;
    PrepareMaskStage(myAlpide, PT_DIGITAL, istage, nPixels);
    DigitalInject   (ts, myAlpide, ANEvents);
  }
  //myAlpide->SetMaskAllPixels (false);
  if (ts->GetDAQBoard(0)->GetFirmwareDate() >= 0x7E0912){
      ts->GetDAQBoard(0)->StopTrigger();
      }
  sprintf(fName, "%s/DigitalScan_%s.dat", OutputPath, Suffix);
  WriteDataToFile (fName, 1, true);

}

void SEUMemory(TTestSetup *ts, TpAlpidefs *myAlpide,int pattern,int n,float dt)
{
  char fName[100];
  if (myAlpide->GetConfig()->ChipType != DUT_PALPIDEFS3) return;
  TpAlpidefs3 *myAlpide3 = (TpAlpidefs3*) myAlpide;
  // start with writing supplied pattern to all addresses
  std::cerr<<"Writing initial pattern to memories: region";
  for (int ireg = 0; ireg < 32; ireg ++) {
    std::cerr<<" "<<ireg;
    for (int iword = 0; iword < myAlpide->GetDataFifoSize(); iword ++) {
      int AddressLow  = 0x100 | (ireg << 11) | (iword);
      int AddressHigh = 0x200 | (ireg << 11) | (iword);
      int word=pattern>=0?pattern:(iword<<8|ireg);
      myAlpide3->WriteRegister(AddressLow,   word        & 0xffff);
      myAlpide3->WriteRegister(AddressHigh, (word >> 16) & 0xff);
    }
  }
  std::cerr<<std::endl;

  std::cerr<<"Reading back:"<<std::endl;
  for (int i = 0; i < n; i++) {
    time_t       t = time(0);   // get time now
    struct tm *now = localtime( & t );
    char Suffix2[100];
    sprintf(Suffix2, "%02d%02d%02d_%02d%02d%02d", now->tm_year - 100, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
    sprintf(fName, "%s/SEUMemory_%06X_%s_%d_%s.dat", OutputPath, pattern, Suffix,i+1,Suffix2);
    std::ofstream f(fName);
    std::cerr << "Event " << (i+1) <<": reading region";
    for (int ireg = 0; ireg < 32; ireg ++) {
      std::cerr<<" "<<ireg;
      for (int iword = 0; iword < myAlpide->GetDataFifoSize(); iword ++) {
        int AddressLow  = 0x100 | (ireg << 11) | (iword);
        int AddressHigh = 0x200 | (ireg << 11) | (iword);
        int ReadValue, ReadLow, ReadHigh;
        myAlpide3->ReadRegister(AddressLow,  &ReadLow);
        myAlpide3->ReadRegister(AddressHigh, &ReadHigh);
        ReadValue = ReadLow | (ReadHigh << 16);
        f<<ireg<<"\t"<<iword<<"\t"<<ReadValue<<std::endl;
      }
    }
    std::cerr<<std::endl;
    std::cerr<<"Writing results to: "<<fName<<std::endl;
    f.close();
    std::cerr<<"Sleeping for "<<dt<<" seconds..."<<std::endl;
    usleep(1e6*dt);
  }
}


void SEUPixelDigital(TTestSetup *ts, TpAlpidefs *myAlpide,int mask,int n,float dt)
{
  char fName[100];
  // Start with (un)masking full chip
  myAlpide->SetChipMode      (MODE_ALPIDE_CONFIG);
  myAlpide->SetMaskAllPixels (mask==1);

  for (int i = 0; i < n; i++) {
    time_t       t = time(0);   // get time now
    struct tm *now = localtime( & t );
    char Suffix2[100];

    sprintf(Suffix2, "%02d%02d%02d_%02d%02d%02d", now->tm_year - 100, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
    sprintf(fName, "%s/SEUPixelDigital_%d_%s_%d_%s.dat", OutputPath, mask,Suffix,i+1,Suffix2);
    std::cerr << "Event " << (i+1) <<": "<<std::endl;
    // enable pulsing of full matrix
    myAlpide->SetChipMode      (MODE_ALPIDE_CONFIG);
    myAlpide->SetMaskAllPixels (true, PR_PULSEENABLE);
    // clear spurious hits in pixel memories
    ClearThresholdData();
    // disable all columns
    for (int ireg = 0; ireg < 32; ireg++)
      myAlpide->SetDisableAllColumns(ireg, true);
    // read all columns of a region at a time
    std::cerr<<"  - region";
    for (int ireg = 0; ireg < 32; ireg++) {
      std::cerr<<" "<<ireg;
      myAlpide->SetDisableAllColumns(ireg, false);
      myAlpide->PixelReset();
      DigitalInject   (ts, myAlpide,1);
      myAlpide->SetDisableAllColumns(ireg, true);
//            std::cerr<<"  - region "<<ireg<<", col: ";
//            for (int icol = 0; icol < 8; icol++) {
//                std::cerr<<" "<<(ireg*16+icol);
//                myAlpide->SetDisableColumn(ireg, icol, false);
//                DigitalInject   (ts, myAlpide,1);
//                myAlpide->SetDisableColumn(ireg, icol, true );
//            }
//            std::cerr<<std::endl;
    }
    std::cerr<<std::endl;
    std::cerr<<"Writing results to: "<<fName<<std::endl;
    WriteDataToFile (fName, 1, true);
    std::cerr<<"Sleeping for "<<dt<<" seconds..."<<std::endl;
    usleep(1e6*dt);
  }
}

void SEUPixelAnalogue(TTestSetup *ts, TpAlpidefs *myAlpide,int mask,int n,int m)
{
  char fName[100];
  int StrobeBLength = fConfigGeneral->StrobeBlengthSource;

  myAlpide->SetChipMode      (MODE_ALPIDE_CONFIG);
  myAlpide->SetMaskAllPixels (mask==1);
  char tmp[100];
  strcpy(tmp,Suffix);
  for (int i = 0; i < n; i++) {
    time_t       t = time(0);   // get time now
    struct tm *now = localtime( & t );
    char Suffix2[100];
    sprintf(Suffix,"%s_%d",tmp,i+1);
    std::cerr<<"Run "<<(i+1)<<"..."<<std::endl;
    sprintf(Suffix2, "%02d%02d%02d_%02d%02d%02d", now->tm_year - 100, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
    sprintf(fName, "%s/SEUPixelAnalogue_%d_%s_%s.dat", OutputPath, mask,Suffix,Suffix2);

    OccupancyMapSelfTriggered(ts, myAlpide, ts->GetDAQBoard(USB_id)->GetConfig()->StrobeLength, StrobeBLength, myAlpide->GetConfig()->ReadoutDelay, m, MODE_ALPIDE_READOUT_B, true, fName, true);

  }
}

void ReadClusterFile (const char *fName, std::vector<TPixHit> &ACluster)
{
  int col,  row;
  int dcol, address;

  ACluster.clear();
  FILE *fp = fopen(fName, "r");

  while (fscanf (fp, "%d %d", &col, &row) ==2) {
    //address = TpAlpidefs::ColRowToAddress (col, row);
    // dcol    = TpAlpidefs::ColToDoubleCol  (col);
    TPixHit Pixel;
    Pixel.region    = 0;
    Pixel.doublecol = dcol;
    Pixel.address   = address;
    ACluster.push_back(Pixel);
  }
  std::cout << "Read cluster with " << ACluster.size() << " pixels" << std::endl;
}


bool CheckHitPattern (std::vector<TPixHit> &ACluster, int AMaskStage, int ANEvents) {
  int NErrors       = 0;
  int DColOffset    = AMaskStage / 1024;
  int AddressOffset = AMaskStage % 1024;

  // Do this in two steps to not only catch missing hits, but also extra hits:
  //   1) subtract the number of expected hits from the ThresholdData array
  //   2) check that the array is 0 everywhere, otherwise means missing (< 0) or extra (> 0) hits
  for (int ipix = 0; ipix < ACluster.size(); ipix ++) {
    for (int ireg = 0; ireg < 32; ireg ++) {
      int DCol    = ACluster.at(ipix).doublecol + DColOffset;
      int Address = ACluster.at(ipix).address   + AddressOffset;
      if ((DCol >= 16) || (Address >= 1024)) continue;
      ThresholdData [DCol + 16 *ireg][Address][0] -= ANEvents;
    }
  }

  for (int icol = 0; icol < 512; icol ++) {
    for (int iadd = 0; iadd < 1024; iadd ++) {
      if (ThresholdData[icol][iadd][0] < 0) {
        std::cout << "Mask stage " << AMaskStage << ": " << (-1) * ThresholdData[icol][iadd][0] << " missing hits in pixel " << icol << "/" << iadd << std::endl;
        NErrors ++;
      }
      else if (ThresholdData[icol][iadd][0] > 0) {
        std::cout << "Mask stage " << AMaskStage << ": " << ThresholdData[icol][iadd][0] << " extra hits in pixel " << icol << "/" << iadd << std::endl;
        NErrors ++;
      }
    }
  }
  std::cout << "Mask stage " << AMaskStage << ": Found " << NErrors << " errors" << std::endl;
  return (NErrors == 0);
}


void DigitalScanCluster(TTestSetup *ts, TpAlpidefs *myAlpide, int ANEvents, int NMaskStages, const char *fNameCluster)
{
  std::vector<TPixHit> Cluster;
  ReadClusterFile(fNameCluster, Cluster);
  for (int istage = 0; istage < NMaskStages; istage ++) {
    ClearThresholdData      ();
    PrepareMaskStageCluster (myAlpide, PT_DIGITAL, istage, Cluster);
    DigitalInject           (ts, myAlpide, ANEvents);
    CheckHitPattern         (Cluster, istage, ANEvents);
  }

}


void AnalogueScan(TTestSetup *ts, TpAlpidefs *myAlpide, int ACharge, int ANEvents, int NMaskStages, int nPixels)
{
  char fName[100];

  ClearThresholdData();

  for (int istage = 0; istage < NMaskStages; istage ++) {
    if (!(istage %1)) std::cout << "AnalogueScan: Mask stage " << istage << std::endl;
    PrepareMaskStage(myAlpide, PT_ANALOGUE, istage, nPixels);
    AnalogueInject  (ts, myAlpide, ACharge, ANEvents);
  }
  sprintf(fName, "%s/AnalogueScan_%s.dat", OutputPath, Suffix);
  WriteDataToFile (fName, 1, true);
}


bool ThresholdScan(TTestSetup *ts, TpAlpidefs *myAlpide, int NEvts, int AStart, int AStop, int AStep, bool Append) {
  int PulseMode = 2;
  std::vector <TPixHit> Hits;
  bool PacketBased = false;
  ts->PrepareAnalogueInjection(USB_id, AStart, PulseMode, NEvts, PacketBased);
  int ipoint = 0;
  for (int icharge = AStart; icharge < AStop; icharge += AStep) {
    Hits.clear();
    myAlpide->SetDAC("VPULSEL", 170-icharge);

    if (! ts->PulseAndReadEvent(USB_id, fConfigGeneral->PulselengthAnalogue, &Hits, NEvts)) return false;
    for (int ihit = 0; ihit < Hits.size(); ihit ++) {
      ThresholdData[Hits.at(ihit).doublecol + Hits.at(ihit).region * 16][Hits.at(ihit).address][ipoint] ++;
    }
    ThresholdPoints[ipoint] = icharge;
    ipoint ++;
  }
   if (ts->GetDAQBoard(0)->GetFirmwareDate() < 0x7E0912) ts->FinaliseAnalogueInjection(USB_id, PulseMode);
   if (ts->GetDAQBoard(0)->GetFirmwareDate() >= 0x7E0912 && PacketBased == true){
      unsigned char  data_buf[1024*100];
      int            Length;
      ts->GetDAQBoard(0)->ReadChipEvent(data_buf, &Length, sizeof(data_buf));
      }

  return true;
}


bool DACParameterScanStep (TTestSetup *ts, TpAlpidefs *myAlpide, const char *DAC, int ACharge, int NEvts, int AStart, int AStop, int AStep, bool Append) {
  int PulseMode = 2;
  std::vector <TPixHit> Hits;

  ts->PrepareAnalogueInjection(USB_id, ACharge, PulseMode, NEvts);
  int ipoint = 0;
  for (int ivalue = AStart; ivalue < AStop; ivalue += AStep) {
    Hits.clear();
    myAlpide->SetDAC(DAC, ivalue);

    if (! ts->PulseAndReadEvent(USB_id, fConfigGeneral->PulselengthAnalogue, &Hits, NEvts)) return false;
    for (int ihit = 0; ihit < Hits.size(); ihit ++) {
      ThresholdData[Hits.at(ihit).doublecol + Hits.at(ihit).region * 16][Hits.at(ihit).address][ipoint] ++;
    }
    ThresholdPoints[ipoint] = ivalue;
    ipoint ++;
  }

  ts->FinaliseAnalogueInjection(USB_id, PulseMode);

  return true;

}


void ThresholdScanFast (TTestSetup *ts, TpAlpidefs *myAlpide, int NMaskStages, int Start, int Stop, int VCASN, int ITHR)
{
  char fName[100];
  sprintf(fName, "%s/ThresholdScanFast_%s.dat", OutputPath, Suffix);

  int  Step    = 2;    // Default 1 (VCAL step size)
  int  NInj    = 20;   // Default 50 injections per point
  int  NPixels = 8;    // Default 1 (32 pixels per mask stage)
  bool ReadFailure = false;

  ClearThresholdData();
  ResetClocks       ();
  SetDAC (myAlpide, myAlpide->FindDAC("VCASN"), VCASN);
  SetDAC (myAlpide, myAlpide->FindDAC("ITHR"),  ITHR);
  high_resolution_clock::time_point t1 = high_resolution_clock::now();

  for (int istage =0; (istage < NMaskStages) && (!ReadFailure); istage ++) {
    if (!(istage %10))
      std::cout << "Threshold scan: mask stage " << istage << std::endl;
    PrepareMaskStage (myAlpide, PT_ANALOGUE, istage, NPixels);
    if (! ThresholdScan (ts, myAlpide, NInj, Start, Stop, Step, true)) ReadFailure = true;
    if (!(istage %1000))
      WriteDataToFile(fName, Stop - Start, true);
  }

  myAlpide->SetMaskAllPixels (false);

  WriteDataToFile(fName, Stop - Start, true);
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  duration<double> TimeSpan = duration_cast<duration<double>>(t2 - t1);
  TotalTime = TimeSpan.count();
  PrintClocks();
}


void ThresholdScanFull (TTestSetup *ts, TpAlpidefs *myAlpide, int NMaskStages, int Start, int Stop, int VCASN, int ITHR, int IDB, int VCASN2, int VCLIP) {
  char fName[100];
  sprintf(fName, "%s/ThresholdScan_%s.dat", OutputPath, Suffix);

  int  Step = 1;
  bool ReadFailure = false;

  ClearThresholdData();
  ResetClocks       ();
  SetDAC (myAlpide, myAlpide->FindDAC("VCASN2"), VCASN2);
  SetDAC (myAlpide, myAlpide->FindDAC("VCLIP" ), VCLIP );
  SetDAC (myAlpide, myAlpide->FindDAC("VCASN"), VCASN);
  SetDAC (myAlpide, myAlpide->FindDAC("ITHR"),  ITHR);
  SetDAC (myAlpide, myAlpide->FindDAC("IDB"),   IDB);

  high_resolution_clock::time_point t1 = high_resolution_clock::now();

  for (int istage =0; (istage < NMaskStages) && (!ReadFailure); istage ++) {
    if (!(istage %10))
      std::cout << "Threshold scan: mask stage " << istage << std::endl;
    PrepareMaskStage (myAlpide, PT_ANALOGUE, istage);
    if (! ThresholdScan (ts, myAlpide, 50, Start, Stop, Step, true)) ReadFailure = true;
    if (ReadFailure) std::cerr<<"warning: read failure"<<std::endl;
    if (!(istage %1000))
      WriteDataToFile(fName, Stop - Start, true);
  }

  //myAlpide->SetMaskAllPixels (false);
  if (ts->GetDAQBoard(0)->GetFirmwareDate() >= 0x7E0912){
      ts->GetDAQBoard(0)->StopTrigger();
      }
  WriteDataToFile(fName, Stop - Start, true);

  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  duration<double> TimeSpan = duration_cast<duration<double>>(t2 - t1);
  TotalTime = TimeSpan.count();
  PrintClocks();
}


void DACParameterScan (TTestSetup *ts, TpAlpidefs *myAlpide, int NMaskStages, int ACharge, const char *DAC, int AStart, int AStop, int AStep) {
  char fName[100];
  sprintf(fName, "%s/%sScan_%s.dat", OutputPath, DAC, Suffix);

  ClearThresholdData();

  bool ReadFailure = false;

  for (int istage =0; (istage < NMaskStages) && (!ReadFailure); istage ++) {
    if (!(istage %10))
	    std::cout << DAC << " scan: mask stage " << istage << std::endl;
    PrepareMaskStage (myAlpide, PT_ANALOGUE, istage);
    if (! DACParameterScanStep (ts, myAlpide, DAC, ACharge, 50, AStart, AStop, AStep, true)) ReadFailure = true;
    if (ReadFailure) std::cerr<<"warning: read failure"<<std::endl;
    if (!(istage %1000))
      WriteDataToFile(fName, (AStop - AStart)/AStep, true);
  }
}


void PulselengthScan (TTestSetup *ts, TpAlpidefs *myAlpide, int NEvts, int ADCol, int AAddress, int AStart, int AStop, int AStep, int VCASN, int ITHR, int IDB) {
  char fName[100];
  std::vector <TPixHit> Hits;
  int ChargeStart     = 1 ;
  int ChargeStop      = 160;
  int ChargeStep      = 2;
  int Pulselength     = 65535;
  //int Pulselength     = 256*8 - 1;
  const int PulseMode = 2;
  uint32_t  NewDate2  = 0x7DEB1C;
  uint32_t  NewDate   = 0x7DEB05;  // Date when register format was changed in the firmware


  if (ts->GetDAQBoard(0)->GetFirmwareDate() < NewDate) {
    std::cout << "Firmware too old for large pulselength (maximum = 255), exiting. " << std::endl;
    return;
  }

  if (ts->GetDAQBoard(0)->GetFirmwareDate() < NewDate2) {
    std::cout << "Firmware too old for large pulselength, setting to maximum (2047). " << std::endl;
    Pulselength = 2047;
  }

  sprintf (fName, "%s/PulselengthScan_%s.dat", OutputPath, Suffix);

  std::cout << "Doing pulselength scan." << std::endl;

  SetDAC (myAlpide, myAlpide->FindDAC("VCASN"), VCASN);
  SetDAC (myAlpide, myAlpide->FindDAC("ITHR"),  ITHR);
  SetDAC (myAlpide, myAlpide->FindDAC("IDB"),   IDB);

  myAlpide->SetMaskAllPixels(true);

  for (int ireg = 0; ireg < 32; ireg ++) {
    myAlpide->SetMaskSinglePixel   (ireg, ADCol, AAddress, false);
    myAlpide->SetInjectSinglePixel (ireg, ADCol, AAddress, true, PT_ANALOGUE, false);
  }

  for (int Charge = ChargeStart; Charge < ChargeStop; Charge += ChargeStep) {
    std::cout << "Charge = " << Charge << std::endl;

    ts->PrepareAnalogueInjection (USB_id, Charge, PulseMode);

    for (int idelay = AStart; idelay < AStop; idelay += AStep) {
      Hits.clear();
      ts->GetDAQBoard(0)->ConfigurePulse(idelay, Pulselength, PulseMode);
      for (int ievt = 0; ievt < NEvts; ievt++) {
        ts->PulseAndReadEvent(USB_id, Pulselength, &Hits);
      }
      if ((idelay == AStart) && (Charge == ChargeStart)) {
        DumpHitDataToFile(fName, Hits, idelay, Charge, true);
      }
      else {
        DumpHitDataToFile(fName, Hits, idelay, Charge);
      }
    }
  }
}


void ResetScan (TTestSetup *ts, TpAlpidefs *myAlpide, int NEvts, int AStart, int AStop, int AStep, int NMaskStages, int Charge) {
  int                   PulseMode = 2;
  std::vector <TPixHit> Hits;
  bool                  ReadFailure = false;
  char                  fName[100];
  sprintf(fName, "%s/ResetScan_%s.dat", OutputPath, Suffix);

  ClearThresholdData();

  for (int istage =0; (istage < NMaskStages) && (!ReadFailure); istage ++) {
    //if (!(istage %10))
    std::cout << "Reset scan: mask stage " << istage << std::endl;

    PrepareMaskStage (myAlpide, PT_ANALOGUE, istage);

    ts->PrepareAnalogueInjection(USB_id, Charge, PulseMode, NEvts);

    int ipoint = 0;
    for (int ireset = AStart; ireset < AStop; ireset += AStep) {
      Hits.clear();
      myAlpide->SetDAC("VRESET", ireset);
      myAlpide->SetDAC("VAUX",   ireset);
      if (ireset == AStart) sleep(10);
	    if (! ts->PulseAndReadEvent(USB_id, fConfigGeneral->PulselengthAnalogue, &Hits, NEvts)) {
        ReadFailure = true;
        ireset = AStop;
        continue;
      }
      for (int ihit = 0; ihit < Hits.size(); ihit ++) {
        ThresholdData[Hits.at(ihit).doublecol + Hits.at(ihit).region * 16][Hits.at(ihit).address][ipoint] ++;
      }
      ThresholdPoints[ipoint] = ireset;
      ipoint ++;
    }

    ts->FinaliseAnalogueInjection(USB_id, PulseMode);
  }

  myAlpide->SetMaskAllPixels (false);

  WriteDataToFile(fName, AStop - AStart, true);

}


void IResetScan (TTestSetup *ts, TpAlpidefs *myAlpide, int NEvts, int AStart, int AStop, int AStep, int NMaskStages, int Charge, int Vreset) {
  std::cout << "nmaskstages: " << NMaskStages << "   vreset: " << Vreset << "     charge: " << Charge << std::endl;
  int                   PulseMode = 2;
  std::vector <TPixHit> Hits;
  bool                  ReadFailure = false;
  char                  fName[100];
  sprintf(fName, "%s/IResetScan_%s.dat", OutputPath, Suffix);
  // set vrst val

  ClearThresholdData();

  //myAlpide->SetDAC("IRESET", AStart);
  myAlpide->SetDAC("VRESET", Vreset);
  myAlpide->SetDAC("VAUX", Vreset);

  for (int istage =0; (istage < NMaskStages) && (!ReadFailure); istage ++) {
    if (!(istage %10))
      std::cout << "IReset scan: mask stage " << istage << std::endl;

    PrepareMaskStage (myAlpide, PT_ANALOGUE, istage);

    ts->PrepareAnalogueInjection(USB_id, Charge, PulseMode);

    int ipoint = 0;
    for (int ireset = AStart; ireset < AStop; ireset += AStep) {
      std::cout << "irst: " << ireset << std::endl;
      Hits.clear();
      myAlpide->SetDAC("IRESET", ireset);

      //ts->ReadAllChipDacs(USB_id);

      //if (ireset == AStart) sleep(1);
      sleep(1);
      for (int ievt = 0; ievt < NEvts; ievt++) {
        if (! ts->PulseAndReadEvent(USB_id, fConfigGeneral->PulselengthAnalogue, &Hits)) {
          ReadFailure = true;
          ireset = AStop;
          continue;
        }
      }
      for (int ihit = 0; ihit < Hits.size(); ihit ++) {
        ThresholdData[Hits.at(ihit).doublecol + Hits.at(ihit).region * 16][Hits.at(ihit).address][ipoint] ++;
      }
      ThresholdPoints[ipoint] = ireset;
      ipoint ++;
    }

    ts->FinaliseAnalogueInjection(USB_id, PulseMode);
    if (!(istage %10))
      WriteDataToFile(fName, AStop - AStart, true);
  }

  myAlpide->SetMaskAllPixels (false);

  WriteDataToFile(fName, AStop - AStart, true);
}


std::vector<TPixHit> OccupancyMapSelfTriggered (TTestSetup *ts,
                                                TpAlpidefs *myAlpide,
                                                int         AStrobeLength,
                                                int         AStrobeBLength,
                                                int         AReadoutDelay,
                                                int         NEvts,
                                                TAlpideMode AReadoutMode,
                                                bool        recreate,
                                                const char *fName,
                                                bool        WriteSingleHitFile)
{
  int                  NHits;
  std::vector<TPixHit> Hits;

  Hits.clear();

  ClearThresholdData();

  if (!ReadSelfTriggered(ts, myAlpide, AStrobeLength, AStrobeBLength, AReadoutDelay, NEvts, &NHits, &Hits, AReadoutMode, true, WriteSingleHitFile)) {
    std::cout << "OccupancyMap: Warning, ReadNoise returned with read failure, writing nothing" << std::endl;
    return Hits;
  }

  std::cout << "Found " << NHits << " hits." << std::endl;

  WriteDataToFile (fName, 1, recreate);

  return Hits;
}


void PrepareStaticMaskAndSelect (TpAlpidefs *myAlpide, TAlpidePulseType APulseType) {
  int FirstRegion = 0;
  int LastRegion  = 31;
  //int FirstColumn = 10;
  //int LastColumn  = 10;

  //int Row         = 511;

  // Mask all pixels and disable all columns
  myAlpide->SetMaskAllPixels(false);

  //for (int ireg = 0; ireg < 32; ireg ++ ) {
  //    myAlpide->SetDisableAllColumns(ireg, true);
  //}

  for (int ireg = FirstRegion; ireg <= LastRegion; ireg ++) {
    myAlpide->SetDisableAllColumns(ireg, false);
  }

  //   warning: there is somewhere a SetMaskAllPixels (true, PULSE_ENABLE) missing...

  //For selected pixels: enable column, uAAlpide->MonitorDACs(AVmon, DACI_NONE); unmask pixel, enable injection
  //for (int ireg = FirstRegion; ireg <= LastRegion; ireg++) {
  //  for (int icol = FirstColumn; (icol <= LastColumn) || ((ireg < LastRegion) && (icol < 16)); icol +=2) {
  //    myAlpide->SetDisableColumn     (ireg, icol, false);
  //  myAlpide->SetMaskSinglePixel   (ireg, icol, Row, false);
  //myAlpide->SetInjectSinglePixel (ireg, icol, Row, true, APulseType, false);
  //myAlpide->SetMaskSinglePixel (ireg, icol, Row+512, false)
  //SelectPixel                  (myAlpide, ireg, icol, Row+512, APulseType);
  //SelectPixel                  (myAlpide, ireg, icol, Row+256, APulseType);
  //SelectPixel                  (myAlpide, ireg, icol, Row-256, APulseType);
  //SelectPixel                  (myAlpide, ireg, icol, Row-128, APulseType);
  //SelectPixel                  (myAlpide, ireg, icol, Row+128, APulseType);
  //SelectPixel                  (myAlpide, ireg, icol, Row-384, APulseType);
  //SelectPixel                  (myAlpide, ireg, icol, Row+384, APulseType);
  //SelectPixel                  (myAlpide, ireg, icol, Row-64, APulseType);
  //SelectPixel                  (myAlpide, ireg, icol, Row+64, APulseType);
  //SelectPixel                  (myAlpide, ireg, icol, Row-192, APulseType);
  //SelectPixel                  (myAlpide, ireg, icol, Row+192, APulseType);
  //SelectPixel                  (myAlpide, ireg, icol, Row-32, APulseType);
  //SelectPixel                  (myAlpide, ireg, icol, Row+32, APulseType);
  //SelectPixel                  (myAlpide, ireg, icol, Row-48, APulseType);
  //SelectPixel                  (myAlpide, ireg, icol, Row+48, APulseType);
  //SelectPixel                  (myAlpide, ireg, icol, Row-16, APulseType);
  //SelectPixel                  (myAlpide, ireg, icol, Row+16, APulseType);
  //Row = ((Row + 8) & 0x3ff);
  //Row += 8;
  //Row &= 0x3ff;
  // }
  // }
}


int NoiseOccupancy (TTestSetup *ts, TpAlpidefs *myAlpide, int NEvts, TAlpideMode AReadoutMode, bool recreate, int VCASN, int ITH, const char *maskFile) {
  char fName[100];
  int  StrobeBLength;
  int  StrobeLength  = ts->GetDAQBoard(USB_id)->GetConfig()->StrobeLength;
  int  ReadoutDelay  = myAlpide->GetConfig()->ReadoutDelay;
  if (AReadoutMode == MODE_ALPIDE_READOUT_B) {  // StrobeBLength is the length of internal strobe signal
    StrobeBLength = fConfigGeneral->StrobeBlengthStandard;
  }
  else {                                        // StrobeBLength is the delay between end of readout and strobeB reassertion
    StrobeBLength = myAlpide->GetConfig()->StrobeBDelay;
  }

  std::cout << "Setting VCASN to " << VCASN << ", ITH to " << ITH << std::endl;
  SetDAC (myAlpide, myAlpide->FindDAC("VCASN"), VCASN);
  SetDAC (myAlpide, myAlpide->FindDAC("ITHR"),  ITH);
  myAlpide->SetMaskAllPixels(false);

  //std::cout << "WARNING: enabling only sector 1" << std::endl;
  //for (int ireg = 0; ireg < 32; ireg ++) {
  //  if ((ireg > 7) && (ireg < 16)) continue;
  //  for (int idcol = 0; idcol < 16; idcol ++) {
  //    myAlpide->SetDisableColumn (ireg, idcol, true);
  //  }
  // }

  if (strcmp (maskFile,"")) {
    myAlpide->ReadNoisyPixelFile(maskFile);
    std::cout << "Masking noisy pixels" << std::endl;
    myAlpide->MaskNoisyPixels   ();
    std::cout << "Done masking" << std::endl;
  }

  myAlpide->ClearNoisyPixels();

  //Do scan
  sprintf(fName, "%s/NoiseOccupancy_%s.dat", OutputPath, Suffix);
  return (int) OccupancyMapSelfTriggered(ts, myAlpide, StrobeLength, StrobeBLength, ReadoutDelay, NEvts, AReadoutMode, recreate, fName).size();

}


int NoiseOccupancySingle (TTestSetup *ts, TpAlpidefs *myAlpide, int NEvts, TAlpideMode AReadoutMode, bool recreate, int APar1, int APar2, int APar3) {

  char fName[100];
  int  StrobeBLength;
  int  StrobeLength  = ts->GetDAQBoard(USB_id)->GetConfig()->StrobeLength;
  int  ReadoutDelay  = myAlpide->GetConfig()->ReadoutDelay;
  if (AReadoutMode == MODE_ALPIDE_READOUT_B) {  // StrobeBLength is the length of internal strobe signal
    StrobeBLength = fConfigGeneral->StrobeBlengthStandard;
  }
  else {                                        // StrobeBLength is the delay between end of readout and strobeB reassertion
    StrobeBLength = myAlpide->GetConfig()->StrobeBDelay;
  }

  myAlpide->SetMaskAllPixels   (true);
  myAlpide->SetMaskSinglePixel (APar1, APar2, APar3, false);

  //Do scan
  sprintf(fName, "%s/NoiseOccupancy_%s.dat", OutputPath, Suffix);
  return (int) OccupancyMapSelfTriggered(ts, myAlpide, StrobeLength, StrobeBLength, ReadoutDelay, NEvts, AReadoutMode, recreate, fName).size();

}

int NoiseOccupancyScan (TTestSetup *ts, TpAlpidefs *myAlpide, int NEvts, TAlpideMode AReadoutMode, unsigned short vcasn_low, unsigned short vcasn_high, unsigned short ithr_low, unsigned short ithr_high, const char* maskFile) {
  char fName[100];

  if (vcasn_low > 255 || vcasn_high > 255 || ithr_low > 255 || ithr_high > 255) {
    std::cout << "Scan value out of bounds! DAC range is from 0 to 255" << std::endl;
    return 1;
  }

  myAlpide->SetMaskAllPixels(false);

  //for (int ireg = 0; ireg < 15; ireg ++) {
  //  for (int icol = 0; icol < 15; icol ++) {
  //    myAlpide->SetDisableColumn(ireg, icol, true);
  //  }
  //}
  //for (int ireg = 24; ireg < 31; ireg ++) {
  //  for (int icol = 0; icol < 15; icol ++) {
  //    myAlpide->SetDisableColumn(ireg, icol, true);
  //  }
  // }


  //  for (int icol = FirstColumn; (icol <= LastColumn) || ((ireg < LastRegion) && (icol < 16)); icol +=2) {
  //    myAlpide->SetDisableColumn     (ireg, icol, false);


  // mask noisy pixels if there is a mask file
  if (strcmp (maskFile,"")) {
    PrepareStaticMaskAndSelect(myAlpide, PT_ANALOGUE);
    myAlpide->ReadNoisyPixelFile(maskFile);
    myAlpide->MaskNoisyPixels   ();
  }

  sprintf(fName, "%s/NoiseOccupancyScan_%s.dat", OutputPath, Suffix);

  int                  NHits;
  std::vector<TPixHit> Hits;

  FILE *fp = fopen (fName, "w");
  fclose (fp);

  for(int ivcasn=vcasn_low; ivcasn <= vcasn_high; ++ivcasn) {
    myAlpide->SetDAC("VCASN", ivcasn);
    for(int iithr=ithr_high; iithr >= ithr_low; --iithr) {
	    myAlpide->SetDAC("ITHR", iithr);

      NHits=0;
      Hits.clear();

      if (!ReadSelfTriggered(ts, myAlpide, ts->GetDAQBoard(USB_id)->GetConfig()->StrobeLength, fConfigGeneral->StrobeBlengthStandard, myAlpide->GetConfig()->ReadoutDelay, NEvts, &NHits, &Hits, AReadoutMode, false, false)) {
        std::cout << "OccupancyMap: Warning, ReadNoise returned with read failure, writing nothing" << std::endl;
        return 1;
      }

      int nhits_sec[4] = {0, 0, 0, 0};

      for (int idcol = 0; idcol < 512; idcol ++) {
        for (int irow = 0; irow < 1024; irow ++) {
          int sector = idcol / 128;
          nhits_sec[sector] += ThresholdData[idcol][irow][0];
	      }
	    }

	    //            for (unsigned int ihit=0; ihit<Hits.size(); ++ihit) {
      //    if      (Hits.at(ihit).region < 8)  nhits_sec[0]++;
      //    else if (Hits.at(ihit).region < 16) nhits_sec[1]++;
      //    else if (Hits.at(ihit).region < 24) nhits_sec[2]++;
      //    else                                nhits_sec[3]++;
      //}

      printf("VCASN: %i ITHR: %i HITS %i\n", ivcasn, iithr, NHits);
      fp = fopen (fName, "a");
      fprintf(fp, "%i %i %i %i %i %i\n", ivcasn, iithr, nhits_sec[0], nhits_sec[1], nhits_sec[2], nhits_sec[3]);
      fclose(fp);
      if (NHits > 50 * NEvts) {  // to be checked, dirty hack to avoid that the DAQ does crash
        if (ivcasn == vcasn_high) return 0;
        iithr = ithr_high +1;
        ivcasn++;
        myAlpide->SetDAC("VCASN", ivcasn);
      }
    }
  }


  std::cout << "Done!" << std::endl;
  return 0;
}

void SourceScan (TTestSetup *ts, TpAlpidefs *myAlpide, int NEvts, TAlpideMode AReadoutMode, bool recreate, const char *maskFile) {
  char fName[100];
  int  StrobeBLength;

  sprintf(fName, "%s/SourceScan_%s.dat", OutputPath, Suffix);

  if (AReadoutMode == MODE_ALPIDE_READOUT_B) {  // StrobeBLength is the length of internal strobe signal
    StrobeBLength = fConfigGeneral->StrobeBlengthSource;
  }
  else {                                        // StrobeBLength is the delay between end of readout and strobeB reassertion
    StrobeBLength = fConfigGeneral->StrobeBlengthStandard;
  }

  PrepareStaticMaskAndSelect(myAlpide, PT_ANALOGUE);
  if (strcmp (maskFile,"")) {
    myAlpide->ReadNoisyPixelFile(maskFile);
    myAlpide->MaskNoisyPixels   ();
  }

  OccupancyMapSelfTriggered(ts, myAlpide, ts->GetDAQBoard(USB_id)->GetConfig()->StrobeLength, StrobeBLength, myAlpide->GetConfig()->ReadoutDelay, NEvts, AReadoutMode, recreate, fName, true);

}


void PrepareNoiseMask (TTestSetup *ts, TpAlpidefs *myAlpide, int NEvts, const char *AFileName, float NoiseCut, int VCASN, int ITH) {
  // To be checked: should the noise mask be prepared with the source or the noise settings?
  char fName       [100];
  char fNameRawData[100];
  sprintf(fName,        "%s/%s", OutputPath, AFileName);
  sprintf(fNameRawData, "%s/NoisemapData_%s.dat", OutputPath, Suffix);

  SetDAC(myAlpide, myAlpide->FindDAC("VCASN"), VCASN);
  SetDAC(myAlpide, myAlpide->FindDAC("ITHR"),  ITH);

  myAlpide->SetMaskAllPixels(false);

  std::vector<TPixHit> Hits = OccupancyMapSelfTriggered(ts,
                                                        myAlpide,
                                                        ts->GetDAQBoard(USB_id)->GetConfig()->StrobeLength,
                                                        fConfigGeneral->StrobeBlengthSource,
                                                        myAlpide->GetConfig()->ReadoutDelay,
                                                        NEvts,
                                                        MODE_ALPIDE_READOUT_B,
                                                        true,
                                                        fNameRawData);

  myAlpide->ClearNoisyPixels();

  int HitCut;
  if (NoiseCut < 0) {
    HitCut = 1;
  }
  else {
    HitCut = NEvts * NoiseCut;
  }
  for (int idcol = 0; idcol < 512; idcol ++) {
    for (int iaddr = 0; iaddr < 1024; iaddr ++) {
      if (ThresholdData[idcol][iaddr][0] >= HitCut) {
        myAlpide->AddNoisyPixel(idcol / 16, idcol % 16, iaddr);
      }
    }
  }


  //    for (int ihit = 0; ihit < Hits.size(); ihit ++) {
  //    myAlpide->AddNoisyPixel(Hits.at(ihit));
  //}
  myAlpide->WriteNoisyPixelFile(fName);
}


void IDBScan (TTestSetup *ts, TpAlpidefs *myAlpide, int region, int dcol, int address) {
  int NEvts = 10000;
  int IDB   = 64;
  float Occupancy = 0;
  FILE *fp = fopen("/Users/mkeil/Work/FSTestsystem/IDBScan.dat", "w");
  while (/*(Occupancy < .1) && */(IDB > 0)){
    myAlpide->SetDAC("IDB", IDB);
    int NHits = NoiseOccupancySingle (ts, myAlpide, NEvts, MODE_ALPIDE_READOUT_B, true , region, dcol, address);
    Occupancy = (float)NHits;
    Occupancy /= NEvts;
    std::cout << "IDB " << IDB << ", occupancy " << Occupancy << std::endl;
    fprintf(fp, "%d %e\n", IDB, Occupancy);
    IDB -= 1;
  }
  fclose(fp);
}


void RateNoiseTest(TTestSetup *ts, TpAlpidefs *myAlpide, int NEvents, int NPrintOut) {
  char fName[100];
  char fNameDebug[100];
  char fNameDebugRaw[100];

  bool writeDebug = false; // activate this to write the

  // WARNING/TODO: make this routine backward-compatible?

  const int StrobeLength  = ts->GetDAQBoard(USB_id)->GetConfig()->StrobeLength;
  const int StrobeBLength = fConfigGeneral->StrobeBlengthStandard;
  const int ReadoutDelay  = myAlpide->GetConfig()->ReadoutDelay;
  const int TriggerDelay  = ts->GetDAQBoard(USB_id)->GetConfig()->TriggerDelay;
  const bool packetBasedRdo = true;

  // PrepareEmptyReadout
  //ts->GetDAQBoard(0)->ConfigureReadout (1, false, false);       // buffer depth = 1, sampling on rising edge, event based readout
  ts->GetDAQBoard(0)->ConfigureReadout (3, true, packetBasedRdo);       // multi-event buffer depth = 3, sampling on falling edge, packet-based readout
  ts->GetDAQBoard(0)->ConfigureTrigger (0, StrobeLength, 2, 0, TriggerDelay);
  ts->GetDAQBoard(0)->WriteBusyOverrideReg(true);  // enable chip busy at daqboard connector

  // PrepareChipReadout
  myAlpide->SetChipMode(MODE_ALPIDE_CONFIG);
  myAlpide->SetReadoutDelay     (ReadoutDelay);
  myAlpide->SetEnableClustering (false);
  myAlpide->SetStrobeTiming     (StrobeBLength);
  myAlpide->SetEnableOutputDrivers(true, true);
  myAlpide->SetChipMode         (MODE_ALPIDE_READOUT_B);
  myAlpide->SetMaskAllPixels    (false);


  int count = 0;
  int countBad = 0;
  int countCorrupt = 0;
  time_t beg_status = time(0);
  time_t end_status = time(0);
  time_t last_status = time(0);

  // TODO check event ID.
  uint64_t lastEventID  = 0;
  uint64_t lastStrobeID = 0;
  unsigned int lastStrobeCounter = (unsigned int)-1;
  unsigned int lastBunchCounter = (unsigned int)-1;

  sprintf(fName, "%s/NoiseOccupancyScan_%s.dat", OutputPath, Suffix);
  FILE *fp = fopen (fName, "w");
  fclose (fp);


  sprintf(fNameDebug, "%s/NoiseOccupancyScanDebug_%s.dat", OutputPath, Suffix);
  std::ofstream outDebug;
  if (writeDebug) {
    outDebug.open(fNameDebug, std::ofstream::out);
    outDebug << "AlmostFull\tBufferDepth\tEventId\tTimeStamp[12.5ns]\tTrigType\tStrobeCount\tChiBusy\tDAQbusy\tExtTrig\tEventSize\tChipStrobeCount\tChipBunchCount[8:3]\tTimeDiffDAQboard\tTimeDiffChip" << std::endl;
  }

  std::vector<unsigned char> debugRaw;
  sprintf(fNameDebugRaw, "%s/NoiseOccupancyScanDebugRaw_%s.dat", OutputPath, Suffix);
  std::ofstream outDebugRaw;
  if (writeDebug) outDebugRaw.open(fNameDebugRaw, std::ios::out | std::ios::binary);

  // TriggerAndReadEvent
  const int length_buf = 15000; // length needed at ITHR=10 ~5000!!!
  unsigned char  data_buf[length_buf];   // TODO large enough?
  int            Length;

  const int headerLength = ts->GetDAQBoard(0)->GetEventHeaderLength();
  const int trailerLength = ts->GetDAQBoard(0)->GetEventTrailerLength();

  ts->GetDAQBoard(0)->StartTrigger();
  int NHits;
  std::vector<TPixHit> Hits;
  NHits=0;
  Hits.clear();

  ClearThresholdData();

  std::cout << "Configured. Starting data taking..." << std::endl;

  // evt loop
  while (count < NEvents && countBad < NEvents && countCorrupt < NEvents) {
    // get data
    int error = 0;
    unsigned char* debug = 0x0;
    int debug_length = 0;
    int ret_value = ts->GetDAQBoard(0)->ReadChipEvent(data_buf, &Length, length_buf, &error, (writeDebug) ? &debug : 0x0 , (writeDebug) ? &debug_length : 0x0);
    for (int i =0;i< Length; i++){
       printf("%02X ", data_buf[i]);
    }
    if (writeDebug && debug_length>0) {
      std::vector<unsigned char> vec(&debug[0], &debug[debug_length]);
      debugRaw.insert(debugRaw.end(), vec.begin(), vec.end());
    }
    if (Length<headerLength+trailerLength) {
      std::cout << "Read incomplete event of length " << Length << "byte, header: " << headerLength <<"byte, trailer: " << trailerLength << "byte!"  << std::endl;
      ++countCorrupt;
      continue;
    }
    TEventHeader header;
    header.AFull = true;
    header.TrigType = -1;
    header.BufferDepth = -1;
    header.EventId = (uint64_t)-1;
    header.TimeStamp = (uint64_t)-1;
    header.EventSize = -1;
    header.StrobeCountTotal = -1;
    header.TrigCountChipBusy = -1;
    header.TrigCountDAQbusy = -1;
    header.ExtTrigCount = -1;
    unsigned int StrobeCounter = (unsigned int)-1;
    unsigned int BunchCounter  = (unsigned int)-1;
    bool HeaderOK  = ts->GetDAQBoard(0)->DecodeEventHeader(data_buf, &header);
    bool EventOK   = myAlpide->DecodeEvent(data_buf+headerLength, Length-trailerLength-headerLength, &Hits, &StrobeCounter, &BunchCounter);
    bool TrailerOK = ts->GetDAQBoard(0)->DecodeEventTrailer(data_buf + Length - trailerLength, &header);

    bool DAQboardEventOK = (Length==header.EventSize*4); // Length: DAQ board packet length in Byte; header.EventSize: DAQ board packet length in 32-bit words

    bool eventIDOK = true;
    bool strobeCountOK = true;
    bool timeStampsOK = true;
    double timeStampDAQboard = -1.;
    double timeStampChip     = -1.;
    if (count > 0) {
      //if (header.EventId > lastEventID + 1) {
      //  std::cout << "It seems that an DAQ event was missed. Last id: " << lastEventID << " Current id: " << header.EventId << std::endl;
      //  eventIDOK = false;
      //}
      //if (StrobeCounter > lastStrobeCounter + 1) {
      //  std::cout << "It seems that a chip event was missed. Last strobe id: " << lastStrobeCounter << " Current strobe id: " << StrobeCounter << std::endl;
      //  eventIDOK = false;
      //}
      // timestamp comparison:
      // * DAQboard time stamp
      //   resolution of 12.5ns, returns Nclk-2 with Nclk being the distance to the last event
      // * Chip bunch counter
      //   200ns resolution, maximum value of 12.8us, as only bits 3 to 8 of the 40MHz Counter are availabe
      timeStampDAQboard = (double)(((unsigned int)((header.TimeStamp+2)*12.5))%12800);
      timeStampChip = ((BunchCounter+64-lastBunchCounter)%64)*200.;
      if (fabs(timeStampDAQboard-timeStampChip)>400) {
//        std::cout << "Event: " << count << ",\tDAQ board timestamp: " << (header.TimeStamp+2)*12.5 << "ns (modulo: " <<  timeStampDAQboard << "ns),\tchip timestamp: " << timeStampChip << "ns,\tdeviation: " << (timeStampDAQboard-timeStampChip) << "ns,\tBC (200ns): " << BunchCounter << ",\tprev. BC: " << lastBunchCounter << ",\tDAQ board time stamp (12.5ns):" << header.TimeStamp << std::endl;
        timeStampsOK = false;
      }
    }

    if (writeDebug) {
      outDebug << header.AFull << '\t' << header.BufferDepth << '\t' << header.EventId << '\t' << header.TimeStamp << '\t' << header.TrigType << '\t' << header.StrobeCountTotal << '\t' << header.TrigCountChipBusy << '\t' << header.TrigCountDAQbusy << '\t' << header.ExtTrigCount << '\t' << header.EventSize << '\t' << StrobeCounter << '\t' << BunchCounter << '\t' << timeStampDAQboard << '\t' << timeStampChip << std::endl;
    }

    lastEventID = header.EventId;
    lastStrobeCounter = StrobeCounter;
    lastBunchCounter  = BunchCounter;

#ifdef MYDEBUG
    std::string str = "Chip event raw data:\n";
    for (int j=headerLength; j<Length-trailerLength; ++j) {
      char buffer[20];
      sprintf(buffer, "%02x ", data_buf[j]);
      str += buffer;
      if (j%4==3) {
        str += '\n';
      }
    }
    str += "\n";
    std::cout << str.data() << std::endl;
#endif

    if (!HeaderOK || !EventOK || !TrailerOK || !eventIDOK || !DAQboardEventOK || !strobeCountOK) { // || !timeStampsOK) {
      std::cout << "ERROR in data stream: Header: " << HeaderOK << " Event: " << EventOK << " Trailer: " << TrailerOK << " DAQboardEvent: " << DAQboardEventOK << " StrobeCount: " << strobeCountOK << " TimeStamps: " << timeStampsOK << std::endl;

      std::string str = "RAW dump: ";
      for (int j=0; j<Length; j++) {
        char buffer[20];
        sprintf(buffer, "%02x ", data_buf[j]);
        str += buffer;
      }
      str += "\n";
      std::cout << str.data() << std::endl;
      ++countBad;
    }

    count++;

    // write data to ThresholdData array
    if (!(count %1000)) {
      NHits += (int)Hits.size();
      for (int ihit = 0; ihit < Hits.size(); ihit ++) {
        ThresholdData[Hits.at(ihit).doublecol + Hits.at(ihit).region * 16][Hits.at(ihit).address][0] ++;
      }
      Hits.clear();
    }

    // print run progress
    if (count % NPrintOut == 0) {
      //std::cout << "Event: " << count << " Last event id: " << lastEventID << std::endl;
      int diff = time(0) - last_status;
      if (diff > 2) { // print something only less than every 2 seconds
        std::cout << "event: " << count << " last event id: " << lastEventID << std::endl;
        std::cout << "event length: " << Length << std::endl;
        last_status = time(0);
      }
    }
  }

  if (writeDebug) outDebug.close();
  // writing information to file
  WriteDataToFile (fName, 1, true);

  // noise occupancy information
  std::cout << "**********************************************************" << std::endl;
  std::cout << "found " << NHits << " hits." << std::endl;
  std::cout << "avg noise occupancy " << (float)NHits/(512.*1024*NEvents) << std::endl;
  std::cout << "**********************************************************" << std::endl;

  // rate information
  std::cout << "beg time: " << beg_status << std::endl;
  end_status = time(0);
  std::cout << "end time: " << end_status << std::endl;
  int diff = end_status - beg_status;
  std::cout << "total time: " << diff << std::endl;
  if (diff > 0) {
    float rate = (float) NEvents / diff;
    std::cout << "approximate avg rate: " << rate << std::endl;
  }
  std::cout << "**********************************************************" << std::endl;
  std::cout << "Count:      " << count << std::endl;
  std::cout << "Bad events: " << countBad << "\t(" << (double)countBad/(double)count*100. << "%)" << std::endl;
  std::cout << "NEvents:    " << NEvents << std::endl;
  std::cout << "**********************************************************" << std::endl;


  ts->GetDAQBoard(0)->WriteBusyOverrideReg(true);  // enable chip busy at daqboard connector
  sleep(1);
  ts->GetDAQBoard(0)->StopTrigger();

  // read events left in the queue of the DAQ board
  int timeoutCount=0;
  while (ts->GetDAQBoard(0)->GetNextEventId() > lastEventID && timeoutCount<10) {
    int error = 0;
    unsigned char* debug = 0x0;
    int debug_length = 0;
    int ret_value = ts->GetDAQBoard(0)->ReadChipEvent(data_buf, &Length, length_buf, &error, (writeDebug) ? &debug : 0x0 , (writeDebug) ? &debug_length : 0x0);
    if (ret_value == -3) break; // stop-trigger marker received
    if (ret_value == -2) ++timeoutCount;
    for (int i =0;i< Length; i++){
       printf("%02X ", data_buf[i]);
    }
    if (writeDebug && debug_length>0) {
      std::vector<unsigned char> vec(&debug[0], &debug[debug_length]);
      debugRaw.insert(debugRaw.end(), vec.begin(), vec.end());
    }

    if (Length > headerLength+trailerLength) {
      TEventHeader header;
      bool HeaderOK  = ts->GetDAQBoard(0)->DecodeEventHeader(data_buf, &header);
      bool EventOK   = myAlpide->DecodeEvent(data_buf+headerLength, Length-trailerLength-headerLength, &Hits);
      bool TrailerOK = ts->GetDAQBoard(0)->DecodeEventTrailer(data_buf + Length - trailerLength);

      lastEventID = header.EventId;
    }
  }
  if (timeoutCount==10) {
      std::cout << "ERROR: Waiting for the stop-trigger command timed out!" << std::endl;
      std::cout << std::endl << std::endl;
      ts->GetDAQBoard(0)->ReadMonitorRegisters();
      std::cout << std::endl << std::endl;
  }
  ts->GetDAQBoard(0)->ReadMonitorRegisters();
    uint32_t Value;
  int addr= (3 << 8) + 0x6;
  ts->GetDAQBoard(0)->ReadRegister(addr, &Value);
  std::cout <<std::dec<< "CountStrobe: " << Value << std::endl;
  if (writeDebug) {
    std::cout << "DEBUG INFORMATION:" << std::endl;
    std::cout << std::endl;
    ts->GetDAQBoard(0)->ReadMonitorRegisters();
    std::cout << std::endl;
    ts->GetDAQBoard(0)->ReadAllRegisters();
    std::cout << std::endl;
    uint64_t eventID = (uint64_t)-1;
    uint32_t tmp_value = 0;
    ts->GetDAQBoard(0)->ReadRegister(0x202, &tmp_value); // event id low
    eventID = tmp_value & 0xffffff;
    ts->GetDAQBoard(0)->ReadRegister(0x202, &tmp_value); // event id high
    eventID |= (tmp_value & 0xffffff) << 24;
    std::cout << "DAQ board event ID: " << eventID << std::endl;
    ts->GetDAQBoard(0)->ReadRegister(0x306, &tmp_value);
    std::cout << "Strobe count :      " << tmp_value << std::endl;
    std::cout << std::endl;
    TpAlpidefs3* myAlpide3 = dynamic_cast<TpAlpidefs3*>(myAlpide);
    if (myAlpide3) {
      int StrobeCounter = myAlpide3->ReadStrobeCounter();
      std::cout << "pALPIDE-3 strobe counter: " << StrobeCounter << std::endl;
    }
    outDebugRaw.write(reinterpret_cast<char*>(&debugRaw[0]), debugRaw.size());
    outDebugRaw.close();
  }
}



void ExtTrigRawDump(TTestSetup *ts, TpAlpidefs *myAlpide, int NEvents, int NPrintOut) {
  char fName[500];

  // WARNING/TODO: make this routine backward-compatible?

  const int StrobeLength  = ts->GetDAQBoard(USB_id)->GetConfig()->StrobeLength;
  const int StrobeBLength = fConfigGeneral->StrobeBlengthStandard;
  const int ReadoutDelay  = myAlpide->GetConfig()->ReadoutDelay;
  const int TriggerDelay  = ts->GetDAQBoard(USB_id)->GetConfig()->TriggerDelay;
  const bool packetBasedRdo = false;

  // PrepareEmptyReadout
  ts->GetDAQBoard(0)->ConfigureReadout (3, true, packetBasedRdo);    // multi-event buffer depth = 3, sampling on falling edge
  ts->GetDAQBoard(0)->ConfigureTrigger (0, StrobeLength, 2, 0, TriggerDelay);
  ts->GetDAQBoard(0)->WriteBusyOverrideReg(true);  // enable chip busy at daqboard connector

  // PrepareChipReadout
  myAlpide->SetChipMode(MODE_ALPIDE_CONFIG);
  myAlpide->SetReadoutDelay     (ReadoutDelay);
  myAlpide->SetEnableClustering (false);
  myAlpide->SetStrobeTiming     (StrobeBLength);
  myAlpide->SetEnableOutputDrivers(true, true);
  myAlpide->SetChipMode         (MODE_ALPIDE_READOUT_B);
  myAlpide->SetMaskAllPixels    (false);


  int count = 0;
  int countBad = 0;
  time_t beg_status = time(0);
  time_t end_status = time(0);
  time_t last_status = time(0);

  uint64_t lastEventID  = 0;
  uint64_t lastStrobeID = 0;
  unsigned int lastStrobeCounter = (unsigned int)-1;
  unsigned int lastBunchCounter = (unsigned int)-1;

  sprintf(fName, "%s/ExtTrigRawDump_%s.dat", OutputPath, Suffix);
  FILE *fp = fopen (fName, "wb");

  // TriggerAndReadEvent
  const int length_buf = 1024*4000; // length needed at ITHR=10 ~5000!!!
  unsigned char  data_buf[length_buf];   // TODO large enough?
  int            Length;

  const int headerLength = ts->GetDAQBoard(0)->GetEventHeaderLength();
  const int trailerLength = ts->GetDAQBoard(0)->GetEventTrailerLength();

  uint32_t fwVer = ts->GetDAQBoard(0)->GetFirmwareVersion();
  fwrite(&fwVer, sizeof(uint32_t), 1, fp); // write firmware version

  ts->GetDAQBoard(0)->StartTrigger();

  std::cout << "Configured. Starting data taking..." << std::endl;

  // evt loop
  while (count < NEvents) {
    // get data
    ts->GetDAQBoard(0)->ReadChipEvent(data_buf, &Length, length_buf);

    fwrite(&Length,sizeof(int)          ,     1,fp);
    fwrite(data_buf,sizeof(unsigned char),Length,fp);


    TEventHeader header;
    unsigned int StrobeCounter = (unsigned int)-1;
    unsigned int BunchCounter  = (unsigned int)-1;
    bool HeaderOK  = ts->GetDAQBoard(0)->DecodeEventHeader(data_buf, &header);
    bool EventOK   = myAlpide->DecodeEvent(data_buf+headerLength, Length-trailerLength-headerLength, 0x0, &StrobeCounter, &BunchCounter);
    bool TrailerOK = ts->GetDAQBoard(0)->DecodeEventTrailer(data_buf + Length - trailerLength, &header);

    bool DAQboardEventOK = (Length==header.EventSize*4); // Length: DAQ board packet length in Byte; header.EventSize: DAQ board packet length in 32-bit words

    bool eventIDOK = true;
    bool strobeCountOK = true;
    bool timeStampsOK = true;
    double timeStampDAQboard = -1.;
    double timeStampChip     = -1.;
    if (count > 0) {
      if (header.EventId > lastEventID + 1) {
        std::cout << "It seems that an DAQ event was missed. Last id: " << lastEventID << " Current id: " << header.EventId << std::endl;
        eventIDOK = false;
      }
      if (StrobeCounter > lastStrobeCounter + 1) {
        std::cout << "It seems that a chip event was missed. Last strobe id: " << lastStrobeCounter << " Current strobe id: " << StrobeCounter << std::endl;
        eventIDOK = false;
      }
      // timestamp comparison:
      // * DAQboard time stamp
      //   resolution of 12.5ns, returns Nclk-2 with Nclk being the distance to the last event
      // * Chip bunch counter
      //   200ns resolution, maximum value of 12.8us, as only bits 3 to 8 of the 40MHz Counter are availabe
      timeStampDAQboard = (double)(((unsigned int)((header.TimeStamp+2)*12.5))%12800);
      timeStampChip = ((BunchCounter+64-lastBunchCounter)%64)*200.;
      if (fabs(timeStampDAQboard-timeStampChip)>400.) {
        std::cout << "Event: " << count << ",\tDAQ board timestamp: " << (header.TimeStamp+2)*12.5 << "ns (modulo: " <<  timeStampDAQboard << "ns),\tchip timestamp: " << timeStampChip << "ns,\tdeviation: " << (timeStampDAQboard-timeStampChip) << "ns,\tBC (200ns): " << BunchCounter << ",\tprev. BC: " << lastBunchCounter << ",\tDAQ board time stamp (12.5ns):" << header.TimeStamp << std::endl;
        timeStampsOK = false;
      }
    }

    lastEventID = header.EventId;
    lastStrobeCounter = StrobeCounter;
    lastBunchCounter  = BunchCounter;

    if (!HeaderOK || !EventOK || !TrailerOK || !eventIDOK || !DAQboardEventOK || !strobeCountOK) { // !timestampsOK
      std::cout << "ERROR in data stream: Header: " << HeaderOK << " Event: " << EventOK << " Trailer: " << TrailerOK << " DAQboardEvent: " << DAQboardEventOK << " StrobeCount: " << strobeCountOK << " TimeStamps: " << timeStampsOK << std::endl;

      std::string str = "RAW dump: ";
      for (int j=0; j<Length; j++) {
        char buffer[20];
        sprintf(buffer, "%02x ", data_buf[j]);
        str += buffer;
      }
      str += "\n";
      std::cout << str.data() << std::endl;
      ++countBad;
    }

    count++;

    // print run progress
    if (count % NPrintOut == 0) {
      //std::cout << "Event: " << count << " Last event id: " << lastEventID << std::endl;
      int diff = time(0) - last_status;
      if (diff > 2) { // print something only less than every 2 seconds
        std::cout << "event: " << count << " last event id: " << lastEventID << std::endl;
        std::cout << "event length: " << Length << std::endl;
        last_status = time(0);
      }
    }
  }

  fclose(fp);

  // rate information
  std::cout << "beg time: " << beg_status << std::endl;
  end_status = time(0);
  std::cout << "end time: " << end_status << std::endl;
  int diff = end_status - beg_status;
  std::cout << "total time: " << diff << std::endl;
  if (diff > 0) {
    float rate = (float) NEvents / diff;
    std::cout << "approximate avg rate: " << rate << std::endl;
  }
  std::cout << "**********************************************************" << std::endl;
  std::cout << "Count:      " << count << std::endl;
  std::cout << "Bad events: " << countBad << "\t(" << (double)countBad/(double)count*100. << "%)" << std::endl;
  std::cout << "NEvents:    " << NEvents << std::endl;
  std::cout << "**********************************************************" << std::endl;

  ts->GetDAQBoard(0)->StopTrigger();

  // read events left in the queue of the DAQ board
  while (ts->GetDAQBoard(0)->GetNextEventId() > lastEventID) {
    ts->GetDAQBoard(0)->ReadChipEvent(data_buf, &Length, length_buf);


    if (Length==0) break; // end-of-readout package => stop
    if (Length<44) {
      std::cout << "Incomplete DAQ board event!" << std::endl;
      break;
    }

    TEventHeader header;
    bool HeaderOK  = ts->GetDAQBoard(0)->DecodeEventHeader(data_buf, &header);
    bool EventOK   = myAlpide->DecodeEvent(data_buf+headerLength, Length-trailerLength-headerLength, 0x0);
    bool TrailerOK = ts->GetDAQBoard(0)->DecodeEventTrailer(data_buf + Length - trailerLength);

    lastEventID = header.EventId;
  }
}


void RateTest(TTestSetup *ts, TpAlpidefs *myAlpide, int NEvents, int NPrintOut) {

  const int StrobeLength = 10;
  const int StrobeBLength = 20;
  const int ReadoutDelay = 10;
  //const int TriggerDelay = 75;
  const int TriggerDelay = 160;

  // PrepareEmptyReadout
  ts->GetDAQBoard(0)->ConfigureReadout (1, false, false);       // buffer depth = 1, sampling on rising edge
  ts->GetDAQBoard(0)->ConfigureTrigger (0, StrobeLength, 2, 0, TriggerDelay);

  // PrepareChipReadout
  myAlpide->SetChipMode(MODE_ALPIDE_CONFIG);
  myAlpide->SetReadoutDelay     (ReadoutDelay);
  myAlpide->SetEnableClustering (false);
  myAlpide->SetStrobeTiming     (StrobeBLength);
  myAlpide->SetEnableOutputDrivers(true, true);
  myAlpide->SetChipMode         (MODE_ALPIDE_READOUT_B);

  // TriggerAndReadEvent
  unsigned char  data_buf[1024];   // TODO large enough?
  int            Length;
  std::vector<TPixHit> Hits;

  std::cout << "Configured. Starting data taking..." << std::endl;

  const int headerLength = ts->GetDAQBoard(0)->GetEventHeaderLength();
  const int trailerLength = ts->GetDAQBoard(0)->GetEventTrailerLength();

  ts->GetDAQBoard(0)->StartTrigger();

  int count = 0;
  time_t beg_status = time(0);
  time_t end_status = time(0);
  time_t last_status = time(0);

  // TODO check event ID.
  uint64_t lastEventID = 0;

  while (count < NEvents) {
    ts->GetDAQBoard(0)->ReadChipEvent(data_buf, &Length, 1024);

    TEventHeader header;
    bool HeaderOK  = ts->GetDAQBoard(0)->DecodeEventHeader(data_buf, &header);
    bool EventOK   = myAlpide->DecodeEvent(data_buf+headerLength, Length-trailerLength-headerLength, &Hits);
    bool TrailerOK = ts->GetDAQBoard(0)->DecodeEventTrailer(data_buf + Length - trailerLength);

    bool eventIDOK = true;
    if (count > 0) {
      if (header.EventId > lastEventID + 1) {
        std::cout << "It seems that an event was missed. Last id: " << lastEventID << " Current id: " << header.EventId << std::endl;
        eventIDOK = false;
      }
    }

    lastEventID = header.EventId;

    if (!HeaderOK || !EventOK || !TrailerOK || !eventIDOK) {
      std::cout << "ERROR in data stream: Header: " << HeaderOK << " Event: " << EventOK << " Trailer: " << TrailerOK << std::endl;

      std::string str = "RAW dump: ";
      for (int j=0; j<Length; j++) {
        char buffer[20];
        sprintf(buffer, "%02x ", data_buf[j]);
        str += buffer;
      }
      str += "\n";
      std::cout << str.data() << std::endl;
    }

    count++;

    if (count % NPrintOut == 0) {
      std::cout << "Event: " << count << " Last event id: " << lastEventID;
      int diff = time(0) - last_status;
      last_status = time(0);
      if (diff > 0) {
        float rate = (float) NPrintOut / diff;
        std::cout << " Rate: " << rate << std::endl;
      } else {
        std::cout << std::endl << "Too frequent print outs. Increasing NPrintOut by factor 10" << std::endl;
        NPrintOut *= 10;
      }
    }
  }

  std::cout << "beg time: " << beg_status << std::endl;
  end_status = time(0);
  std::cout << "end time: " << end_status << std::endl;
  int diff = end_status - beg_status;
  std::cout << "total time: " << diff << std::endl;
  if (diff > 0) {
    float rate = (float) NEvents / diff;
    std::cout << " Rate: " << rate << std::endl;
  }

  ts->GetDAQBoard(0)->StopTrigger();

}



void ReadoutRun(TTestSetup *ts,TpAlpidefs *myAlpide, int NEvents, int NPrintOut){
  int size_packet = 1024;
  int num_packet = 16;
  int size_transfer= size_packet *num_packet;
  unsigned char        data_buf[size_transfer];
  int                  Length;
  std::vector<TPixHit> Hits;
  int count = 0;
  int count_byte=0;
  int count_word_header =0;
  int count_word_data = 0;
  int Header[4];
  int16_t Data[8192];
  int Trailer[2];
  int last_word=0;
  int16_t last_word0=0;
  int16_t last_word1=0;
  time_t last_status = time(0);
  uint64_t lastEventID = 0;
  int length_data =0;
  TEventHeader header;
  TEventHeader trailer;
  bool HeaderOK,TrailerOK,EventOK,isHeader,isTrailer,isData,isRunning,endHeader,eventIDOK;
  int size=0;
  char buffer[20];
  int diff;
  float byte_packet;
  float rate;
  int j;

  // Const
  const int StrobeLength = 10;
  const int StrobeBLength = 2;
  const int ReadoutDelay = 2;
  const int TriggerDelay = 75;

  // PrepareEmptyReadout
  ts->GetDAQBoard(0)->ConfigureReadout (1, false, true);       // buffer depth = 1, sampling on rising edge
  ts->GetDAQBoard(0)->ConfigureTrigger (0, StrobeLength, 2, 0, TriggerDelay);

  // PrepareChipReadout
  myAlpide->SetChipMode(MODE_ALPIDE_CONFIG);
  myAlpide->SetReadoutDelay     (ReadoutDelay);
  myAlpide->SetEnableClustering (false);
  myAlpide->SetStrobeTiming     (StrobeBLength);
  myAlpide->SetEnableOutputDrivers(true, true);
  myAlpide->SetChipMode         (MODE_ALPIDE_READOUT_B);
  //myAlpide->SetDAC(DAC_ALPIDE_VCASN, 66);
  //myAlpide->SetDAC(DAC_ALPIDE_ITHR, 20);
  std::cout << "Configured. Starting data taking..." << std::endl;

  ts->GetDAQBoard(0)->StartTrigger();

  HeaderOK = TrailerOK = EventOK = isTrailer = isData = endHeader=false;
  isHeader = isRunning = eventIDOK=true;
  while (isRunning == true) {
    ts->GetDAQBoard(0)->ReadChipEvent(data_buf, &Length, size_transfer);
    //std::cout << "***************************** Event Number: " << count<<" Length: " << Length << std::endl;
    count_byte =0;
    while (count_byte < Length){
      // Last Word of buffer

      if (count_byte==0 && ts->GetDAQBoard(0)->GetIntFromBinaryStringReversed(4, data_buf + count_byte)==0xabfeabfe && isHeader==false && endHeader==false) {
        Trailer[0]= last_word;
        Trailer[1]= ts->GetDAQBoard(0)->GetIntFromBinaryStringReversed(4, data_buf + count_byte) ;
        count_byte+=4;
        EventOK = myAlpide->DecodeEventNew(Data,count_word_data, &Hits);
        TEventHeader trailer;
        TrailerOK=ts->GetDAQBoard(0)->DecodeEventTrailer(Trailer,&trailer);
        count++; // incerment number of event.
        count_word_data=0;
        isData = false;
        isTrailer = false;
        isHeader = true;
        lastEventID = header.EventId;
        size += trailer.EventSize;
      }
      if (count_byte==0 && ts->GetDAQBoard(0)->GetIntFromBinaryStringReversed(4, data_buf + count_byte) != 0xabfeabfe && isHeader==false && endHeader==false){
        Data[count_word_data]= last_word0;
        count_word_data++;
        Data[count_word_data]= last_word1;
        count_word_data++;
      }
      // Read Header
      if (endHeader == true) endHeader = false;
      if (isHeader== true){
        Header[count_word_header] = ts->GetDAQBoard(0)->GetIntFromBinaryStringReversed(4, data_buf + count_byte);
        count_word_header++;
        count_byte+=4;
        if ( count_word_header == 5){
          HeaderOK  = ts->GetDAQBoard(0)->DecodeEventHeader(Header, 4, &header);
          count_word_header = 0;
          isHeader = false;
          endHeader= true;
        }
      }
      if (count_byte + 4 <Length){
        if (isHeader==false){
          if (ts->GetDAQBoard(0)->GetIntFromBinaryStringReversed(4, data_buf + count_byte +4) != 0xabfeabfe ) {isData = true;endHeader=false;}
          else {isData=false; isTrailer=true;endHeader=false;}
        }
        //Read Data
        if ( isData == true ){

          Data[count_word_data]= GetChipWord(data_buf+count_byte);
          //std::cout << std::hex<< "Data[" << count_word_data << "] " << Data[count_word_data] << std::endl;
          count_word_data++;
          count_byte+=2;
        }
        // Read Trailer
        if(isTrailer == true){
          Trailer[0]= ts->GetDAQBoard(0)->GetIntFromBinaryStringReversed(4, data_buf + count_byte);
          count_byte+=4;
          Trailer[1]= ts->GetDAQBoard(0)->GetIntFromBinaryStringReversed(4, data_buf + count_byte);
          count_byte+=4;
          EventOK = myAlpide->DecodeEventNew(Data,count_word_data, &Hits);
          count_word_data=0;
          TrailerOK=ts->GetDAQBoard(0)->DecodeEventTrailer(Trailer,&trailer);
          size += trailer.EventSize;
          isData = false;
          isTrailer = false;
          isHeader = true;
          Hits.clear();
          // Check if the event is currupted
          if (count > 0) {
            if (header.EventId > lastEventID + 1) {
              std::cout << "It seems that an event was missed. Last id: " << lastEventID << " Current id: " << header.EventId << std::endl;
              eventIDOK = false;
            }
          }
          lastEventID = header.EventId;
          if (!HeaderOK || !EventOK || !TrailerOK || !eventIDOK) {
            std::cout << "ERROR in data stream: Header: " << HeaderOK << " Event: " << EventOK << " Trailer: " << TrailerOK<<"ID: "<< eventIDOK<<std::endl;
            std::string str = "RAW dump: ";
            for (j=0; j<Length; j++) {
              sprintf(buffer, "%02x ", data_buf[j]);
              str += buffer;
            }
            str += "\n";
            std::cout << str.data() << std::endl;
          }
          if (eventIDOK == false) eventIDOK = true;
          count++; // increment number of event.
          // Print info Event.
          if (count % NPrintOut == 0) {
            std::cout << "Event: " << count << " Last event id: " << lastEventID;
            diff = time(0) - last_status;
            last_status = time(0);
            if (diff > 0) {
              byte_packet= size/NPrintOut;
              rate = (float)  NPrintOut/ diff ;
              size =0;
              std::cout << " Word Event mean: "<< byte_packet << " Rate: " << rate << " Event/s" << std::endl;
            } else {
              std::cout << std::endl << "Too frequent print outs. Increasing NPrintOut by factor 10" << std::endl;
              NPrintOut *= 10;
            }
          }
        }
      }
      else{
        if (isHeader == false && endHeader == false){
          last_word=ts->GetDAQBoard(0)->GetIntFromBinaryStringReversed(4, data_buf + count_byte);
          last_word0=GetChipWord(data_buf + count_byte);
          last_word1=GetChipWord(data_buf + count_byte + 2);
          count_byte+=4;
          //                   std::cout<<std::dec<<"CountByte :"<<count_byte << std::endl;
        }
      }
      if( count == NEvents) {
        ts->GetDAQBoard(0)->StopTrigger();
      }
      if (count >= NEvents &&  ts->GetDAQBoard(0)->GetIntFromBinaryStringReversed(4, data_buf + count_byte) == 0xebfeebfe){
        isRunning = false;
        count_byte+=4;
        ts->GetDAQBoard(0)->EndOfRun();
      }
    }
  }
}


void RateTest2(TTestSetup *ts,TpAlpidefs *myAlpide, int NEvents, int NPrintOut){
  unsigned char        data_buf[1024];   // TODO large enough?
  int                  Length;
  int count = 0;
  time_t time_start = time(0);
  int size=0;
  // Const
  const int StrobeLength = 10;
  const int StrobeBLength = 20;
  const int ReadoutDelay = 10;
  const int TriggerDelay = 0;

  // PrepareEmptyReadout
  ts->GetDAQBoard(0)->ConfigureReadout (1, false, true);       // buffer depth = 1, sampling on rising edge
  ts->GetDAQBoard(0)->ConfigureTrigger (0, StrobeLength, 2, 0, TriggerDelay);

  // PrepareChipReadout
  myAlpide->SetChipMode(MODE_ALPIDE_CONFIG);
  myAlpide->SetReadoutDelay     (ReadoutDelay);
  myAlpide->SetEnableClustering (false);
  myAlpide->SetStrobeTiming     (StrobeBLength);
  myAlpide->SetEnableOutputDrivers(true, true);
  myAlpide->SetChipMode         (MODE_ALPIDE_READOUT_B);
  // myAlpide->SetDAC(DAC_ALPIDE_VCASN, 64);
  // myAlpide->SetDAC(DAC_ALPIDE_ITHR, 20);
  std::cout << "Configured. Starting data taking..." << std::endl;

  ts->GetDAQBoard(0)->StartTrigger();
  std::cout << "Wait until the end of test.The duration depends by number of events."<< std::endl;
  std::cout << "Start Test:"<< std::endl;
  time_start = time(0);
  while (count < NEvents) {
    // ts->GetDAQBoard(0)->ReadChipEvent(data_buf, &Length, 1024);
    Length=ts->GetDAQBoard(0)->ReceiveData(3, data_buf, 1024);
    count ++;
    size+=Length;
  }
  std::cout << "Duration: " << time(0) - time_start << " Seconds"<< std::endl;
  std::cout << "Number of byte transfer: " << size << std::endl;
  int diff = time(0) - time_start;
  float rate = (float) size / diff / 96;
  std::cout << "Rate: " << rate <<" Events/s "<< std::endl;
  float rate2 = (float) size / diff / 1024 / 1024;
  std::cout << "Rate: " << rate2 <<"MB/s "<< std::endl;
  ts->GetDAQBoard(0)->StopTrigger();
}

void SoftwareReset (TTestSetup *ts,int duration){
  ts->GetDAQBoard(0)->ResetBoardFPGA(duration);
  sleep(100);
  ts->GetDAQBoard(0)->ResetBoardFX3(duration);
}

void SoftwareResetFX3 (TTestSetup *ts,int duration){
  ts->GetDAQBoard(0)->ResetBoardFX3(duration);
}

void SoftwareResetFPGA (TTestSetup *ts,int duration){
  ts->GetDAQBoard(0)->ResetBoardFPGA(duration);
}


int16_t GetChipWord (unsigned char *Data)
{
  int16_t       Data16 = 0;
  unsigned char LSByte, MSByte;

  LSByte = Data[0];
  MSByte = Data[1];

  Data16 += ((int16_t)MSByte) << 8;
  Data16 += LSByte;

  return Data16;
}


void RunSpecialTest(TTestSetup *ts, TpAlpidefs *myAlpide, int par1, int par2, int par3) {
  TpAlpidefs2 *myAlpide2 = (TpAlpidefs2*) myAlpide;
  if (par1 == -1) {
    ts->ReadTempOnChip(USB_id);
  }
  else if (par2 == -1) {
  }
  else if (par3 == -1) {
    ResetScan (ts, myAlpide, 50, 0, 255, 5, par1 /*NMaskStages*/, par2/*Charge*/);
  }
  else {
    //IDBScan(ts, myAlpide, par1, par2, par3);
    IResetScan (ts, myAlpide, 50, 0, 50, 1, par1 /*NMaskStages*/, par2/*Charge*/, par3/*Vreset*/);
  }
}


void MyThresholdHigh(TTestSetup *ts, TpAlpidefs *myAlpide, int NMaskStages, int Start, int Stop) {

  std::cout << "Overriding DAC" << std::endl;
  myAlpide->OverrideVoltageDAC (SWCNTL_VPLSE_HIGH);
  for (int i = 30; i > 0; i --) {
    std::cout << "Starting threshold scan in " << i << std::endl;
    //sleep(1);
  }

  ThresholdScanFull (ts, myAlpide, NMaskStages, Start, Stop);

  for (int i = 60; i > 0; i --) {
    std::cout << "Powering down in " << i << std::endl;
    //sleep(1);
  }
}


void SourceLoop(TTestSetup *ts, TpAlpidefs *myAlpide, int NTriggers, int Start, int Stop) {

  for (int ITH = Start; ITH <= Stop; ITH +=2) {
    sprintf(Suffix, "_ITH_%d", ITH);
    myAlpide->SetDAC("ITHR", ITH);
    SourceScan (ts, myAlpide, NTriggers, MODE_ALPIDE_READOUT_B, true);
  }
}


void FirmwareVersion(TTestSetup *ts)
{
  ts->GetDAQBoard(USB_id)->PrintVersionFirmware();
  /*
  // Test Write and read id chip
  ts->GetDAQBoard(0)->WriteChipIdReg(100);
  std::cout << std::dec <<"Value Id Chip " << ts->GetDAQBoard(0)->GetChipId () << std::endl;
  //Test busy override
  while(1){
  std::cout << std::dec <<"Enable Busy " << std::endl;
  ts->GetDAQBoard(0)->WriteBusyOverrideReg(true);
  sleep(5);
  std::cout << std::dec <<"Disable Busy" << std::endl;
  ts->GetDAQBoard(0)->WriteBusyOverrideReg(false);
  sleep(5);
  }*/
}


void TestEvent(TDAQBoard2 *myBoard2){
  myBoard2->WriteSlaveDataEmulator(0xfffffff1);  //idle
  myBoard2->WriteSlaveDataEmulator(0xffffffa1);  //chip Heade
  myBoard2->WriteSlaveDataEmulator(0xffffffc0);  // Region Header
  myBoard2->WriteSlaveDataEmulator(0xffffffc1);
  myBoard2->WriteSlaveDataEmulator(0xffffffc2);
  myBoard2->WriteSlaveDataEmulator(0xff6a3d46); // data long
  myBoard2->WriteSlaveDataEmulator(0xffffffc3);
  myBoard2->WriteSlaveDataEmulator(0xffffffc4);
  myBoard2->WriteSlaveDataEmulator(0xffffffc5);
  myBoard2->WriteSlaveDataEmulator(0xffffffc6);
  myBoard2->WriteSlaveDataEmulator(0xfffff408); // data Short
  myBoard2->WriteSlaveDataEmulator(0xffffffc7);
  myBoard2->WriteSlaveDataEmulator(0xffffffc8);
  myBoard2->WriteSlaveDataEmulator(0xffffffc9);
  myBoard2->WriteSlaveDataEmulator(0xffffffca);
  myBoard2->WriteSlaveDataEmulator(0xffffffcb);
  myBoard2->WriteSlaveDataEmulator(0xffffffcc);
  myBoard2->WriteSlaveDataEmulator(0xff6a3d46); // data long
  myBoard2->WriteSlaveDataEmulator(0xffffffcd);
  myBoard2->WriteSlaveDataEmulator(0xffffffce);
  myBoard2->WriteSlaveDataEmulator(0xffffffcf);
  myBoard2->WriteSlaveDataEmulator(0xfffff308); // data Short
  myBoard2->WriteSlaveDataEmulator(0xfffff408); // data Short
  myBoard2->WriteSlaveDataEmulator(0xffffffd0);
  myBoard2->WriteSlaveDataEmulator(0xffffffd1);
  myBoard2->WriteSlaveDataEmulator(0xffffffd2);
  myBoard2->WriteSlaveDataEmulator(0xffffffd3);
  myBoard2->WriteSlaveDataEmulator(0xffffffd4);
  myBoard2->WriteSlaveDataEmulator(0xfffff308); // data Short
  myBoard2->WriteSlaveDataEmulator(0xffffffd5);
  myBoard2->WriteSlaveDataEmulator(0xffffffd6);
  myBoard2->WriteSlaveDataEmulator(0xffffffd7);
  myBoard2->WriteSlaveDataEmulator(0xffffffd8);
  myBoard2->WriteSlaveDataEmulator(0xffffffd9);
  myBoard2->WriteSlaveDataEmulator(0xffffffda);
  myBoard2->WriteSlaveDataEmulator(0xffffffdb);
  myBoard2->WriteSlaveDataEmulator(0xffffffdc);
  myBoard2->WriteSlaveDataEmulator(0xffffffdd);
  myBoard2->WriteSlaveDataEmulator(0xff6a3d46); // data long
  myBoard2->WriteSlaveDataEmulator(0xffffffde);
  myBoard2->WriteSlaveDataEmulator(0xffffffdf);
  myBoard2->WriteSlaveDataEmulator(0xffffffb1); // Chip Trailer
}
