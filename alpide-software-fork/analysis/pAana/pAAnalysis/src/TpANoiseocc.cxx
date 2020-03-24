#include "TH2D.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TMath.h"
#include "TLeaf.h"
#include "TBranch.h"

#include "TpANoiseocc.h"
#include "TpAAnalysis.h"

ClassImp(TpANoiseocc)

//_____________________________________________________________________________
TpANoiseocc::TpANoiseocc(TpAAnalysis *AAnalysis,
                         const char  *dataFile,
                         bool         Qualification,
                         const char  *Comment,
                         int          version) :
TpAHitmap(SCANTYPE_NOISEOCC,AAnalysis,dataFile,Qualification,Comment,version),
fNoisiestPixels(0),
fHistoNoisiestMap(0),
fHistoNoise(0),
fHistoNoiseFrac(0),
fHistoNoisiest(0),
fHistoNoisiestFrac(0)
{
//
//  TpANoiseocc::TpANoiseocc
//
}

//_____________________________________________________________________________
TpANoiseocc::TpANoiseocc(TpAAnalysis *AAnalysis, TConfig AConfig) :
TpAHitmap(AAnalysis,AConfig),
fNoisiestPixels(0),
fHistoNoisiestMap(0),
fHistoNoise(0),
fHistoNoiseFrac(0),
fHistoNoisiest(0),
fHistoNoisiestFrac(0)
{
//
//  TpANoiseocc::TpANoiseocc
//

  Mapping(0, "nhits");
}

//_____________________________________________________________________________
TpANoiseocc::TpANoiseocc(const TpANoiseocc &src) :
TpAHitmap(src),
fNoisiestPixels(src.fNoisiestPixels),
fHistoNoisiestMap(src.fHistoNoisiestMap),
fHistoNoise(src.fHistoNoise),
fHistoNoiseFrac(src.fHistoNoiseFrac),
fHistoNoisiest(src.fHistoNoisiest),
fHistoNoisiestFrac(src.fHistoNoisiestFrac)
{
//
// TpANoiseocc::TpANoiseocc
//
}

//_____________________________________________________________________________
TpANoiseocc& TpANoiseocc::operator=(const TpANoiseocc &src)
{
//
// TpANoiseocc::operator=
//

  if (&src==this) return *this;

  TpAHitmap::operator=(src);

  fNoisiestPixels   = src.fNoisiestPixels;
  fHistoNoisiestMap = src.fHistoNoisiestMap;

  fHistoNoise        = src.fHistoNoise;
  fHistoNoiseFrac    = src.fHistoNoiseFrac;
  fHistoNoisiest     = src.fHistoNoisiest;
  fHistoNoisiestFrac = src.fHistoNoisiestFrac;

  return *this;
}

//_____________________________________________________________________________
TpANoiseocc::~TpANoiseocc()
{
//
//  TpANoiseocc::~TpANoiseocc
//

  if (fHistoNoise)        { delete fHistoNoise;         fHistoNoise        = 0; }
  if (fHistoNoiseFrac)    { delete fHistoNoiseFrac;     fHistoNoiseFrac    = 0; }
  if (fHistoNoisiest)     { delete fHistoNoisiest;      fHistoNoisiest     = 0; }
  if (fHistoNoisiestFrac) { delete fHistoNoisiestFrac;  fHistoNoisiestFrac = 0; }
  if (fHistoNoisiestMap)  { delete fHistoNoisiestMap;   fHistoNoisiestMap  = 0; }
}

//_____________________________________________________________________________
void TpANoiseocc::Summarise()
{
//
//  TpANoiseocc::Summarise
//

  TString sName = GetScanName((TScanType)fConfig.SCANTYPE);
  Info(__func__, "%s: %s, ID: %s", fConfig.SCANNAME, sName.Data(), fConfig.IDENTSTRING);
//=============================================================================

  Info(__func__, "# of noisy pixels = %8d", fNoisyPixels);
  for (int i=0; i<NSEC; i++) Info(__func__, "  Sector %d: # noisy pixels = %8d, fraction = %8f%%", i, (int)GetNoisePixels(i), GetNoiseFraction(i));

  Info(__func__, "===========================================================");
//=============================================================================

  return;
}

//_____________________________________________________________________________
void TpANoiseocc::Mapping(const float TargetValue, const char *ALeaf)
{
//
//  TpANoiseocc::Mapping
//

  TBranch *bPixel = fAnalysis->GetScanData()->GetBranch("Pixel");
  TBranch *bScan  = fAnalysis->GetScanData()->GetBranch(fConfig.SCANNAME);

  if ((!bPixel) || (!bScan)) {
    Error(__func__, "Problem reading scan branch!!");
    return;
  }
//=============================================================================

  TLeaf *leaf = bScan->GetLeaf(ALeaf);
  if (!leaf) {
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

  int nPixels[NSEC];
  int nNoisePixels[NSEC];

  const int ns = 66;
  double dsVal[NSEC][ns];
  int ksCol[NSEC][ns], ksRow[NSEC][ns];

  for (int i=0; i<NSEC; i++) {
    nPixels[i] = 0;
    nNoisePixels[i] = 0;

    for (int j=0; j<ns; j++) {
      dsVal[i][j] = 0.;
      ksCol[i][j] = 0;
      ksRow[i][j] = 0;
    }
  }

//=============================================================================

  const TString sID   = fAnalysis->GetChipID();
  const TString sName = GetScanName((TScanType)fConfig.SCANTYPE);
  if (fHistoPixelMap) { delete fHistoPixelMap; fHistoPixelMap = 0; }
  fHistoPixelMap = new TH2D(Form("h%s_%s_%s_%s_PixelMap",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                            Form("%s %s %s %s pixel map",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                            1024, -.5, 1023.5, 512, -.5, 511.5);

  if (fHistoNoisyMap) { delete fHistoNoisyMap; fHistoNoisyMap = 0; }
  fHistoNoisyMap = new TH2D(Form("h%s_%s_%s_%s_NoisyMap",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                            Form("%s %s %s %s noisy map",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                            1024, -.5, 1023.5, 512, -.5, 511.5);
//=============================================================================

  fNPixels        = 0;
  fNoisyPixels    = 0;
  fNoisiestPixels = 0;
  float Value; leaf->SetAddress(&Value);
  for (int i=0; i<bScan->GetEntries(); i++) {
    bPixel->GetEntry(i);
    bScan ->GetEntry(i);
    leaf  ->GetValue(0);

    int kSector = address.col / (1024/NSEC);

    fNPixels += 1;
    nPixels[kSector] += 1;
    fHistoPixelMap->Fill(address.col, address.row, Value);

    if (Value>TargetValue) {
      fNoisyPixels += 1;
      nNoisePixels[kSector] += 1;
      fHistoNoisyMap->Fill(address.col, address.row, Value);

      for (int j=0; j<ns; j++) {
        if (Value>=dsVal[kSector][j]) {
          for (int k=ns-1; k>j; k--) {
            dsVal[kSector][k] = dsVal[kSector][k-1];
            ksCol[kSector][k] = ksCol[kSector][k-1];
            ksRow[kSector][k] = ksRow[kSector][k-1];
          }

          dsVal[kSector][j] = Value;
          ksCol[kSector][j] = address.col;
          ksRow[kSector][j] = address.row;
          break;
        }
      }
    }
  }
//=============================================================================

  if (fHistoNoise) { delete fHistoNoise; fHistoNoise = 0; }
  fHistoNoise = new TH1D(Form("h%s_%s_%s_%s_Noise",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                         Form("%s %s %s %s Noises",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                         NSEC, -0.5, -0.5+NSEC);

  if (fHistoNoiseFrac) { delete fHistoNoiseFrac; fHistoNoiseFrac = 0; }
  fHistoNoiseFrac = new TH1D(Form("h%s_%s_%s_%s_NoiseFrac",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                             Form("%s %s %s %s Noise frac",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                             NSEC, -0.5, -0.5+NSEC);

  if (fHistoNoisiest) { delete fHistoNoisiest; fHistoNoisiest = 0; }
  fHistoNoisiest = new TH1D(Form("h%s_%s_%s_%s_Noisiest",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                            Form("%s %s %s %s Noisiests",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                            NSEC, -0.5, -0.5+NSEC);

  if (fHistoNoisiestFrac) { delete fHistoNoisiestFrac; fHistoNoisiestFrac = 0; }
  fHistoNoisiestFrac = new TH1D(Form("h%s_%s_%s_%s_NoisiestFrac",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                                Form("%s %s %s %s Noisiest frac",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                                NSEC, -0.5, -0.5+NSEC);

  if (fHistoNoisiestMap) { delete fHistoNoisiestMap; fHistoNoisiestMap = 0; }
  fHistoNoisiestMap = new TH2D(Form("h%s_%s_%s_%s_NoisiestMap",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                               Form("%s %s %s %s noisiest map",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                               1024, -.5, 1023.5, 512, -.5, 511.5);
//=============================================================================

  for (int i=0, k=1; i<NSEC; i++, k++) {
    fHistoNoise->SetBinContent(k, nNoisePixels[i]);
    fHistoNoiseFrac->SetBinContent(k, 100.*nNoisePixels[i]/nPixels[i]);

    double dcs = 0.;
    for (int j=0; j<ns; j++) if (dsVal[i][j]>0.) {
      fHistoNoisiestMap->Fill(ksCol[i][j], ksRow[i][j], dsVal[i][j]);
      fNoisiestPixels += 1;
      dcs += 1.;
    }

    fHistoNoisiest->SetBinContent(k, dcs);
    fHistoNoisiestFrac->SetBinContent(k, 100.*dcs/nPixels[i]);
  }
//=============================================================================

  return;
}

//_____________________________________________________________________________
void TpANoiseocc::Plot()
{
//
//  TpANoiseocc::Plot
//

  new TStyle();
  gStyle->SetOptStat(0);
  const Bool_t bPrint = fAnalysis->IsPrint();
//=============================================================================

  TCanvas *cMap = 0;
  if ((fNoisyPixels>0) && (fHistoNoisyMap)) {
    cMap = new TCanvas(Form("c%s",fHistoNoisyMap->GetName()), fHistoNoisyMap->GetTitle(), 500, 500);

    fHistoNoisyMap->Draw("COLZ");

    cMap->cd(); cMap->Modified();
    cMap->cd(); cMap->SetSelected(cMap);
    if (bPrint) cMap->Print(Form("%s.pdf",cMap->GetName()));
  }
//=============================================================================

  if ((fNoisiestPixels>0) && (fHistoNoisiestMap)) {
    cMap = new TCanvas(Form("c%s",fHistoNoisiestMap->GetName()), fHistoNoisiestMap->GetTitle(), 500, 500);

    fHistoNoisiestMap->Draw("COLZ");

    cMap->cd(); cMap->Modified();
    cMap->cd(); cMap->SetSelected(cMap);
    if (bPrint) cMap->Print(Form("%s.pdf",cMap->GetName()));
  }
//=============================================================================

  TH1F *hFrame = 0;
  double dVal[NSEC];
  double dSector[NSEC];
  for (int i=0; i<NSEC; i++) dSector[i] = 1.*i;

  TGraph *g = 0;
  const TString sID   = fAnalysis->GetChipID();
  const TString sName = GetScanName((TScanType)fConfig.SCANTYPE);
  TCanvas *cNoisy = new TCanvas(Form("ch%s_%s_%s_Noise",sID.Data(),sName.Data(),fConfig.IDENTSTRING),
                                 Form( "%s %s %s Noise",sID.Data(),sName.Data(),fConfig.IDENTSTRING), 1000, 500);
  cNoisy->Divide(2,1);

  TVirtualPad *pad = cNoisy->cd(1); if (pad) {
    for (int i=0; i<NSEC; i++) dVal[i] = GetNoisePixels(i);
    hFrame = pad->DrawFrame(-0.5, 0.5*TMath::MinElement(NSEC,dVal), -0.5+NSEC, 1.2*TMath::MaxElement(NSEC,dVal));
    hFrame->SetXTitle("Sector");
    hFrame->SetTitle("# of noisy pixels");

    g = new TGraph(NSEC, dSector, dVal);
    g->SetMarkerStyle(kFullCircle);
    g->Draw("P");
  }

  pad = cNoisy->cd(2); if (pad) {
    for (int i=0; i<NSEC; i++) dVal[i] = GetNoiseFraction(i);
    hFrame = pad->DrawFrame(-0.5, 0.5*TMath::MinElement(NSEC,dVal), -0.5+NSEC, 1.2*TMath::MaxElement(NSEC,dVal));
    hFrame->SetXTitle("Sector");
    hFrame->SetTitle("Fraction of noisy pixels (%)");

    g = new TGraph(NSEC, dSector, dVal);
    g->SetMarkerStyle(kFullCircle);
    g->Draw("P");
  }

  cNoisy->cd(); cNoisy->Modified();
  cNoisy->cd(); cNoisy->SetSelected(cNoisy);
  if (bPrint) cNoisy->Print(Form("%s.pdf",cNoisy->GetName()));
//=============================================================================

  TCanvas *cNoisiest = new TCanvas(Form("ch%s_%s_%s_Noisiest",sID.Data(),sName.Data(),fConfig.IDENTSTRING),
                                   Form(  "%s %s %s Noisiest",sID.Data(),sName.Data(),fConfig.IDENTSTRING), 1000, 500);
  cNoisiest->Divide(2,1);

  pad = cNoisiest->cd(1); if (pad) {
    for (int i=0; i<NSEC; i++) dVal[i] = GetNoisiestPixels(i);
    hFrame = pad->DrawFrame(-0.5, 0.5*TMath::MinElement(NSEC,dVal), -0.5+NSEC, 1.2*TMath::MaxElement(NSEC,dVal));
    hFrame->SetXTitle("Sector");
    hFrame->SetTitle("# of noisiest pixels");

    g = new TGraph(NSEC, dSector, dVal);
    g->SetMarkerStyle(kFullCircle);
    g->Draw("P");
  }

  pad = cNoisiest->cd(2); if (pad) {
    for (int i=0; i<NSEC; i++) dVal[i] = GetNoisiestFraction(i);
    hFrame = pad->DrawFrame(-0.5, 0.5*TMath::MinElement(NSEC,dVal), -0.5+NSEC, 1.2*TMath::MaxElement(NSEC,dVal));
    hFrame->SetXTitle("Sector");
    hFrame->SetTitle("Fraction of noisiest pixels (%)");

    g = new TGraph(NSEC, dSector, dVal);
    g->SetMarkerStyle(kFullCircle);
    g->Draw("P");
  }

  cNoisiest->cd(); cNoisiest->Modified();
  cNoisiest->cd(); cNoisiest->SetSelected(cNoisiest);
  if (bPrint) cNoisiest->Print(Form("%s.pdf",cNoisiest->GetName()));
//=============================================================================

  return;
}

//_____________________________________________________________________________
void TpANoiseocc::PrintNoisiestPixels()
{
//
//  TpANoiseocc::PrintNoisiestPixels
//

  if (!fHistoNoisiestMap) {
    Error(__func__, "Noisiest map is not defined!!");
    return;
  }
//=============================================================================

  const int ns = 66;
  double dVal[NSEC][ns];
  int kn[NSEC], kCol[NSEC][ns], kRow[NSEC][ns];

  for (int i=0; i<NSEC; i++) {
    kn[i] = 0;

    for (int j=0; j<ns; j++) {
      dVal[i][j] = 0;
      kCol[i][j] = 0;
      kRow[i][j] = 0;
    }
  }
//=============================================================================

  for (int ic=0, kc=1; ic<fHistoNoisiestMap->GetNbinsX(); ic++, kc++) {

    int ks = ic / (1024/NSEC);
    for (int ir=0, kr=1; ir<fHistoNoisiestMap->GetNbinsY(); ir++, kr++) {
      if (fHistoNoisiestMap->GetBinContent(kc,kr)>0.) {
        dVal[ks][kn[ks]] = fHistoNoisiestMap->GetBinContent(kc,kr);
        kCol[ks][kn[ks]] = ic;
        kRow[ks][kn[ks]] = ir;
        kn[ks] += 1;
      }
    }
  }
//=============================================================================

  std::cout << std::endl;
  const TString sName = GetScanName((TScanType)fConfig.SCANTYPE);
  Info(__func__, "%s %s %s", sName.Data(), fConfig.SCANNAME, fConfig.IDENTSTRING);

  for (int i=0; i<NSEC; i++) {
    Info(__func__, "Sector %d:", i);

    for (int j=0; j<ns; j++) if (dVal[i][j]>0.) {
      Info(__func__, "  col = %4d, row = %4d, val = %d", kCol[i][j], kRow[i][j], (int)dVal[i][j]);
    }
  }

  Info(__func__, "Total # of noisiest pixels = %d", fNoisiestPixels);
  Info(__func__, "===========================================================");
//=============================================================================

  return;
}
