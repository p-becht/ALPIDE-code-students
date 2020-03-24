#include "TStyle.h"
#include "TMath.h"
#include "TH2D.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TCanvas.h"

#include "TLeaf.h"
#include "TBranch.h"

#include "TpAAnalysis.h"
#include "TpAThresholdScan.h"

ClassImp(TpAThresholdScan)

//_____________________________________________________________________________
TpAThresholdScan::TpAThresholdScan(TpAAnalysis *AAnalysis,
                                   const char  *dataFile,
                                   bool         Qualification,
                                   const char  *Comment,
                                   int          version) :
TpAHitmap(SCANTYPE_THRESHOLD,AAnalysis,dataFile,Qualification,Comment,version),
fHistoThreshMean(0),
fHistoNoiseMean(0),
fHistoChisqMean(0),
fHistoThreshDist(0),
fHistoNoiseDist(0),
fHistoChisqDist(0)
{
//
//  TpAThresholdScan::TpAThresholdScan
//
}

//_____________________________________________________________________________
TpAThresholdScan::TpAThresholdScan(TpAAnalysis *AAnalysis, TConfig AConfig) :
TpAHitmap(AAnalysis,AConfig),
fHistoThreshMean(0),
fHistoNoiseMean(0),
fHistoChisqMean(0),
fHistoThreshDist(0),
fHistoNoiseDist(0),
fHistoChisqDist(0)
{
//
//  TpAThresholdScan::TpAThresholdScan
//

  Mapping(0., "thresh");
}

//_____________________________________________________________________________
TpAThresholdScan::TpAThresholdScan(const TpAThresholdScan &src) :
TpAHitmap(src),
fHistoThreshMean(src.fHistoThreshMean),
fHistoNoiseMean(src.fHistoNoiseMean),
fHistoChisqMean(src.fHistoChisqMean),
fHistoThreshDist(src.fHistoThreshDist),
fHistoNoiseDist(src.fHistoNoiseDist),
fHistoChisqDist(src.fHistoChisqDist)
{
//
// TpAThresholdScan::TpAThresholdScan
//
}

//_____________________________________________________________________________
TpAThresholdScan& TpAThresholdScan::operator=(const TpAThresholdScan &src)
{
//
// TpAThresholdScan::operator=
//

  if (&src==this) return *this;

  TpAHitmap::operator=(src);

  fHistoThreshMean = src.fHistoThreshMean;
   fHistoNoiseMean = src.fHistoNoiseMean;
   fHistoChisqMean = src.fHistoChisqMean;

  fHistoThreshDist = src.fHistoThreshDist;
   fHistoNoiseDist = src.fHistoNoiseDist;
   fHistoChisqDist = src.fHistoChisqDist;

  return *this;
}

//_____________________________________________________________________________
TpAThresholdScan::~TpAThresholdScan()
{
//
// TpAThresholdScan::~TpAThresholdScan
//

  if (fHistoThreshMean) { delete fHistoThreshMean; fHistoThreshMean = 0; }
  if (fHistoNoiseMean)  { delete fHistoNoiseMean;  fHistoNoiseMean  = 0; }
  if (fHistoChisqMean)  { delete fHistoChisqMean;  fHistoChisqMean  = 0; }

  if (fHistoThreshDist) { delete fHistoThreshDist; fHistoThreshDist = 0; }
  if (fHistoNoiseDist)  { delete fHistoNoiseDist;  fHistoNoiseDist  = 0; }
  if (fHistoChisqDist)  { delete fHistoChisqDist;  fHistoChisqDist  = 0; }
}

//_____________________________________________________________________________
void TpAThresholdScan::Summarise()
{
//
//  TpAThresholdScan::Summarise
//

  TpAHitmap::Summarise();
//=============================================================================

  Info(__func__, "Thresholds:");
  for (int i=0; i<NSEC; i++) Info(__func__, "  Sector %d: %f +- %f", i, GetThresholdMean(i), GetThresholdRMS(i));
//=============================================================================

  Info(__func__, "Noise:");
  for (int i=0; i<NSEC; i++) Info(__func__, "  Sector %d: %f +- %f", i, GetNoiseMean(i), GetNoiseRMS(i));
//=============================================================================

  Info(__func__, "Chisq:");
  for (int i=0; i<NSEC; i++) Info(__func__, "  Sector %d: %f +- %f", i, GetChisqMean(i), GetChisqRMS(i));
//=============================================================================

  Info(__func__, "===========================================================");
//=============================================================================

  return;
}

//_____________________________________________________________________________
void TpAThresholdScan::Mapping(const float TargetValue, const char *ALeaf)
{
//
//  TpAThresholdScan::Mapping
//

  TBranch *bPixel = fAnalysis->GetScanData()->GetBranch("Pixel");
  TBranch *bScan  = fAnalysis->GetScanData()->GetBranch(fConfig.SCANNAME);

  if ((!bPixel) || (!bScan)) {
    Error(__func__, "Problem reading branches!!");
    return;
  }
//=============================================================================

  TLeaf *lThresh = bScan->GetLeaf("thresh");
  TLeaf *lNoise  = bScan->GetLeaf("noise");
  TLeaf *lChisq  = bScan->GetLeaf("chisq");

  if ((!lThresh) || (!lNoise) || (!lChisq)) {
    Error(__func__, "Problem reading leaf!!");
    return;
  }
//=============================================================================

  struct address_t {
    int col;
    int row;
  } address;

  bPixel->SetAddress(&address);
//=============================================================================

  float dThresh; lThresh->SetAddress(&dThresh);
  float dNoise;  lNoise->SetAddress(&dNoise);
  float dChisq;  lChisq->SetAddress(&dChisq);
//=============================================================================

  int NPix[NSEC];
  double dThreshMean[NSEC], dThreshRMS[NSEC];
  double  dNoiseMean[NSEC],  dNoiseRMS[NSEC];
  double  dChisqMean[NSEC],  dChisqRMS[NSEC];

  for (int i=0; i<NSEC; i++) {
    NPix[i] = 0;
    dThreshMean[i] = 0.; dThreshRMS[i] = 0.;
     dNoiseMean[i] = 0.;  dNoiseRMS[i] = 0.;
     dChisqMean[i] = 0.;  dChisqRMS[i] = 0.;
  }
//=============================================================================

  const TString sID   = fAnalysis->GetChipID();
  const TString sName = GetScanName((TScanType)fConfig.SCANTYPE);
  if (fHistoDeadMap) { delete fHistoDeadMap; fHistoDeadMap = 0; }
  fHistoDeadMap = new TH2D(Form("h%s_%s_%s_%s_DeadMap",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                           Form("%s %s %s %s dead map",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                           1024, -.5, 1023.5, 512, -.5, 511.5);

  if (fHistoNoisyMap) { delete fHistoNoisyMap; fHistoNoisyMap = 0; }
  fHistoNoisyMap = new TH2D(Form("h%s_%s_%s_%s_NoisyMap",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                            Form("%s %s %s %s noisy map",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                            1024, -.5, 1023.5, 512, -.5, 511.5);
//=============================================================================

  if (fHistoThreshDist) { delete fHistoThreshDist; fHistoThreshDist = 0; }

  fHistoThreshDist = new TH2D(Form("h%s_%s_%s_%s_ThreshDist",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                              Form("%s %s %s %s Thresh dist",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                              300, 0., 600., NSEC, -0.5, -0.5+NSEC);

  if (fHistoNoiseDist) { delete fHistoNoiseDist; fHistoNoiseDist = 0; }
  fHistoNoiseDist = new TH2D(Form("h%s_%s_%s_%s_NoiseDist",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                             Form("%s %s %s %s Noise dist",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                             150, 0.,  60., NSEC, -0.5, -0.5+NSEC);

  if (fHistoChisqDist) { delete fHistoChisqDist; fHistoChisqDist = 0; }
  fHistoChisqDist = new TH2D(Form("h%s_%s_%s_%s_ChisqDist",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                             Form("%s %s %s %s Chisq dist",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                             150, 0.,  30., NSEC, -0.5, -0.5+NSEC);
//=============================================================================

  for (int i=0; i<bPixel->GetEntries(); i++) {
    bPixel->GetEntry(i);
    bScan ->GetEntry(i);

    lThresh->GetValue(0);
    lNoise->GetValue(0);
    lChisq->GetValue(0);

    if (dThresh==0) {
      fNDeadPixels += 1;
      fHistoDeadMap->Fill(address.col, address.row);
      continue;
    }

    int kSector = address.col / (1024/NSEC);
    fHistoChisqDist ->Fill(dChisq,  kSector);

    if (dChisq<CHISQCUT) {
      fHistoThreshDist->Fill(dThresh, kSector);
      fHistoNoiseDist ->Fill(dNoise,  kSector);

      fNPixels += 1;
      NPix[kSector] += 1;
      dThreshMean[kSector] += dThresh;
      dThreshRMS[kSector]  += dThresh*dThresh;

      dNoiseMean[kSector] += dNoise;
      dNoiseRMS[kSector]  += dNoise*dNoise;

      dChisqMean[kSector] += dChisq;
      dChisqRMS[kSector]  += dChisq*dChisq;
    } else {
      fNoisyPixels += 1;
      fHistoNoisyMap->Fill(address.col, address.row, dChisq);
    }
  }
//=============================================================================

  if (fHistoThreshMean) { delete fHistoThreshMean; fHistoThreshMean = 0; }
  fHistoThreshMean = new TH1D(Form("h%s_%s_%s_%s_ThreshMean",sName.Data(),fConfig.SCANNAME,fConfig.IDENTSTRING,ALeaf),
                              Form("%s %s %s %s Thresh mean",sName.Data(),fConfig.SCANNAME,fConfig.IDENTSTRING,ALeaf),
                              NSEC, -0.5, -0.5+NSEC);

  if (fHistoNoiseMean) { delete fHistoNoiseMean; fHistoNoiseMean = 0; }
  fHistoNoiseMean = new TH1D(Form("h%s_%s_%s_%s_NoiseMean",sName.Data(),fConfig.SCANNAME,fConfig.IDENTSTRING,ALeaf),
                             Form("%s %s %s %s Noise mean",sName.Data(),fConfig.SCANNAME,fConfig.IDENTSTRING,ALeaf),
                             NSEC, -0.5, -0.5+NSEC);

  if (fHistoChisqMean) { delete fHistoChisqMean; fHistoChisqMean = 0; }
  fHistoChisqMean = new TH1D(Form("h%s_%s_%s_%s_ChisqMean",sName.Data(),fConfig.SCANNAME,fConfig.IDENTSTRING,ALeaf),
                             Form("%s %s %s %s Chisq mean",sName.Data(),fConfig.SCANNAME,fConfig.IDENTSTRING,ALeaf),
                             NSEC, -0.5, -0.5+NSEC);
//=============================================================================

  for (int i=0, k=1; i<NSEC; i++, k++) {
    dThreshMean[i] /= NPix[i];
    dThreshRMS[i]   = TMath::Abs(dThreshMean[i]*dThreshMean[i] - dThreshRMS[i]/NPix[i]);
    dThreshRMS[i]   = TMath::Sqrt(dThreshRMS[i]);
    fHistoThreshMean->SetBinContent(k, dThreshMean[i]);
    fHistoThreshMean->SetBinError(k, dThreshRMS[i]);

    dNoiseMean[i] /= NPix[i];
    dNoiseRMS[i]   = TMath::Abs(dNoiseMean[i]*dNoiseMean[i] - dNoiseRMS[i]/NPix[i]);
    dNoiseRMS[i]   = TMath::Sqrt(dNoiseRMS[i]);
    fHistoNoiseMean->SetBinContent(k, dNoiseMean[i]);
    fHistoNoiseMean->SetBinError(k, dNoiseRMS[i]);

    dChisqMean[i] /= NPix[i];
    dChisqRMS[i]   = TMath::Abs(dChisqMean[i]*dChisqMean[i] - dChisqRMS[i]/NPix[i]);
    dChisqRMS[i]   = TMath::Sqrt(dChisqRMS[i]);
    fHistoChisqMean->SetBinContent(k, dChisqMean[i]);
    fHistoChisqMean->SetBinError(k, dChisqRMS[i]);
  }
//=============================================================================

  return;
}

//_____________________________________________________________________________
void TpAThresholdScan::Plot()
{
//
//  TpAThresholdScan::Plot
//

  TpAHitmap::Plot();
//=============================================================================

  gStyle->SetOptStat(0);
  const Bool_t bPrint = fAnalysis->IsPrint();
//=============================================================================

  double dTmp = 0.;
  double dThreshMax = 0.;
  double dNoiseMax  = 0.;
  double dChisqMax  = 0.;

  TH1D *hThreshDist[NSEC];
  TH1D *hNoiseDist [NSEC];
  TH1D *hChisqDist [NSEC];
  for (int i=0, k=1; i<NSEC; i++, k++) {
    hThreshDist[i] = fHistoThreshDist->ProjectionX(Form("%s_Sec%d",fHistoThreshDist->GetName(),i), k, k);
    hNoiseDist [i] = fHistoNoiseDist ->ProjectionX(Form("%s_Sec%d",fHistoNoiseDist ->GetName(),i), k, k);
    hChisqDist [i] = fHistoChisqDist ->ProjectionX(Form("%s_Sec%d",fHistoChisqDist ->GetName(),i), k, k);

    dTmp = hThreshDist[i]->GetMaximum(); if (dThreshMax<dTmp) dThreshMax = dTmp;
    dTmp =  hNoiseDist[i]->GetMaximum(); if (dNoiseMax <dTmp) dNoiseMax  = dTmp;
    dTmp =  hChisqDist[i]->GetMaximum(); if (dChisqMax <dTmp) dChisqMax  = dTmp;
  }
//=============================================================================

  const TString sID   = fAnalysis->GetChipID();
  const TString sName = GetScanName((TScanType)fConfig.SCANTYPE);
  const Color_t wcl[] = { kBlack,  kRed+1,  kBlue+1, kGreen+3, kMagenta+1, kOrange-1, kCyan+2, kYellow+2 };
  TCanvas *cDist = new TCanvas(Form("ch%s_%s_%s_Dist",sID.Data(),sName.Data(),fConfig.IDENTSTRING),
                               Form(  "%s %s %s Dist",sID.Data(),sName.Data(),fConfig.IDENTSTRING), 1500, 500);

  cDist->Divide(3,1);

  TVirtualPad *pad = cDist->cd(1); if (pad) {
    pad->SetLogy();
    for (int i=0; i<8; i++) {
      if (i==0) {
        hThreshDist[i]->SetTitle("Threshold");
        hThreshDist[i]->SetAxisRange(0.8, 1.2*dThreshMax, "Y");
      }

      hThreshDist[i]->SetLineWidth(2);
      hThreshDist[i]->SetLineColor(wcl[i]);
      hThreshDist[i]->Draw(i==0 ? "" : "SAME");
    }
  }

  pad = cDist->cd(2); if (pad) {
    pad->SetLogy();
    for (int i=0; i<8; i++) {
      if (i==0) {
        hNoiseDist[i]->SetTitle("Noise");
        hNoiseDist[i]->SetAxisRange(0.8, 1.2*dNoiseMax, "Y");
      }

      hNoiseDist[i]->SetLineWidth(2);
      hNoiseDist[i]->SetLineColor(wcl[i]);
      hNoiseDist[i]->Draw(i==0 ? "" : "SAME");
    }
  }

  pad = cDist->cd(3); if (pad) {
    pad->SetLogy();
    TLegend *leg = new TLegend(0.7, 0.3 ,0.98, 0.94);
    for (int i=0; i<8; i++) {

      if (i==0) {
        hChisqDist[i]->SetTitle("#chi^{2}/NDF");
        hChisqDist[i]->SetAxisRange(0.8, 1.2*dChisqMax, "Y");
      }

      hChisqDist[i]->SetLineWidth(2);
      hChisqDist[i]->SetLineColor(wcl[i]);
      hChisqDist[i]->Draw(i==0 ? "" : "SAME");
      leg->AddEntry(hChisqDist[i], Form("Sector %d",i), "L");
    } leg->Draw();
  }

  cDist->cd(); cDist->Modified();
  cDist->cd(); cDist->SetSelected(cDist);
  if (bPrint)  cDist->Print(Form("%s.pdf",cDist->GetName()));
//=============================================================================

  TH1F *hFrame = 0;
  double dVal[NSEC];
  double dSector[NSEC];
  for (int i=0; i<NSEC; i++) dSector[i] = 1.*i;

  TCanvas *cMean = new TCanvas(Form("ch%s_%s_%s_Mean",sID.Data(),sName.Data(),fConfig.IDENTSTRING),
                               Form(  "%s %s %s Mean",sID.Data(),sName.Data(),fConfig.IDENTSTRING), 1500, 1000);
  cMean->Divide(3,2);

  pad = cMean->cd(1); if (pad) {
    for (int i=0; i<NSEC; i++) dVal[i] = GetThresholdMean(i);
    hFrame = pad->DrawFrame(-0.5, 0.5*TMath::MinElement(NSEC,dVal), -0.5+NSEC, 1.2*TMath::MaxElement(NSEC,dVal));
    hFrame->SetXTitle("Sector");
    hFrame->SetTitle("Mean threshold");

    TGraph *gThreshVal = new TGraph(NSEC, dSector, dVal);
    gThreshVal->SetMarkerStyle(kFullCircle);
    gThreshVal->Draw("P");
  }

  pad = cMean->cd(2); if (pad) {
    for (int i=0; i<NSEC; i++) dVal[i] = GetNoiseMean(i);
    hFrame = pad->DrawFrame(-0.5, 0.5*TMath::MinElement(NSEC,dVal), -0.5+NSEC, 1.2*TMath::MaxElement(NSEC,dVal));
    hFrame->SetXTitle("Sector");
    hFrame->SetTitle("Mean noise");

    TGraph *gNoiseVal = new TGraph(NSEC, dSector, dVal);
    gNoiseVal->SetMarkerStyle(kFullCircle);
    gNoiseVal->Draw("P");
  }

  pad = cMean->cd(3); if (pad) {
    for (int i=0; i<NSEC; i++) dVal[i] = GetChisqMean(i);
    hFrame = pad->DrawFrame(-0.5, 0.5*TMath::MinElement(NSEC,dVal), -0.5+NSEC, 1.2*TMath::MaxElement(NSEC,dVal));
    hFrame->SetXTitle("Sector");
    hFrame->SetTitle("Mean #chi^{2}");

    TGraph *gChisqVal = new TGraph(NSEC, dSector, dVal);
    gChisqVal->SetMarkerStyle(kFullCircle);
    gChisqVal->Draw("P");
  }

  pad = cMean->cd(4); if (pad) {
    for (int i=0; i<NSEC; i++) dVal[i] = GetThresholdRMS(i);
    hFrame = pad->DrawFrame(-0.5, 0.5*TMath::MinElement(NSEC,dVal), -0.5+NSEC, 1.2*TMath::MaxElement(NSEC,dVal));
    hFrame->SetXTitle("Sector");
    hFrame->SetTitle("Threshold RMS");

    TGraph *gThreshRMS = new TGraph(NSEC, dSector, dVal);
    gThreshRMS->SetMarkerStyle(kFullCircle);
    gThreshRMS->Draw("P");
  }

  pad = cMean->cd(5); if (pad) {
    for (int i=0; i<NSEC; i++) dVal[i] = GetNoiseRMS(i);
    hFrame = pad->DrawFrame(-0.5, 0.5*TMath::MinElement(NSEC,dVal), -0.5+NSEC, 1.2*TMath::MaxElement(NSEC,dVal));
    hFrame->SetXTitle("Sector");
    hFrame->SetTitle("Noise RMS");

    TGraph *gNoiseRMS = new TGraph(NSEC, dSector, dVal);
    gNoiseRMS->SetMarkerStyle(kFullCircle);
    gNoiseRMS->Draw("P");
  }

  pad = cMean->cd(6); if (pad) {
    for (int i=0; i<NSEC; i++) dVal[i] = GetChisqRMS(i);
    hFrame = pad->DrawFrame(-0.5, 0.5*TMath::MinElement(NSEC,dVal), -0.5+NSEC, 1.2*TMath::MaxElement(NSEC,dVal));
    hFrame->SetXTitle("Sector");
    hFrame->SetTitle("#chi^{2} RMS");

    TGraph *gChisqRMS = new TGraph(NSEC, dSector, dVal);
    gChisqRMS->SetMarkerStyle(kFullCircle);
    gChisqRMS->Draw("P");
  }

  cMean->cd(); cMean->Modified();
  cMean->cd(); cMean->SetSelected(cMean);
  if (bPrint)  cMean->Print(Form("%s.pdf",cMean->GetName()));
//=============================================================================

  return;
}
