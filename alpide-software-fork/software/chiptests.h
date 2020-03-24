//
//  chiptests.h
//  pALPIDEfs-software
//
//  Created by Markus Keil on 24/06/14.
//  Copyright (c) 2014 Markus Keil. All rights reserved.
//

#ifndef __pALPIDEfs_software__chiptests__
#define __pALPIDEfs_software__chiptests__

#include <iostream>
#include <chrono>
#include "TPalpidefs.h"
#include "TTestsetup.h"

extern char OutputPath[];
extern char Suffix[];
extern int USB_id;

using namespace std::chrono;

extern double PrepTime;
extern double MaskTime;
extern double PulseTime;
extern double DecodeTime;
extern double TotalTime;
extern double ReadTime;
extern double SignalTime;
extern double PulseReadTime;


void ResetClocks();
bool FifoTest          (TpAlpidefs *myAlpide);
void SetDAC            (TpAlpidefs *myAlpide, int ADAC, int AValue);
void PrepareMaskStage  (TpAlpidefs *myAlpide, TAlpidePulseType APulseType, int AMaskStage, int nPixels = 1);
void PrepareMaskStageFast(TpAlpidefs *myAlpide, TAlpidePulseType APulseType, int AMaskStage, int nPixels);
void PrepareMaskStageCluster (TpAlpidefs *myAlpide, TAlpidePulseType APulseType, int AMaskStage, std::vector<TPixHit> ACluster);
void DumpHitDataToFile (const char *fName, std::vector<TPixHit> hits, int inner, int outer = 0, bool Recreate = false);
void DigitalInject     (TTestSetup *ts, TpAlpidefs *myAlpide, int NEvts);
void AnalogueInject    (TTestSetup *ts, TpAlpidefs *myAlpide, int ACharge, int NEvts);
bool ReadSelfTriggered (TTestSetup           *ts,
                        TpAlpidefs           *myAlpide,
                        int                   AStrobeLength,
                        int                   AStrobeBLength,
                        int                   AReadoutDelay,
                        int                   NEvts,
                        int                  *NHits,
                        std::vector<TPixHit> *Hits,
                        TAlpideMode           AReadoutMode    = MODE_ALPIDE_READOUT_B,
                        bool                  verbose         = false,
                        bool                  WriteHitsToFile = false);
void DigitalScan          (TTestSetup *ts, TpAlpidefs *myAlpide, int ANEvents, int NMaskStages = 16*1024, int nPixels = 1);
void DigitalScanCluster   (TTestSetup *ts, TpAlpidefs *myAlpide, int ANEvents, int NMaskStages, const char *fNameCluster);
void AnalogueScan         (TTestSetup *ts, TpAlpidefs *myAlpide, int ACharge, int ANEvents, int NMaskStages = 16*1024, int nPixels = 1);
bool ThresholdScan        (TTestSetup *ts, TpAlpidefs *myAlpide, int NEvts, int AStart, int AStop, int AStep, bool Append = false);
bool DACParameterScanStep (TTestSetup *ts, TpAlpidefs *myAlpide, const char *DAC, int NEvts, int AStart, int AStop, int AStep, bool Append);
void ThresholdScanFast    (TTestSetup *ts, TpAlpidefs *myAlpide, int NMaskStages, int Start, int Stop, int VCASN = -1, int ITHR = -1);
void ThresholdScanFull    (TTestSetup *ts, TpAlpidefs *myAlpide, int NMaskStages = 16*1024, int Start = 10, int Stop = 50, int VCASN = -1, int ITHR = -1, int IDB = -1, int VCASN2 = -1, int VCLIP = -1);
void DACParameterScan     (TTestSetup *ts, TpAlpidefs *myAlpide, int NMaskStages, int ACharge, const char *DAC, int AStart, int AStop, int AStep);
void PulselengthScan   (TTestSetup *ts, TpAlpidefs *myAlpide, int NEvts, int ADCol, int AAddress, int AStart, int AStop, int AStep, int VCASN = -1, int ITHR = -1, int IDB = -1);
std::vector<TPixHit> OccupancyMapSelfTriggered (TTestSetup *ts,
                          TpAlpidefs *myAlpide,
                          int         AStrobeLength,
                          int         AStrobeBLength,
                          int         AReadoutDelay,
                          int         NEvts,
                          TAlpideMode AReadoutMode       = MODE_ALPIDE_READOUT_B,
                          bool        recreate           = false,
                          const char *fName              = "Hitmap.dat",
                          bool        WriteSingleHitFile = false);
void ResetScan         (TTestSetup *ts, TpAlpidefs *myAlpide, int NEvts, int AStart, int AStop, int AStep, int NMaskStages = 16*1024, int Charge = 50);
void IResetScan         (TTestSetup *ts, TpAlpidefs *myAlpide, int NEvts, int AStart = 100, int AStop = 1, int AStep = 1, int NMaskStages = 160, int Charge = 50, int Vreset = 117);
int  NoiseOccupancySingle (TTestSetup *ts, TpAlpidefs *myAlpide, int NEvts, TAlpideMode AReadoutMode = MODE_ALPIDE_READOUT_B, bool recreate = false,
                        int APar1 = 0,
                        int APar2 = 0,
                        int APar3 = 0);
int NoiseOccupancy (TTestSetup *ts, TpAlpidefs *myAlpide, int NEvts,
                    TAlpideMode AReadoutMode = MODE_ALPIDE_READOUT_B,
                    bool        recreate     = false,
                    int         VCASN        = -1,
                    int         ITH          = -1,
                    const char *mask         = "");
int  NoiseOccupancyScan(TTestSetup *ts, TpAlpidefs *myAlpide, int NEvts, TAlpideMode AReadoutMode = MODE_ALPIDE_READOUT_B, unsigned short vcasn_low=30, unsigned short vcasn_high=70, unsigned short ithr_low=30, unsigned short ithr_high=70, const char *maskFile = "");
void SourceScan        (TTestSetup *ts, TpAlpidefs *myAlpide, int NEvts, TAlpideMode AReadoutMode = MODE_ALPIDE_READOUT_B, bool recreate = false, const char *maskFile = "");
void PrepareNoiseMask  (TTestSetup *ts, TpAlpidefs *myAlpide, int NEvts, const char *fName, float NoiseCut = -1., int VCASN = 57, int ITH = 51);
void PrepareStaticMaskAndSelect (TpAlpidefs *myAlpide, TAlpidePulseType APulseType);
void IDBScan                    (TTestSetup *ts, TpAlpidefs *myAlpide, int region, int dcol, int address);
void RateTest(TTestSetup *ts, TpAlpidefs *myAlpide, int NEvents, int NPrintOut);
void RateNoiseTest(TTestSetup *ts, TpAlpidefs *myAlpide, int NEvents, int NPrintOut);
void ExtTrigRawDump(TTestSetup *ts, TpAlpidefs *myAlpide, int NEvents, int NPrintOut);
void ReadoutRun   (TTestSetup *ts,TpAlpidefs *myAlpide, int NEvents, int NPrintOut); // new
void RateTest2    (TTestSetup *ts,TpAlpidefs *myAlpide, int NEvents, int NPrintOut); // new
int16_t GetChipWord     (unsigned char *Data);                                       // new
void SoftwareReset (TTestSetup *ts,int duration);                                    // new
void SoftwareResetFX3 (TTestSetup *ts,int duration);                                 // new
void SoftwareResetFPGA (TTestSetup *ts,int duration);                                // new
void FirmwareVersion(TTestSetup *ts);
void RunSpecialTest             (TTestSetup *ts, TpAlpidefs *myAlpide, int par1 = -1, int par2 = -1, int par3 = -1);
void MyThresholdHigh            (TTestSetup *ts, TpAlpidefs *myAlpide, int NMaskStages, int Start, int Stop);
void SourceLoop(TTestSetup *ts, TpAlpidefs *myAlpide, int NTriggers, int Start, int Stop);
void ClearThresholdData();
void WriteDataToFile (const char *fName, int ASteps, bool Recreate);
void TestEvent(TDAQBoard2 *myBoard2);
void SEUPixelDigital(TTestSetup *ts, TpAlpidefs *myAlpide,int mask,int n,float dt);
void SEUPixelAnalogue(TTestSetup *ts, TpAlpidefs *myAlpide,int mask,int n,int m);
void SEUMemory(TTestSetup *ts, TpAlpidefs *myAlpide,int pattern,int n,float dt);
#endif /* defined(__pALPIDEfs_software__chiptests__) */
