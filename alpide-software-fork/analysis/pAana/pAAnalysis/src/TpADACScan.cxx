#include "TMath.h"
#include "TStyle.h"
#include "TAxis.h"
#include "TF1.h"
#include "TH1F.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TList.h"
#include "TBranch.h"

#include "TpAAnalysis.h"
#include "TpADACScan.h"

ClassImp(TpADACScan)

// probably not needed to derive from TScan

// loop over DACs
// for first DAC:  create tree, read setting and measured value
// for other DACs: read only measured values, assuming the steps are the same...

//_____________________________________________________________________________
TpADACScan::TpADACScan(TpAAnalysis *AAnalysis,
                       const char  *dataPath,
                       bool         Qualification,
                       const char  *Comment,
                       int          version) :
TpAScan(SCANTYPE_DACSCAN,AAnalysis,Qualification,Comment,version)
{
//
//  TDACScan::TDACScan
//

  Init(dataPath);
}

//_____________________________________________________________________________
TpADACScan::TpADACScan(TpAAnalysis *AAnalysis, TConfig AConfig) :
TpAScan(AAnalysis,AConfig)
{
//
//  TDACScan::TDACScan
//
}

//_____________________________________________________________________________
TpADACScan::TpADACScan(const TpADACScan &src) :
TpAScan(src)
{
//
// TpADACScan::TpADACScan
//
}

//_____________________________________________________________________________
TpADACScan& TpADACScan::operator=(const TpADACScan &src)
{
//
// TpADACScan::operator=
//

  if (&src==this) return *this;

  TpAScan::operator=(src);

  return *this;
}

//_____________________________________________________________________________
TpADACScan::~TpADACScan()
{
//
// TpADACScan::~TpADACScan
//
}

//_____________________________________________________________________________
bool TpADACScan::ReadDataFile(const char *fName)
{
//=============================================================================

  const char *dataPath = fName;
//const int kv = 3;
  if (!ReadDACFile(dataPath,"VDACVCASN"))   return false;
  if (!ReadDACFile(dataPath,"VDACVCASP"))   return false;
  if (!ReadDACFile(dataPath,"VDACVPULSEL")) return false;
  if (!ReadDACFile(dataPath,"VDACVPULSEH")) return false;

  if (!ReadDACFile(dataPath,"IDACIDB"))    return false;
  if (!ReadDACFile(dataPath,"IDACITHR"))   return false;
  if (!ReadDACFile(dataPath,"IDACIBIAS"))  return false;
  if (!ReadDACFile(dataPath,"IDACIRESET")) return false;

//if ((kv==3) || (kv==2)) {
    ReadDACFile(dataPath, "VDACVCASN2");
    ReadDACFile(dataPath, "VDACVRESETD");
    ReadDACFile(dataPath, "VDACVRESETP");
    ReadDACFile(dataPath, "IDACIAUX2");
//}

/*if ((kv==2) || (kv==1)) {
    ReadDACFile(dataPath, "VDACVAUX");
    ReadDACFile(dataPath, "VDACVRESET");
  }

  if ((kv==3) || (kv==2)) {
    ReadDACFile(dataPath, "VDACVCLIP");
    ReadDACFile(dataPath, "VDACVTEMP");
  }*/
//=============================================================================

  return true;
}

//_____________________________________________________________________________
bool TpADACScan::ReadDACFile(const char *dataPath, const char *DACName)
{
  char fName[512];
  sprintf(fName, "%s/%s.dat", dataPath, DACName);
//=============================================================================

// check if setting branch exists, ifnot create and read values

  bool firstDAC = false;
  TBranch *bSetting = fAnalysis->GetDACData()->GetBranch("Setting");

  int setting;
  if (!bSetting) {
    bSetting = fAnalysis->GetDACData()->Branch("Setting", &setting, "setting/I");
    firstDAC = true;
  }
//=============================================================================

  FILE *fp = fopen(fName, "r");
  if (!fp) {
    std::cout << "Problem opening DAC data file " << fName << std::endl;
    return false;
  }
//=============================================================================

  float value;
  TBranch *bValue = fAnalysis->GetDACData()->Branch(DACName, &value, "value/F");

  while (fscanf(fp, "%d %f\n", &setting, &value)==2) {
    if (firstDAC) bSetting->Fill();
    bValue->Fill();
  }

  fclose (fp);
//=============================================================================

  return true;
}

//_____________________________________________________________________________
void TpADACScan::Summarise()
{
//
//  TpADACScan::Summarise
//

  TString sName = GetScanName((TScanType)fConfig.SCANTYPE);
  Info(__func__, "%s: %s, ID: %s", fConfig.SCANNAME, sName.Data(), fConfig.IDENTSTRING);
  Info(__func__, "===========================================================");

  return;
}

//_____________________________________________________________________________
void TpADACScan::Plot()
{
  new TStyle();
  gStyle->SetOptStat(0);
  const Bool_t bPrint = fAnalysis->IsPrint();
//=============================================================================

  const int nV=9, nI=5;
  const TString sI[nI] = { "IDB",   "ITHR",   "IBIAS", "IRESET",  "IAUX2" };
  const TString sV[nV] = { "VCASN", "VCASN2", "VCASP", "VPULSEL", "VPULSEH", "VCLIP", "VRESETD", "VRESETP", "VTEMP" };
//=============================================================================

  Double_t dVx[nV], dVi[nV], dVs[nV];
  TH1F *hVs = new TH1F(Form("hDACVs_%s",fConfig.IDENTSTRING), Form("V-slope %s",    fConfig.IDENTSTRING), nV, 0., 1.*nV);
  TH1F *hVi = new TH1F(Form("hDACVi_%s",fConfig.IDENTSTRING), Form("V-intercept %s",fConfig.IDENTSTRING), nV, 0., 1.*nV);

  const TString sID   = fAnalysis->GetChipID();
  const TString sName = GetScanName((TScanType)fConfig.SCANTYPE);
  TCanvas *cV = new TCanvas(Form("ch%s_%s_%s_VDAC",sID.Data(),sName.Data(),fConfig.IDENTSTRING),
                            Form(  "%s %s %s VDAC",sID.Data(),sName.Data(),fConfig.IDENTSTRING), 900, 900);

  cV->Divide(3, 3);
  TVirtualPad *pad = 0;
  for (int i=0, k=1; i<nV; i++, k++) {
    pad = cV->cd(k); if (!pad) continue;
    dVi[i] = 0.; dVs[i] = 0.; dVx[i] = 0.5 + i;
    hVi->GetXaxis()->SetBinLabel(k, sV[i].Data());
    hVs->GetXaxis()->SetBinLabel(k, sV[i].Data());

    pad->SetRightMargin(0.02);
    TGraph *g = PlotDAC(Form("VDAC%s",sV[i].Data()));

    if (g) {
      TList *list = g->GetListOfFunctions();
      TF1 *fxn = (TF1*)list->At(0);

      if (fxn) {
        fxn->SetName(Form("fVDAC%s",sV[i].Data()));
        dVi[i] = fxn->GetParameter(0);
        dVs[i] = fxn->GetParameter(1);
      }
    }
  }

  cV->cd(); cV->Modified();
  cV->cd(); cV->SetSelected(cV);
  if (bPrint) cV->Print(Form("%s.pdf",cV->GetName()));
//=============================================================================

  TCanvas *cLV = new TCanvas(Form("ch%s_%s_%s_VLDAC",sID.Data(),sName.Data(),fConfig.IDENTSTRING),
                             Form(  "%s %s %s VLDAC",sID.Data(),sName.Data(),fConfig.IDENTSTRING), 900, 900);

  cLV->Divide(3, 3);
  for (int i=0, k=1; i<nV; i++, k++) {
    pad = cLV->cd(k); if (!pad) continue;

    pad->SetRightMargin(0.02);
    PlotLinearity(Form("VDAC%s",sV[i].Data()));
  }

  cLV->cd(); cLV->Modified();
  cLV->cd(); cLV->SetSelected(cLV);
  if (bPrint) cLV->Print(Form("%s.pdf",cLV->GetName()));
//=============================================================================

  Double_t dIx[nI], dIi[nI], dIs[nI];
  TH1F *hIs = new TH1F(Form("hDACIs_%s",fConfig.IDENTSTRING), Form("I-slope %s",    fConfig.IDENTSTRING), nI, 0., 1.*nI);
  TH1F *hIi = new TH1F(Form("hDACIi_%s",fConfig.IDENTSTRING), Form("I-intercept %s",fConfig.IDENTSTRING), nI, 0., 1.*nI);

  TCanvas *cI = new TCanvas(Form("ch%s_%s_%s_IDAC",sID.Data(),sName.Data(),fConfig.IDENTSTRING),
                            Form(  "%s %s %s IDAC",sID.Data(),sName.Data(),fConfig.IDENTSTRING), 900, 600);

  cI->Divide(3, 2);
  for (int i=0, k=1; i<nI; i++, k++) {
    dIi[i] = 0.; dIs[i] = 0.; dIx[i] = 0.5 + i;
    hIi->GetXaxis()->SetBinLabel(k, sI[i].Data());
    hIs->GetXaxis()->SetBinLabel(k, sI[i].Data());

    pad = cI->cd(k);
    if (!pad) continue;
    pad->SetRightMargin(0.02);
    TGraph *g = PlotDAC(Form("IDAC%s",sI[i].Data()));

    if (g) {
      TList *list = g->GetListOfFunctions();
      TF1 *fxn = (TF1*)list->At(0);

      if (fxn) {
        fxn->SetName(Form("fIDAC%s",sI[i].Data()));
        dIi[i] = fxn->GetParameter(0);
        dIs[i] = fxn->GetParameter(1);
      }
    }
  }

  cI->cd(); cI->Modified();
  cI->cd(); cI->SetSelected(cI);
  if (bPrint) cI->Print(Form("%s.pdf",cI->GetName()));
//=============================================================================

  TCanvas *cLI = new TCanvas(Form("ch%s_%s_%s_LIDAC",sID.Data(),sName.Data(),fConfig.IDENTSTRING),
                             Form(  "%s %s %s LIDAC",sID.Data(),sName.Data(),fConfig.IDENTSTRING), 900, 600);

  cLI->Divide(3, 2);
  for (int i=0, k=1; i<nI; i++, k++) {
    pad = cLI->cd(k); if (!pad) continue;

    pad->SetRightMargin(0.02);
    PlotLinearity(Form("IDAC%s",sI[i].Data()));
  }

  cLI->cd(); cLI->Modified();
  cLI->cd(); cLI->SetSelected(cI);
  if (bPrint) cLI->Print(Form("%s.pdf",cLI->GetName()));
//=============================================================================

  TCanvas *cF = new TCanvas(Form("ch%s_%s_%s_FDAC",sID.Data(),sName.Data(),fConfig.IDENTSTRING),
                            Form(  "%s %s %s FDAC",sID.Data(),sName.Data(),fConfig.IDENTSTRING), 1000, 1000);
  cF->Divide(2, 2);

  pad = cF->cd(1); if (pad) {
    pad->SetRightMargin(0.02);

    hVi->GetYaxis()->SetRangeUser(0.5*TMath::MinElement(nV,dVi), 1.2*TMath::MaxElement(nV,dVi));
    hVi->Draw();

    TGraph *gVi = new TGraph(nV, dVx, dVi);
    gVi->SetMarkerStyle(kFullCircle);
    gVi->Draw("P");
  }

  pad = cF->cd(2); if (pad) {
    pad->SetRightMargin(0.02);

    hVs->GetYaxis()->SetRangeUser(0.5*TMath::MinElement(nV,dVs), 1.2*TMath::MaxElement(nV,dVs));
    hVs->Draw();

    TGraph *gVs = new TGraph(nV, dVx, dVs);
    gVs->SetMarkerStyle(kFullCircle);
    gVs->Draw("P");
  }


  pad = cF->cd(3); if (pad) {
    pad->SetRightMargin(0.02);

    hIi->GetYaxis()->SetRangeUser(0.5*TMath::MinElement(nV,dIi), 1.2*TMath::MaxElement(nV,dIi));
    hIi->Draw();

    TGraph *gIi = new TGraph(nI, dIx, dIi);
    gIi->SetMarkerStyle(kFullCircle);
    gIi->Draw("P");
  }

  pad = cF->cd(4); if (pad) {
    pad->SetRightMargin(0.02);

    hIs->GetYaxis()->SetRangeUser(0.5*TMath::MinElement(nI,dIs), 1.2*TMath::MaxElement(nI,dIs));
    hIs->Draw();

    TGraph *gIs = new TGraph(nI, dIx, dIs);
    gIs->SetMarkerStyle(kFullCircle);
    gIs->Draw("P");
  }

  cF->cd(); cF->Modified();
  cF->cd(); cF->SetSelected(cF);
  if (bPrint) cF->Print(Form("%s.pdf",cF->GetName()));
//=============================================================================

  return;
}

//_____________________________________________________________________________
TGraph* TpADACScan::PlotDAC(const char *DACName)
{
//
//  TpADACScan::PlotDAC
//

  TBranch *bSetting = fAnalysis->GetDACData()->GetBranch("Setting");
  if (!bSetting) {
    Error(__func__, "DAC Scan data not found in tree !!");
    return 0x0;
  }

  TBranch *bValue = fAnalysis->GetDACData()->GetBranch(DACName);
  if (!bValue) {

    bValue = fAnalysis->GetDACData()->GetBranch(Form("/%s",DACName));
    if (!bValue) {
      Warning(__func__, "Data for %s not found in tree !!", DACName);
      return 0x0;
    }
  }
//=============================================================================

  int NEntries = bSetting->GetEntries();
  if (NEntries>256) {
    Error(__func__, "Illegal number of entries in tree: %d", NEntries);
    return 0x0;
  }
//=============================================================================

  int Setting; bSetting->SetAddress(&Setting);
  float Reading; bValue->SetAddress(&Reading);

  float Settings[256];
  float Readings[256];
  for (int i=0; i<NEntries; i++) {
    bSetting->GetEntry(i);
    bValue  ->GetEntry(i);
    Settings[i] = Setting;
    Readings[i] = Reading;
  }
//=============================================================================

  const TString sName = DACName;
  TGraph *g = new TGraph(NEntries, Settings, Readings);
  g->SetName(Form("g%s",DACName));
  g->SetTitle(DACName);

  g->GetXaxis()->SetTitle("DAC setting");
  g->GetYaxis()->SetTitleOffset(1.5);
  g->Draw("APL");

  if (sName.Contains("VDACV")) {
    g->GetYaxis()->SetTitle("Voltage [V]");
    g->Fit("pol1", "R", "L", 0., 180.);
  }

  if (sName.Contains("IDACI")) {
    g->GetYaxis()->SetTitle("I [nA]");
    g->Fit("pol1", "R", "L", 0., 220.);
  }
//=============================================================================

  return g;
}

//_____________________________________________________________________________
TGraph* TpADACScan::PlotLinearity(const char *DACName)
{
//
//  TpADACScan::PlotLinearity
//

  TBranch *bSetting = fAnalysis->GetDACData()->GetBranch("Setting");
  if (!bSetting) {
    Error(__func__, "DAC Scan data not found in tree !!");
    return 0x0;
  }

  TBranch *bValue = fAnalysis->GetDACData()->GetBranch(DACName);
  if (!bValue) {

    bValue = fAnalysis->GetDACData()->GetBranch(Form("/%s",DACName));
    if (!bValue) {
      Warning(__func__, "Data for %s not found in tree !!", DACName);
      return 0x0;
    }
  }
//=============================================================================

  int NEntries = bSetting->GetEntries();
  if (NEntries>256) {
    Error(__func__, "Illegal number of entries in tree: %d", NEntries);
    return 0x0;
  }
//=============================================================================

  int Setting; bSetting->SetAddress(&Setting);
  float Reading; bValue->SetAddress(&Reading);

  float Settings[256];
  float Readings[256];
  for (int i=0; i<NEntries; i++) {
    bSetting->GetEntry(i);
    bValue  ->GetEntry(i);
    Settings[i] = Setting;
    Readings[i] = Reading;
  }
//=============================================================================

  const TString sName = DACName;
  TGraph *gDAC = new TGraph(NEntries, Settings, Readings);
  gDAC->SetName(Form("gSDAC%s",DACName));

  if (sName.Contains("VDACV")) gDAC->Fit("pol1", "Q0R", "L", 0., 180.);
  if (sName.Contains("IDACI")) gDAC->Fit("pol1", "Q0R", "L", 0., 220.);
//=============================================================================

  TList *list = gDAC->GetListOfFunctions(); if (!list) return 0x0;
  TF1 *fxn = (TF1*)list->At(0); if (!fxn) return 0x0;

  fxn->SetName(Form("fSDAC%s",DACName));
  Double_t dSlop = fxn->GetParameter(1);

  float dLin[256];
  for (int i=0, k=1; k<NEntries; i++, k++) {
    dLin[i] = (Readings[k]-Readings[i]) / dSlop -1.;
  }
//=============================================================================

  TGraph *g = new TGraph(NEntries, Settings, dLin);
  g->SetName(Form("gL%s",DACName));
  g->SetTitle(DACName);

  g->GetXaxis()->SetTitle("DAC setting");
  g->GetYaxis()->SetTitle("Linearity");
  g->GetYaxis()->SetTitleOffset(1.5);
  g->Draw("APL");

  return g;
}
