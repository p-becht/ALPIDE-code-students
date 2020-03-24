#include "TSystem.h"

#include "TLeaf.h"
#include "TBranch.h"
#include "TString.h"

#include "TpAScan.h"
#include "TpAAnalysis.h"

ClassImp(TpAScan)

//_____________________________________________________________________________
TpAScan::TpAScan(TScanType    AScanType,
                 TpAAnalysis *AAnalysis,
                 bool         Qualification,
                 const char  *Comment,
                 int          version) :
TObject(),
fAnalysis(AAnalysis),
fType(AScanType)
{
//
//  TpAScan::TpAScan
//
  InitConfig(Qualification, version, Comment);
}

//_____________________________________________________________________________
TpAScan::TpAScan(TpAAnalysis *AAnalysis, TConfig AConfig) :
TObject(),
fConfig(AConfig),
fAnalysis(AAnalysis),
fType((TScanType)AConfig.SCANTYPE)
{
//
//  TpAScan::TpAScan
//
}

//_____________________________________________________________________________
TpAScan::TpAScan(const TpAScan &src) :
TObject(src),
fConfig(src.fConfig),
fAnalysis(src.fAnalysis),
fType(src.fType)
{
//
// TpAScan::TpAScan
//
}

//_____________________________________________________________________________
TpAScan& TpAScan::operator=(const TpAScan &src)
{
//
// TpAScan::operator=
//

  if (&src==this) return *this;

  TObject::operator=(src);

  fConfig   = src.fConfig;
  fAnalysis = src.fAnalysis;
  fType     = src.fType;

  return *this;
}

//_____________________________________________________________________________
TpAScan::~TpAScan()
{
//
// TpAScan::~TpAScan
//
}

//_____________________________________________________________________________
void TpAScan::Init(const char *dataFile)
{
  TString sCfgFile = "Dummy";
  if (fConfig.SCANTYPE!=SCANTYPE_DACSCAN) {
    new TSystem();
    FindIdentifier(dataFile);
    sCfgFile = Form("%s/ScanConfig_%s.cfg",gSystem->DirName(dataFile),fConfig.IDENTSTRING);
    Info(__func__, "Cfg file = %s", sCfgFile.Data());
  }
//=============================================================================

  if (!(ReadConfigFile(sCfgFile.Data()))) {
    Fatal(__func__, "Reading config file (%s) failed!!!", sCfgFile.Data());
    exit(0);
  }
//=============================================================================

  if (!(ReadDataFile(dataFile))) {
    Fatal(__func__, "Reading data file (%s) failed!!!", dataFile);
    exit(0);
  }
//=============================================================================

  return;
}

//_____________________________________________________________________________
bool TpAScan::ReadConfigFile(const char *fName)
{
//
//  TpAScan::ReadConfigFile
//

  if (fConfig.SCANTYPE!=SCANTYPE_DACSCAN) if (strcmp(fName,"")) {
    FILE *fp = fopen(fName, "r"); if (!fp) return false;

    char string[128];
    while (fgets(string,128,fp)!=NULL) ParseLine(string);
    fclose(fp);
  }
//=============================================================================

// Fill config tree

  TBranch *bConfig = fAnalysis->GetScanConfigs()->GetBranch("Configs");

  if (!bConfig) {
    std::cout << "Config data not found, creating new branch" << std::endl;

    TString sCfgList = "mask[80]/C:comment[80]/C:git[50]/C:id[20]/C:name[10]/C";
    sCfgList += ":TempInit/F:TempFinal:OnChipTemp:IDDD:IDDA:IDDDPowerOn:IDDAPowerOn";
    sCfgList += ":Firmware/I:type/I:NEvts/I";
    sCfgList += ":VAUX:VRESET:VRESETD:VRESETP:VCASN:VCASN2:VCASP:VCLIP:VPULSEL:VPULSEH:IRESET:IAUX2:IBIAS:IDB:ITHR";
    sCfgList += ":STROBEBLENGTH:STROBEBDELAY:READOUTDELAY:STROBELENGTH:PULSEMODE:PULSELENGTH:PULSEDELAY:Quali";
    sCfgList += ":READOUTMODE/B:Dummy";

    bConfig = fAnalysis->GetScanConfigs()->Branch("Configs", (void*)(&fConfig), sCfgList.Data());
  } else {
    bConfig->SetAddress(&fConfig);
  }

  bConfig->Fill();
//=============================================================================

  return true;
}

//_____________________________________________________________________________
void TpAScan::ParseLine(const char *Line)
{
  if (Line[0]=='\n') return;  // empty line
//=============================================================================

  char Param[20], Rest[80];
  sscanf(Line, "%s\t%s", Param, Rest);
  if (!strcmp(Param,"VAUX")) sscanf(Rest, "%d", &(fConfig.VAUX));
  else if (!strcmp(Param,"VRESET"))        sscanf(Rest, "%d", &(fConfig.VRESET));
  else if (!strcmp(Param,"VRESETD"))       sscanf(Rest, "%d", &(fConfig.VRESETD));
  else if (!strcmp(Param,"VRESETP"))       sscanf(Rest, "%d", &(fConfig.VRESETP));
  else if (!strcmp(Param,"VCASN"))         sscanf(Rest, "%d", &(fConfig.VCASN));
  else if (!strcmp(Param,"VCASN2"))        sscanf(Rest, "%d", &(fConfig.VCASN2));
  else if (!strcmp(Param,"VCASP"))         sscanf(Rest, "%d", &(fConfig.VCASP));
  else if (!strcmp(Param,"VCLIP"))         sscanf(Rest, "%d", &(fConfig.VCLIP));
  else if (!strcmp(Param,"VPULSEL"))       sscanf(Rest, "%d", &(fConfig.VPULSEL));
  else if (!strcmp(Param,"VPULSEH"))       sscanf(Rest, "%d", &(fConfig.VPULSEH));
  else if (!strcmp(Param,"IRESET"))        sscanf(Rest, "%d", &(fConfig.IRESET));
  else if (!strcmp(Param,"IAUX2"))         sscanf(Rest, "%d", &(fConfig.IAUX2));
  else if (!strcmp(Param,"IBIAS"))         sscanf(Rest, "%d", &(fConfig.IBIAS));
  else if (!strcmp(Param,"IDB"))           sscanf(Rest, "%d", &(fConfig.IDB));
  else if (!strcmp(Param,"ITHR"))          sscanf(Rest, "%d", &(fConfig.ITHR));
  else if (!strcmp(Param,"NEvents"))       sscanf(Rest, "%d", &(fConfig.NEvents));
  else if (!strcmp(Param,"STROBEBLENGTH")) sscanf(Rest, "%d", &(fConfig.STROBEBLENGTH));
  else if (!strcmp(Param,"STROBEBDELAY"))  sscanf(Rest, "%d", &(fConfig.STROBEBDELAY));
  else if (!strcmp(Param,"READOUTDELAY"))  sscanf(Rest, "%d", &(fConfig.READOUTDELAY));
  else if (!strcmp(Param,"STROBELENGTH"))  sscanf(Rest, "%d", &(fConfig.STROBELENGTH));
  else if (!strcmp(Param,"PULSEMODE"))     sscanf(Rest, "%d", &(fConfig.PULSEMODE));
  else if (!strcmp(Param,"PULSELENGTH"))   sscanf(Rest, "%d", &(fConfig.PULSELENGTH));
  else if (!strcmp(Param,"PULSEDELAY"))    sscanf(Rest, "%d", &(fConfig.PULSEDELAY));
  else if (!strcmp(Param,"InitialTemp"))   sscanf(Rest, "%f", &(fConfig.TempInit));
  else if (!strcmp(Param,"FinalTemp"))     sscanf(Rest, "%f", &(fConfig.TempFinal));
  else if (!strcmp(Param,"IDDD_PowerOn"))  sscanf(Rest, "%f", &(fConfig.IDDDPowerOn));
  else if (!strcmp(Param,"OnChipTemp"))    sscanf(Rest, "%f", &(fConfig.OnChipTemp));
  else if (!strcmp(Param,"IDDA_PowerOn"))  sscanf(Rest, "%f", &(fConfig.IDDAPowerOn));
  else if (!strcmp(Param,"IDDD_Init"))     sscanf(Rest, "%f", &(fConfig.IDDD));
  else if (!strcmp(Param,"IDDA_Init"))     sscanf(Rest, "%f", &(fConfig.IDDA));
  else if (!strcmp(Param,"SWGitCommit"))   sprintf(fConfig.GitCommit, "%s", Rest);
  else if (!strcmp(Param,"MASKFILE"))      sprintf(fConfig.MASK,      "%s", Rest);
  else if (!strcmp(Param,"STUCK")) {
    int reg, dcol, address;
    sscanf(Rest, "%d/%d/%d", &reg, &dcol, &address);
    fAnalysis->AddStuckPixel(reg, dcol, address);
  } else if (!strcmp(Param,"READOUTMODE")) {
    char dummy;
    sscanf(Rest, "%c", &dummy);
    fConfig.READOUTMODE = (Char_t)dummy;
//  std::cout << "READOUTMODE = " << fConfig.READOUTMODE << std::endl;
  } else if (!strcmp(Param,"FWVersion")) {
    sscanf(Rest, "%x", &(fConfig.FWVERSION));
  } else {
    Warning(__func__, "Ignored line: %s\t%s !", Param, Rest);
  }
//=============================================================================

  return;
}

//_____________________________________________________________________________
void TpAScan::FindIdentifier(const char *fName)
{
  char String[1024]; sprintf(String, "%s", fName);
  char   date[1024]; sprintf(date,   "%s", strtok(String, "_."));
  char   time[1024]; sprintf(time,   "%s", strtok(NULL, "_."));
  char suffix[1024]; sprintf(suffix, "%s", strtok(NULL, "_."));

  char *test = strtok(NULL, "._");

  while (test!=NULL) {
    sprintf(date,   "%s", time);
    sprintf(time,   "%s", suffix);
    sprintf(suffix, "%s", test);
    test = strtok(NULL, "._");
  }

  sprintf(fConfig.IDENTSTRING, "%s_%s", date, time);
  return;
}

//_____________________________________________________________________________
void TpAScan::InitConfig(bool Qualification, int version, const char *Comment)
{
  sprintf(fConfig.MASK, "None");
  sprintf(fConfig.Comment, "%s", Comment);
  sprintf(fConfig.GitCommit," ");
  sprintf(fConfig.IDENTSTRING, "---");

  if (IsDigitalScan())   sprintf(fConfig.SCANNAME, "Digital");
  if (IsAnalogueScan())  sprintf(fConfig.SCANNAME, "Analogue");
  if (IsThresholdScan()) sprintf(fConfig.SCANNAME, "Threshold");
  if (IsSourceScan())    sprintf(fConfig.SCANNAME, "Source");
  if (IsNoiseoccScan())  sprintf(fConfig.SCANNAME, "Noiseocc");
  if (IsDACScan())       sprintf(fConfig.SCANNAME, "DACScan");

  fConfig.TempInit    = 0.;
  fConfig.TempFinal   = 0.;
  fConfig.OnChipTemp  = 0.;
  fConfig.IDDD        = 0.;
  fConfig.IDDA        = 0.;
  fConfig.IDDDPowerOn = 0.;
  fConfig.IDDAPowerOn = 0.;

  fConfig.FWVERSION     = 0;
  fConfig.SCANTYPE      = (int)fType;
  fConfig.NEvents       = 0;
  fConfig.VAUX          = 0;
  fConfig.VRESET        = 0;
  fConfig.VRESETD       = 0;
  fConfig.VRESETP       = 0;
  fConfig.VCASN         = 0;
  fConfig.VCASN2        = 0;
  fConfig.VCASP         = 0;
  fConfig.VCLIP         = 0;
  fConfig.VPULSEL       = 0;
  fConfig.VPULSEH       = 0;
  fConfig.IRESET        = 0;
  fConfig.IAUX2         = 0;
  fConfig.IBIAS         = 0;
  fConfig.IDB           = 0;
  fConfig.ITHR          = 0;
  fConfig.STROBEBLENGTH = 0;
  fConfig.STROBEBDELAY  = 0;
  fConfig.READOUTDELAY  = 0;
  fConfig.STROBELENGTH  = 0;
  fConfig.PULSEMODE     = 0;
  fConfig.PULSELENGTH   = 0;
  fConfig.PULSEDELAY    = 0;

  fConfig.Qualification = (Qualification ? 1 : 0);

  fConfig.READOUTMODE = 42;
  fConfig.Dummy       = 42;

  return;
}
