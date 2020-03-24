//
//  TestSystem.h
//  pALPIDEfs-software
//
//  Created by Carlo Puggioni and Markus Keil on 18/03/14.
//  Copyright (c) 2014 Carlo Puggioni, Markus Keil. All rights reserved.
//

#ifndef __pALPIDEfs_software__TestSystem__
#define __pALPIDEfs_software__TestSystem__

#include <iostream>
#include <vector>
#include <map>
#include <string>

//#include "libusb.h"
#include <libusb-1.0/libusb.h>

#include "USB.h"
#include "TPalpidefs.h"
#include "TDaqboard.h"
#include "TConfig.h"


const int DAQ_BOARD_VENDOR_ID  = 0x4b4;
const int DAQ_BOARD_PRODUCT_ID = 0xf1;
const int INTERFACE_NUMBER     = 0;

extern bool StopADCData;

class TTestSetup {
 private:
    struct libusb_context *fContext;
    float                  fTempInitial;
    float                  fTempFinal;

    int  InitLibUsb      ();
    bool IsDAQBoard      (libusb_device *ADevice);
    int  AddDAQBoard     (libusb_device *ADevice, TDAQBoardConfig *AConfig);
    int  AddDAQBoard     (libusb_device *ADevice, TConfig *AConfig);
    void GetPlaneSummary (int index);
    void GetSetupSummary ();
 protected:
    int                    fNDAQBoards;
    int                    fNDUTs;
    std::vector<TDAQBoard*>fDAQBoards;
    std::vector<TDUT*>     fDUTs;
    bool   PulseAndReadEvent         (TDAQBoard *myDAQBoard,
                                      TpAlpidefs *myAlpide,
                                      int APulseLength,
                                      std::vector<TPixHit> *Hits, int NTriggers = 1,
                                      TEventHeader *Header=0x0);
    bool   TriggerAndReadEvent       (int ADAQBoardID, TpAlpidefs *myAlpide, std::vector<TPixHit> *Hits);

 public:
    TTestSetup        ();
    ~TTestSetup       ();
    virtual int  FindDAQBoards          (TConfig *AConfig);
    virtual int  AddDUTs                (TConfig *AConfig);
    int          GetNDAQBoards          () {return fNDAQBoards;};
    virtual TDAQBoard   *GetDAQBoard    (int i);
    TDUT        *GetDUT                 (int ABoardID, int AChipID = -1);
    virtual int  GetBoardID             (int AChipID) {return AChipID;}; // To be overwritten by TModuleSetup 
    int          GetBoardIndexByAddress (int AAddress);
    struct libusb_context *GetContext   () {return fContext;};
    void         DoBulkloopTest         (int DAQBoardID);
    virtual bool PowerOnBoard           (int ADAQBoardID, int &AOverflow);
    virtual void PowerOffBoard          (int ADAQBoardID);
    virtual bool InitialiseChip         (int ADAQBoardID, int &AOverflow, bool Reset = true);
    virtual void   ReadAllChipDacs      (int ADAQBoardID);
    virtual void   ReadAllChipDacsCSV   (int ADAQBoardID);
    virtual double ReadDacMonV          (int ADAQBoardID, TAlpideDacmonV AVmon);
    virtual double ReadDacMonI          (int ADAQBoardID, TAlpideDacmonI AImon);
    float          ReadInitialTemp      (int ADAQBoardID) {fTempInitial = GetDAQBoard(ADAQBoardID)->GetTemperature(); return fTempInitial;};
    float          ReadFinalTemp        (int ADAQBoardID) {fTempFinal   = GetDAQBoard(ADAQBoardID)->GetTemperature(); return fTempFinal;};
    virtual void   scanVoltageDAC    (int ADAQBoardID, int ADAC, TAlpideDacmonV AVmon, int SampleDistance = 1);
    virtual void   scanCurrentDAC    (int ADAQBoardID, int ADAC, TAlpideDacmonI AImon, int SampleDistance = 1);
    virtual void   ScanAllChipDacs   (int ADAQBoardID, int SampleDistance = 1);
    void   PrepareEmptyReadout       (int AChipID, int AStrobeLength, bool APacketBased = false);
    void   ConfigurePulse            (int AChipID, int ADelay, int APulseLength, int APulseMode);
    void   PrepareDigitalInjection   (int AChipID);
    void   PrepareDigitalNew         (int AChipID, int NumEvt, bool PacketBased);
    void   PrepareAnalogueInjection  (int AChipID, int ACharge, int PulseMode = 1, int NumEvt = 1, bool PacketBased = false); 
    void   FinaliseAnalogueInjection (int AChipID, int PulseMode = 1);
    bool   PulseAndReadEvent         (int AChipID, int APulseLength, std::vector<TPixHit> *Hits, int NTriggers = 1, TEventHeader *Header = 0x0);
    bool   TriggerAndReadEvent       (int AChipID, std::vector <TPixHit> *Hits);
    void   DumpRawData               (unsigned char *data_buf, int Length);
    void   WriteConfigToFile         (const char *fName);
    void   WriteConfigToFile         (const char *fName, TConfig *AConfig);
    void   WriteConfigToFileCSV      (const char *fName);
    const char* GetGITVersion        ();
    void   ReadADCData               ();
    virtual double ReadTempOnChip            (int ADAQBoardID);
};

#endif /* defined(__pALPIDEfs_software__TestSystem__) */
