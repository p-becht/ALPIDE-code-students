//
//  TPalpidefs.h
//  pALPIDEfs-software
//
//  Created by Carlo Puggioni and Markus Keil on 14/04/14.
//  Copyright (c) 2014 Carlo Puggioni, Markus Keil. All rights reserved.
//

#ifndef __pALPIDEfs_software__TPalpidefs__
#define __pALPIDEfs_software__TPalpidefs__

#include <iostream>
#include <vector>
#include "stdint.h"
#include "TDut.h"
//#include "TConfig.h"

// TODO: 
//   - variables and methods, which (exists but) are different for the different pAlpide versions have to go to the resepctive classes and be declared as abstract methods in TpAlpidefs
//   - methods, which do not make sense for one or the other (due to changed functionality): implement method in daughter class, where applicable, implement empty method with warning method in TpAlpidefs
//   - methods, which are identical: implement in TpAlpidefs
//
//   - The variable containing the chip in TTestsetup will stay of type TpAlpidefs, however it should be generated with the correct constructor of TpAlpidefs1 or 2


// pAlpide register definitions
enum TAlpideMode      {MODE_ALPIDE_CONFIG, MODE_ALPIDE_READOUT_A, MODE_ALPIDE_READOUT_B, MODE_ALPIDE_PATTERNGEN};
enum TAlpideDAC       {DAC_ALPIDE_VAUX, DAC_ALPIDE_VRESET, DAC_ALPIDE_VCASN, DAC_ALPIDE_VCASP, DAC_ALPIDE_VPULSEL, DAC_ALPIDE_VPULSEH, DAC_ALPIDE_IRESET, DAC_ALPIDE_IAUX2, DAC_ALPIDE_IBIAS, DAC_ALPIDE_IDB, DAC_ALPIDE_ITHR};
enum TAlpidePulseType {PT_DIGITAL, PT_ANALOGUE};
enum TAlpidePixreg    {PR_PULSEENABLE, PR_MASK};
enum TAlpideDacmonV   {DACV_NONE, SWCNTL_VAUX, SWCNTL_VCASN, SWCNTL_VCASP, SWCNTL_VPLSE_HIGH, SWCNTL_VPLSE_LOW, SWCNTL_VRESET, SWCNTL_VRESETP, SWCNTL_VRESETD, SWCNTL_VCASN2, SWCNTL_VCLIP, SWCNTL_VTEMP};
enum TAlpideDacmonI   {DACI_NONE, SWCNTL_IRESET, SWCNTL_IAUX2, SWCNTL_IBIAS, SWCNTL_IDB, SWCNTL_IREF, SWCNTL_ITHR, SWCNTL_IREFBUF};

enum TPalpideDataType        {DT_IDLE, DT_NOP, DT_CHIPHEADER, DT_CHIPTRAILER, DT_REGHEADER, DT_DATASHORT, DT_DATALONG, DT_BUSYON, DT_BUSYOFF, DT_COMMA, DT_EMPTYFRAME, DT_UNKNOWN};
struct TChipConfig;

 
typedef struct StatusRegister {
    bool PixelSelfTest;
    bool MemSelfTest;
    bool ColsDisabled;
    int  RegionROSMState;
    int  RegionSTSMState;
    int  PeriROSMState;
} TStatusReg;


const char DACNames[11][10] = {"VAUX", "VRESET", "VCASN", "VCASP", "VPULSEL", "VPULSEH", "IRESET", "IAUX2", "IBIAS", "IDB", "ITHR"};


class TpAlpidefs : public TDUT {
 private:
    static const int  REG_CONTROL         = 0x0;
    static const int  SIZEDATAFIFO = 128;
    static const int  NDACS        = 11;
    static const char DACNames[11][10];
    static const int  DACDefaults[11];
 public:
    static const int NREG                = 32;          // number of regions
    static const int NDCOL               = 16;           // number of double columns per region
    static const int NPIX                = 1024;        // number of pixels per double column
    static const int NCOL                = 32 * 16 * 2;  // total number of columns (not double columns!)
    static const int NROW                = 512;          // number of rows
    static const int DAC_DEFAULTS[11];
    static const int NVDACS              = 6;
    static const int NIDACS              = 5;
    static const int REG_COLDISABLEBASE  = 0x300;
    static const int REG_DATAFIFOBASE    = 0x200;
protected:
    static const int REG_REGION_DISABLE1 = 0x1;
    static const int REG_REGION_DISABLE2 = 0x2;
    static const int REG_STROBE_TIMING   = 0x3;
    static const int REG_PIXELCONFIG1    = 0x500;
    static const int REG_PIXELCONFIG2    = 0x501;
    static const int REG_VPULSEH_VPULSEL = 0x602;
    static const int REG_DACBASE         = 0x600;
    static const int REG_MONITORING      = 0x606;
    static const int REG_RGNSTATUSBASE   = 0x700;

    bool             fClusteringEnabled;                                         // Checked, OK v1 + v2
    TAlpideMode      fChipMode;                                                  // Checked, OK v1 + v2
    bool             fRegionDisabled[NREG];                                      // Checked, OK v1 + v2
    int8_t           fReadoutDelay;                                              // Checked, OK v1 + v2
    int              fStrobeTiming;                                              // Checked, OK v1 + v2
    TAlpidePulseType fPulseType;                                                 // Checked, OK v1 + v2
    int              fPixelConfig1;                                                 // Checked, OK v1 + v2
    int              fPixelConfig2;                                                 // Checked, OK v1 + v2
    bool             fColumnDisabled[NREG][NDCOL];                                             // Checked, OK v1 + v2
    bool             fMasked        [NCOL][NROW];                                              // Checked, OK v1 + v2
    int              *fDACs;                                                              // Redundant with fConfig, remove
    TStatusReg       fStatusReg     [32];                                                 // to be checked, not yet documented
    std::vector<TPixHit> fNoisyPixels;                                                    // Checked, OK v1 + v2
    int              WriteRegionDisable   (bool LowHalf);                                 // Checked, OK v1 + v2
    virtual int      WriteControlRegister (bool SelfTest, bool ConfigStrobe) = 0; 
    int              WriteColumnDisable   (int  ARegion);                                 // Checked, OK v1 + v2
    int              SetDACValue          (int  ADAC, int AValue);   // Sets the value only, no writing to chip  // Checked, OK v1 + v2
    virtual void     SetDefaultDACValues  ();                                        // Checked, OK v1 + v2
    virtual int      WriteDACRegister     (int ADAC) = 0;                    // Writes the register of a given DAC with the value stored in fDACs
    virtual int      WriteAllDACRegisters ();                               // Checked, OK v1 + v2
    bool             RegReadbackTest      (int Aaddress, int AValue);      // to be called by TestRegister     // Checked, OK v1 + v2
    
    static int16_t     GetDataWord          (unsigned char *Data);                   // to be checked, can probably be moved to pAlpide1
    static int16_t     GetDataWord          (unsigned char *Data, int offset) {return GetDataWord(Data + offset);};  // to be checked, can probably be moved to pAlpide1

public:
    TpAlpidefs                                  (TTestSetup *ASetup, int AIndex, TChipConfig *AConfig, bool Parent = false);   // Constructor
    virtual ~TpAlpidefs                         () { delete [] fDACs; };            
    virtual const char  *GetClassName           () { return "TpAlpidefs"; };
    virtual int          Init                   (bool Reset = true) = 0;                                 // Chip initialisation after power-on
    //  Enabling / Disabling of columns and regions
    int                  EnableAllRegions       ();                                                           // Checked, OK v1 + v2
    int                  DisableAllRegions       ();                                                           // Checked, OK v1 + v2
    int                  SetRegionDisable       (int ARegion, bool Disabled);                                 // Checked, OK v1 + v2
    int                  SetDisableAllColumns   (int ARegion, bool Disabled);                                 // Checked, OK v1 + v2
    int                  SetDisableColumn       (int ARegion, int AColumn, bool Disabled);                    // Checked, OK v1 + v2
    //  Periphery control register methods
    int                  DoSelfTest             ();                                                           // -> Depends on region readout control reg
    virtual int          StrobePixelConfig      ();                                                           // Checked, OK v1 + v2
    int                  SetEnableClustering    (bool enabled);                                               // Checked, OK v1 + v2
    int                  SetChipMode            (TAlpideMode AMode);                                          // Checked, OK v1 + v2
    int                  SetReadoutDelay        (int8_t ADelay);                                              // Checked, OK v1 + v2
    virtual int          SetEnableOutputDrivers (bool OEN1, bool OEN2) {return 1;};                           // to be checked, move?
    //  Pixel select and mask
    int                  LoadSinglePixelConfig  (int ACol, int ARow, TAlpidePixreg APixReg, bool AValue, TAlpidePulseType APulseType);  //; only writes the pixel config registers, does not strobe                                                 // Checked, OK v1 + v2
    int                  LoadPixelConfigAll     (TAlpidePixreg APixReg, bool AValue, TAlpidePulseType APulseType);//only writes the pixel config registers, does not strobe                                                 // Checked, OK v1 + v2
    virtual int          SetMaskAllPixels       (bool AMasked, TAlpidePixreg APixReg = PR_MASK); // writes reg and strobes       // Checked, OK v1 + v2
    virtual int          SetMaskSinglePixel     (int ACol, int ARow, bool AMasked); // writes reg and strobes                    // Checked, OK v1 + v2
    virtual int          SetMaskSinglePixel     (int AReg, int ADCol, int APixel, bool AMasked);                                                 // Checked, OK v1 + v2
    virtual int          SetInjectSinglePixel   (int ACol, int ARow, bool AValue, TAlpidePulseType APulseType, bool ADisableOthers = false);     // Checked, OK v1 + v2
    virtual int          SetInjectSinglePixel   (int AReg, int ADCol, int APixel, bool AValue, TAlpidePulseType APulseType, bool ADisableOthers = false);  // Checked, OK v1 + v2
    // Noisy Pixel methods
    void                 ReadNoisyPixelFile     (const char *fName, bool incremental = false);      // Checked, OK v1 + v2
    void                 WriteNoisyPixelFile    (const char *fName);                                // Checked, OK v1 + v2
    void                 ClearNoisyPixels       () {fNoisyPixels.clear();};                         // Checked, OK v1 + v2
    void                 AddNoisyPixel          (TPixHit APixel);                                   // Checked, OK v1 + v2
    void                 AddNoisyPixel          (int AReg, int ADCol, int AAddress);                // Checked, OK v1 + v2
    void                 AddStuckPixel          (TPixHit APixel);
    void                 MaskNoisyPixels        ();                                                 // Checked, OK v1 + v2
    const std::vector<TPixHit> GetNoisyPixels   () const { return fNoisyPixels; }                   // Checked, OK v1 + v2
    //  DACs
    int                  SetDAC                 (int ADAC, int AValue);                      // Checked, OK v1 + v2
    int                  SetDAC                 (const char ADAC[10], int AValue) {return SetDAC(FindDAC(ADAC), AValue);};                      // Checked, OK v1 + v2
    int                  SetTestPulse           (int AHigh, int ALow);                              // Checked, OK v1 + v2
    // current and voltage monitoring / overriding
    virtual int          SetMonitoringRegister  (TAlpideDacmonV ADacV, TAlpideDacmonI ADacI, bool OverrideV, bool OverrideI) = 0;
    int                  MonitorDACs            (TAlpideDacmonV ADacV, TAlpideDacmonI ADacI);       // Checked, OK v1 + v2
    int                  OverrideDACs           (TAlpideDacmonV ADacV, TAlpideDacmonI ADacI);       // Checked, OK v1 + v2
    int                  OverrideCurrentDAC     (TAlpideDacmonI ADacI);                             // Checked, OK v1 + v2
    int                  OverrideVoltageDAC     (TAlpideDacmonV ADacV);                             // Checked, OK v1 + v2
    // region status registers
    int                  ReadStatusRegister     (int ARegion, int *AValue);                // to be checked, not yet documented
    int                  GetRegionStatus        (int ARegion);                             // to be checked, not yet documented
    int                  GetRegionStatusAll     ();                                        // to be checked, not yet documented
    // Reset signals etc
    virtual int          GlobalReset            (int ALength = 10) = 0;
    virtual int          PixelReset             (int ALength = 16) = 0;
    virtual int          Pulse                  (int ALength = 10) = 0;
    virtual int          ConfigurePulse         (int ALength) = 0;
    //  Getter functions
    virtual int          GetNDacs             ()                        {return NDACS;};
    virtual const char  *GetDACName           (int ADAC)                {return GetDACNameGen    (ADAC, DACNames, NDACS);};
    virtual int          GetDACDefault        (int ADAC)                {return GetDACDefaultGen (ADAC, DACDefaults, NDACS);};
    virtual int          FindDAC              (const char ADACName[10]) {return FindDACGen (DACNames, ADACName, NDACS);};
    TAlpideMode          GetChipMode          ()                        {return fChipMode;};    // all OK v1 + v2
    int                  GetDAC               (int ADAC)                {return fDACs[ADAC];};
    bool                 GetClusteringEnabled ()                        {return fClusteringEnabled;};
    int                  GetReadoutDelay      ()                        {return fReadoutDelay;};
    int                  GetStrobeTiming      ()                        {return fStrobeTiming;};
    bool                 GetColumnDisabled    (int Region, int Col)     {return fColumnDisabled[Region][Col];};
    bool                 GetRegionDisabled    (int Region)              {return fRegionDisabled[Region];};
    bool                 GetIsMasked          (int Col, int Row)        {return fMasked[Col][Row];};
    int                  GetDataFifoSize      ()                        {return SIZEDATAFIFO;};
    //  Event decoder
    virtual bool          DecodeEvent         (unsigned char *evtData, int maxLength, std::vector<TPixHit> *Hits, unsigned int* StrobeCounter=0x0, unsigned int* BunchCounter=0x0, unsigned int* StatusBits=0x0) = 0;
    virtual bool          DecodeEventNew      (int16_t *evtData, int maxLength, std::vector<TPixHit> *Hits) = 0; //new
    void                  DumpHitData         (std::vector<TPixHit> Hits);                     // Checked, OK v1 + v2
    
    // miscellaneous test methods
    bool                 TestRegister         (int Aaddress, int AValue = -1);                 // Checked, OK v1 + v2
    void                 FillFifos            ();                                              // Checked, OK v1 + v2
    
    virtual int          SetStrobeTiming      (int AStrobeTiming);    // Checked, OK v1 + v2
    
    // Readout and injection methods
    virtual void         PrepareReadout       (int AStrobeBLength, int AReadoutDelay, TAlpideMode AReadoutMode) = 0;
    void                 StartReadout         (TAlpideMode AReadoutMode);  // redundant, but for better legibility    // OK v1 + v2
        
    // Address conversion methods
    virtual int           AddressToColumn      (int ARegion, int ADoubleCol, int AAddress);
    virtual int           AddressToRow         (int ARegion, int ADoubleCol, int AAddress);
    virtual int           ColRowToAddress      (int AColumn, int ARow);
    virtual int           ColToRegion          (int AColumn);
    virtual int           ColToDoubleCol       (int AColumn);
    virtual void          ReadAllRegisters     () {}
};


class TpAlpidefs1 : public TpAlpidefs {
 private:
    bool              fEvtDataPortOen1; 
    bool              fEvtDataPortOen2;
    void              DecodeRegionHeader   (int16_t Data, int *Region, int *EvtLength);
    bool              DecodeDataWord       (int16_t Data, int Region, std::vector<TPixHit> *Hits, int NHitsOld);
    static const int  SIZEDATAFIFO        = 256;
    static const int  REG_CONTROL         = 0x0;
    static const int  NDACS = 11;
    static const char DACNames[11][10];
    static const int  DACDefaults[11];
 protected:
    virtual int WriteControlRegister (bool SelfTest, bool ConfigStrobe);
    virtual int WriteDACRegister     (int  ADAC);                    // Writes the register of a given DAC with the value stored in fDACs public:  
 public:
    TpAlpidefs1 (TTestSetup *ASetup, int AIndex, TChipConfig *AConfig, bool Parent= false);   // Constructor
    virtual const char  *GetClassName           ()                        {return "TpAlpidefs1";};
    virtual int          GetNDacs               ()                        {return NDACS;};
    virtual const char  *GetDACName             (int ADAC)                {return GetDACNameGen    (ADAC, DACNames, NDACS);};
    virtual int          GetDACDefault          (int ADAC)                {return GetDACDefaultGen (ADAC, DACDefaults, NDACS);};
    virtual int          FindDAC                (const char ADACName[10]) {return FindDACGen (DACNames, ADACName, NDACS);};
    virtual int          Init                   (bool Reset = true);                                 // Chip initialisation after power-on
    virtual int          GlobalReset            (int ALength = 10);
    virtual int          PixelReset             (int ALength = 16);
    virtual int          Pulse                  (int ALength = 10);
    virtual int          ConfigurePulse         (int ALength) {return 1;};
    virtual void         PrepareReadout       (int AStrobeBLength, int AReadoutDelay, TAlpideMode AReadoutMode);
    virtual int          SetMonitoringRegister  (TAlpideDacmonV ADacV, TAlpideDacmonI ADacI, bool OverrideV, bool OverrideI);
    virtual int          SetEnableOutputDrivers (bool OEN1, bool OEN2);
    bool                 GetDataPort1Enabled    ()    {return fEvtDataPortOen1;};
    bool                 GetDataPort2Enabled    ()    {return fEvtDataPortOen2;};
    virtual bool         DecodeEvent            (unsigned char *evtData, int maxLength, std::vector<TPixHit> *Hits, unsigned int* StrobeCounter=0x0, unsigned int* BunchCounter=0x0, unsigned int* StatusBits=0x0);
    virtual bool         DecodeEventNew         (int16_t *evtData, int maxLength, std::vector<TPixHit> *Hits); //new
};


class TpAlpidefs2 : public TpAlpidefs {
 private:
    int  fPrstLength;
    bool fMEBMemEnabled;
    int  fDPORTRcvrCurrent;
    bool DecodeRegionHeader (const char Data, int &Region);
    bool DecodeChipHeader   (const char Data, int &ChipId);
    bool DecodeChipTrailer  (const char Data, int &ChipId);
    bool DecodeDataWord     (unsigned char *Data, int Region, std::vector<TPixHit> *Hits, int NOldHits, bool Long);
    static const int  NDACS = 11;
    static const char DACNames[11][10];
    static const int  DACDefaults[11];
    static const int  REG_CONTROL          = 0x0;
    static const int  REG_CMUCURRENTS      = 0x607;
    static const int  REG_CMUDMU_CONFIG    = 0x609;
    static const int  REG_FUSEWRITE_LOW    = 0x60a;
    static const int  REG_FUSEWRITE_HIGH   = 0x60b;
    static const int  REG_FUSEREAD_LOW     = 0x60c;
    static const int  REG_FUSEREAD_HIGH    = 0x60d;
    static const int  REG_CMUERRORS        = 0x60e;
    static const int  REG_TEMP             = 0x60f;

 protected:
    int  fChipId;
    bool fChipMasterFPGASlave;
    bool fSimData;
    int  fDCTRLRcvrCurrent;
    int  fDCTRLDrvrCurrent;
    int  fDCLKRcvrCurrent;
    int  fDPORTDrvrCurrent;
    int  fPulseLength;
    bool fDisableDoubleDataRate;
    bool fDisableManchester;
    int  fPreviousChip;
    virtual int WriteControlRegister (bool SelfTest, bool ConfigStrobe);
    virtual int WriteDACRegister     (int ADAC);                    // Writes the register of a given DAC with the value stored in fDACs
    void        WriteFuseBit         (int NBit); 
    static const int REG_COMMANDREG       = 0x4;
    static const int REG_PRLENGTH         = 0x5; 
    static const int REG_DPUCURRENTS      = 0x608;
    static const int SIZEDATAFIFO         = 128;
 public:  
    virtual int WriteRegister (int Address, int Value);
    virtual int ReadRegister  (int Address, int *Value);
    static const int OPCODE_STROBE        = 0xb1;
    static const int OPCODE_STROBE2       = 0x55;
    static const int OPCODE_STROBE3       = 0xc9;
    static const int OPCODE_STROBE4       = 0x2d;
    static const int OPCODE_GRST          = 0xd2;   
    static const int OPCODE_RORST         = 0x63;
    static const int OPCODE_PRST          = 0xe4;
    static const int OPCODE_BCRST         = 0x36;
    static const int OPCODE_ECRST         = 0x87;
    static const int OPCODE_PULSE         = 0x78;
    static const int OPCODE_WROP          = 0x9c;
    static const int OPCODE_RDOP          = 0x4e;
    static const int OPCODE_CLEARCMUFLAGS = 0xff00;
    static const int OPCODE_DEBUG         = 0xaa;
    TpAlpidefs2 (TTestSetup *ASetup, int AIndex, TChipConfig *AConfig, bool Parent = false);   // Constructor
    
    virtual const char *GetClassName          ()                        {return "TpAlpidefs2";};
    virtual int         GetNDacs              ()                        {return NDACS;};
    virtual const char *GetDACName            (int ADAC)                {return GetDACNameGen    (ADAC, DACNames, NDACS);};
    virtual int         GetDACDefault         (int ADAC)                {return GetDACDefaultGen (ADAC, DACDefaults, NDACS);};
    virtual int         FindDAC               (const char ADACName[10]) {return FindDACGen (DACNames, ADACName, NDACS);};
    int                 GetChipId             ()                        {return fChipId;};
    int                 GetPreviousChipId     ()                        {return fPreviousChip;};
    int                 SendCommand           (int OpCode);
    int                 SetMEBMemEnabled      (bool AEnabled);
    int                 WriteCommandReg       (int ACommand) {return WriteRegister(REG_COMMANDREG, ACommand & 0xffff);}; 
    virtual int         Init                  (bool Reset = true);    
    virtual int         GlobalReset           (int ALength = 0);   // Length not used
    virtual int         PixelReset            (int ALength = 16);
    virtual int         Pulse                 (int ALength = 10); 
    virtual int         ConfigurePulse        (int ALength);
    virtual void        PrepareReadout        (int AStrobeBLength, int AReadoutDelay, TAlpideMode AReadoutMode);
    void                SetPrstLength         (int ALength);
    void                SetPulseLength        (int ALength);
    int                 WritePRLengthReg      ();
    int                 WritePRLengthReg      (int APrstLength, int APulseLength);
    virtual int         WriteCMUCurrents      ();
    virtual int         WriteCMUCurrents      (int ADCTRLRcvrCurrent, int ADCTRLDrvrCurrent, int ADCLKRcvrCurrent);
    int                 WriteDPUCurrents      ();
    int                 WriteDPUCurrents      (int ADPORTRcvrCurrent, int ADPORTDrvrCurrent);
    virtual int         WriteCMUDMUConfig     ();
    virtual int         GetCMUErrorCounts     (int &CodeUnknown, int &Timeout, int &Deserializer);
    void                PrintCMUErrors        ();
    virtual int         SetMonitoringRegister (TAlpideDacmonV ADacV, TAlpideDacmonI ADacI, bool OverrideV, bool OverrideI);
    virtual TPalpideDataType   CheckDataType         (unsigned char DataWord);
    virtual bool        DecodeEvent           (unsigned char *evtData, int maxLength, std::vector<TPixHit> *Hits, unsigned int* StrobeCounter=0x0, unsigned int* BunchCounter=0x0, unsigned int* StatusBits=0x0);
    virtual bool        DecodeEventNew        (int16_t *evtData, int maxLength, std::vector<TPixHit> *Hits) {return false;}; //new
    void                SetChipMasterFPGASlave(bool AValue) {fChipMasterFPGASlave = AValue;};
    bool                GetChipMasterFPGASlave()            {return fChipMasterFPGASlave;};
    void                WriteHitToFile        (const char *fName, TPixHit Hit);
    virtual void        GetTemperature        (int &Value1, int &Value2);
    int                 ReadFuseValue         ();                          // move this to protected later
    void                WriteFuseValue        (int Value);                 // move this to protected later
    void                WriteFuseValue        (int AWafer, int AChip);
    void                ReadFuseValue         (int &AWafer, int &AChip);
};


class TpAlpidefs3 : public TpAlpidefs2 
{
 private: 
    static const int  NDACS = 14;
    static const char DACNames[14][10];
    static const int  DACDefaults[14];
    static const int  REG_CONTROL       = 0x1;
    static const int  REG_FROMUCONFIG1  = 0x4;
    static const int  REG_FROMUCONFIG2  = 0x5;
    static const int  REG_FROMUPULSING1 = 0x6;
    static const int  REG_FROMUPULSING2 = 0x7;
    static const int  REG_FROMUSTATUS1  = 0x8;
    static const int  REG_FROMUSTATUS2  = 0x9;
    static const int  REG_CLKCURRENTS   = 0xa;
    static const int  REG_CMUCURRENTS   = 0xb;
    static const int  REG_CMUDMU_CONFIG = 0xc;
    static const int  REG_CMUERRORS     = 0xd;
    static const int  REG_DTUCONFIG     = 0xe;
    static const int  REG_DTUDACS       = 0xf;

    static const int  REG_DTU_PLLLOCKREG1= 0x10;
    static const int  REG_DTU_PLLLOCKREG2= 0x11;
    static const int  REG_DTU_TESTREG1   = 0x12;
    static const int  REG_DTU_TESTREG2   = 0x13;
    static const int  REG_DTU_TESTREG3   = 0x14;

    static const int  REG_BUSYWIDTH     = 0x15;
    static const int  REG_FUSEWRITE_LOW = 0x16;
    static const int  REG_FUSEWRITE_HIGH= 0x17;
    static const int  REG_FUSEREAD_LOW  = 0x18;
    static const int  REG_FUSEREAD_HIGH = 0x19;
    static const int  REG_TEMP          = 0x1a;
    static const int  REG_PIXELCONFIG3  = 0x502;
    static const int  REG_DATALSBBASE   = 0x100;
    static const int  REG_DATAMSBBASE   = 0x200;
    static const int  REG_MONITORING    = 0x600;
    static const int  REG_BYPASSBUFFER  = 0x60f;

protected:
    int  fDCLKDrvrCurrent;
    int  fMCLKRcvrCurrent;
    int  fIRefCurrent;
    int  fPLLDAC;
    int  fPreempCurrent;
    int  fStrobeLength;
    int  fStrobeDelay;
    int  fBusyWidth;
    bool fXOff;
    bool fSlowMatrix;
    bool fForceBusy;
    int  fMEBMask;              // 3 bit mask for MEBuffers. 0 = all enabled, 7 = all disabled
    bool fBusyMonitoring;
    bool fInternalStrobeCont;   // Repetetive strobe for continuous mode
    bool fInternalStrobe;       // Internal strobe after pulse
    int  Write16BitReg      (int Register, int &fValue, int AValue);
    bool DecodeRegionHeader (const char Data, int &Region);
    bool DecodeChipHeader   (unsigned char *Data, int &ChipId, unsigned int& StrobeCounter, unsigned int& BunchCounter);
    bool DecodeChipTrailer  (unsigned char *Data, unsigned int& StatusBits);
    bool DecodeDataWord     (unsigned char *Data, int Region, std::vector<TPixHit> *Hits, int NOldHits, bool Long);
    virtual int      WriteAllDACRegisters ();
    virtual int      WriteDACRegister     (int ADAC);
 public:
    TpAlpidefs3                               (TTestSetup *ASetup, int AIndex, TChipConfig *AConfig, bool Parent = false);
    int                 Init                  (bool Reset = true);
    virtual const char *GetClassName          ()                        {return "TpAlpidefs3";};
    virtual int         GetNDacs              ()                        {return NDACS;};
    virtual const char *GetDACName            (int ADAC)                {return GetDACNameGen    (ADAC, DACNames, NDACS);};
    virtual int         GetDACDefault         (int ADAC)                {return GetDACDefaultGen (ADAC, DACDefaults, NDACS);};
    virtual void        GetTemperature        (int &Value1, int &Value2);
    virtual int         FindDAC               (const char ADACName[10]) {return FindDACGen (DACNames, ADACName, NDACS);};
    virtual int         WriteControlRegister  (bool SelfTest, bool Busy);
    //virtual int         WriteDACRegister      (int ADAC);                    // Writes the register of a given DAC with the value stored in fDACs
    virtual int         WriteClockCurrents    ();
    virtual int         WriteClockCurrents    (int ADCLKRcvrCurrent, int ADCLKDrvrCurrent, int AMCLKRcvrCurrent);
    virtual int         WriteCMUCurrents      ();
    virtual int         WriteCMUCurrents      (int ADCTRLRcvrCurrent, int ADCTRLDrvrCurrent);
    int                 WriteDTUCurrents      ();
    int                 WriteDTUCurrents      (int APLLDAC, int ADPORTDrvrCurrent, int APreempCurrent);
    int                 WriteDTUCfgReg        (int pllCfg, int serPhase, int PLLRst) ;
    virtual int         SetMonitoringRegister (TAlpideDacmonV ADacV, TAlpideDacmonI ADacI, bool OverrideV, bool OverrideI);
    int                 WritePulseLength      (int APulseLength)  {return Write16BitReg (REG_FROMUPULSING2, fPulseLength,  APulseLength);};
    int                 WriteStrobeLength     (int AStrobeLength) {return Write16BitReg (REG_FROMUCONFIG2,  fStrobeLength, AStrobeLength);};
    int                 WriteStrobeDelay      (int AStrobeDelay)  {return Write16BitReg (REG_FROMUPULSING1, fStrobeDelay,  AStrobeDelay);};
    int                 WriteMinBusyWidth     (int ABusyWidth)    {return Write16BitReg (REG_BUSYWIDTH,     fBusyWidth,    std::min(ABusyWidth,0x1f));};
    virtual int         WriteCMUDMUConfig     ();
    int                 WriteFROMUConfig      ();
    int                 ReadSMStates          () {int Value; ReadRegister(REG_FROMUCONFIG1, &Value); return ((Value >> 8) & 0x1f);};
    int                 MaskMEB               (int ABuffer);
    int                 UnmaskMEB             (int ABuffer);
    int                 SetPulseType          (TAlpidePulseType APulseType);
    virtual int         GetCMUErrorCounts     (int &CodeUnknown, int &Timeout, int &Deserializer);
    int                 ReadFROMUCounters     (int &BunchCounter, int &EventCounter);
    int                 ReadStrobeCounter     ();
    virtual int         ConfigurePulse        (int ALength);
    virtual int         StrobePixelConfig     ();
    virtual int         SetStrobeTiming       (int AStrobeTiming); 
    virtual TPalpideDataType   CheckDataType         (unsigned char DataWord);
    virtual bool        DecodeEvent           (unsigned char *evtData, int maxLength, std::vector<TPixHit> *Hits, unsigned int* StrobeCounter=0x0, unsigned int* BunchCounter=0x0, unsigned int* StatusBits=0x0);
    bool                FifoReadbackTest      (int AReg, int AWord, int AValue);
    bool                TestFifo              (int AReg, int AWord, int AValue = -1);
    // GAR ML 20151216 DTU_test
    virtual int         ConfigureTestDTU      ();
    virtual int         ConfigureDTU_PropPLLclock();
    virtual int         ConfigureDTU_setCurrents (int pll_dac , int driver_dac , int preemph_dac);
    virtual int         ConfigureTestDTUprbs  ();
    // Address conversion methods
    virtual int          AddressToColumn      (int ARegion, int ADoubleCol, int AAddress);
    virtual int          AddressToRow         (int ARegion, int ADoubleCol, int AAddress);
    virtual int          ColRowToAddress      (int AColumn, int ARow);
    virtual void         ReadAllRegisters     ();
};

class TpAlpidefs4 : public TpAlpidefs3 
{
 private: 
    static const int  NDACS = 14;
    static const char DACNames[14][10];
    static const int  DACDefaults[14];
    static const int  REG_CONTROL         = 0x001;
    static const int  REG_FROMUCONFIG1    = 0x004;
    static const int  REG_FROMUCONFIG2    = 0x005;
    static const int  REG_FROMUCONFIG3    = 0x006;
    static const int  REG_FROMUPULSING1   = 0x007;
    static const int  REG_FROMUPULSING2   = 0x008;
    static const int  REG_FROMUSTATUS1    = 0x009;
    static const int  REG_FROMUSTATUS2    = 0x00a;
    static const int  REG_FROMUSTATUS3    = 0x00b;
    static const int  REG_FROMUSTATUS4    = 0x00c;
    static const int  REG_FROMUSTATUS5    = 0x00d;
    static const int  REG_CLKCURRENTS     = 0x00e;
    static const int  REG_CMUCURRENTS     = 0x00f;
    static const int  REG_CMUDMU_CONFIG   = 0x010;
    static const int  REG_CMUERRORS       = 0x011;
    static const int  REG_DMUFIFO1        = 0x012;
    static const int  REG_DMUFIFO2        = 0x013;
    static const int  REG_DTUCONFIG       = 0x014;
    static const int  REG_DTUDACS         = 0x015;
    static const int  REG_DTU_PLLLOCKREG1 = 0x016;
    static const int  REG_DTU_PLLLOCKREG2 = 0x017;
    static const int  REG_DTU_TESTREG1    = 0x018;
    static const int  REG_DTU_TESTREG2    = 0x019;
    static const int  REG_DTU_TESTREG3    = 0x01a;
    static const int  REG_BUSYWIDTH       = 0x01b;
    
    static const int  REG_SATUSREGION     = 0x301;
    static const int  REG_COLUMNSLICE1    = 0x401;
    static const int  REG_COLUMNSLICE2    = 0x402;
    static const int  REG_ROWSLICE        = 0x404;
    static const int  REG_PULSESLICE      = 0x408;
    
    static const int  REG_PIXELCONFIG     = 0x500;
    
    static const int  REG_ADCCONTROL	  = 0x610;
    static const int  REG_ADCDAC	  = 0x611;
    static const int  REG_ADCCALIB	  = 0x612;
    static const int  REG_ADCAVSS   	  = 0x613;
    static const int  REG_ADCDVSS   	  = 0x614;
    static const int  REG_ADCAVDD   	  = 0x615;
    static const int  REG_ADCDVDD   	  = 0x616;
    static const int  REG_ADCVCASN  	  = 0x617;
    static const int  REG_ADCVCASP  	  = 0x618;
    static const int  REG_ADCVPULSEH	  = 0x619;
    static const int  REG_ADCVPULSEL	  = 0x61a;
    static const int  REG_ADCVRESETP	  = 0x61b;
    static const int  REG_ADCVRESETD	  = 0x61c;
    static const int  REG_ADCVCASN2 	  = 0x61d;
    static const int  REG_ADCVCLIP  	  = 0x61d;
    static const int  REG_ADCVTEMP  	  = 0x61f;
    static const int  REG_ADCITHR   	  = 0x620;
    static const int  REG_ADCIREF   	  = 0x621;
    static const int  REG_ADCIDB    	  = 0x622;
    static const int  REG_ADCIBIAS  	  = 0x623;
    static const int  REG_ADCIAUX2  	  = 0x624;
    static const int  REG_ADCIRESET 	  = 0x625;
    static const int  REG_ADCBG2V   	  = 0x626;
    static const int  REG_ADCT2V    	  = 0x627;
    
    // 0x0001 Mode Control Register
    bool fSlowMatrix;  
    int  fChipMode;	  
    int  fClusteringEnable;
    int  fMatrixReadoutSpeed;  
    int  fIBSerialLinkSpeed;  
    int  fSkewingGlobalSignal;
    int  fSkewingStartReadout;
    int  fReadoutClockGating; 
    int  fReadoutCMU;	     
    //0x0004  FROMU Configuration Register 1   
    int fMEBMask;              // 3 bit mask for MEBuffers. 0 = all enabled, 7 = all disabled
    int fBusyMonitoring;
    int fInternalStrobeCont;   // Repetetive strobe for continuous mode    
    int fPulseType;	   
    int fInternalStrobe;    
    int fRotatePulseLine;   
    int fTriggerDelay;
    //0x0010 CMUDMU
    int fPreviousChipID;
    int fInitialToken;
    int fEnableDoubleDataRate;
    /*int  fDCLKDrvrCurrent;
    int  fMCLKRcvrCurrent;
    int  fIRefCurrent;
    int  fPLLDAC;
    int  fPreempCurrent;
    int  fStrobeLength;
    int  fStrobeDelay;
    int  fBusyWidth;
    bool fXOff;
    bool fForceBusy;

    int  Write16BitReg      (int Register, int &fValue, int AValue);
    bool DecodeRegionHeader (const char Data, int &Region);
    bool DecodeChipHeader   (unsigned char *Data, int &ChipId, unsigned int& StrobeCounter, unsigned int& BunchCounter);
    bool DecodeChipTrailer  (unsigned char *Data);
    bool DecodeDataWord     (unsigned char *Data, int Region, std::vector<TPixHit> *Hits, int NOldHits, bool Long);*/
    
 protected:
    /*virtual int      WriteAllDACRegisters (); 
    virtual int      WriteDACRegister     (int ADAC);*/
 
 public: 
    TpAlpidefs4                               (TTestSetup *ASetup, int AIndex, TChipConfig *AConfig, bool Parent = false);
    virtual const char *GetClassName          ()                        {return "TpAlpidefs4";};
    virtual int         GetDACDefault         (int ADAC)                {return GetDACDefaultGen (ADAC, DACDefaults, NDACS);};
    virtual int         GetCMUErrorCounts     (int &CodeUnknown, int &Timeout, int &Deserializer);
    // 0x0001 Mode Control Register
    virtual void PrepareReadout (int AStrobeBLength, int AReadoutDelay, TAlpideMode AReadoutMode);
    void SetChipMode(int Value)            {fChipMode            = Value;};
    void SetClusteringEnable(int Value)    {fClusteringEnable    = Value;};
    void SetMatrixReadoutSpeed(int Value)  {fMatrixReadoutSpeed  = Value;};
    void SetIBSerialLinkSpeed(int Value)   {fIBSerialLinkSpeed   = Value;};  
    void SetSkewingGlobalSignal(int Value) {fSkewingGlobalSignal = Value;};
    void SetSkewingStartReadout(int Value) {fSkewingStartReadout = Value;};
    void SetReadoutClockGating(int Value)  {fReadoutClockGating  = Value;}; 
    void SetReadoutCMU(int Value)          {fReadoutCMU	         = Value;};
    int GetChipMode()            {return fChipMode            ;};
    int GetClusteringEnable()    {return fClusteringEnable    ;};
    int GetMatrixReadoutSpeed()  {return fMatrixReadoutSpeed  ;};
    int GetIBSerialLinkSpeed()   {return fIBSerialLinkSpeed   ;};  
    int GetSkewingGlobalSignal() {return fSkewingGlobalSignal ;};
    int GetSkewingStartReadout() {return fSkewingStartReadout ;};
    int GetReadoutClockGating()  {return fReadoutClockGating  ;}; 
    int GetReadoutCMU()          {return fReadoutCMU	      ;};
    int  WriteControlRegister();	  
    int  WriteControlRegister(TAlpideMode AMode, int AClustering, int ASlowMatrix, int AIBSerialLinkSpeed,int ASkewingGlobalSignal, int ASkewingStartReadout,int AReadoutClockGating,int AReadoutCMU);
       
    //0x0004  FROMU Configuration Register 1
    void SetMEBMask(int Value)  	      {fMEBMask		= Value; WriteFROMUConfig(); };	       
    void SetInternalStrobeCont(int Value) {fInternalStrobeCont  = Value; WriteFROMUConfig(); };
    void SetBusyMonitoring(int Value)	  {fBusyMonitoring	= Value; WriteFROMUConfig(); };
    void SetPulseType(int Value)	      {fPulseType		= Value; WriteFROMUConfig(); };	   
    void SetInternalStrobe(int Value)	  {fInternalStrobe	= Value; WriteFROMUConfig(); };     
    void SetRotatePulseLine(int Value)    {fRotatePulseLine	= Value; WriteFROMUConfig(); };
    void SetTriggerDelay(int Value)	      {fTriggerDelay	= Value; WriteFROMUConfig(); };
    
    int WriteFROMUConfig();
    int WriteStrobeLength(int AValue) { return WriteRegister(0x5, AValue & 0xffff); };
    int WriteStrobeDelay (int AValue) { return WriteRegister(0x6, AValue & 0xffff); };
    int WritePulseDelay  (int AValue) { return WriteRegister(0x7, AValue & 0xffff); };
    int WritePulseLength (int AValue) { return WriteRegister(0x8, AValue & 0xffff); };
    virtual int         ConfigurePulse(int ALength) { return WritePulseLength(ALength); };
    // FROMU Status
    int ReadStatusFROMU1 (int *AValue) { return ReadRegister(0x9, AValue); };
    int ReadStatusFROMU2 (int *AValue) { return ReadRegister(0xA, AValue); };
    int ReadStatusFROMU3 (int *AValue) { return ReadRegister(0xB, AValue); };
    int ReadStatusFROMU4 (int *AValue) { return ReadRegister(0xC, AValue); };
    int ReadStatusFROMU5 (int *AValue) { return ReadRegister(0xD, AValue); };
    void DecodeStatusFROMU5 (int AValue, int *ABunchCounter, int *AEventMEB, int *AFrameExt);

    int WriteDTUCfgReg (int PLL_VCO, int PLL_Bandwidth, int PLL_Off, int SerializerPhase, int PLL_Reset);
    int WriteDTUDACReg (int PLL_ChargePump, int HighSpeedLineDriver, int PreEmphasis );
    int WriteBusyWidth (int Length) { return WriteRegister(0x1B, Length & 0x1f); };
    int WriteADCControlReg(int Mode, int Input, int ComparatorCurrent, int Discriminator, int RampSpeed, int HalfLSB, int ComparatorOutput);
    virtual int PixelReset(int ALength = 16) { return SendCommand (OPCODE_PRST); };
    
    //MASK AND PULSE PIXELS
    int ClearSelBits();
    virtual int SetMaskAllPixels       (bool AMasked, TAlpidePixreg APixReg = PR_MASK);
    virtual int SetMaskSinglePixel     (int ACol, int ARow, bool AMasked); // writes reg and strobes                    // Checked, OK v1 + v2
    virtual int SetInjectSinglePixel   (int ACol, int ARow, bool AValue, TAlpidePulseType APulseType, bool ADisableOthers = false);     // Checked, OK v1 + v2
            
    //DECODE DATA ALPIDE 4

    TPalpideDataType CheckDataType (unsigned char DataWord);
    bool DecodeRegionHeader (const char Data, int &Region);
    bool DecodeChipHeader   (unsigned char *Data, int &ChipId, unsigned int& BunchCounter);
    bool DecodeChipTrailer  (unsigned char *Data, unsigned int& StatusBits);
    bool DecodeDataWord     (unsigned char *Data, int Region, std::vector<TPixHit> *Hits, int NOldHits, bool Long);
    bool DecodeEmptyFrame   (unsigned char *data, int &chipId, unsigned int &bunchCounter);
    bool DecodeEvent        (unsigned char *evtData,int maxLength,std::vector<TPixHit>  *Hits, unsigned int* StrobeCounter=0x0, unsigned int* BunchCounter=0x0, unsigned int* StatusBits=0x0);

    // INIT CHIP

    int Init (bool Reset);
    int WriteCMUDMUConfig();
    virtual void ReadAllRegisters();

    virtual int AddressToColumn(int ARegion, int ADoubleCol, int AAddress);
    virtual int AddressToRow   (int ARegion, int ADoubleCol, int AAddress);
};
#endif /* defined(__pALPIDEfs_software__TPalpidefs__) */
