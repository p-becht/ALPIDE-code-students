#include <iostream>
#include <stdio.h>
#include "TAnalysis.h"
#include <TLeaf.h>
#include <TCanvas.h>
#include <TGraph.h>

const int   NSEC        = 8;
const int   NOISECUT    = 10;
const int   NHITDIGITAL = 50;
const float CHISQCUT    = 4.0;

//////////////////////////////////////////////////////////////////////
//                                                                  //
//       TAnalysis I, creation, re-opening, adding scans etc.       //
//                                                                  //
//////////////////////////////////////////////////////////////////////


TAnalysis::TAnalysis(const char *fName) {
  fRootFile = new TFile(fName, "UPDATE");

  fRootFile->GetObject("ScanData",    fScanData);
  fRootFile->GetObject("ScanConfigs", fScanConfigs);
  fRootFile->GetObject("DACData",     fDACData);
  fRootFile->GetObject("NoisyPixels", fNoisyPixels);
  fRootFile->GetObject("StuckPixels", fStuckPixels);

  if (!fScanConfigs) { 
    if ((fScanData) || (fDACData)) {   // if there is no config tree there should also be no data tree
      std::cout << "Corrupt data file (no config, but data tree). Exiting..." << std::endl;
      exit(0);
    }
    std::cout << "Config tree not found, generating new one" << std::endl;
    fScanConfigs = new TTree("ScanConfigs", "Tree containing the scan configurations");
  }

  if (!fScanData) {
    std::cout << "Data tree not found, generating new one" << std::endl;
    CreateDataTree();
  }

  if (!fDACData) {
    std::cout << "DAC Data tree not found, generating new one" << std::endl;
    fDACData = new TTree("DACData", "Tree containing the DAC scan data");    
  }

  if (!fNoisyPixels) {
    std::cout << "Noisy pixel tree not found, generating new one" << std::endl;
    fNoisyPixels = new TTree("NoisyPixels", "Tree containing noisy pixels");    
  }

  if (!fStuckPixels) {
    std::cout << "Stuck pixel tree not found, generating new one" << std::endl;
    fStuckPixels = new TTree("StuckPixels", "Tree containing stuck-on pixels");    
  }

}


//Creates a new data tree containing only the pixel addresses
void TAnalysis::CreateDataTree() {
  struct address_t {int col; int row;};
  address_t address;

  fScanData = new TTree("ScanData", "Tree containing the scan data");

  fScanData->Branch("Pixel", &address.col, "col/I:row/I");
  for (int ipix = 0; ipix < NPIX; ipix ++) {
    EntryToColRow(ipix, address.col, address.row);
    fScanData->Fill();
  }
}


TScan *TAnalysis::AddScan(TScanType AType, const char *dataFile, const char *cfgFile, bool Qualification, const char *Comment) {
  TScan *scan = 0;

  switch (AType) 
    {
    case SCANTYPE_DIGITAL:
      //      scan = new TDigitalScan   (fScanData, fScanConfigs, GetNScans(), dataFile, Qualification, Comment);
      scan = new TDigitalScan   (this, GetNScans(), dataFile, Qualification, Comment);
      break;
    case SCANTYPE_THRESHOLD:
      scan = new TThresholdScan (this, GetNScans(), dataFile, cfgFile, Qualification, Comment);
      break;
    case SCANTYPE_SOURCE:
      scan = new TSourceScan    (this, GetNScans(), dataFile, cfgFile, Qualification, Comment);
      break;
    case SCANTYPE_NOISEOCC:
      scan = new TNoiseocc      (this, GetNScans(), dataFile, cfgFile, Qualification, Comment);
      break;
    case SCANTYPE_DACSCAN:
      scan = new TDACScan       (this, GetNScans(), dataFile, Qualification, Comment);
      break;
    case SCANTYPE_ANALOGUE:
      scan = new TAnalogueScan  (this, GetNScans(), dataFile, Qualification, Comment);
      break;
    default:
      std::cout << "Unknown scan type " << AType << std::endl;
    }

  Write();
  return scan;

}


TScan *TAnalysis::AddDigitalScan  (const char *dataFile, bool Qualification, const char *Comment)
{
  return AddScan(SCANTYPE_DIGITAL, dataFile, "", Qualification, Comment);
}


TScan *TAnalysis::AddAnalogueScan (const char *dataFile, bool Qualification, const char *Comment)
{
  return AddScan(SCANTYPE_ANALOGUE, dataFile, "", Qualification, Comment);
}


TScan *TAnalysis::AddThresholdScan(const char *dataFile, const char *cfgFile, bool Qualification, const char *Comment)
{
  return AddScan(SCANTYPE_THRESHOLD, dataFile, cfgFile, Qualification, Comment);
}


TScan *TAnalysis::AddSourceScan   (const char *dataFile, const char *cfgFile, bool Qualification, const char *Comment)
{
  return AddScan(SCANTYPE_SOURCE, dataFile, cfgFile, Qualification, Comment);
}


TScan *TAnalysis::AddDACScan      (const char *dataFile, bool Qualification, const char *Comment)
{
  return AddScan(SCANTYPE_DACSCAN, dataFile, "", Qualification, Comment);
}


TScan *TAnalysis::AddNoiseoccScan (const char *dataFile, const char *cfgFile, bool Qualification, const char *Comment)
{
  return AddScan(SCANTYPE_NOISEOCC, dataFile, cfgFile, Qualification, Comment);
}



bool TAnalysis::ReadConfig (const char *AScan, TConfig *AConfig) {
  TConfig Config;
  bool    Found = false;
  TBranch *bConfig = fScanConfigs->GetBranch("Configs");
  if (!bConfig) {
    std::cout << "Problem reading configuration tree" << std::endl;
    return false;
  } 

  bConfig->SetAddress(&Config);
  int nEntries = bConfig->GetEntries();
  
  for (int i = 0; (i < nEntries) & !Found; i++) {
    bConfig->GetEntry(i);
    if (strcmp (AScan, Config.SCANNAME) == 0) Found = true;
  }
  *AConfig = Config;
  //DumpConfiguration (*AConfig);
  return Found;
}


void TAnalysis::AddPixel (TTree *ATree, int AReg, int ADCol, int AAddress) {
  struct pixel_t {int reg; int dcol; int address;};
  pixel_t Pixel;
  Pixel.reg       = AReg;
  Pixel.dcol      = ADCol;
  Pixel.address   = AAddress;
  TBranch *bPixel = ATree->GetBranch("Pixels");

  if (!bPixel)  {
    std::cout << "Pixel branch not found, creating new one" << std::endl;
    bPixel = ATree->Branch("Pixels", (void*)(&Pixel), "Reg/I:DCol:Address");
  }
  else {
    bPixel->SetAddress(&Pixel);
  }
  bPixel->Fill();
}


TScan *TAnalysis::RetrieveScan(TScanType AType, const char *AScan) {
  TConfig Config;
  TScan  *scan = 0;
  
  if ((!ReadConfig (AScan, &Config)) || (Config.SCANTYPE != AType)) {
    std::cout << "Error: config not found or wrong scan type" << std::endl;
    return 0;
  }

  if (AType == SCANTYPE_THRESHOLD) scan = new TThresholdScan (this, Config);
  if (AType == SCANTYPE_DACSCAN)   scan = new TDACScan       (this, Config);
  if (AType == SCANTYPE_DIGITAL)   scan = new TDigitalScan   (this, Config);
  if (AType == SCANTYPE_ANALOGUE)  scan = new TAnalogueScan  (this, Config);
  if (AType == SCANTYPE_SOURCE)    scan = new TSourceScan    (this, Config);
  if (AType == SCANTYPE_NOISEOCC)  scan = new TNoiseocc      (this, Config);
  return scan;
}


void TAnalysis::Map(const char *AScan, const char *ALeaf) {
  TH2F  *hMap = new TH2F("hMap", ALeaf, 1024, -.5, 1023.5, 512, -.5, 511.5);
  float  Value;
  struct address_t {int col; int row;};
  address_t address;

  TBranch *bPixel = fScanData->GetBranch ("Pixel");
  TBranch *bScan  = fScanData->GetBranch (AScan);
  if ((!bPixel) || (!bScan)) {
    std::cout << "Problem reading branches" << std::endl;
    return;
  } 

  TLeaf *leaf = bScan->GetLeaf(ALeaf);   
  std::cout << "Maximum = " << leaf->GetMaximum() << ", minimum = " << leaf->GetMinimum() <<std::endl;

  if (!leaf) {
    std::cout << "Problem reading leaf" << std::endl;
    return;
  }
  bPixel->SetAddress (&address);
  leaf  ->SetAddress (&Value);
  int nEntries = bPixel->GetEntries();
  for (int i = 0; i < nEntries; i++) {
    bPixel->GetEntry(i);
    bScan ->GetEntry(i);
    leaf  ->GetValue(0);
    hMap  ->Fill(address.col, address.row, Value);
  }
  hMap->Draw("COLZ");
}


void TAnalysis::DumpConfiguration(TConfig AConfig) {
    std::cout << "Found config for " << AConfig.SCANNAME << std::endl;
    std::cout << "Scan type: " << AConfig.SCANTYPE << std::endl;
    std::cout << "Identifier " << AConfig.IDENTSTRING << std::endl;
    std::cout << "Git commit " << AConfig.GitCommit << std::endl;
    std::cout << "NEvents : " << AConfig.NEvents << std::endl;
    std::cout << "VAUX    = " << AConfig.VAUX << std::endl;
    std::cout << "VRESET  = " << AConfig.VRESET << std::endl;
    std::cout << "VCASN   = " << AConfig.VCASN << std::endl;
    std::cout << "VCASP   = " << AConfig.VCASP << std::endl;
    std::cout << "VPULSEL = " << AConfig.VPULSEL << std::endl;
    std::cout << "VPULSEH = " << AConfig.VPULSEH << std::endl;
    std::cout << "IRESET  = " << AConfig.IRESET << std::endl;
    std::cout << "IAUX2  = " << AConfig.IAUX2 << std::endl;
    std::cout << "IBIAS  = " << AConfig.IBIAS << std::endl;
    std::cout << "IDB  = " << AConfig.IDB << std::endl;
    std::cout << "ITHR  = " << AConfig.ITHR << std::endl;
    std::cout << "Mask  = " << AConfig.MASK << std::endl;
    std::cout << "Qualification = " << AConfig.Qualification << std::endl;
}


void TAnalysis::ListScan(TConfig AConfig) {
  if (AConfig.Qualification) std::cout << "* ";
  else std::cout << "  ";
  std::cout << AConfig.SCANNAME << ":  \t Scan type: " << AConfig.SCANTYPE << ",\t Id: " << AConfig.IDENTSTRING << ",\t" << AConfig.Comment << std::endl;
}


void TAnalysis::ListAllScans() {
  TConfig Config;
  TBranch *bConfig = fScanConfigs->GetBranch("Configs");
  if (!bConfig) {
    std::cout << "Problem reading configuration tree" << std::endl;
    return;
  } 

  std::cout << std::endl;
  bConfig->SetAddress(&Config);
  int nEntries = bConfig->GetEntries();
  
  for (int i = 0; i < nEntries; i++) {
    bConfig->GetEntry(i);
    ListScan(Config);
  }

  std::cout << std::endl << "(Scantypes: 0 = digital, 1 = analogue, 2 = threshold, 3 = source, 4 = noise occ., 5 = DAC scan)" << std::endl;
  std::cout << "(* = qualification scan)" << std::endl << std::endl;


}


void TAnalysis::DumpAllConfigurations() {
  TConfig Config;

  TBranch *bConfig = fScanConfigs->GetBranch("Configs");
  if (!bConfig) {
    std::cout << "Problem reading configuration tree" << std::endl;
    return;
  } 

  bConfig->SetAddress(&Config);
  int nEntries = bConfig->GetEntries();
  
  for (int i = 0; i < nEntries; i++) {
    bConfig->GetEntry(i);
    DumpConfiguration (Config);
  }

}


void TAnalysis::Summarise() 
{
  TConfig Config;

  TBranch *bConfig = fScanConfigs->GetBranch("Configs");
  if (!bConfig) {
    std::cout << "Problem reading configuration tree" << std::endl;
    return;
  } 

  std::cout << std::endl << "===================================" <<std::endl;
  std::cout <<              "            Chip Summary           " <<std::endl;
  std::cout << "===================================" << std::endl <<std::endl;

  int nEntries = bConfig->GetEntries();
  
  for (int i = 0; i < nEntries; i++) {
    bConfig->SetAddress(&Config);
    bConfig->GetEntry(i);
    TScanType type = (TScanType) Config.SCANTYPE;
    TScan    *scan = RetrieveScan(type, Config.SCANNAME);
    if (scan) scan->Summarise();
  }
}



//////////////////////////////////////////////////////////////////////
//                                                                  //
//                  TScan and derivatives                           //
//                                                                  //
//////////////////////////////////////////////////////////////////////


TScan::TScan (TScanType AScanType, TAnalysis *AAnalysis, bool Qualification, const char *Comment) {
  fType          = AScanType;
  fAnalysis      = AAnalysis;

  InitConfig(Qualification, Comment);

}


void TScan::InitConfig(bool Qualification, const char *Comment) {
  sprintf (fConfig.MASK, " ");
  sprintf (fConfig.GitCommit," ");
  sprintf (fConfig.Comment, "%s", Comment);
  fConfig.SCANTYPE = (int) fType;
  fConfig.VAUX = 0;
  fConfig.VRESET = 0;
  fConfig.VCASN = 0;
  fConfig.VCASP = 0;
  fConfig.VPULSEL = 0;
  fConfig.VPULSEH = 0;
  fConfig.IRESET = 0; 
  fConfig.IAUX2 = 0;
  fConfig.IBIAS = 0;
  fConfig.IDB = 0;
  fConfig.ITHR = 0;
  fConfig.Qualification = Qualification?1:0;
  fConfig.Dummy = 42;
}


void TScan::Init (int AScanNumber, const char *dataFile, const char *cfgFile) {
  char BranchName[10];
  sprintf(BranchName, "Scan%d", AScanNumber);
  std::cout << "In TScan::Init(), BranchName = " << BranchName << std::endl;
  FindIdentifier(dataFile);

  if (! ReadDataFile(dataFile, BranchName)) {
    std::cout << "Error reading data file " << dataFile << std::endl;
    exit(0);
  }

  if (! ReadConfigFile(cfgFile, BranchName)) {
    std::cout << "Error reading config file " << cfgFile << std::endl;
    exit(0);
  }
}


bool TScan::FindIdentifier(const char *fName) {
  char String[100];
  char suffix[100];
  char date[100];
  char time[100];
  char *test;

  sprintf (String, "%s", fName);
  sprintf(date, "%s", strtok(String, "_."));
  sprintf(time, "%s", strtok(NULL, "_."));
  sprintf(suffix, "%s", strtok(NULL, "_."));

  test = strtok(NULL, "._");

  while (test != NULL) {
    sprintf(date, "%s", time);
    sprintf(time, "%s", suffix);
    sprintf(suffix, "%s", test);
    test = strtok(NULL, "._");
  }
  sprintf(fConfig.IDENTSTRING, "%s_%s", date, time);
  return true;
}


void TScan::ParseLine (const char *Line) {
    char Param[20], Rest[80];
    if (Line[0] == '\n') return;  // empty line

    sscanf (Line, "%s\t%s", Param, Rest);

    if (!strcmp(Param,"VAUX")) {
      sscanf (Rest, "%d", &(fConfig.VAUX));
    }
    else if (!strcmp(Param,"VRESET")) {
      sscanf (Rest, "%d", &(fConfig.VRESET));      
    }
    else if (!strcmp(Param,"VRESETD")) {
      sscanf (Rest, "%d", &(fConfig.VRESETD));      
    }
    else if (!strcmp(Param,"VRESETP")) {
      sscanf (Rest, "%d", &(fConfig.VRESETP));      
    }
    else if (!strcmp(Param,"VCASN")) {
      sscanf (Rest, "%d", &(fConfig.VCASN));      
    }
    else if (!strcmp(Param,"VCASN2")) {
      sscanf (Rest, "%d", &(fConfig.VCASN2));      
    }
    else if (!strcmp(Param,"VCASP")) {
      sscanf (Rest, "%d", &(fConfig.VCASP));      
    }
    else if (!strcmp(Param,"VCLIP")) {
      sscanf (Rest, "%d", &(fConfig.VCLIP));      
    }
    else if (!strcmp(Param,"VPULSEL")) {
      sscanf (Rest, "%d", &(fConfig.VPULSEL));      
    }
    else if (!strcmp(Param,"VPULSEH")) {
      sscanf (Rest, "%d", &(fConfig.VPULSEH));      
    }
    else if (!strcmp(Param,"IRESET")) {
      sscanf (Rest, "%d", &(fConfig.IRESET));      
    }
    else if (!strcmp(Param,"IAUX2")) {
      sscanf (Rest, "%d", &(fConfig.IAUX2));      
    }
    else if (!strcmp(Param,"IBIAS")) {
      sscanf (Rest, "%d", &(fConfig.IBIAS));      
    }
    else if (!strcmp(Param,"IDB")) {
      sscanf (Rest, "%d", &(fConfig.IDB));      
    }
    else if (!strcmp(Param,"ITHR")) {
      sscanf (Rest, "%d", &(fConfig.ITHR));      
    }
    else if (!strcmp(Param,"NEvents")) {
      sscanf (Rest, "%d", &(fConfig.NEvents));
    }    
    else if (!strcmp(Param,"STROBEBLENGTH")) {
      sscanf (Rest, "%d", &(fConfig.STROBEBLENGTH));
    }    
    else if (!strcmp(Param,"STROBEBDELAY")) {
      sscanf (Rest, "%d", &(fConfig.STROBEBDELAY));
    }    
    else if (!strcmp(Param,"READOUTDELAY")) {
      sscanf (Rest, "%d", &(fConfig.READOUTDELAY));
    }    
    else if (!strcmp(Param,"STROBELENGTH")) {
      sscanf (Rest, "%d", &(fConfig.STROBELENGTH));
    }    
    else if (!strcmp(Param,"PULSEMODE")) {
      sscanf (Rest, "%d", &(fConfig.PULSEMODE));
    }    
    else if (!strcmp(Param,"PULSELENGTH")) {
      sscanf (Rest, "%d", &(fConfig.PULSELENGTH));
    }    
    else if (!strcmp(Param,"PULSEDELAY")) {
      sscanf (Rest, "%d", &(fConfig.PULSEDELAY));
    }    
    else if (!strcmp(Param,"InitialTemp")) {
      sscanf (Rest, "%f", &(fConfig.TempInit));
    }    
    else if (!strcmp(Param,"FinalTemp")) {
      sscanf (Rest, "%f", &(fConfig.TempFinal));
    }    
    else if (!strcmp(Param,"IDDD_PowerOn")) {
      sscanf (Rest, "%f", &(fConfig.IDDDPowerOn));
    }    
    else if (!strcmp(Param,"OnChipTemp")) {
      sscanf (Rest, "%f", &(fConfig.OnChipTemp));
    }
    else if (!strcmp(Param,"IDDA_PowerOn")) {
      sscanf (Rest, "%f", &(fConfig.IDDAPowerOn));
    }    
    else if (!strcmp(Param,"IDDD_Init")) {
      sscanf (Rest, "%f", &(fConfig.IDDD));
    }    
    else if (!strcmp(Param,"IDDA_Init")) {
      sscanf (Rest, "%f", &(fConfig.IDDA));
    }    
    else if (!strcmp(Param,"SWGitCommit")) {
      sprintf (fConfig.GitCommit, "%s", Rest);
    }    
    else if (!strcmp(Param,"MASKFILE")) {
      sprintf (fConfig.MASK, "%s", Rest);
    }    
    else if (!strcmp(Param,"STUCK")) {
      int reg, dcol, address;
      sscanf (Rest, "%d/%d/%d", &reg, &dcol, &address);
      fAnalysis->AddStuckPixel(reg, dcol, address);            
    }
    else if (!strcmp(Param,"READOUTMODE")) {
      char dummy;
      sscanf (Rest, "%c", &dummy);
      fConfig.READOUTMODE = (Char_t) dummy;
      //std::cout << "READOUTMODE = " << fConfig.READOUTMODE << std::endl;
    }
    else if (!strcmp(Param,"FWVersion")) {
      sscanf (Rest, "%x", &(fConfig.FWVERSION));
    }
    else {
      printf("Warning, ignored line: %s\t%s\n", Param, Rest);
    }
}


void TScan::DumpConfig() {
    std::cout << "Scan name: " << fConfig.SCANNAME << std::endl;
    std::cout << "Scan type: " << fConfig.SCANTYPE << std::endl;
    std::cout << "Qualification: " << fConfig.Qualification << std::endl;
    std::cout << "Identifier: " << fConfig.IDENTSTRING << std::endl;
    std::cout << "Git commit: " << fConfig.GitCommit << std::endl;
    std::cout << "Firmware: "   << std::hex << fConfig.FWVERSION << std::dec << std::endl;
    std::cout << std::endl;
    std::cout << "IDDD Poweron: " << fConfig.IDDDPowerOn << std::endl;
    std::cout << "IDDA Poweron: " << fConfig.IDDAPowerOn << std::endl;
    std::cout << "IDDD Init: " << fConfig.IDDD << std::endl;
    std::cout << "IDDA Init: " << fConfig.IDDA << std::endl;    
    std::cout << "Inital Temp: " << fConfig.TempInit << std::endl;
    std::cout << "Final Temp: " << fConfig.TempFinal << std::endl;
    std::cout << std::endl;
    std::cout << "NEvents : " << fConfig.NEvents << std::endl;
    std::cout << "VAUX    = " << fConfig.VAUX << std::endl;
    std::cout << "VRESET  = " << fConfig.VRESET << std::endl;
    std::cout << "VCASN   = " << fConfig.VCASN << std::endl;
    std::cout << "VCASP   = " << fConfig.VCASP << std::endl;
    std::cout << "VPULSEL = " << fConfig.VPULSEL << std::endl;
    std::cout << "VPULSEH = " << fConfig.VPULSEH << std::endl;
    std::cout << "IRESET  = " << fConfig.IRESET << std::endl;
    std::cout << "IAUX2  = " << fConfig.IAUX2 << std::endl;
    std::cout << "IBIAS  = " << fConfig.IBIAS << std::endl;
    std::cout << "IDB  = " << fConfig.IDB << std::endl;
    std::cout << "ITHR  = " << fConfig.ITHR << std::endl;
    std::cout << "Mask  = " << fConfig.MASK << std::endl;
    std::cout << "STROBEBLENGTH = " << fConfig.STROBEBLENGTH << std::endl;
    std::cout << "STROBEBDELAY = " << fConfig.STROBEBDELAY << std::endl;
    std::cout << "READOUTDELAY = " << fConfig.READOUTDELAY << std::endl;
    std::cout << "STROBELENGTH = " << fConfig.STROBELENGTH << std::endl;
    std::cout << "PULSEMODE = " << fConfig.PULSEMODE << std::endl;
    std::cout << "PULSELENGTH = " << fConfig.PULSELENGTH << std::endl;
    std::cout << "PULSEDELAY = " << fConfig.PULSEDELAY << std::endl;
    std::cout << "READOUTMODE = " << (char)(fConfig.READOUTMODE) << std::endl;
}



bool TScan::ReadConfigFile(const char *fName, const char *ABranch) {
  char     string[100];
  TBranch *bConfig;

  // initialise variables that are not in all config files. 
  sprintf(fConfig.SCANNAME, "%s", ABranch);
  fConfig.NEvents = 0;
  sprintf(fConfig.MASK, "None");

  std::cout << "In TScan::ReadConfigFile, branch name = " << ABranch << std::endl;

  // read the config file
  if (strcmp(fName, ""))  {    
    FILE *fp = fopen (fName, "r");
    if (!fp) return false;

    while (fgets(string, 100, fp) != NULL) {
      ParseLine(string);
    }
    fclose(fp);
  }

  // Fill config tree
  bConfig = fAnalysis->GetScanConfigs()->GetBranch("Configs");

  if (!bConfig)  {
    std::cout << "Config data not found, creating new branch" << std::endl;
    bConfig = fAnalysis->GetScanConfigs()->Branch("Configs", (void*)(&fConfig), "mask[80]/C:comment[80]/C:git[50]/C:id[20]/C:name[10]/C:TempInit/F:TempFinal:OnChipTemp:IDDD:IDDA:IDDDPowerOn:IDDAPowerOn:Firmware/I:type/I:NEvts/I:VAUX:VRESET:VRESETD:VRESETP:VCASN:VCASN2:VCASP:VCLIP:VPULSEL:VPULSEH:IRESET:IAUX2:IBIAS:IDB:ITHR:STROBEBLENGTH:STROBEBDELAY:READOUTDELAY:STROBELENGTH:PULSEMODE:PULSELENGTH:PULSEDELAY:Quali:READOUTMODE/B:Dummy");    
  }
  else {
    bConfig->SetAddress(&fConfig);
  }

  bConfig->Fill();

  return true;
}


void TScan::Map (const char *ALeaf) {
  TH2F  *hMap = new TH2F("hMap", ALeaf, 1024, -.5, 1023.5, 512, -.5, 511.5);
  float  Value;
  struct address_t {int col; int row;};
  address_t address;

  TBranch *bPixel = fAnalysis->GetScanData()->GetBranch ("Pixel");
  TBranch *bScan  = fAnalysis->GetScanData()->GetBranch (fConfig.SCANNAME);
  if ((!bPixel) || (!bScan)) {
    std::cout << "Problem reading branches" << std::endl;
    return;
  } 

  TLeaf *leaf = bScan->GetLeaf(ALeaf);   
  std::cout << "Maximum = " << leaf->GetMaximum() << ", minimum = " << leaf->GetMinimum() <<std::endl;

  if (!leaf) {
    std::cout << "Problem reading leaf" << std::endl;
    return;
  }
  bPixel->SetAddress (&address);
  leaf  ->SetAddress (&Value);
  int nEntries = bPixel->GetEntries();
  for (int i = 0; i < nEntries; i++) {
    bPixel->GetEntry(i);
    bScan ->GetEntry(i);
    leaf  ->GetValue(0);
    hMap  ->Fill(address.col, address.row, Value);
  }
  hMap->Draw("COLZ");
}


TThresholdScan::TThresholdScan (TAnalysis *AAnalysis, int AScanNumber, const char *dataFile, const char *cfgFile, bool Qualification, const char *Comment) 
  : TScan (SCANTYPE_THRESHOLD, AAnalysis, Qualification, Comment) {

  Init(AScanNumber, dataFile, cfgFile);
}


TThresholdScan::TThresholdScan (TAnalysis *AAnalysis, TConfig AConfig) 
  : TScan (SCANTYPE_THRESHOLD, AAnalysis, false, "") {
  fConfig = AConfig;
}


bool TThresholdScan::ReadDataFile (const char *fName, const char *ABranch) {
  int   dcol, address;
  float thresh, noise, chisq;
  int   row, col;

  struct result_t {float thresh; float noise; float chisq;};
  result_t result;

  std::cout << "In TThresholdScan::ReadDataFile, branch name = " << ABranch << std::endl;

  float allthresh[1024][512];
  float allnoise [1024][512];
  float allchisq [1024][512];

  for (int icol = 0; icol < 1024; icol ++) {
    for (int irow = 0; irow < 512; irow ++) {
      allthresh[icol][irow] = 0;
      allnoise [icol][irow] = 0;
      allchisq [icol][irow] = 0;
    }
  }

  FILE *fp = fopen (fName, "r");
  if (!fp) {
    std::cout << "ThresholdScan::ReadDataFile: Error reading data file " << fName << std::endl;
    return false;
  }

  while (fscanf(fp, "%d %d %f %f %f", &dcol, &address, &thresh, &noise, &chisq) == 5) {
    col = AddressToColumn (dcol , address);
    row = AddressToRow    (address);
    allthresh[col][row] = thresh;
    allnoise[col][row]  = noise;
    allchisq[col][row]  = chisq;
  }
  fclose(fp);

  TBranch *scanBranch = fAnalysis->GetScanData()->Branch(ABranch, &result.thresh, "thresh/F:noise/F:chisq/F");
  for (int ipix = 0; ipix < NPIX; ipix ++) {
    EntryToColRow (ipix, col, row);
    result.thresh = allthresh[col][row];
    result.noise  = allnoise [col][row];
    result.chisq  = allchisq [col][row];
    scanBranch->Fill();
  }

  return true;
}


int TThresholdScan::GetAverage(const char *ALeaf, float Av[], float RMS[])
{
  float  Value, Chisq;
  struct address_t {int col; int row;};
  int Sector, NPix[NSEC];
  address_t address;

  for (int i = 0; i < NSEC; i ++) {
    NPix[i] = 0;
    Av  [i] = 0;
    RMS [i] = 0;
  }

  TBranch *bPixel = fAnalysis->GetScanData()->GetBranch ("Pixel");
  TBranch *bScan  = fAnalysis->GetScanData()->GetBranch (fConfig.SCANNAME);
  if ((!bPixel) || (!bScan)) {
    std::cout << "Problem reading branches, bPixel = " << bPixel << ", bScan = " << bScan << ", Scan name = " << fConfig.SCANNAME << std::endl;
    return 0;
  } 

  TLeaf *leaf   = bScan->GetLeaf(ALeaf);   
  TLeaf *lChisq = bScan->GetLeaf ("chisq");

  if (!leaf) {
    std::cout << "Problem reading leaf" << std::endl;
    return 0;
  }
  bPixel->SetAddress (&address);
  leaf  ->SetAddress (&Value);
  lChisq->SetAddress (&Chisq);

  int nEntries = bPixel->GetEntries();
  for (int i = 0; i < nEntries; i++) {
    bPixel->GetEntry(i);
    bScan ->GetEntry(i);
    leaf  ->GetValue(0);
    lChisq->GetValue(0);
    Sector = address.col / (1024/NSEC);
    if (Chisq < CHISQCUT) {
      Av  [Sector] += Value;
      RMS [Sector] += Value*Value;
      NPix[Sector] ++;
    }
  }
  
  for (int i = 0; i < NSEC; i++) {
    Av[i] /= NPix[i];
    RMS[i] = sqrt(fabs ( Av[i] * Av[i] - RMS[i]/NPix[i]));
  }
  int NPixTotal = 0;
  for (int i = 0; i < NSEC; i++) {
    NPixTotal += NPix[i];
  }
  return NPixTotal;
}


void TThresholdScan::Summarise() {
  float thresh[NSEC], threshRMS[NSEC], noise[NSEC], noiseRMS[NSEC];
  GetAverage ("thresh", thresh, threshRMS);
  int NPix = GetAverage ("noise",  noise,  noiseRMS);

  std::cout << std::endl << "===============" << std::endl;
  std::cout << "--- " << fConfig.SCANNAME << " (Threshold Scan) " << fConfig.IDENTSTRING << std::endl;
  std::cout << "===============" << std::endl;

  std::cout << std::endl << "Thresholds:" << std::endl;
  for (int isec = 0; isec < NSEC; isec ++) {
    std::cout << "  Sector " << isec << ": " << thresh[isec] << " +- " << threshRMS[isec] << std::endl;
  }
  std::cout << std::endl << "Noise:" << std::endl;
  for (int isec = 0; isec < NSEC; isec ++) {
    std::cout << "  Sector " << isec << ": " << noise[isec] << " +- " << noiseRMS[isec] << std::endl;
  }
 
  std::cout << std::endl << "Pixels without threshold or with high chisq: " << 1024*512 - NPix << std::endl << std::endl;
}


THitmap::THitmap (TAnalysis *AAnalysis, TScanType AScantype, int AScanNumber, const char *dataFile, const char *cfgFile, bool Qualification, const char *Comment)
  : TScan (AScantype, AAnalysis, Qualification, Comment) {
  Init(AScanNumber, dataFile, cfgFile);
}


THitmap::THitmap (TAnalysis *AAnalysis, TScanType AScanType, TConfig AConfig) 
  : TScan (AScanType, AAnalysis, false, "") {
  fConfig = AConfig;
}


bool THitmap::ReadDataFile (const char *fName, const char *ABranch) {
  float allhits[1024][512];
  int   col, row, nhits, dcol, address;

  struct result_t {float nhits;};
  result_t result;

  for (int icol = 0; icol < 1024; icol ++) {
    for (int irow = 0; irow < 512; irow ++) {
      allhits [icol][irow] = 0;
    }
  }

  FILE *fp = fopen (fName, "r");
  if (!fp) {
    std::cout << "Hitmap::ReadDataFile: Error reading data file " << fName << std::endl;
    return false;
  }

  while (fscanf(fp, "%d %d %d", &dcol, &address, &nhits) == 3) {
    col = AddressToColumn (dcol , address);
    row = AddressToRow    (address);
    allhits[col][row] = (float) nhits;

  }
  fclose(fp);

  TBranch *scanBranch = fAnalysis->GetScanData()->Branch(ABranch, &result.nhits, "nhits/F");
  for (int ipix = 0; ipix < NPIX; ipix ++) {
    EntryToColRow (ipix, col, row);
    result.nhits  = allhits [col][row];
    scanBranch->Fill();
  }
  return true;
}


int THitmap::CountPixels(int TargetValue, int &Equal, int &Above, int &Below) 
{
  float Value;
  int NPix = 0;
  Equal = 0;
  Above = 0;
  Below = 0;

  TBranch *bScan  = fAnalysis->GetScanData()->GetBranch (fConfig.SCANNAME);
  if (!bScan) {
    std::cout << "Problem reading scan branch" << std::endl;
    return 0;
  } 

  TLeaf *leaf   = bScan->GetLeaf("nhits");   

  if (!leaf) {
    std::cout << "Problem reading leaf" << std::endl;
    return 0;
  }
  leaf  ->SetAddress (&Value);

  int nEntries = bScan->GetEntries();
  for (int i = 0; i < nEntries; i++) {
    bScan ->GetEntry(i);
    leaf  ->GetValue(0);
    if (Value == TargetValue) Equal ++;
    else if (Value < TargetValue) Below ++;
    else if (Value > TargetValue) Above ++;
    NPix ++;
  }
  return NPix;
}


TNoiseocc::TNoiseocc (TAnalysis *AAnalysis, int AScanNumber, const char *dataFile, const char *cfgFile, bool Qualification, const char *Comment)
  : THitmap (AAnalysis, SCANTYPE_NOISEOCC, AScanNumber, dataFile, cfgFile, Qualification, Comment) {
}


TNoiseocc::TNoiseocc (TAnalysis *AAnalysis, TConfig AConfig)
  : THitmap (AAnalysis, SCANTYPE_NOISEOCC, AConfig) {
}


void TNoiseocc::Summarise()
{
  int Above, Below, Equal, Noisy;
  CountPixels (0, Equal, Above, Below);
  Noisy = Above;

  std::cout << std::endl << "===============" << std::endl;
  std::cout << "-- " << fConfig.SCANNAME << " (Noise Occupancy Scan) " << fConfig.IDENTSTRING << std::endl;
  std::cout << "===============" << std::endl;
  std::cout << std::endl << Noisy << " noisy pixels" << std::endl;  
}


TSourceScan::TSourceScan (TAnalysis *AAnalysis, int AScanNumber, const char *dataFile, const char *cfgFile, bool Qualification, const char *Comment)
  : THitmap (AAnalysis, SCANTYPE_SOURCE, AScanNumber, dataFile, cfgFile, Qualification, Comment) {
}


TSourceScan::TSourceScan (TAnalysis *AAnalysis, TConfig AConfig)
  : THitmap (AAnalysis, SCANTYPE_SOURCE, AConfig) {
}

void TSourceScan::Summarise() 
{
  int Above, Below, Equal, Dead;
  CountPixels (0, Equal, Above, Below);
  Dead = 1024 * 512 - Above;

  std::cout << std::endl << "===============" << std::endl;
  std::cout << "--- " << fConfig.SCANNAME << " (Source Scan) " << fConfig.IDENTSTRING << std::endl;
  std::cout << "===============" << std::endl;
  std::cout << std::endl << Dead << " dead pixels" << std::endl;  
}


TDigitalScan::TDigitalScan (TAnalysis *AAnalysis, int AScanNumber, const char *dataFile,  bool Qualification, const char *Comment)
  : THitmap (AAnalysis, SCANTYPE_DIGITAL, AScanNumber, dataFile, "", Qualification, Comment) {
}


void TDigitalScan::Summarise() 
{
  int Above, Below, Equal, Dead;
  int TooMany, TooFew;
  int NPix = CountPixels (0, Equal, Above, Below);

  Dead = 1024 * 512 - Above;
  CountPixels (NHITDIGITAL, Equal, Above, Below);
  TooMany = Above;
  TooFew  = Below - Dead;

  std::cout << std::endl << "===============" << std::endl;
  std::cout << "--- "<< fConfig.SCANNAME << " (Digital Scan) " << fConfig.IDENTSTRING << std::endl;
  std::cout << "===============" << std::endl;
  std::cout << std::endl << Dead << " dead pixels" << std::endl;  
  std::cout << Above << " pixels with more than " << NHITDIGITAL << " hits" << std::endl;  
  std::cout << Below << " pixels with less than " << NHITDIGITAL << " hits" << std::endl;  
}


TDigitalScan::TDigitalScan (TAnalysis *AAnalysis, TConfig AConfig) 
  : THitmap (AAnalysis, SCANTYPE_DIGITAL, AConfig) {
}


TAnalogueScan::TAnalogueScan (TAnalysis *AAnalysis, int AScanNumber, const char *dataFile,  bool Qualification, const char *Comment)
  : THitmap (AAnalysis, SCANTYPE_ANALOGUE, AScanNumber, dataFile, "", Qualification, Comment) {
}


TAnalogueScan::TAnalogueScan (TAnalysis *AAnalysis, TConfig AConfig) 
  : THitmap (AAnalysis, SCANTYPE_ANALOGUE, AConfig) {
}


void TAnalogueScan::Summarise() 
{
  int Above, Below, Equal, Dead;
  int TooMany, TooFew;
  int NPix = CountPixels (0, Equal, Above, Below);

  Dead = 1024 * 512 - Above;
  CountPixels (NHITDIGITAL, Equal, Above, Below);
  TooMany = Above;
  TooFew  = Below - Dead;

  std::cout << std::endl << "===============" << std::endl;
  std::cout << "--- "<< fConfig.SCANNAME << " (Analogue Scan) " << fConfig.IDENTSTRING << std::endl;
  std::cout << "===============" << std::endl;
  std::cout << std::endl << Dead << " dead pixels" << std::endl;  
  std::cout << Above << " pixels with more than " << NHITDIGITAL << " hits" << std::endl;  
  std::cout << Below << " pixels with less than " << NHITDIGITAL << " hits" << std::endl;  
}


TDACScan::TDACScan (TAnalysis *AAnalysis, int AScanNumber, const char *dataPath, bool Qualification, const char *Comment) 
  : TScan (SCANTYPE_DACSCAN, AAnalysis, Qualification, Comment) {
  (void)AScanNumber;
  Init(dataPath);
}

TDACScan::TDACScan (TAnalysis *AAnalysis, TConfig AConfig) 
  : TScan (SCANTYPE_DACSCAN, AAnalysis, false, "") {
  fConfig = AConfig;
}

void TDACScan::Init (const char *dataPath) {
  ReadDataFile (dataPath);

  fConfig.SCANTYPE = (int) fType;

  sprintf(fConfig.IDENTSTRING, "---");

  ReadConfigFile ("dummy","dummy");

}

// probably not needed to derive from TScan

// loop over DACs
// for first DAC: create tree, read setting and measured value
// for other DACs: read only measured values, assuming the steps are the same...

bool TDACScan::ReadConfigFile (const char *fName, const char *ABranch) {
  TBranch *bConfig;
  (void)   fName;
  (void)   ABranch;

  sprintf(fConfig.SCANNAME, "DACScan");
  // initialise variables that are not in all config files. 
  fConfig.NEvents = 0;
  sprintf(fConfig.MASK, "None");
  bConfig = fAnalysis->GetScanConfigs()->GetBranch("Configs");

  if (!bConfig)  {
    std::cout << "Config data not found, creating new branch" << std::endl;
    bConfig = fAnalysis->GetScanConfigs()->Branch("Configs", (void*)(&fConfig), "mask[80]/C:comment[80]/C:git[50]/C:id[20]/C:name[10]/C:TempInit/F:TempFinal:OnChipTemp:IDDD:IDDA:IDDDPowerOn:IDDAPowerOn:Firmware/I:type/I:NEvts/I:VAUX:VRESET:VRESETD:VRESETP:VCASN:VCASN2:VCASP:VCLIP:VPULSEL:VPULSEH:IRESET:IAUX2:IBIAS:IDB:ITHR:STROBEBLENGTH:STROBEBDELAY:READOUTDELAY:STROBELENGTH:PULSEMODE:PULSELENGTH:PULSEDELAY:Quali:READOUTMODE/B:Dummy");    
  }
  else {
    bConfig->SetAddress(&fConfig);
  }
  bConfig->Fill();

  return true;
}


bool TDACScan::ReadDataFile (const char *dataPath) {
  if (!ReadDACFile(dataPath,"/VDACVCASN")) return false;
  if (!ReadDACFile(dataPath,"/VDACVCASP")) return false;
  if (!ReadDACFile(dataPath,"/VDACVPULSEL")) return false;
  if (!ReadDACFile(dataPath,"/VDACVPULSEH")) return false;
  if (!ReadDACFile(dataPath,"/IDACIRESET")) return false;
  //if (!ReadDACFile(dataPath,"IDACIAUX2")) return false;
  if (!ReadDACFile(dataPath,"/IDACIBIAS")) return false;
  if (!ReadDACFile(dataPath,"/IDACIDB")) return false;
  if (!ReadDACFile(dataPath,"/IDACITHR")) return false;
  // Dacs which are not present in every chip type (-> don't return false if not found)
  ReadDACFile(dataPath,"/VDACVAUX");
  ReadDACFile(dataPath,"/VDACVCASN2");
  ReadDACFile(dataPath,"/VDACVRESET");
  ReadDACFile(dataPath,"/VDACVRESETD");
  ReadDACFile(dataPath,"/VDACVRESETP");

  return true;
}


bool TDACScan::ReadDACFile (const char *dataPath, const char *DACName) {
  TBranch *bSetting = fAnalysis->GetDACData()->GetBranch ("Setting");
  TBranch *bValue;
  char     fName[200];
  int      setting;
  float    value;
  bool     firstDAC= false;

  sprintf(fName, "%s%s.dat", dataPath, DACName);
  // check if setting branch exists, ifnot create and read values
  if (!bSetting) {
    bSetting = fAnalysis->GetDACData()->Branch ("Setting", &setting, "setting/I");
    firstDAC = true;
  }
  bValue = fAnalysis->GetDACData()->Branch (DACName, &value, "value/F");

  FILE *fp = fopen (fName, "r");
  if (!fp) {
    std::cout << "Problem opening DAC data file " << fName << std::endl;
    return false;
  }
  while (fscanf(fp, "%d %f\n", &setting, &value) == 2) {
    if (firstDAC) bSetting->Fill();
    bValue->Fill();
  }
  fclose (fp);
  return true;
}


void TDACScan::PlotDAC (const char *DACName) {
  TBranch *bSetting = fAnalysis->GetDACData()->GetBranch ("Setting");
  TBranch *bValue   = fAnalysis->GetDACData()->GetBranch (DACName);
  int      Setting;
  float    Reading;
  float    Settings[256];
  float    Readings[256];

  if (!bSetting) {
    std::cout << "DAC Scan data not found in tree" << std::endl;
    return;
  }
  if (!bValue) {
    std::cout << "Data for " << DACName << " not found in tree" << std::endl;
    return;
  }
  
  int NEntries = bSetting->GetEntries();
  if (NEntries > 256) {
    std::cout << "Illegal number of entries in tree: " << NEntries << std::endl;
    return;
  }

  bSetting->SetAddress(&Setting);
  bValue  ->SetAddress(&Reading);

  for (int i = 0; i < NEntries; i++) {
    bSetting->GetEntry(i);
    bValue  ->GetEntry(i);
    Settings[i] = Setting;
    Readings[i] = Reading;
  }
  TGraph *g = new TGraph(NEntries, Settings, Readings);
  g->SetTitle(DACName);
  g->Draw("APL");
}


void TDACScan::Plot () {
  TCanvas *cDAC = new TCanvas ("cDAC", "DAC Characteristics", 1500, 750);
  cDAC->Divide (6,2);

  cDAC->cd(1);
  PlotDAC("VDACVAUX");
  cDAC->cd(2);
  PlotDAC("VDACVRESET");
  cDAC->cd(3);
  PlotDAC("VDACVCASN");
  cDAC->cd(4);
  PlotDAC("VDACVCASP");
  cDAC->cd(5);
  PlotDAC("VDACVPULSEL");
  cDAC->cd(6);
  PlotDAC("VDACVPULSEH");
  cDAC->cd(7);
  PlotDAC("IDACIRESET");
  cDAC->cd(8);
  PlotDAC("IDACIBIAS");
  cDAC->cd(9);
  PlotDAC("IDACIDB");
  cDAC->cd(10);
  PlotDAC("IDACITHR");

}


//////////////////////////////////////////////////////////////////////
//                                                                  //
//       TAnalysis II, analyses, correlations etc.                  //
//                                                                  //
//////////////////////////////////////////////////////////////////////


void TAnalysis::Compare (const char *AScan1, const char *ALeaf1, const char *AScan2, const char *ALeaf2, TH1F *Histo1D, TH2F *Histo2D) {
  float    Value1, Value2;
  TBranch *bScan1  = fScanData->GetBranch (AScan1);
  TBranch *bScan2  = fScanData->GetBranch (AScan2);
  if ((!bScan1) || (!bScan2)) {
    std::cout << "Problem reading branches" << std::endl;
    return;
  }
  TLeaf *leaf1 = bScan1->GetLeaf(ALeaf1);   
  TLeaf *leaf2 = bScan2->GetLeaf(ALeaf2);   
  if ((!leaf1) || (!leaf2)) {
    std::cout << "Problem reading leaf" << std::endl;
    return;
  }

  leaf1->SetAddress(&Value1);
  leaf2->SetAddress(&Value2);
  
  int nEntries = bScan1->GetEntries();

  for (int i = 0; i < nEntries; i++) {
    bScan1->GetEntry(i);
    bScan2->GetEntry(i);
    leaf1 ->GetValue(0);
    leaf2 ->GetValue(0);
    if (Value1 * Value2 == 0) continue;
    if (Histo2D) Histo2D->Fill(Value1, Value2);
    if (Histo1D) Histo1D->Fill(Value2 - Value1);
  }
}


void TAnalysis::Correlation (const char *AScan1, const char *ALeaf1, const char *AScan2, const char *ALeaf2) {
  TH2F *hCorr = new TH2F("hCorr", "Correlation plot", 10000, 0., 250., 10000, 0., 250.);
  Compare (AScan1, ALeaf1, AScan2, ALeaf2, 0, hCorr);
  hCorr->Draw("COLZ");
}


void   TAnalysis::DiffHisto   (const char *AScan1, const char *ALeaf1, const char *AScan2, const char *ALeaf2) {
  TH1F *hDiff = new TH1F("hDiff", "Difference plot", 400, -20, 20);
  Compare (AScan1, ALeaf1, AScan2, ALeaf2, hDiff, 0);
  hDiff->Draw();
}


void TAnalysis::NoiseMap() {
  TH2F *hMap = new TH2F("hMap", "Noise Map", 1024, -.5, 1023.5, 512, -.5, 511.5);
  struct address_t {int col; int row;};
  address_t address;
  struct result_t {float thresh; float noise; float chisq;};
  result_t result;  

  TBranch *bPixel = fScanData->GetBranch ("Pixel");
  TBranch *bScan  = fScanData->GetBranch ("Scan0");

  if ((!bPixel) || (!bScan)) {
    std::cout << "Problem reading branches" << std::endl;
    return;
  } 

  bPixel->SetAddress (&address);
  bScan ->SetAddress (&result);

  int nEntries = bPixel->GetEntries();
  for (int i = 0; i < nEntries; i++) {
    bPixel->GetEntry(i);
    bScan ->GetEntry(i);
    hMap  ->Fill(address.col, address.row, result.noise);
  }
  hMap->Draw("COLZ");
}


void TAnalysis::NoiseAnalysis(const char *AScan, const char *ARefScan) {
  struct address_t {int col; int row;};
  address_t address;
  float     noise, refnoise;
  TConfig   Config1, Config2;
  int       Sector;
  float     FixedPattern[4], Random[4];
  char      histName[10], histTitle[50];
  TH1F     *hFP[4], *hRand[4];
    


  for (int i = 0; i < 4; i++) {
    FixedPattern[i] = 0;
    Random      [i] = 0;
  }

  if ((!ReadConfig (AScan, &Config1)) || (Config1.SCANTYPE != SCANTYPE_NOISEOCC)) {
    std::cout << "Error: config not found or wrong scan type" << std::endl;
    return;
  }
  if ((!ReadConfig (ARefScan, &Config2)) || (Config2.SCANTYPE != SCANTYPE_NOISEOCC)) {
    std::cout << "Error: config not found or wrong scan type" << std::endl;
    return;
  }

  int      NEvents = Config1.NEvents;
  TBranch *bPixel  = fScanData->GetBranch ("Pixel");
  TBranch *bScan   = fScanData->GetBranch (AScan);
  TBranch *bRef    = fScanData->GetBranch (ARefScan);

  for (int isec = 0; isec < 4; isec ++) {
    sprintf(histName, "hFP%d", isec);
    sprintf(histTitle, "Fixed pattern noise, sector %d", isec);
    hFP[isec] = new TH1F(histName, histTitle, 50, log(10)/ log (NEvents), 0.);
    sprintf(histName, "hRand%d", isec);
    sprintf(histTitle, "Random noise, sector %d", isec);
    hRand[isec] = new TH1F(histName, histTitle, 50, log(10)/ log(NEvents), 0.);
  }



  if ((!bScan) || (!bRef) || (!bPixel)) {
    std::cout << "Problem reading branches" << std::endl;
    return;
  }

  bPixel->SetAddress (&address);
  bScan ->SetAddress (&noise);
  bRef  ->SetAddress (&refnoise);

  int nEntries = bPixel->GetEntries();

  for (int i = 0; i < nEntries; i++) {
    bPixel->GetEntry(i);
    bScan ->GetEntry(i);
    bRef  ->GetEntry(i);
    Sector = address.col / 256;
    if (noise > 0) {
      if (refnoise > 0) {
        FixedPattern[Sector] += noise;
        hFP         [Sector] -> Fill(log(noise / NEvents) / log(10)); 
      }
      else {
        Random [Sector] += noise;
        hRand  [Sector] -> Fill(log(noise / NEvents) / log(10));
      }
    }
  }

  TCanvas *c1 = new TCanvas ("c1", "c1", 600, 600);
  c1->Divide(2,2);

  for (int isec = 0; isec < 4; isec ++) {
    FixedPattern[isec] /= (NEvents * 131072);
    Random      [isec] /= (NEvents * 131072);
    std::cout << "Sector " << isec << ": Random noise " << Random[isec] << ", fixed pattern noise " << FixedPattern[isec] << std::endl;
    c1         ->cd(isec + 1);
    hFP[isec]  ->SetLineColor(kRed);
    hFP[isec]->Draw();
    //hRand[isec]->GetXaxis()->SetRangeUser(log(10)/ log (NEvents), 0.);
    hRand[isec]  ->Draw("SAME");
  }
}


void TAnalysis::DumpPixels(TTree *APixelTree) 
{
  struct pixel_t {int reg; int dcol; int address;};
  pixel_t  Pixel;
  TBranch *bPixel = APixelTree->GetBranch("Pixels");

  if (bPixel) {
    bPixel->SetAddress(&Pixel);
  }
  else {
    std::cout << "No pixels found" << std::endl;
    return;
  }
  for (int i = 0; i < bPixel->GetEntries(); i++) {
    bPixel->GetEntry(i);
    std::cout << "  " << Pixel.reg << "/" << Pixel.dcol << "/" << Pixel.address << std::endl;
  }
}


//////////////////////////////////////////////////////////////////////
//                                                                  //
//                Helper Functions                                  //
//                                                                  //
//////////////////////////////////////////////////////////////////////


void EntryToColRow(int AEntry, int &ACol, int &ARow) {
  ACol = AEntry / NROWS;
  ARow = AEntry % NROWS;
}

int AddressToColumn      (int ADoubleCol, int AAddress)
{
    int Column    = ADoubleCol * 2;                   // Double columns before ADoubleCol
    int LeftRight = ((AAddress % 4) < 2 ? 1:0);       // Left or right column within the double column
    
    Column += LeftRight;
    
    return Column;
}


int AddressToColumn (int ARegion, int ADoubleCol, int AAddress) 
{
    int DoubleColumn = ARegion * 16 + ADoubleCol;
    return AddressToColumn(DoubleColumn, AAddress);
}


int AddressToRow         (int AAddress)
{
    int Row = AAddress / 2;                // This is OK for the top-right and the bottom-left pixel within a group of 4
    if ((AAddress % 4) == 3) Row -= 1;      // adjust the top-left pixel
    if ((AAddress % 4) == 0) Row += 1;      // adjust the bottom-right pixel
    return Row;
}



