#include <stdio.h>
//#include <cstring>
#include <string.h>
#include <stdlib.h>
#include "TConfig.h"
#include "TPalpidefs.h"
#include "TDaqboard.h"


TGenConfig *fConfigGeneral;
TConfig    *fConfig;
const char *gEmulatedDataFile = "Data/SimData.dat";

// Put all recognised DAC names here, including all chip versions
const char TChipConfig::DACNames [16][10] = {"VAUX", "VRESET", "VCASN", "VCASP", "VPULSEL", "VPULSEH", "IRESET", "IAUX2", "IBIAS", "IDB", "ITHR", "VRESETP", "VRESETD", "VCASN2", "VCLIP", "VTEMP"};

// Standard constructor for lab tests; reads config from config file
TConfig::TConfig (const char *fName)
{
  for (int i = 0; i < NMAXCHIPS; i ++) {
    fChipConfig[i] = TChipConfig();     // Chip configuration
  }
  ReadConfigFile (fName);  // Read config file
  ApplyMeasMode  (fMode);  // Write correct settings for fMode into chip and board config
  fConfigGeneral = &((this)->fGenConfig);  // Publish general config

}


// Constructor for test beam; initialises setup with given number of chips, without reading config file
// (Test beam config is saved in different file)
TConfig::TConfig (TDeviceType AType, int NChips)
{
  SetDeviceType (AType, NChips);           // Only set the device type and number of chips
  Init          ();                        // Initialise all variables with default parameters
  fConfigGeneral = &((this)->fGenConfig);  // Publish general config
}


void TConfig::Init()
{
  // initialise all parameters to default values in case they are not given in config file.
  InitialiseChipConfig ();
  InitialiseBoardConfig();
  InitialiseGenConfig  ();
  InitialiseScanConfig ();
  fMode  = MMODE;
  fDUTID = DUTID;
}


void TConfig::SetDeviceType (TDeviceType AType, int NChips) {
  fType = AType;
  if (fType == TYPE_CHIP) {
    fNChips      = 1;
    fNBoards     = 1;
    fInitialised = true;
  }
  else if (fType == TYPE_TELESCOPE) {
    fNChips      = NChips;
    fNBoards     = NChips;
    fInitialised = true;
  }
  else if (fType == TYPE_MODULE) {
    fNChips      = NChips;
    fNBoards     = 1;
    fInitialised = true;
  }
  std::cout << "TConfig: Initialised setup of type " << fType << " with " << fNChips << " chips and " << fNBoards << " DAQ boards." <<std::endl;
}


void TConfig::InitialiseChipConfig ()
{
  // Depending on chip type, DAQ board type is set correspondingly.
  // To be checked whether new DAQ board will have different product ID
  for (int i = 0; i < NMAXCHIPS; i ++) {
    SetChipType(i, DUTTYPE);
    fChipConfig[i].ChipId       = CHIPID;
    fChipConfig[i].PreviousChip = PREVIOUS_CHIP;

    //    for (int idac = 0; idac < 11; idac ++) {
    //  fChipConfig[i].DACS[idac] = TpAlpidefs1::DAC_DEFAULTS[idac];
    //}
    fChipConfig[i].StrobeBDelay     = STROBEBDELAY;
    fChipConfig[i].ReadoutDelay     = READOUTDELAY;
    fChipConfig[i].ReadoutMode      = READOUTMODE;
    fChipConfig[i].PrstDuration     = DEFAULT_PRST_DURATION;
    fChipConfig[i].DCTRLRcvrCurrent = DCTRL_RCVR_CURRENT;
    fChipConfig[i].DCTRLDrvrCurrent = DCTRL_DRVR_CURRENT;
    fChipConfig[i].DCLKRcvrCurrent  = DCLK_RCVR_CURRENT;
    fChipConfig[i].DCLKDrvrCurrent  = DCLK_DRVR_CURRENT;
    fChipConfig[i].MCLKRcvrCurrent  = MCLK_RCVR_CURRENT;
    fChipConfig[i].DPORTRcvrCurrent = DPORT_RCVR_CURRENT;
    fChipConfig[i].DPORTDrvrCurrent = DPORT_DRVR_CURRENT;
    fChipConfig[i].DisableDDR       = !DOUBLE_DATA_RATE;
    sprintf(fChipConfig[i].MaskFile, "");
    fChipConfig[i].fStuckPixels.clear();
  }

}


void TConfig::InitialiseBoardConfig()
{
  for (int i = 0; i < NMAXCHIPS; i ++) {
    fBoardConfig[i].ResetDuration    = DEFAULT_RESET_DURATION;
    fBoardConfig[i].PrstDuration     = DEFAULT_PRST_DURATION;
    fBoardConfig[i].PulseMode        = TDAQBoard::DEFAULT_PULSE_MODE;

    fBoardConfig[i].LimitDigital     = LimitDigital;
    fBoardConfig[i].LimitAnalog      = LimitAnalog;
    fBoardConfig[i].LimitAnalogInit  = LimitAnalogDuringInit;
    fBoardConfig[i].LimitIO          = LimitOutput;
    fBoardConfig[i].AutoShutdownTime = AUTOSHTDWN_TIME;
    fBoardConfig[i].ClockEnableTime  = CLOCK_ENABLE_TIME;
    fBoardConfig[i].SignalEnableTime = SIGNAL_ENABLE_TIME;
    fBoardConfig[i].DrstTime         = DRST_TIME;

    fBoardConfig[i].StrobeLength     = STROBELENGTH;
    fBoardConfig[i].TriggerDelay     = TRIGGERDELAY;

    fBoardConfig[i].GeoAdd           = GEOADD;
    fBoardConfig[i].DataPort         = DATA_PORT;
    fBoardConfig[i].EnableDDR        = DOUBLE_DATA_RATE;
    fBoardConfig[i].InvertCMU        = INVERT_CMU;
  }
}


void TConfig::InitialiseScanConfig()
{
    fScanConfig.NEvents = NEVENTS;
}


void TConfig::InitialiseGenConfig  ()
{
    fGenConfig.StrobeBlengthStandard = STROBEBLENGTH_STANDARD;   // analogue (threshold ...), noise, data taking
    fGenConfig.StrobeBlengthDigital  = STROBEBLENGTH_DIGITAL;
    fGenConfig.StrobeBlengthSource   = STROBEBLENGTH_SOURCE;     // source (long for random trigger)
    fGenConfig.PulsedelayAnalogue    = PULSEDELAY_ANALOGUE;
    fGenConfig.PulsedelayDigital     = PULSEDELAY_DIGITAL;
    fGenConfig.PulselengthAnalogue   = PULSELENGTH_ANALOGUE;
    fGenConfig.PulselengthDigital    = PULSELENGTH_DIGITAL;
}


TChipConfig *TConfig::GetChipConfig (int index)
{
  if (index >= fNChips) {
    std::cout << "Error: request for config of non-existing chip index " << index << std::endl;
    return NULL;
  }
  else return &(fChipConfig[index]);
};


int TConfig::GetIndexForChipId (int AChipId)
{
  for (int i = 0; i < fNChips; i++) {
    if (fChipConfig[i].ChipId == AChipId) return i;
  }
  return -1;
}


TDAQBoardConfig *TConfig::GetBoardConfig (int index)
{
  if (index >= fNBoards) {
    std::cout << "Error: request for config of non-existing board index " << index << std::endl;
    return NULL;
  }
  return &(fBoardConfig[index]);
}


int TConfig::GetIndexForGeoAdd (int AGeoAdd)
{
  for (int i = 0; i < fNBoards; i++) {
    if (fBoardConfig[i].GeoAdd == AGeoAdd) return i;
  }
  return -1;
}


void TConfig::ReadConfigFile (const char *fName)
{
  char        Line[1024], Param[128], Rest[896];
  int         NChips = 0;
  int         Chip;
  TDeviceType type   = TYPE_UNKNOWN;
  FILE       *fp     = fopen (fName, "r");

  fInitialised = false;

  if (!fp) {
    std::cout << "WARNING: Config file " << fName << " not found, using default configuration." << std::endl;
    return;
  }

  // first look for setup type and number of chips
  while ((!fInitialised) && (fgets(Line, 1023, fp) != NULL)) {
    if ((Line[0] == '\n') || (Line[0] == '#')) continue;
    ParseLine (Line, Param, Rest, &Chip);
    if (!strcmp(Param,"NCHIPS")){
      sscanf(Rest, "%d", &NChips);
    }
    if (!strcmp(Param, "DEVICE")) {
      type = ReadDeviceType (Rest);
    }
    if ((((NChips > 0) || type == TYPE_CHIP)) && (type != TYPE_UNKNOWN)) {   // type and nchips has been found (nchips not needed for type chip)
      SetDeviceType(type, NChips);
    }
  }

  Init();

  // now read the rest
  while (fgets(Line, 1023, fp) != NULL) {
    DecodeLine(Line);
  }
}


void TConfig::ParseLine(const char *Line, char *Param, char *Rest, int *Chip) {
  char MyParam[132];
  char *MyParam2;
  if (!strchr(Line, '_')) {
    *Chip = -1;
    sscanf (Line,"%s\t%s",Param, Rest);
  }
  else {
    sscanf (Line,"%s\t%s", MyParam, Rest);
    MyParam2 = strtok(MyParam, "_");
    sprintf(Param, "%s", MyParam2);
    sscanf (strpbrk(Line, "_")+1, "%d", Chip);
  }
}


void TConfig::DecodeLine(const char *Line)
{
  int Chip, ChipStart, ChipStop;
  char Param[128], Rest[896];
  if ((Line[0] == '\n') || (Line[0] == '#')) {   // empty Line or comment
      return;
  }

  ParseLine(Line, Param, Rest, &Chip);

  if (Chip == -1) {
    ChipStart = 0;
    ChipStop  = fNChips;
  }
  else {
    ChipStart = Chip;
    ChipStop  = Chip+1;
  }


  for (int i = ChipStart; i < ChipStop; i++) {
    int bi;
    if (fType == TYPE_TELESCOPE) {
      bi = i;
    }
    else {
      bi = 0;
    }
    if (!strcmp(Param,"DUTID")) {
      ReadIntParameter(Rest, &fDUTID);
    }

    if (fChipConfig[i].GetDACIndex(Param) >= 0) {
      int Value;
      sscanf (Rest,"%d", &Value);
      fChipConfig[i].SetDACValue(Param, Value);
    }
    if (!strcmp(Param,"STROBELENGTH")) {
      ReadIntParameter(Rest, &(fBoardConfig[bi].StrobeLength));
    }
    if (!strcmp(Param,"STROBEBDELAY")) {
      ReadIntParameter(Rest, &(fChipConfig[i].StrobeBDelay));
    }
    if (!strcmp(Param,"READOUTDELAY")) {
      ReadIntParameter(Rest, &(fChipConfig[i].ReadoutDelay));
    }
    if (!strcmp(Param,"TRIGGERDELAY")) {
      ReadIntParameter(Rest, &(fBoardConfig[i].TriggerDelay));
    }
    if (!strcmp(Param,"CHIP")) {
      ReadChipType (i, Rest);
    }
    if (!strcmp(Param,"DAQBOARD")) {
      ReadDaqBoardVersion (i, Rest);
    }
    if (!strcmp(Param,"CHIPID")) {
      ReadIntParameter(Rest, &(fChipConfig[i].ChipId));
      if ((fChipConfig[i].ChipType == DUT_PALPIDEFS3 || fChipConfig[i].ChipType == DUT_PALPIDEFS4) && (fChipConfig[i].ChipId & 0x7)) {
	std::cout <<"Warning: using pALPIDE-3 with Chip ID " << fChipConfig[i].ChipId << std::endl;
      }
    }
    if (!strcmp(Param,"READOUTMODE")) {
      ReadReadoutMode (i, Rest);
    }
    if (!strcmp(Param,"DDR")) {
      ReadDDR(i, Rest);
    }
    if (!strcmp(Param,"PORT")) {
      ReadDataPort(i, Rest);
    }
    if (!strcmp(Param,"INVERTCMU")) {
      int Value;
      ReadIntParameter (Rest, &Value);
      fBoardConfig[i].InvertCMU = (bool) Value;
    }
  }  // end chip loop, after this point only general settings

  if (!strcmp(Param,"STROBEBLENGTHSTANDARD")) {
    ReadIntParameter(Rest, &(fGenConfig.StrobeBlengthStandard));
  }
  if (!strcmp(Param,"STROBEBLENGTHDIGITAL")) {
    ReadIntParameter(Rest, &(fGenConfig.StrobeBlengthDigital));
  }
  if (!strcmp(Param,"STROBEBLENGTHSOURCE")) {
    ReadIntParameter(Rest, &(fGenConfig.StrobeBlengthSource));
  }

  if (!strcmp(Param,"PULSEDELAYANALOGUE")) {
    ReadIntParameter(Rest, &(fGenConfig.PulsedelayAnalogue));
  }
  if (!strcmp(Param,"PULSEDELAYDIGITAL")) {
    ReadIntParameter(Rest, &(fGenConfig.PulsedelayDigital));
  }

  if (!strcmp(Param,"PULSELENGTHANALOGUE")) {
    ReadIntParameter(Rest, &(fGenConfig.PulselengthAnalogue));
  }
  if (!strcmp(Param,"PULSELENGTHDIGITAL")) {
    ReadIntParameter(Rest, &(fGenConfig.PulselengthDigital));
  }

  if (!strcmp(Param,"MODE")) {
    ReadMeasMode (Rest);
  }
}


void TConfig::SetChipType (int index, TDUTType type) {
  int bi;
  if (fType == TYPE_TELESCOPE) {
    bi = index;
  }
  else {
    bi = 0;
  }
  switch (type) {
    case DUT_PALPIDEFS:
    case DUT_PALPIDEFS1:
        fChipConfig [index].ChipType  = DUT_PALPIDEFS1;
        fBoardConfig[bi]   .BoardType = 1;
        break;
    case DUT_PALPIDEFS2:
        fChipConfig [index].ChipType  = DUT_PALPIDEFS2;
        fBoardConfig[bi]   .BoardType = 2;
        break;
    case DUT_PALPIDEFS3:
        fChipConfig [index].ChipType  = DUT_PALPIDEFS3;
        fBoardConfig[bi]   .BoardType = 2;
        fBoardConfig[bi]   .DataPort  = PORT_PARALLEL;   // fix parallel port and OB master for pALPIDE-3
        break;
    case DUT_PALPIDEFS4:
        fChipConfig [index].ChipType  = DUT_PALPIDEFS4;
        fBoardConfig[bi]   .BoardType = 2;
        fBoardConfig[bi]   .DataPort  = PORT_PARALLEL;   // parallel port and OB master for pALPIDE-4 ??
        break;	
    case DUT_UNKNOWN:
        fChipConfig[index] .ChipType  = DUT_UNKNOWN;
        break;
  }
}


// Currently the same measurement mode is applied to all chips / boards
// Any use case for different modes?
void TConfig::ApplyMeasMode (TMeasurementMode AMode)
{
  for (int i = 0; i < fNChips; i ++) {
    switch (AMode) {
    case MODE_DIGITAL:
      fChipConfig[i].StrobeBLength = fGenConfig.StrobeBlengthDigital;
      fChipConfig[i].PulseLength   = fGenConfig.PulselengthDigital;
      break;
    case MODE_ANALOGUE:
      fChipConfig[i].StrobeBLength = fGenConfig.StrobeBlengthStandard;
      fChipConfig[i].PulseLength    = fGenConfig.PulselengthAnalogue;
      break;
    case MODE_NOISE:
      fChipConfig[i].StrobeBLength = fGenConfig.StrobeBlengthStandard;
      fChipConfig[i].PulseLength   = 0;
      break;
    case MODE_SOURCE:
      fChipConfig[i].StrobeBLength = fGenConfig.StrobeBlengthSource;
      fChipConfig[i].PulseLength   = 0;
      break;
    case MODE_UNKNOWN:
      std::cout << "Unknown measurement mode" << std::endl;
      break;
    }
  }
  for (int i = 0; i < fNBoards; i ++) {
    switch (AMode) {
    case MODE_DIGITAL:
      fBoardConfig[i].PulseDelay    = fGenConfig.PulsedelayDigital;
      fBoardConfig[i].PulseLength   = fGenConfig.PulselengthDigital;
      break;
    case MODE_ANALOGUE:
      fBoardConfig[i].PulseDelay    = fGenConfig.PulsedelayAnalogue;
      fBoardConfig[i].PulseLength   = fGenConfig.PulselengthAnalogue;
      break;
    case MODE_NOISE:
      fBoardConfig[i].PulseDelay    = 0;
      fBoardConfig[i].PulseLength   = 0;
      break;
    case MODE_SOURCE:
      fBoardConfig[i].PulseDelay    = 0;
      fBoardConfig[i].PulseLength   = 0;
      break;
    case MODE_UNKNOWN:
      std::cout << "Unknown measurement mode" << std::endl;
      break;
    }
  }

}


void TConfig::ReadDDR(int index, const char *Line)
{
  int  Value;
  bool DDR;
  ReadIntParameter(Line, &Value);
  DDR = (bool) Value;
  fChipConfig [index].DisableDDR = !DDR;
  fBoardConfig[index].EnableDDR  = DDR;
}


void TConfig::ReadDataPort(int index, const char *Line)
{
  if ((fChipConfig [index].ChipType == DUT_PALPIDEFS3 || fChipConfig [index].ChipType == DUT_PALPIDEFS4) && (strcmp(Line, "PARALLEL"))) {
    std::cout << "Warning, pALPIDE-3, forcing data port to PARALLEL" << std::endl;
    fBoardConfig[index].DataPort = PORT_PARALLEL;
    return;
  }

  if (!strcmp(Line, "PARALLEL")) {
    fBoardConfig[index].DataPort = PORT_PARALLEL;
  }
  else if (!strcmp(Line, "SERIAL")) {
    fBoardConfig[index].DataPort = PORT_SERIAL;
  }
  else {
    std::cout << "Error, unknown port type " << Line << ", setting to parallel" << std::endl;
    fBoardConfig[index].DataPort = PORT_PARALLEL;
  }
}


void TConfig::ReadIntParameter (const char *Line, int *Address)
{
  sscanf (Line,"%d", Address);
  //std::cout << "Found parameter value " << *Address << std::endl;
}


void TConfig::ReadDaqBoardVersion (int index, const char *Line)
{
  int version = 3;
  if (!strcmp(Line, "V3")) {
    version = 3;
  }
  else if (!strcmp(Line, "V2")) {
    version = 2;
  }
  else if (!strcmp(Line, "V1")) {
    version = 1;
  }
  fBoardConfig[(fType == TYPE_TELESCOPE)?index:0].BoardVersion = version;
}


void TConfig::ReadChipType (int index, const char *Line)
{
  TDUTType type;

  if (!strcmp(Line, "PALPIDE1")) {
    type = DUT_PALPIDEFS1;
  }
  else if (!strcmp(Line, "PALPIDE2")) {
    type = DUT_PALPIDEFS2;
  }
  else if (!strcmp(Line, "PALPIDE3")) {
    type = DUT_PALPIDEFS3;
  }
  else if (!strcmp(Line, "PALPIDE4")) {
    type = DUT_PALPIDEFS4;
  }
  else {
    std::cout << "Warning, unknown DUT type found: " << Line << std::endl;
    type = DUT_UNKNOWN;
  }
  SetChipType(index, type);
}


void TConfig::ReadReadoutMode (int index, const char *Line)
{
  if (!strcmp(Line, "A")) {
    fChipConfig[index].ReadoutMode = MODE_ALPIDE_READOUT_A;
  }
  else if (!strcmp(Line, "B")) {
    fChipConfig[index].ReadoutMode = MODE_ALPIDE_READOUT_B;
  }
  else {
    std::cout << "Warning, unknown readout mode: " << Line << ", ignored." << std::endl;
  }
}


void TConfig::ReadMeasMode (const char *Line)
{
  TMeasurementMode mode;

  if (!strcmp(Line, "DIGITAL")) {
    mode = MODE_DIGITAL;
  }
  else if (!strcmp(Line, "ANALOGUE")) {
    mode = MODE_ANALOGUE;
  }
  else if (!strcmp(Line, "SOURCE")) {
    mode = MODE_SOURCE;
  }
  else if (!strcmp(Line, "NOISE")) {
    mode = MODE_NOISE;
  }
  else {
    std::cout << "Warning, unknown measurement mode found: " << Line << std::endl;
    mode = MODE_UNKNOWN;
  }
  fMode = mode;
}


TDeviceType TConfig::ReadDeviceType (const char *Line) {
  TDeviceType type = TYPE_UNKNOWN;
  if (!strcmp (Line, "CHIP")) {
    type = TYPE_CHIP;
  }
  else if (!strcmp(Line, "TELESCOPE")) {
    type = TYPE_TELESCOPE;
  }
  else if (!strcmp(Line, "MODULE")) {
    type = TYPE_MODULE;
  }
  else {
    std::cout << "Error, unknown setup type found: " << Line << std::endl;
    exit (EXIT_FAILURE);
  }
  return type;
}


void TConfig::WriteChipConfig (const char *fName, int index) {
  FILE *fp = fopen ("fname","a");
  WriteChipConfig(fp, index);
  fclose (fp);
}


void TConfig::WriteBoardConfig (const char *fName, int index, bool Full) {
  FILE *fp = fopen ("fname","a");
  WriteBoardConfig(fp, index, Full);
  fclose (fp);
}


void TConfig::WriteChipConfig (FILE *fp, int index) {
  for (int i = 0; i < fChipConfig[index].GetNDACS(); i++) {
    fprintf(fp, "%s\t%d\n", fChipConfig[index].GetDACName(i), fChipConfig[index].DACS[i]);
  }
  if (fChipConfig[index].ReadoutMode == MODE_ALPIDE_READOUT_A) {
    fprintf(fp, "READOUTMODE\t%c\n", 'A');
  }
  else if (fChipConfig[index].ReadoutMode == MODE_ALPIDE_READOUT_B) {
    fprintf(fp, "READOUTMODE\t%c\n", 'B');
  }
  fprintf(fp, "STROBEBLENGTH\t%d\n", fChipConfig[index].StrobeBLength);
  fprintf(fp, "STROBEBDELAY\t%d\n",  fChipConfig[index].StrobeBDelay);
  fprintf(fp, "READOUTDELAY\t%d\n",  fChipConfig[index].ReadoutDelay);
  if (strcmp (fChipConfig[index].MaskFile, "")) {
    fprintf(fp, "MASKFILE\t%s\n",      fChipConfig[index].MaskFile);
  }
  else {
    fprintf(fp, "MASKFILE\t---\n");
  }
  for (int i = 0; i < fChipConfig[index].fStuckPixels.size(); i ++) {
    fprintf(fp, "STUCK\t%d/%d/%d\n", fChipConfig[index].fStuckPixels.at(i).region,
                                     fChipConfig[index].fStuckPixels.at(i).doublecol,
                                     fChipConfig[index].fStuckPixels.at(i).address);
  }
}


void TConfig::WriteBoardConfig (FILE *fp, int index, bool Full) {
  fprintf(fp, "STROBELENGTH\t%d\n", fBoardConfig[index].StrobeLength);
  fprintf(fp, "TRIGGERDELAY\t%d\n", fBoardConfig[index].TriggerDelay);
  fprintf(fp, "PULSEMODE   \t%d\n", fBoardConfig[index].PulseMode);
  fprintf(fp, "PULSELENGTH \t%d\n", fBoardConfig[index].PulseLength);
  fprintf(fp, "PULSEDELAY  \t%d\n", fBoardConfig[index].PulseDelay);

  if (Full) {
    // Write all the rest
  }
}


void TConfig::WriteScanConfig (FILE *fp)
{
  fprintf(fp, "NEvents\t%d\n", fScanConfig.NEvents);
}


void TConfig::WriteCondData (FILE *fp)
{
  fprintf(fp, "SWGitCommit\t%s\n", fCondData.SWVersion);
  fprintf(fp, "FWVersion \t%x\n\n", fCondData.FWVersion);

  fprintf(fp, "InitialTemp \t%.1f\n", fCondData.TempInit );
  fprintf(fp, "FinalTemp \t%.1f\n",   fCondData.TempFinal );
  fprintf(fp, "OnChipTemp\t%.3f\n",   fCondData.TempVoltage);
  fprintf(fp, "IDDD_PowerOn\t%.1f\n", fCondData.IDDDPowerOn);
  fprintf(fp, "IDDA_PowerOn\t%.1f\n", fCondData.IDDAPowerOn);
  fprintf(fp, "IDDD_Init\t%.1f\n",    fCondData.IDDD);
  fprintf(fp, "IDDA_Init\t%.1f\n",    fCondData.IDDA);
}


void TConfig::WriteToFile (const char *fName) {
  FILE *fp = fopen(fName, "w");
  WriteToFile(fp);
  fclose(fp);
}


void TConfig::WriteToFile (FILE *fp) {
  WriteCondData(fp);
  fprintf(fp, "\n");
  WriteScanConfig(fp);
  fprintf(fp,"\n");
  if (fType == TYPE_CHIP) {
    WriteChipConfig  (fp, 0);
    fprintf (fp, "\n");
    WriteBoardConfig (fp, 0);
  }
  else if (fType == TYPE_TELESCOPE) {
    for (int i = 0; i < fNChips; i++) {
      fprintf(fp, "=========\n");
      fprintf(fp, "Plane %d:\n", i);
      fprintf(fp, "=========\n\n");
      WriteChipConfig  (fp, i);
      fprintf (fp, "\n");
      WriteBoardConfig (fp, i);
      fprintf (fp, "\n\n");
    }
  }
  else if (fType == TYPE_MODULE) {
    for (int i = 0; i < fNChips; i++) {
      fprintf(fp, "========\n");
      fprintf(fp, "Chip %d:\n", i);
      fprintf(fp, "========\n\n");
      WriteChipConfig  (fp, i);
      fprintf (fp, "\n");
    }
    fprintf(fp, "==========\n");
    fprintf(fp, "DAQ Board:\n");
    fprintf(fp, "==========\n\n");
    WriteBoardConfig (fp, 0);
    fprintf (fp, "\n\n");
  }


}


TChipConfig::TChipConfig () {
  DACS = new int[NDACS];
  for (int i = 0; i < NDACS; i++) {
    DACS[i] = -1;
  }
}


int TChipConfig::GetDACIndex (const char *Name)
{
  for (int i = 0; i < NDACS; i ++) {
    if (!strcmp(Name, DACNames[i])) return i;
  }
  return -1;
}

int TChipConfig::GetDACValue (const char *Name)
{
  int Index = GetDACIndex (Name);
  if (Index >= 0) return DACS[Index];
  return -1;
}


bool TChipConfig::SetDACValue (const char *Name, int Value)
{
  int Index = GetDACIndex (Name);
  if (Index >= 0) {
    DACS[Index] = Value;
    return true;
  }
  else {
    std::cout << "Index for " << Name << " = " << Index << std::endl;
  }
  return false;
}
