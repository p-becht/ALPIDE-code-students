//
//  TDaqboard.h
//  pALPIDEfs-software
//
//  Created by Carlo Puggioni and Markus Keil on 14/04/14.
//  Copyright (c) 2014 Carlo Puggioni, Markus Keil. All rights reserved.
//

#ifndef __pALPIDEfs_software__TDaqboard__
#define __pALPIDEfs_software__TDaqboard__

#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <map>
#include <string>
#include <stdint.h>

#include "TDut.h"
#include "USB.h"
#include "TConfig.h"


enum TTriggerMode  {TRIGMODE_TRIG_ONLY, TRIGMODE_PULSE_AFTER_TRIG, TRIGMODE_TRIG_AFTER_PULSE};

typedef struct SEventHeader {
    bool     AFull;
    int      TrigType;
    int      BufferDepth;
    uint64_t EventId;
    uint64_t TimeStamp;
    int      EventSize;
    int      StrobeCountTotal;
    int      TrigCountChipBusy;
    int      TrigCountDAQbusy;
    int      ExtTrigCount;
} TEventHeader;


struct SFieldReg {
    std::string name;
    int  size;
    int  addr;
    uint32_t  min_value;
    uint32_t  max_value;
    uint32_t setValue;
    uint32_t readValue;
};

typedef struct SADCData {
    bool     LDOOn;
    uint64_t TimeStamp;
    float    Temp;
    float    MonV;
    float    MonI;
    float    IDig;
    float    IDigIO;
    float    IAna;
} TADCData;

struct SADCCountData {
    bool              LDOStatus;
    unsigned int      ModuleAddress;
    unsigned short    DataType;
    uint32_t          TimeStamp1;
    uint32_t          TimeStamp2;
    unsigned short    Temp_adc0;
    unsigned short    MonV_adc1;
    unsigned short    MonI_adc2;
    unsigned short    IDig_adc3;
    unsigned short    IDigIO_adc4;
    unsigned short    IAna_adc5;
};

class TDAQBoard : public TUSBBoard {
public:
    static const bool DEFAULT_PULSE_MODE     = 0;
    //Endpoints
    static const int NEndpoints = 4;
    static const int ENDPOINT_WRITE_REG =0;
    static const int ENDPOINT_READ_REG  =1;
    static const int ENDPOINT_READ_ADC  =2;
    static const int ENDPOINT_READ_DATA =3;
    // Schema Instruction Word
    static const int SIZE_WORD        = 4; // byte
    static const int SIZE_ADDR_REG    = 8; // bit
    static const int SIZE_ADDR_MODULE = 4; // bit
    static const int SIZE_READ_WRITE  = 1; // bit
protected:
    // Address Module
    static const int MODULE_FPGA      = 0x0;
    static const int MODULE_ADC       = 0x1;
    static const int MODULE_READOUT   = 0x2;
    static const int MODULE_TRIGGER   = 0x3;
    static const int MODULE_JTAG      = 0x4;
    static const int MODULE_RESET     = 0x5;
    static const int MODULE_IDENT     = 0x6;
    static const int MODULE_SOFTRESET = 0x7;

    // ADC Module: Register
    static const int ADC_CONFIG0  = 0x0;
    static const int ADC_CONFIG1  = 0x1;
    static const int ADC_CONFIG2  = 0x2;
    static const int ADC_READ0    = 0x3; // Read only
    static const int ADC_READ1    = 0x4; // Read only
    static const int ADC_READ2    = 0x5; // Read only
    static const int ADC_OVERFLOW = 0x9; // Read only

    // JTAG Module: Register
    static const int DAQ_WRITE_INSTR_REG  = 0x0;
    static const int DAQ_WRITE_DATA_REG   = 0x1;

    // TRIGGER Module: Register
    static const int TRIG_BUSY_DURATION    = 0x0;
    static const int TRIG_TRIGGER_CONFIG   = 0x1;
    static const int TRIG_START            = 0x2;
    static const int TRIG_STOP             = 0x3;
    static const int TRIG_DELAY            = 0x4;
    static const int TRIG_BUSY_OVERRIDE    = 0x5;
    static const int TRIG_STROBE_COUNT     = 0x6;
    static const int TRIG_MONITOR1         = 0x7;
    static const int TRIG_PULSE_DELAY      = 0x8;

    // READOUT Module: Register
    static const int READOUT_CHIP_DATA  = 0x0;
    static const int READOUT_ENDOFRUN   = 0x1;
    static const int READOUT_EVTID1     = 0x2;
    static const int READOUT_EVTID2     = 0x3;
    static const int READOUT_TIMESTAMP1 = 0x6;
    static const int READOUT_TIMESTAMP2 = 0x7;
    static const int READOUT_MONITOR1   = 0x8;

    // RESET Module: Register
    static const int RESET_DURATION    = 0x0;
    static const int RESET_DELAYS      = 0x1;
    static const int RESET_DRST        = 0x2;
    static const int RESET_PRST        = 0x3;
    static const int RESET_PULSE       = 0x4;
    static const int RESET_PULSE_DELAY = 0x5;

    // IDENTIFICATION Module: Register
    static const int IDENT_ADDRESS     = 0x0;
    static const int IDENT_CHIP        = 0x1;
    static const int IDENT_FIRMWARE    = 0x2;

    // SOFTRESET Module Register
    static const int SOFTRESET_DURATION   = 0x0;
    static const int SOFTRESET_FPGA_RESET = 0x1;
    static const int SOFTRESET_FX3_RESET  = 0x2;
private:
    std::vector <SFieldReg> fAckHeader;
    std::vector <SFieldReg> fAckData;
    std::vector <SFieldReg> fADCConfigReg0;
    std::vector <SFieldReg> fADCConfigReg1;
    std::vector <SFieldReg> fADCRead0;
    std::vector <SFieldReg> fADCRead1;
    std::vector <SFieldReg> fADCRead2;
    std::vector <SFieldReg> fADCHeader;
    std::vector <SFieldReg> fIDReg;
    std::vector <SFieldReg> fIDChip;
    std::vector <SFieldReg> fIDFirmware;
    std::vector <SFieldReg> fEvtIDReg1;
    std::vector <SFieldReg> fEvtIDReg2;

    bool fDummy;
    std::map <std::string,int> fMapNameId;
    int      fResetDuration;    // Redundant with fConfig, remove
    int      fPulseDuration;    // Redundant with fConfig, remove
    int      fPrstDuration;     // Redundant with fConfig, remove

// Readout ADC
    std::ofstream fADCFileCSV_RO;
    std::vector <SADCCountData> fADCPacket;
    uint32_t fLastADCWords[6];
    int      fNumADCEvent;
    int      fNumLastADCWords;
    void     ReadFirmwareVersion ();
protected:
    TDAQBoardConfig        *fConfig;
    uint32_t fFirmwareVersion;
    int      fHeaderFormat;     // 0: short header, 1: long header (only for firmware v247E0611 onwards)
    int      fPulseDelay;       // Redundant with fConfig, remove
    int      fPulseMode;        // 0: no pulse, 1: pulse <fPulseDelay> after strobe, 2: strobe <fPulseDelay> after pulse
                                // Redundant with fConfig, remove

    std::deque<unsigned char> fEventBuffer;

    uint32_t fTrailerWord;
    int      fReadoutMode;  // 0: event based; 1: packet based
    void     GetBinaryStringFromInt         (unsigned char * binStr, uint32_t number, int sizeWord);
    uint32_t GetIntFromBinaryString         (int numByte, unsigned char *str);
    int  GetNumberByte(int number);
    uint32_t GetMaskBit(int sizeField, int numByteBefore);
    bool DecodeStringRead(std::vector <SFieldReg>& reg,uint32_t value);
    bool SetFieldValue(std::vector <SFieldReg>& reg, int id, uint32_t value);
    bool GetRegValue(std::vector <SFieldReg> reg, uint32_t * value);
    bool SendWord(uint32_t value);
    bool ReadAck();
    void SplitStringRead(unsigned char *string_read, unsigned char *header, unsigned char *data);
    void DefineADCConfigReg0 ();
    void DefineADCConfigReg1 ();
    void DefineADCRead0      ();
    void DefineADCRead1      ();
    void DefineADCRead2      ();
    void DefineIDReg         ();
    void DefineAckHeader     ();
    void DefineAckData       ();
    void DefineADCHeader     ();
    void DefineEventIDReg    ();
    void DefineIDFirmware    ();
    void DefineIDChip        ();

    bool CheckNameExist(std::string name);
    void DumpMap();
    bool WriteResetDuration();
    bool WritePulseDelay();
    int  ReadChipRegisterLL(int Address, int *Value);
    virtual int  SendChipSignal      (TChipSignal ASignal, int ADuration);
    friend int TDUT::SendSignal (TChipSignal ASignal, int ADuration);   // this is the only method that should call SendChipSignal
public:
    TDAQBoard (libusb_device *ADevice, TDAQBoardConfig *AConfig);
    virtual const char *GetClassName () { return "TDAQBoard 1";};

// ADC
    int GetIdField(std::string name);
    std::vector <SFieldReg>& GetADCConfigReg0(){return fADCConfigReg0;};
    std::vector <SFieldReg>& GetADCConfigReg1(){return fADCConfigReg1;};
    std::vector <SFieldReg>& GetADCRead0(){return fADCRead0;};
    std::vector <SFieldReg>& GetADCRead1(){return fADCRead1;};
    std::vector <SFieldReg>& GetADCRead2(){return fADCRead2;};

    bool SendFieldValue(std::string name, std::vector <SFieldReg>& reg, uint32_t value);
    bool SendIrefValues(std::vector <SFieldReg>& ADCreg0, std::vector <SFieldReg>& ADCreg1,uint32_t iref0,uint32_t iref1,uint32_t iref2);
    bool SendADCControlReg(std::vector <SFieldReg>& ADCreg0, uint32_t LDOSelfShtdn, uint32_t LDOff);
    bool SendStartStreamDataADC(std::vector <SFieldReg>& ADCreg0);
    bool SendEndStreamDataADC(std::vector <SFieldReg>& ADCreg0);
    bool SendADCConfigReg0(std::vector <SFieldReg>& ADCreg0,uint32_t iref0,uint32_t iref1,uint32_t LDOSelfShtdn, uint32_t LDOff, uint32_t StreamADC,uint32_t ADCSelfStop, uint32_t DisableResetTimeStamp, uint32_t EnablePacketBased);
    bool SendADCConfigReg1(std::vector <SFieldReg>& ADCreg1,uint32_t iref2);
    bool ReadoutADCPacketRawData(int *ANumEv, int *ALength, int AMaxLength, bool *endRun, std::ofstream * AFileRawData, bool write =true, bool countEV = true);
    bool ReadoutADCPacket(int *ANumEv, int *ALength, int AMaxLength, bool *endRun);
    uint32_t GetADCLDOstatus(int id_event)     {return fADCPacket.at(id_event).LDOStatus;}
    uint32_t GetADCModuleAddress(int id_event) {return fADCPacket.at(id_event).ModuleAddress;}
    uint32_t GetADCDataType(int id_event)      {return fADCPacket.at(id_event).DataType;}
    uint32_t GetADCTimeStamp1(int id_event)    {return fADCPacket.at(id_event).TimeStamp1;}
    uint32_t GetADCTimeStamp2(int id_event)    {return fADCPacket.at(id_event).TimeStamp2;}
    uint32_t GetADC0(int id_event)	       {return fADCPacket.at(id_event).Temp_adc0;}
    uint32_t GetADC1(int id_event)	       {return fADCPacket.at(id_event).MonV_adc1;}
    uint32_t GetADC2(int id_event)	       {return fADCPacket.at(id_event).MonI_adc2;}
    uint32_t GetADC3(int id_event)	       {return fADCPacket.at(id_event).IDig_adc3;}
    uint32_t GetADC4(int id_event)	       {return fADCPacket.at(id_event).IDigIO_adc4;}
    uint32_t GetADC5(int id_event)             {return fADCPacket.at(id_event).IAna_adc5;}
    unsigned long long GetADCTimeStamp(int id_event)        {return (((unsigned long long)(GetADCTimeStamp2(id_event))) << 24) + GetADCTimeStamp1(id_event);}
    void CreateCSVFileADC_RO(const char * fileName);
    void AddEventCSVFileADC_RO(int num_event);
    void DumpADCWords(int id_event);

    // The following methods are supposed to get register addresses and values at input parameters and translate them into the corresponding command
    // "Command" here means the 1-to-4 word structure which is to be sent to the USB interface.
    // The methods will then directly call the correct USB transfer function, inherited from TUSBBoard
    // Parameters and types of return values still to be defined, the ints are only placeholders...

    virtual int WriteChipRegister    (int Address, int Value);
    virtual int ReadChipRegister     (int Address, int *Value);

    bool WriteRegister       (int AAddress, int AValue);
    bool ReadRegister        (std::vector <SFieldReg>& reg, uint32_t *ReadValue);
    bool ReadRegister        (int AAddress, uint32_t *AValue);
    bool WriteRegister       (std::vector <SFieldReg>& reg);
    bool SetPowerOnSequence  (int ADelayShtdn, int ADelayClk, int ADelaySig, int ADelayDrst);

    bool         PowerOn      (int &AOverflow, bool disablePOR=false);
    virtual void PowerOff     ();
    bool DisableTimeStampReset(bool ADisableTimeStamp);
    uint32_t ReadADC      (std::vector <SFieldReg>& reg, int id);
    uint32_t ReadMonI     ();
    uint32_t ReadMonV     ();
    float ReadDigitalI    ();
    float ReadOutputI     ();
    float ReadAnalogI     ();
    bool  GetLDOStatus    (int &AOverflow);
    void  DecodeOverflow  (int AOverflow);
    static int      CurrentToADC (int ACurrent);  // conversion functions for IDD, IDDO and IDDA ADCs
    static float    ADCToCurrent (int AValue);    // current values given in mA
    static float    ADCToTemperature (int AValue);
    void            ReadAllADCs  ();
    float           GetTemperature();

    virtual bool    ConfigureReadout (int ABufferDepth, bool AFallingEdgeSampling, bool AEnPacketBasedReadout = false);
    bool            ConfigureTrigger (int ABusyDuration, int ATriggerWidth, int ATriggerMode, int ANumTrigger, int ADelay = 0);
    virtual bool    ConfigurePulse   (int ADelay, int ADuration, int AMode);
    bool            StartTrigger     ();
    bool            StopTrigger      ();

    int             GetEventTrailerLength() { return 8; }
    int             GetEventHeaderLength();
    bool            DecodeEventHeader  (unsigned char *data_buf, TEventHeader *AHeader = 0);
    bool            DecodeEventTrailer (unsigned char *data_buf, TEventHeader *AHeader = 0);
    bool            DecodeEventHeader  (int *Header, int length, TEventHeader *AHeader = 0);
    bool            DecodeEventTrailer (int *Trailer, TEventHeader *ATrailer =0);

    int             ReadChipEvent      (unsigned char *data_buf, int *ALength, int AMaxLength, int *error=0x0, unsigned char **debug=0x0, int *debug_length=0x0);

    int             GetBoardAddress ();
    uint64_t        GetNextEventId  ();
    bool            ResetBoardFPGA  (int ADuration = 8);
    bool            ResetBoardFX3   (int ADuration = 8);
    bool            EndOfRun        () {return WriteRegister((MODULE_READOUT << SIZE_ADDR_REG) + READOUT_ENDOFRUN, 5);};
    uint32_t        GetIntFromBinaryStringReversed (int numByte, unsigned char *str);

    void            PrintVersionFirmware();
    std::string     GetFirmwareName     ();
    uint32_t        GetFirmwareVersion  () {return fFirmwareVersion;};
    void            SetFirmwareVersion  (uint32_t ver) { if (fDummy) fFirmwareVersion = ver; }
    uint32_t        GetFirmwareDate     () {return (fFirmwareVersion & 0xffffff);};
    TDUTType        GetFWChipVersion    ();

    uint32_t        GetChipId             ();
    bool            WriteChipIdReg        (uint32_t AChipId );
    bool            WriteBusyOverrideReg  (bool ABusyOverride);
    bool            WriteTriggerPulseDelay (int ADelay);
    void            DecodeADCData         (unsigned char *data_buf, TADCData &Data);
    virtual void    ReadAllRegisters () { }

    TDAQBoardConfig *GetConfig    () {return fConfig;};
    //bool            ConfigureTriggerAndPulse (TTriggerMode *AMode, int AStrobeLength,
    // ReadChipEvent    ();
    // GetADCFrame      ();
    // ...

    void            SetHeaderFormat(int AHeaderFormat) { fHeaderFormat = AHeaderFormat; }

    bool ReadMonitorRegisters();
    bool ReadMonitorReadoutRegister();
    bool ReadMonitorTriggerRegister();
};



class TDAQBoard2 : public TDAQBoard {
private:
    static const int IDENT_ACKCOUNT = 0x3;
    static const int DAQ_CONFIG_REG = 0x2;
    static const int READOUT_RESYNC = 0x4;
    static const int READOUT_SLAVE_DATA_EMULATOR = 0x5;
    bool      fDisableManchester;
    bool      fCMUFallingEdge;
    bool      fInvertCMUBus;
    bool      fEnableDoubleDataRate;
    bool      fSetChipSlave;
    int       fEmulationMode;
    TDataPort fDataPort;
protected:
    virtual int SendChipSignal    (TChipSignal ASignal, int ADuration);
    friend  int TDUT::SendSignal  (TChipSignal ASignal, int ADuration);   // this is the only method that should call SendChipSignal
    void        WriteI2C          (int SlaveAddress, int Command, int Data);
    bool        WriteCMUConfigReg ();
public:
    TDAQBoard2 (libusb_device *ADevice, TDAQBoardConfig *AConfig);   // Constructor
    virtual const char *GetClassName          () { return "TDAQBoard 2";};
    int                 WriteChipRegister     (int Address, int Value, int ChipId);
    int                 ReadChipRegister      (int Address, int *Value, int ChipId);
    virtual int         WriteChipRegister     (int Address, int Value);
    virtual int         ReadChipRegister      (int Address, int *Value);
    int                 WriteChipCommand      (int OpCode);
    TDataPort           GetDataPort           ()                    {return fDataPort;};
    void                SetDataPort           (TDataPort ADataPort) {fDataPort = ADataPort;};
    virtual bool        ConfigurePulse        (int ADelay, int ADuration, int AMode);
    void                SetChipIdI2C          (int AId);
    void                SelectDataOutI2C      (int AChip);   // for IB Stave with adapter board only
    virtual void        PowerOff              (int i2c);
    virtual bool        ConfigureReadout      (int ABufferDepth, bool AFallingEdgeSampling, bool AEnPacketBasedReadout);
    bool                ResyncReadout         ();
    bool                WriteSlaveDataEmulator(int AWord);
    void                SetChipSlave          (bool AChipSlave)    {fSetChipSlave  = AChipSlave;};
    void                SetEmulationMode      (int AEmulationMode) {fEmulationMode = AEmulationMode;};
    virtual void        ReadAllRegisters      ();
};

#endif /* defined(__pALPIDEfs_software__TDaqboard__) */
