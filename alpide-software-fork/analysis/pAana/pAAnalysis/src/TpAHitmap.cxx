#include "TH2D.h"
#include "TLeaf.h"
#include "TBranch.h"
#include "TCanvas.h"
#include "TStyle.h"

#include "TpAHitmap.h"
#include "TpAAnalysis.h"

ClassImp(TpAHitmap)

//_____________________________________________________________________________
TpAHitmap::TpAHitmap(TScanType    AScanType,
                     TpAAnalysis *AAnalysis,
                     const char  *dataFile,
                     bool         Qualification,
                     const char  *Comment,
                     int          version) :
TpAScan(AScanType,AAnalysis,Qualification,Comment,version),
fNPixels(0),
fNDeadPixels(0),
fNInefficientPixels(0),
fNoisyPixels(0),
fHistoPixelMap(0),
fHistoDeadMap(0),
fHistoInefficientMap(0),
fHistoNoisyMap(0)
{
//
//  TpAHitmap::TpAHitmap
//
  Init(dataFile);
}

//_____________________________________________________________________________
TpAHitmap::TpAHitmap(TpAAnalysis *AAnalysis, TConfig AConfig) :
TpAScan(AAnalysis,AConfig),
fNPixels(0),
fNDeadPixels(0),
fNInefficientPixels(0),
fNoisyPixels(0),
fHistoPixelMap(0),
fHistoDeadMap(0),
fHistoInefficientMap(0),
fHistoNoisyMap(0)
{
//
//  TpAHitmap::TpAHitmap
//
}

//_____________________________________________________________________________
TpAHitmap::TpAHitmap(const TpAHitmap &src) :
TpAScan(src),
fNPixels(src.fNPixels),
fNDeadPixels(src.fNDeadPixels),
fNInefficientPixels(src.fNInefficientPixels),
fNoisyPixels(src.fNoisyPixels),
fHistoPixelMap(src.fHistoPixelMap),
fHistoDeadMap(src.fHistoDeadMap),
fHistoInefficientMap(src.fHistoInefficientMap),
fHistoNoisyMap(src.fHistoNoisyMap)
{
//
// TpAHitmap::TpAHitmap
//
}

//_____________________________________________________________________________
TpAHitmap& TpAHitmap::operator=(const TpAHitmap &src)
{
//
// TpAHitmap::operator=
//

  if (&src==this) return *this;

  TpAScan::operator=(src);

  fNPixels            = src.fNPixels;
  fNDeadPixels        = src.fNDeadPixels;
  fNInefficientPixels = src.fNInefficientPixels;
  fNoisyPixels        = src.fNoisyPixels;

  fHistoPixelMap       = src.fHistoPixelMap;
  fHistoDeadMap        = src.fHistoDeadMap;
  fHistoInefficientMap = src.fHistoInefficientMap;
  fHistoNoisyMap       = src.fHistoNoisyMap;

  return *this;
}

//_____________________________________________________________________________
TpAHitmap::~TpAHitmap()
{
//
// TpAHitmap::~TpAHitmap
//

  if (fHistoPixelMap)       { delete fHistoPixelMap;       fHistoPixelMap       = 0;}
  if (fHistoDeadMap)        { delete fHistoDeadMap;        fHistoDeadMap        = 0;}
  if (fHistoInefficientMap) { delete fHistoInefficientMap; fHistoInefficientMap = 0;}
  if (fHistoNoisyMap)       { delete fHistoNoisyMap;       fHistoNoisyMap       = 0;}
}

//_____________________________________________________________________________
bool TpAHitmap::ReadDataFile(const char *fName)
{

  if (fConfig.SCANTYPE==SCANTYPE_THRESHOLD) {
    Info(__func__, "Reading threshold fit data");

    float allnoise [1024][512];
    float allchisq [1024][512];
    float allthresh[1024][512];
    for (int icol=0; icol<1024; icol++) {
      for (int irow=0; irow<512; irow++) {
        allnoise [icol][irow] = 0.;
        allchisq [icol][irow] = 0.;
        allthresh[icol][irow] = 0.;
      }
    }

    FILE *fp = fopen(fName, "r");
    if (!fp) {
      Error(__func__, "Can not read data file!!");
      return false;
    }

    int    col, row;
    int   dcol, address;
    float thresh, noise, chisq;
    while (fscanf(fp, "%d %d %f %f %f", &dcol, &address, &thresh, &noise, &chisq)==5) {
//    row = AddressToRow(address);
//    col = AddressToColumn(dcol, address);
      DcolAddr2ColRow(dcol, address, col, row);

      allnoise [col][row] = noise;
      allchisq [col][row] = chisq;
      allthresh[col][row] = thresh;
    }

    fclose(fp);
//=============================================================================

    struct result_t {
      float thresh;
      float noise;
      float chisq;
    } result;

    TBranch *scanBranch = fAnalysis->GetScanData()->Branch(fConfig.SCANNAME, &result.thresh, "thresh/F:noise/F:chisq/F");

    for (int ipix=0; ipix<NPIX; ipix++) {
      EntryToColRow(ipix, col, row);

      result.noise  = allnoise [col][row];
      result.chisq  = allchisq [col][row];
      result.thresh = allthresh[col][row];

      scanBranch->Fill();
    }

    return true;
  }
//=============================================================================

  Info(__func__, "Reading hitmap");
//=============================================================================

  float allhits[1024][512];
  for (int icol=0; icol<1024; icol++) {
    for (int irow=0; irow<512; irow++) {
      allhits[icol][irow] = 0.;
    }
  }

  FILE *fp = fopen (fName, "r");
  if (!fp) {
    Error(__func__, "Can not read data file: %s!!", fName);
    return false;
  }

  int col, row, nhits, dcol, address;
  while (fscanf(fp,"%d %d %d",&dcol,&address,&nhits)==3) {
//  row = AddressToRow(address);
//  col = AddressToColumn(dcol, address);
    DcolAddr2ColRow(dcol, address, col, row);
    allhits[col][row] = (float)nhits;
  }

  fclose(fp);
//=============================================================================

  struct result_t {
    float nhits;
  } result;

  TBranch *scanBranch = fAnalysis->GetScanData()->Branch(fConfig.SCANNAME, &result.nhits, "nhits/F");

  for (int ipix=0; ipix<NPIX; ipix++) {
    EntryToColRow(ipix, col, row);

    result.nhits  = allhits[col][row];
    scanBranch->Fill();
  }
//=============================================================================

  return true;
}

//_____________________________________________________________________________
void TpAHitmap::Summarise()
{
//
//  TpAHitmap::Summarise
//

  TString sName = GetScanName((TScanType)fConfig.SCANTYPE);
  Info(__func__, "%s: %s, ID: %s", fConfig.SCANNAME, sName.Data(), fConfig.IDENTSTRING);
//=============================================================================

  Info(__func__, "  # of dead        pixels = %8d", GetNDeadPixels());
  Info(__func__, "  # of inefficient pixels = %8d", GetNInefficientPixels());
  Info(__func__, "  # of noisy       pixels = %8d", GetNNoisyPixels());
  Info(__func__, "===========================================================");
//=============================================================================

  return;
}

//_____________________________________________________________________________
void TpAHitmap::Mapping(const float TargetValue, const char *ALeaf)
{
//
//  TpAHitmap::Mapping
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

  const TString sID   = fAnalysis->GetChipID();
  const TString sName = GetScanName((TScanType)fConfig.SCANTYPE);
  if (fHistoPixelMap) { delete fHistoPixelMap; fHistoPixelMap = 0; }
  fHistoPixelMap = new TH2D(Form("h%s_%s_%s_%s_PixelMap",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                            Form("%s %s %s %s pixel map",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                            1024, -.5, 1023.5, 512, -.5, 511.5);

  if (fHistoDeadMap) { delete fHistoDeadMap; fHistoDeadMap = 0; }
  fHistoDeadMap = new TH2D(Form("h%s_%s_%s_%s_DeadMap",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                           Form("%s %s %s %s dead map",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                           1024, -.5, 1023.5, 512, -.5, 511.5);

  if (fHistoInefficientMap) { delete fHistoInefficientMap; fHistoInefficientMap = 0; }
  fHistoInefficientMap = new TH2D(Form("h%s_%s_%s_%s_InefficientMap",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                                  Form("%s %s %s %s inefficient map",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                                  1024, -.5, 1023.5, 512, -.5, 511.5);

  if (fHistoNoisyMap) { delete fHistoNoisyMap; fHistoNoisyMap = 0; }
  fHistoNoisyMap = new TH2D(Form("h%s_%s_%s_%s_NoisyMap",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                            Form("%s %s %s %s noisy map",sID.Data(),sName.Data(),fConfig.IDENTSTRING,ALeaf),
                            1024, -.5, 1023.5, 512, -.5, 511.5);
//=============================================================================

  fNPixels            = 0;
  fNDeadPixels        = 0;
  fNInefficientPixels = 0;
  fNoisyPixels        = 0;
  float Value; leaf->SetAddress(&Value);
  for (int i=0; i<bScan->GetEntries(); i++) {
    bPixel->GetEntry(i);
    bScan ->GetEntry(i);
    leaf  ->GetValue(0);

    fNPixels += 1;
    fHistoPixelMap->Fill(address.col, address.row, Value);

    if (Value==0) {
      fNDeadPixels += 1;
      fHistoDeadMap->Fill(address.col, address.row);
    }

    if ((Value>0) && (Value<TargetValue)) {
      fNInefficientPixels += 1;
      fHistoInefficientMap->Fill(address.col, address.row, Value);
    }

    if (Value>TargetValue) {
      fNoisyPixels += 1;
      fHistoNoisyMap->Fill(address.col, address.row, Value);
    }
  }
//=============================================================================

  return;
}

//_____________________________________________________________________________
int TpAHitmap::CountPixels(int TargetValue, int &Equal, int &Above, int &Below, const char *ALeaf)
{
//
//  TpAHitmap::CountPixels
//

  Equal = 0;
  Above = 0;
  Below = 0;
//=============================================================================

  TBranch *bScan = fAnalysis->GetScanData()->GetBranch(fConfig.SCANNAME);
  if (!bScan) {
    Error(__func__, "Problem reading scan branch!!");
    return 0;
  }

  TLeaf *leaf = bScan->GetLeaf(ALeaf);
  if (!leaf) {
    Error(__func__, "Problem reading leaf!!");
    return 0;
  }
//=============================================================================

  int NPix  = 0;
  float Value; leaf->SetAddress(&Value);
  for (int i=0; i<bScan->GetEntries(); i++) {
    bScan ->GetEntry(i);
    leaf->GetValue(0);

    if (Value==TargetValue) Equal++;
    else if (Value<TargetValue) Below ++;
    else if (Value>TargetValue) Above ++;
    NPix++;
  }
//=============================================================================

  return NPix;
}

//_____________________________________________________________________________
void TpAHitmap::Plot()
{
//
//  TpAHitmap::Plot
//

  new TStyle();
  gStyle->SetOptStat(0);
  const Bool_t bPrint = fAnalysis->IsPrint();
//=============================================================================

  TCanvas *cMap = 0;
  if ((fNPixels>0) && (fHistoPixelMap)) {
    cMap = new TCanvas(Form("c%s",fHistoPixelMap->GetName()), fHistoPixelMap->GetTitle(), 500, 500);

    fHistoPixelMap->Draw("COLZ");

    cMap->cd(); cMap->Modified();
    cMap->cd(); cMap->SetSelected(cMap);
    if (bPrint) cMap->Print(Form("%s.pdf",cMap->GetName()));
  }
//=============================================================================

  if ((fNDeadPixels>0) && (fHistoDeadMap)) {
    cMap = new TCanvas(Form("c%s",fHistoDeadMap->GetName()), fHistoDeadMap->GetTitle(), 500, 500);

    fHistoDeadMap->Draw("COLZ");

    cMap->cd(); cMap->Modified();
    cMap->cd(); cMap->SetSelected(cMap);
    if (bPrint) cMap->Print(Form("%s.pdf",cMap->GetName()));
  }
//=============================================================================

  if ((fNInefficientPixels>0) && (fHistoInefficientMap)) {
    cMap = new TCanvas(Form("c%s",fHistoInefficientMap->GetName()), fHistoInefficientMap->GetTitle(), 500, 500);

    fHistoInefficientMap->Draw("COLZ");

    cMap->cd(); cMap->Modified();
    cMap->cd(); cMap->SetSelected(cMap);
    if (bPrint) cMap->Print(Form("%s.pdf",cMap->GetName()));
  }
//=============================================================================

  if ((fNoisyPixels>0) && (fHistoNoisyMap)) {
    cMap = new TCanvas(Form("c%s",fHistoNoisyMap->GetName()), fHistoNoisyMap->GetTitle(), 500, 500);

    fHistoNoisyMap->Draw("COLZ");

    cMap->cd(); cMap->Modified();
    cMap->cd(); cMap->SetSelected(cMap);
    if (bPrint) cMap->Print(Form("%s.pdf",cMap->GetName()));
  }
//=============================================================================

  return;
}

//_____________________________________________________________________________
void TpAHitmap::Map(const char *ALeaf)
{
//
//  TpAHitmap::Map
//

  TBranch *bPixel = fAnalysis->GetScanData()->GetBranch("Pixel");
  TBranch *bScan  = fAnalysis->GetScanData()->GetBranch(fConfig.SCANNAME);

  if ((!bPixel) || (!bScan)) {
    Error(__func__, "Problem reading branches!!");
    return;
  }

  TLeaf *leaf = bScan->GetLeaf(ALeaf);
  if (!leaf) {
    Error(__func__, "Problem reading leaf!!");
    return;
  }

  Info(__func__, "Maximum = %f, minimum = %f", (double)leaf->GetMaximum(), (double)leaf->GetMinimum());
//=============================================================================

  struct address_t {
    int col;
    int row;
  } address;

  bPixel->SetAddress(&address);
  float Value; leaf->SetAddress(&Value);
  TH2D  *hMap = new TH2D(Form("hMap_%s_%s",ALeaf,fConfig.SCANNAME), Form("%s %s",fConfig.SCANNAME,ALeaf), 1024, -.5, 1023.5, 512, -.5, 511.5);

  for (int i=0; i<bPixel->GetEntries(); i++) {
    bPixel->GetEntry(i);
    bScan ->GetEntry(i);
    leaf  ->GetValue(0);
    hMap  ->Fill(address.col, address.row, Value);
  }

  hMap->Draw("COLZ");
//=============================================================================

  return;
}
