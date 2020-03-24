#ifndef __pALPIDEfs_software__Config__
#define __pALPIDEfs_software__Config__

#include "TDut.h"
#include "TPalpidefs.h"

enum TMeasurementMode {MODE_UNKNOWN, MODE_DIGITAL, MODE_ANALOGUE, MODE_NOISE, MODE_SOURCE};
enum TDeviceType      {TYPE_CHIP, TYPE_TELESCOPE, TYPE_MODULE, TYPE_UNKNOWN};
enum TDataPort        {PORT_NONE, PORT_SERIAL, PORT_PARALLEL};

extern const char *gEmulatedDataFile;


//enum TAlpideMode;

const TMeasurementMode MMODE = MODE_ANALOGUE;

const TDUTType DUTTYPE       = DUT_PALPIDEFS2;
const int      CHIPID        = 0x10;
const int      PREVIOUS_CHIP = 0x10;

const int      GEOADD        = 0;      // default geographical address of DAQ board
const int      DUTID         = 0;      // default index of chip to be tested
const int      NMAXCHIPS     = 20;

const int      NEVENTS       = -1;

// current limits in mA
const int LimitDigital = 300; //300;
const int LimitOutput  = 50; //10;
const int LimitAnalog  = 300; //100;
const int LimitAnalogDuringInit = 300;

// timing constants for power-up procedure; times after LDOs are enabled
const int AUTOSHTDWN_TIME    = 100;      // time until enabling of auto shutdown
const int CLOCK_ENABLE_TIME  = 102;      // time until clock is enabled
const int SIGNAL_ENABLE_TIME = 102;      // time until signals are enabled
const int DRST_TIME          = 103;      // time until drst is deasserted

const TAlpideMode READOUTMODE = MODE_ALPIDE_READOUT_B;

// default settings for resets
const int  DEFAULT_RESET_DURATION = 2;
const int  DEFAULT_PRST_DURATION  = 32;

// pALPIDE2 I/O currents
const int DCTRL_DRVR_CURRENT = 10;
const int DCTRL_RCVR_CURRENT = 10;
const int DCLK_RCVR_CURRENT  = 10;
const int DPORT_RCVR_CURRENT = 10;
const int DPORT_DRVR_CURRENT = 10;
const int DCLK_DRVR_CURRENT  = 10;
const int MCLK_RCVR_CURRENT  = 10;

const TDataPort DATA_PORT = PORT_PARALLEL;
const bool      DOUBLE_DATA_RATE = false;
const bool      INVERT_CMU       = false;
// default settings for pulse, strobe and readout timing
// a) settings common for all measurements -> go directly into chip / board config
const int STROBELENGTH = 10;
const int STROBEBDELAY = 500;           // readout mode A only
const int READOUTDELAY = 10;
const int TRIGGERDELAY = 75;

// b) settings depending on measurement type -> go to config gen and
//      - can be used directly from there (via fConfigGeneral) for backward compatibility
//      - depending on the selected mode are written accordingly to chip / board config
//    In the latter case each pALPIDfs and DAQ board object knows the correct configuration
//    such that the configuration methods become simpler / less...
const int STROBEBLENGTH_STANDARD = 20;
const int STROBEBLENGTH_DIGITAL  = 200;
const int STROBEBLENGTH_SOURCE   = 10000;

const int PULSEDELAY_ANALOGUE    = 160;
const int PULSEDELAY_DIGITAL     = 0;

const int PULSELENGTH_ANALOGUE   = 255;
const int PULSELENGTH_DIGITAL    = 500;


class TChipConfig {
 private:
  static const int  NDACS = 16;
  static const char DACNames[][10];
 public:
  TChipConfig ();
  TDUTType     ChipType;
  int          ChipId;
  int          PreviousChip;
  int         *DACS;           // DACS will be initialised to -1; only non-default values will be set in config
  TAlpideMode  ReadoutMode;
  int          StrobeBLength;
  int          StrobeBDelay;   // for readout mode A only; reassertion delay after readout
  int          ReadoutDelay;
  char         MaskFile[100];
  // from here on: pALPIDE2 only
  int          PrstDuration;
  int          PulseLength;
  int          DCTRLRcvrCurrent;
  int          DCTRLDrvrCurrent;
  int          DCLKRcvrCurrent;
  int          DCLKDrvrCurrent;
  int          MCLKRcvrCurrent;
  int          DPORTRcvrCurrent;
  int          DPORTDrvrCurrent;
  bool         DisableDDR;
  std::vector<TPixHit> fStuckPixels;
  int          GetNDACS    () {return NDACS;};
  int          GetDACIndex (const char *Name);
  const char  *GetDACName  (int i) {if (i < NDACS) return DACNames[i]; else return "-";};
  int          GetDACValue (const char *Name);
  bool         SetDACValue (const char *Name, int Value);
};

struct TDAQBoardConfig {
  int BoardType;    // firmware type depending on the chip type
  int BoardVersion; // hardware version (different FPGA pin assignment!)
  int ResetDuration;
  int PrstDuration;
  int StrobeLength;
  int TriggerDelay;
  int PulseDelay;
  int PulseLength;
  int PulseMode;
  int LimitDigital;
  int LimitAnalog;
  int LimitAnalogInit;
  int LimitIO;
  int AutoShutdownTime;
  int ClockEnableTime;
  int SignalEnableTime;
  int DrstTime;
  int GeoAdd;
  TDataPort DataPort;
  bool      EnableDDR;
  bool      InvertCMU;
};

struct TScanConfig {    // Scan related variables (mask stages, number of injections etc.). Needed?
  int NEvents;
};


struct TGenConfig {     // General configuration variables
  int StrobeBlengthStandard;   // analogue (threshold ...), noise, data taking
  int StrobeBlengthDigital;
  int StrobeBlengthSource;     // source (long for random trigger)
  int PulsedelayAnalogue;
  int PulsedelayDigital;
  int PulselengthAnalogue;
  int PulselengthDigital;
};


struct TCondData {
  float TempInit;
  float TempFinal;
  float TempVoltage;
  float IDDD;
  float IDDA;
  float IDDDPowerOn;
  float IDDAPowerOn;
  char  SWVersion[200];
  int   FWVersion;
};

//const char DACNames[11][10] = {"VAUX", "VRESET", "VCASN", "VCASP", "VPULSEL", "VPULSEH", "IRESET", "IAUX2", "IBIAS", "IDB", "ITHR"};


class TConfig {
 private:
  TDeviceType      fType;
  int              fNChips;
  int              fNBoards;
  int              fDUTID;          // ID of device to be tested
  bool             fInitialised;
  TGenConfig       fGenConfig;      // General configuration constants
  TChipConfig      fChipConfig[NMAXCHIPS];     // Chip configuration
  TDAQBoardConfig  fBoardConfig[NMAXCHIPS];    // DAQ board configuration
  TScanConfig      fScanConfig;     // scan configuration
  TCondData        fCondData;
  TMeasurementMode fMode;
  void             Init                 ();
  void             InitialiseChipConfig ();
  void             InitialiseBoardConfig();
  void             InitialiseGenConfig  ();
  void             InitialiseScanConfig ();
  void             DecodeLine          (const char *Line);
  void             ParseLine           (const char *Line, char *Param, char *Rest, int *Chip);
  void             ReadConfigFile      (const char *fName);
  void             ReadIntParameter    (const char *Line, int *Address);
  void             ReadChipType        (int index, const char *Line);
  void             ReadDaqBoardVersion (int index, const char *Line);
  void             ReadMeasMode        (const char *Line);
  void             ReadReadoutMode     (int index, const char *Line);
  void             SetChipType         (int index, TDUTType type);
  void             ReadDDR             (int index, const char *Line);
  void             ReadDataPort        (int index, const char *Line);
  void             ApplyMeasMode       (TMeasurementMode AMode);
  void             WriteChipConfig     (FILE *fp, int index);
  void             WriteBoardConfig    (FILE *fp, int index, bool Full = false);
  void             WriteScanConfig     (FILE *fp);
  void             WriteCondData       (FILE *fp);
  TDeviceType      ReadDeviceType      (const char *Line);
  void             SetDeviceType       (TDeviceType AType, int NChips);
 protected:
 public:
  TConfig (const char *fName = "config.dat");
  TConfig (TDeviceType AType, int NChips) ;
  TDeviceType      GetType           () {return fType;};
  int              GetDUTID          () {return fDUTID;};
  TChipConfig     *GetChipConfig     (int index = 0);
  TDAQBoardConfig *GetBoardConfig    (int index = 0);
  TScanConfig     *GetScanConfig     () {return &fScanConfig;};
  TGenConfig      *GetGenConfig      () {return &fGenConfig;};
  TCondData       *GetCondData       () {return &fCondData;};
  int              GetNChips         () {return fNChips;};
  int              GetNBoards        () {return fNBoards;};
  void             WriteChipConfig   (const char *fName, int index);
  void             WriteBoardConfig  (const char *fName, int index, bool Full = false);
  void             WriteToFile       (const char *fName);
  void             WriteToFile       (FILE *fp);
  void             SetDataPort       (int index, TDataPort APort)     {fBoardConfig[index].DataPort = APort;};
  void             SetChipId         (int index, int       AChipId)   {fChipConfig [index].ChipId   = AChipId;};
  int              GetIndexForGeoAdd (int AGeoAdd);
  int              GetIndexForChipId (int AChipId);
};


extern TGenConfig *fConfigGeneral;

extern TConfig *fConfig;

#endif
