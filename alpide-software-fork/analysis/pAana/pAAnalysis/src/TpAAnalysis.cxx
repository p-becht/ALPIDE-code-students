#include "TH1F.h"
#include "TH2F.h"
#include "TLeaf.h"
#include "TBranch.h"
#include "TCanvas.h"
#include "TProfile.h"

#include "TpAScan.h"
#include "TpADACScan.h"
#include "TpAThresholdScan.h"
#include "TpADigitalScan.h"
#include "TpAAnalogueScan.h"
#include "TpASourceScan.h"
#include "TpANoiseocc.h"
#include "TpAAnalysis.h"

ClassImp(TpAAnalysis)

//_____________________________________________________________________________
TpAAnalysis::TpAAnalysis(const TString sName, const TString sTitle) :
TNamed(sName, sTitle),
fChipID(""),
fIsPrint(kFALSE),
fIsForceQuali(kTRUE),
fRootFile(0),
fScanData(0),
fScanConfigs(0),
fDACData(0),
fNoisyPixels(0),
fStuckPixels(0)
{
//
//  TpAAnalysis::TpAAnalysis
//

  fChipID = CheckChipID(sName);
  fRootFile = TFile::Open(sName.Data(), "UPDATE");

  fRootFile->GetObject("ScanData",    fScanData);
  fRootFile->GetObject("ScanConfigs", fScanConfigs);
  fRootFile->GetObject("DACData",     fDACData);
  fRootFile->GetObject("NoisyPixels", fNoisyPixels);
  fRootFile->GetObject("StuckPixels", fStuckPixels);

  if (!fScanConfigs) {
    if ((fScanData) || (fDACData)) {   // if there is no config tree there should also be no data tree
      Error(__func__, "Corrupt data file (no config, but data tree). Exiting... !!");
      exit(0);
    }

    Warning(__func__, "Config tree not found, generating new one !");
    fScanConfigs = new TTree("ScanConfigs", "Tree containing the scan configurations");
  }

  if (!fScanData) {
    Warning(__func__, "Data tree not found, generating new one !");
    CreateDataTree();
  }

  if (!fDACData) {
    Warning(__func__, "DAC Data tree not found, generating new one !");
    fDACData = new TTree("DACData", "Tree containing the DAC scan data");
  }

  if (!fNoisyPixels) {
    Warning(__func__, "Noisy pixel tree not found, generating new one !");
    fNoisyPixels = new TTree("NoisyPixels", "Tree containing noisy pixels");
  }

  if (!fStuckPixels) {
    Warning(__func__, "Stuck pixel tree not found, generating new one !");
    fStuckPixels = new TTree("StuckPixels", "Tree containing stuck-on pixels");
  }
}

//_____________________________________________________________________________
TpAAnalysis::~TpAAnalysis()
{
//
//  TpAAnalysis::~TpAAnalysis
//

  if (fRootFile) { fRootFile->Close(); /*delete fRootFile; fRootFile = 0;*/ }

/*if (fScanData)    { delete fScanData;    fScanData    = 0; }
  if (fScanConfigs) { delete fScanConfigs; fScanConfigs = 0; }
  if (fDACData)     { delete fDACData;     fDACData     = 0; }
  if (fNoisyPixels) { delete fNoisyPixels; fNoisyPixels = 0; }
  if (fStuckPixels) { delete fStuckPixels; fStuckPixels = 0; }*/
}

//_____________________________________________________________________________
TpAScan *TpAAnalysis::AddScan(TScanType   AType,
                              const char *dataFile,
                              bool        Qualification,
                              const char *Comment,
                              int         version)
{
  TpAScan *scan = 0;
  if (fIsForceQuali && Qualification) if (CheckQualiState(AType)) Qualification = false;
//=============================================================================

  switch (AType) {
    case SCANTYPE_DACSCAN :
      scan = new TpADACScan(this, dataFile, Qualification, Comment, version);
      break;
    case SCANTYPE_THRESHOLD :
      scan = new TpAThresholdScan(this, dataFile, Qualification, Comment, version);
      break;
    case SCANTYPE_DIGITAL :
      scan = new TpADigitalScan(this, dataFile, Qualification, Comment, version);
      break;
    case SCANTYPE_ANALOGUE :
      scan = new TpAAnalogueScan(this, dataFile, Qualification, Comment, version);
      break;
    case SCANTYPE_NOISEOCC :
      scan = new TpANoiseocc(this, dataFile, Qualification, Comment, version);
      break;
    case SCANTYPE_SOURCE :
      scan = new TpASourceScan(this, dataFile, Qualification, Comment, version);
      break;
    default :
      Warning(__func__, "Unknown scan type %d !", AType);
  }
//=============================================================================

  Write();
//=============================================================================

  return scan;
}

//_____________________________________________________________________________
int TpAAnalysis::GetScanType(int kScan)
{
  TBranch *bConfig = fScanConfigs->GetBranch("Configs");
  if (!bConfig) {
    Error(__func__, "Problem reading configuration tree !!");
    return -1;
  }
//=============================================================================

  TConfig Config;
  bConfig->SetAddress(&Config);
  bConfig->GetEntry(kScan);
  return Config.SCANTYPE;
}

//_____________________________________________________________________________
TpAScan *TpAAnalysis::RetrieveScan(int kScan)
{
  TBranch *bConfig = fScanConfigs->GetBranch("Configs");
  if (!bConfig) {
    Error(__func__, "Problem reading configuration tree !!");
    return 0x0;
  }
//=============================================================================

  TConfig Config;
  bConfig->SetAddress(&Config);
  bConfig->GetEntry(kScan);
  return RetrieveScan(Config);
}

//_____________________________________________________________________________
TpAScan *TpAAnalysis::RetrieveScan(TConfig Config)
{
/*TConfig Config;
  if ((!ReadConfig(AScan,&Config)) || (Config.SCANTYPE!=AType)) {
    std::cout << "Error: config not found or wrong scan type" << std::endl;
    return 0;
  }*/
//=============================================================================

  TScanType AType = (TScanType)Config.SCANTYPE;
//const char *AScan = Config.SCANNAME;
//=============================================================================

  TpAScan *scan = 0;
  if (AType==SCANTYPE_THRESHOLD) scan = new TpAThresholdScan (this, Config);
  if (AType==SCANTYPE_DACSCAN)   scan = new TpADACScan       (this, Config);
  if (AType==SCANTYPE_DIGITAL)   scan = new TpADigitalScan   (this, Config);
  if (AType==SCANTYPE_ANALOGUE)  scan = new TpAAnalogueScan  (this, Config);
  if (AType==SCANTYPE_SOURCE)    scan = new TpASourceScan    (this, Config);
  if (AType==SCANTYPE_NOISEOCC)  scan = new TpANoiseocc      (this, Config);
  return scan;
}

//_____________________________________________________________________________
bool TpAAnalysis::ReadConfig(const char *AScan, TConfig *AConfig)
{

  TBranch *bConfig = fScanConfigs->GetBranch("Configs");
  if (!bConfig) {
    std::cout << "Problem reading configuration tree" << std::endl;
    return false;
  }
//=============================================================================

  TConfig Config;
  bConfig->SetAddress(&Config);
  int nEntries = bConfig->GetEntries();

  bool Found = false;
  for (int i=0; (i<nEntries) & (!Found); i++) {
    bConfig->GetEntry(i);
    if (strcmp(AScan,Config.SCANNAME)==0) {
      Found   = true;
     *AConfig = Config;
      break;
    }
  }

//DumpConfiguration(*AConfig);
//=============================================================================

  return Found;
}

//_____________________________________________________________________________
void TpAAnalysis::Summarise()
{
  TBranch *bConfig = fScanConfigs->GetBranch("Configs");
  if (!bConfig) {
    Error(__func__, "Problem reading configuration tree");
    return;
  }
//=============================================================================

  std::cout << std::endl;
  Info(__func__, "===========================================================");
  Info(__func__, "||                     Chip Summary                      ||");
  Info(__func__, "===========================================================");
  std::cout << std::endl;
//=============================================================================

  TConfig Config; bConfig->SetAddress(&Config);
  for (int i=0; i<bConfig->GetEntries(); i++) {
    bConfig->GetEntry(i);

    TpAScan *scan = RetrieveScan(Config);
    if (scan) { scan->Summarise(); std::cout << std::endl; }
  }
//=============================================================================

  Info(__func__, "===========================================================");
  Info(__func__, "===========================================================");
//=============================================================================

  return;
}

//_____________________________________________________________________________
void TpAAnalysis::ListAllScans()
{
  TBranch *bConfig = fScanConfigs->GetBranch("Configs");
  if (!bConfig) {
    Error(__func__, "Problem reading configuration tree !!");
    return;
  }
//=============================================================================

  TConfig Config;
  bConfig->SetAddress(&Config);

  for (int i=0; i<bConfig->GetEntries(); i++) {
    bConfig->GetEntry(i);
    ListScan(Config);
  }
//=============================================================================

//Info(__func__, "Scantypes: 0 = digital, 1 = analogue, 2 = threshold, 3 = source, 4 = noise occ., 5 = DAC scan");
  Info(__func__, "* = qualification scan");
  Info(__func__, "===========================================================");
  Info(__func__, "===========================================================");
  std::cout << std::endl;
//=============================================================================

  return;
}

//_____________________________________________________________________________
void TpAAnalysis::DumpAllConfigurations()
{
  TBranch *bConfig = fScanConfigs->GetBranch("Configs");
  if (!bConfig) {
    Error(__func__, "Problem reading configuration tree !!");
    return;
  }
//=============================================================================

  TConfig Config; bConfig->SetAddress(&Config);
  for (int i=0; i<bConfig->GetEntries(); i++) {
    bConfig->GetEntry(i);
    DumpConfiguration(Config);
    std::cout << std::endl;
  }
//=============================================================================

  Info(__func__, "* = qualification scan");
  Info(__func__, "===========================================================");
  Info(__func__, "===========================================================");
  std::cout << std::endl;
//=============================================================================

  return;
}

//_____________________________________________________________________________
void TpAAnalysis::DumpConfiguration(TConfig AConfig)
{
//
//  TpAAnalysis::DumpConfiguration
//

  TString sQual = " ";
  if (AConfig.Qualification) sQual = "*";
  TString sName = GetScanName((TScanType)AConfig.SCANTYPE);
//=============================================================================

  Info(__func__, "%s %s: %s, ID: %s %s", sQual.Data(),
                                         AConfig.SCANNAME,
                                         sName.Data(),
                                         AConfig.IDENTSTRING,
                                         AConfig.Comment);
  std::cout << std::endl;
//=============================================================================

  Info(__func__, "NEvents    : %d", AConfig.NEvents);
  Info(__func__, "FW VERSION : %d", AConfig.FWVERSION);
  Info(__func__, "Git commit : %s", AConfig.GitCommit);
  std::cout << std::endl;
//=============================================================================

  Info(__func__, "TempInit      = %f", AConfig.TempInit);
  Info(__func__, "TempFinal     = %f", AConfig.TempFinal);
  Info(__func__, "OnChipTemp    = %f", AConfig.OnChipTemp);
  std::cout << std::endl;
//=============================================================================

  Info(__func__, "IDDD          =  %f", AConfig.IDDD);
  Info(__func__, "IDDDPowerOn   =  %f", AConfig.IDDDPowerOn);
  Info(__func__, "IDDA          =  %f", AConfig.IDDA);
  Info(__func__, "IDDAPowerOn   =  %f", AConfig.IDDAPowerOn);
  std::cout << std::endl;
//=============================================================================

  Info(__func__, "VAUX          = %d", AConfig.VAUX);
  Info(__func__, "VRESET        = %d", AConfig.VRESET);
  Info(__func__, "VRESETD       = %d", AConfig.VRESETD);
  Info(__func__, "VRESETP       = %d", AConfig.VRESETP);
  Info(__func__, "VCASN         = %d", AConfig.VCASN);
  Info(__func__, "VCASN2        = %d", AConfig.VCASN2);
  Info(__func__, "VCASP         = %d", AConfig.VCASP);
  Info(__func__, "VCLIP         = %d", AConfig.VCLIP);
  Info(__func__, "VPULSEL       = %d", AConfig.VPULSEL);
  Info(__func__, "VPULSEH       = %d", AConfig.VPULSEH);
  std::cout << std::endl;
//=============================================================================

  Info(__func__, "IRESET        = %d", AConfig.IRESET);
  Info(__func__, "IAUX2         = %d", AConfig.IAUX2);
  Info(__func__, "IBIAS         = %d", AConfig.IBIAS);
  Info(__func__, "IDB           = %d", AConfig.IDB);
  Info(__func__, "ITHR          = %d", AConfig.ITHR);
  std::cout << std::endl;
//=============================================================================

  Info(__func__, "STROBEBLENGTH = %d", AConfig.STROBEBLENGTH);
  Info(__func__, "STROBEBDELAY  = %d", AConfig.STROBEBDELAY);
  Info(__func__, "STROBELENGTH  = %d", AConfig.STROBELENGTH);
  Info(__func__, "READOUTDELAY  = %d", AConfig.READOUTDELAY);
  Info(__func__, "READOUTMODE   = %c", AConfig.READOUTMODE);
  Info(__func__, "PULSEMODE     = %d", AConfig.PULSEMODE);
  Info(__func__, "PULSELENGTH   = %d", AConfig.PULSELENGTH);
  Info(__func__, "PULSEDELAY    = %d", AConfig.PULSEDELAY);
  Info(__func__, "Mask          = %s", AConfig.MASK);
//=============================================================================

  Info(__func__, "===========================================================");
//=============================================================================

  return;
}

//_____________________________________________________________________________
void TpAAnalysis::GetMeanIDDD(double &dIb, double &dIbErr,
                              double &dIa, double &dIaErr,
                              double &dIc, double &dIcErr)
{
//
//  TpAAnalysis::GetMeanIDDD
//

  dIb = 0.; dIbErr = 0.;
  dIa = 0.; dIaErr = 0.;
  dIc = 0.; dIcErr = 0.;
//=============================================================================

  TBranch *bConfig = fScanConfigs->GetBranch("Configs");
  if (!bConfig) {
    std::cout << "Problem reading configuration tree" << std::endl;
    return;
  }
//=============================================================================

  TProfile *hProf = new TProfile("hProfIDDD", "", 3, 0.5, 3.5);
//=============================================================================

  TConfig Config; bConfig->SetAddress(&Config);
  for (int i=0; i<bConfig->GetEntries(); i++) {
    bConfig->GetEntry(i);

    if ((Config.IDDD       <5.) || (Config.IDDD       >10000.)) continue;
    if ((Config.IDDDPowerOn<5.) || (Config.IDDDPowerOn>10000.)) continue;
    Info(__func__, "%s: IDDD = %f, IDDDPowerOn = %f", Config.SCANNAME, Config.IDDD, Config.IDDDPowerOn);


    hProf->Fill(1., Config.IDDD);
    hProf->Fill(2., Config.IDDDPowerOn);

    hProf->Fill(3., Config.IDDD);
    hProf->Fill(3., Config.IDDDPowerOn);
  }
//=============================================================================

  dIb = hProf->GetBinContent(1); dIbErr = hProf->GetBinError(1);
  dIa = hProf->GetBinContent(2); dIaErr = hProf->GetBinError(2);
  dIc = hProf->GetBinContent(3); dIcErr = hProf->GetBinError(3);
//=============================================================================

  delete hProf; hProf = 0;
  return;
}

//_____________________________________________________________________________
void TpAAnalysis::GetMeanIDDA(double &dIb, double &dIbErr,
                              double &dIa, double &dIaErr,
                              double &dIc, double &dIcErr)
{
//
//  TpAAnalysis::GetMeanIDDA
//

  dIb = 0.; dIbErr = 0.;
  dIa = 0.; dIaErr = 0.;
  dIc = 0.; dIcErr = 0.;
//=============================================================================

  TBranch *bConfig = fScanConfigs->GetBranch("Configs");
  if (!bConfig) {
    std::cout << "Problem reading configuration tree" << std::endl;
    return;
  }
//=============================================================================

  TProfile *hProf = new TProfile("hProfIDDA", "", 3, 0.5, 3.5);
//=============================================================================

  TConfig Config; bConfig->SetAddress(&Config);
  for (int i=0; i<bConfig->GetEntries(); i++) {
    bConfig->GetEntry(i);

    if ((Config.IDDA       <5.) || (Config.IDDA       >10000.)) continue;
    if ((Config.IDDAPowerOn<5.) || (Config.IDDAPowerOn>10000.)) continue;
    Info(__func__, "%s: IDDA = %f, IDDAPowerOn = %f", Config.SCANNAME, Config.IDDA, Config.IDDAPowerOn);

    hProf->Fill(1., Config.IDDA);
    hProf->Fill(2., Config.IDDAPowerOn);

    hProf->Fill(3., Config.IDDA);
    hProf->Fill(3., Config.IDDAPowerOn);
  }
//=============================================================================

  dIb = hProf->GetBinContent(1); dIbErr = hProf->GetBinError(1);
  dIa = hProf->GetBinContent(2); dIaErr = hProf->GetBinError(2);
  dIc = hProf->GetBinContent(3); dIcErr = hProf->GetBinError(3);
//=============================================================================

  delete hProf; hProf = 0;
  return;
}

//_____________________________________________________________________________
/*void TpAAnalysis::Map(const char *AScan, const char *ALeaf)
{
  TBranch *bPixel = fScanData->GetBranch("Pixel");
  TBranch *bScan  = fScanData->GetBranch(AScan);
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
//=============================================================================

  struct address_t {
    int col;
    int row;
  } address;

  bPixel->SetAddress(&address);
  float Value; leaf->SetAddress(&Value);

  TH2F  *hMap = new TH2F("hMap", ALeaf, 1024, -.5, 1023.5, 512, -.5, 511.5);

  for (int i=0; i<bPixel->GetEntries(); i++) {
    bPixel->GetEntry(i);
    bScan->GetEntry(i);
    leaf->GetValue(0);
    hMap->Fill(address.col, address.row, Value);
  }
//=============================================================================

  hMap->Draw("COLZ");
//=============================================================================

  return;
}*/

//_____________________________________________________________________________
/*void TpAAnalysis::NoiseMap()
{
  TBranch *bPixel = fScanData->GetBranch("Pixel");
  TBranch *bScan  = fScanData->GetBranch("Scan0");

  if ((!bPixel) || (!bScan)) {
    std::cout << "Problem reading branches" << std::endl;
    return;
  }
//=============================================================================

  struct address_t {
    int col;
    int row;
  } address;

  struct result_t {
    float thresh;
    float noise;
    float chisq;
  } result;

  bScan->SetAddress(&result);
  bPixel->SetAddress(&address);
//=============================================================================

  TH2F *hMap = new TH2F("hMap", "Noise Map", 1024, -0.5, 1023.5, 512, -0.5, 511.5);

  for (int i=0; i<bPixel->GetEntries(); i++) {
    bScan->GetEntry(i);
    bPixel->GetEntry(i);
    hMap->Fill(address.col, address.row, result.noise);
  }
//=============================================================================

  hMap->Draw("COLZ");
//=============================================================================

  return;
}*/

//_____________________________________________________________________________
void TpAAnalysis::NoiseAnalysis(const char *AScan, const char *ARefScan)
{
  TConfig Config1;
  if ((!ReadConfig(AScan,&Config1)) || (Config1.SCANTYPE!=SCANTYPE_NOISEOCC)) {
    std::cout << "Error: config not found or wrong scan type" << std::endl;
    return;
  }

  TConfig Config2;
  if ((!ReadConfig (ARefScan,&Config2)) || (Config2.SCANTYPE!=SCANTYPE_NOISEOCC)) {
    std::cout << "Error: config not found or wrong scan type" << std::endl;
    return;
  }
//=============================================================================

  TBranch *bPixel = fScanData->GetBranch("Pixel");
  TBranch *bScan  = fScanData->GetBranch(AScan);
  TBranch *bRef   = fScanData->GetBranch(ARefScan);

  if ((!bScan) || (!bRef) || (!bPixel)) {
    std::cout << "Problem reading branches" << std::endl;
    return;
  }

  struct address_t {
    int col;
    int row;
  } address;

  bPixel->SetAddress(&address);
  float refnoise; bRef->SetAddress(&refnoise);
  float noise;    bScan->SetAddress(&noise);
//=============================================================================

  TH1F *hFP[4], *hRand[4];
  char histName[10], histTitle[50];
  int NEvents = Config1.NEvents;
  for (int isec=0; isec<4; isec++) {
    sprintf(histName, "hFP%d", isec);
    sprintf(histTitle, "Fixed pattern noise, sector %d", isec);
    hFP[isec] = new TH1F(histName, histTitle, 50, log(10)/log(NEvents), 0.);
    sprintf(histName, "hRand%d", isec);
    sprintf(histTitle, "Random noise, sector %d", isec);
    hRand[isec] = new TH1F(histName, histTitle, 50, log(10)/log(NEvents), 0.);
  }
//=============================================================================

  float FixedPattern[4], Random[4];
  for (int i=0; i<4; i++) {
    Random[i] = 0.;
    FixedPattern[i] = 0.;
  }

  for (int i=0; i<bPixel->GetEntries(); i++) {
    bPixel->GetEntry(i);
    bScan->GetEntry(i);
    bRef->GetEntry(i);

    int Sector = address.col / 256;

    if (noise>0) {
      if (refnoise>0) {
        FixedPattern[Sector] += noise;
        hFP[Sector]->Fill(log(noise/NEvents) / log(10));
      } else {
        Random[Sector] += noise;
        hRand[Sector]->Fill(log(noise/NEvents) / log(10));
      }
    }
  }
//=============================================================================

  TCanvas *c1 = new TCanvas("c1", "c1", 600, 600);
  c1->Divide(2,2);

  for (int isec=0; isec<4; isec++) {
    Random[isec] /= (NEvents*131072);
    FixedPattern[isec] /= (NEvents*131072);
    std::cout << "Sector " << isec
              << ": Random noise " << Random[isec]
              << ", fixed pattern noise " << FixedPattern[isec] << std::endl;

    c1->cd(isec + 1);
    hFP[isec]->SetLineColor(kRed);
    hFP[isec]->Draw();
//  hRand[isec]->GetXaxis()->SetRangeUser(log(10)/ log (NEvents), 0.);
    hRand[isec]->Draw("SAME");
  }
//=============================================================================

  return;
}

//_____________________________________________________________________________
void TpAAnalysis::AddPixel(TTree *ATree, int AReg, int ADCol, int AAddress)
{
  struct pixel_t {
    int reg;
    int dcol;
    int address;
  } Pixel;

  Pixel.reg       = AReg;
  Pixel.dcol      = ADCol;
  Pixel.address   = AAddress;
  TBranch *bPixel = ATree->GetBranch("Pixels");

  if (!bPixel)  {
    std::cout << "Pixel branch not found, creating new one" << std::endl;
    bPixel = ATree->Branch("Pixels", (void*)(&Pixel), "Reg/I:DCol:Address");
  } else {
    bPixel->SetAddress(&Pixel);
  }

  bPixel->Fill();
//=============================================================================

  return;
}

//_____________________________________________________________________________
void TpAAnalysis::DumpPixels(TTree *APixelTree)
{
  TBranch *bPixel = APixelTree->GetBranch("Pixels");

  struct pixel_t {
    int reg;
    int dcol;
    int address;
  }  Pixel;

  if (bPixel) {
    bPixel->SetAddress(&Pixel);
  } else {
    std::cout << "No pixels found" << std::endl;
    return;
  }
//=============================================================================

  for (int i=0; i <bPixel->GetEntries(); i++) {
    bPixel->GetEntry(i);
    std::cout << "  " << Pixel.reg
              << "/"  << Pixel.dcol
              << "/" << Pixel.address << std::endl;
  }
//=============================================================================

  return;
}

//_____________________________________________________________________________
void TpAAnalysis::Correlation(const char *AScan1, const char *ALeaf1, const char *AScan2, const char *ALeaf2)
{
  TH2F *hCorr = new TH2F("hCorr", "Correlation plot", 10000, 0., 250., 10000, 0., 250.);
  Compare(AScan1, ALeaf1, AScan2, ALeaf2, 0, hCorr);
  hCorr->Draw("COLZ");

  return;
}

//_____________________________________________________________________________
void TpAAnalysis::DiffHisto(const char *AScan1, const char *ALeaf1, const char *AScan2, const char *ALeaf2)
{
  TH1F *hDiff = new TH1F("hDiff", "Difference plot", 400, -20, 20);
  Compare (AScan1, ALeaf1, AScan2, ALeaf2, hDiff, 0);
  hDiff->Draw();

  return;
}

//_____________________________________________________________________________
void TpAAnalysis::Compare(const char *AScan1, const char *ALeaf1, const char *AScan2, const char *ALeaf2, TH1F *Histo1D, TH2F *Histo2D)
{
  TBranch *bScan1 = fScanData->GetBranch(AScan1);
  TBranch *bScan2 = fScanData->GetBranch(AScan2);

  if ((!bScan1) || (!bScan2)) {
    std::cout << "Problem reading branches" << std::endl;
    return;
  }
//=============================================================================

  TLeaf *leaf1 = bScan1->GetLeaf(ALeaf1);
  TLeaf *leaf2 = bScan2->GetLeaf(ALeaf2);

  if ((!leaf1) || (!leaf2)) {
    std::cout << "Problem reading leaf" << std::endl;
    return;
  }
//=============================================================================

  float Value1; leaf1->SetAddress(&Value1);
  float Value2; leaf2->SetAddress(&Value2);
  for (int i=0; i<bScan1->GetEntries(); i++) {
    bScan1->GetEntry(i); leaf1->GetValue(0);
    bScan2->GetEntry(i); leaf2->GetValue(0);

    if ((Value1*Value2)==0) continue;
    if (Histo2D) Histo2D->Fill(Value1, Value2);
    if (Histo1D) Histo1D->Fill(Value2 - Value1);
  }
//=============================================================================

  return;
}

//_____________________________________________________________________________
void TpAAnalysis::ListScan(TConfig AConfig)
{
//
//  TpAAnalysis::ListScan
//

  TString sQual = " ";
  if (AConfig.Qualification) sQual = "*";
  TString sName = GetScanName((TScanType)AConfig.SCANTYPE);
//=============================================================================

  Info(__func__, "%s %7s: %12s, ID: %13s %s", sQual.Data(),
                                              AConfig.SCANNAME,
                                              sName.Data(),
                                              AConfig.IDENTSTRING,
                                              AConfig.Comment);
//=============================================================================

  return;
}

//_____________________________________________________________________________
void TpAAnalysis::CreateDataTree()
{
//
//  TpAAnalysis::CreateDataTree
//

  struct address_t {
    int col;
    int row;
  } address;
//=============================================================================

  fScanData = new TTree("ScanData", "Tree containing the scan data");
  fScanData->Branch("Pixel", &address.col, "col/I:row/I");

  for (int ipix=0; ipix<NPIX; ipix++) {
    EntryToColRow(ipix, address.col, address.row);
    fScanData->Fill();
  }
//=============================================================================

  return;
}

//_____________________________________________________________________________
Bool_t TpAAnalysis::CheckQualiState(const TScanType AType)
{
//
//  TpAAnalysis::CheckQualiState
//

  Bool_t bStat = kFALSE;
  if (!fScanConfigs) return bStat;
  TBranch *bConfig = fScanConfigs->GetBranch("Configs"); if (!bConfig) return bStat;
//=============================================================================

  TConfig Config;
  bConfig->SetAddress(&Config);
  for (Int_t i=0; i<bConfig->GetEntries(); i++) {
    bConfig->GetEntry(i);

    if (AType!=Config.SCANTYPE) continue;
    if (Config.Qualification==1) bStat = kTRUE;
  }
//=============================================================================

  return bStat;
}
