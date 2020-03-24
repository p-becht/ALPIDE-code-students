#include <stdio.h>
#include <fstream>

#include "TMath.h"
#include "TF1.h"
#include "TGraph.h"
#include "TString.h"
#include "TObjString.h"
#include "TObjArray.h"
//=============================================================================

using namespace std;

bool bINFO = false;

const int nInj = 50;
const int kELECTRONS_PER_DAC = 7;
const TString sPref = "FitThresholds";

void Process(const TString, const TString);

bool   ProcessPixel (int, int, const int, const int *, const int *, double &, double &, double &);
double EstiThreshold(int, int, const int, const int *, const int *);

Double_t Erf(Double_t *, Double_t *);
TString OutputFileName(const TString);
//=============================================================================

//_____________________________________________________________________________
int main(int argc, char* argv[])
{
  const TString sFxn = Form("%s::main",sPref.Data());
  for (int i=0; i<argc; i++) ::Info(sFxn.Data(), "arg[%d] = %s", i, argv[i]);
//=============================================================================

  if (argc<2) {
    Fatal(sFxn.Data(), "argc = %d (< 2)!!!", argc);
    return -1;
  }
//=============================================================================

  const TString sPathIn = argv[1];
  if (sPathIn.IsNull()) {
    Fatal(sFxn.Data(), "Input file pass is NULL!!!");
    return -1;
  }
//=============================================================================

  if (argc>=3) {
    const TString sPrint = argv[2];
    if (sPrint=="INFO")  bINFO = true;
  }

  ::Info(sFxn.Data(), "Print info = %s", (bINFO ? "NO" : "OFF"));
//=============================================================================

  const TString sFileOut = OutputFileName(sPathIn);
  ::Info(sFxn.Data(), "Input  file = %s", sPathIn.Data());
  ::Info(sFxn.Data(), "Output file = %s", sFileOut.Data());
//=============================================================================

  Process(sPathIn, sFileOut);
//=============================================================================

  return 0;
}

//_____________________________________________________________________________
void Process(const TString sFileIn, const TString sFileOut)
{
  const TString sFxn = Form("%s::ProcessFile",sPref.Data());
//=============================================================================

  FILE *fileIn = fopen(sFileIn.Data(), "r");

  int kPixel = 0;
  int kLastCol = -1, kLastAddr = -1, kN = 0;
  int kCol = 0, kAddr = 0, kAmpl = 0, kHists = 0;

  int  aAmpl[1024]; memset(aAmpl,  0, sizeof(aAmpl));
  int aHists[1024]; memset(aHists, 0, sizeof(aHists));

  FILE *fileOut = fopen(sFileOut.Data(), "w");
  while ((fscanf(fileIn, "%d %d %d %d", &kCol, &kAddr, &kAmpl, &kHists)==4)) {
    if (((kLastCol!=kCol) || (kLastAddr!=kAddr)) && (kN!=0)) {

      double dThre = -1., dNoise = -1., dChiq = -1.;
      if (ProcessPixel(kLastCol,kLastAddr,kN,aAmpl,aHists,dThre,dNoise,dChiq)) {
        fprintf(fileOut, "%d %d %f %f %f\n", kLastCol, kLastAddr, dThre, dNoise, dChiq);

        if (bINFO) if ((kPixel%1000)==0)
          ::Info(sFxn.Data(), "Col = %d, Address = %d, step = %d, threthold = %f, noise = %f, #chi^{2} = %f",
                               kLastCol,    kLastAddr,        kN,          dThre,     dNoise,         dChiq);
      }

/*    double dTheRow = AddressToRow(kLastAddr);
      double dTheCol = AddressToColumn(kLastCol, kLastAddr);

      if (h2Thre)  h2Thre ->Fill(dTheCol, dTheRow, dThre);
      if (h2Noise) h2Noise->Fill(dTheCol, dTheRow, dNoise);
      if (h2Chi2)  h2Chi2 ->Fill(dTheCol, dTheRow, dChiq);*/

      kN = 0;
      kPixel += 1;
      memset(aAmpl,  0, sizeof(aAmpl));
      memset(aHists, 0, sizeof(aHists));
    }

    kLastCol  = kCol;
    kLastAddr = kAddr;

    aAmpl [kN] = kELECTRONS_PER_DAC * kAmpl;
    aHists[kN] = kHists;
    kN += 1;
  }

  fclose(fileOut);
  fclose(fileIn);
//=============================================================================

  return;
}

//_____________________________________________________________________________
bool ProcessPixel(int kCol, int kAddr, const int kN, const int *aA, const int *aH, double &dThre, double &dNoise, double &dChiq)
{
  const TString sFxn = Form("%s::ProcessPixel",sPref.Data());
//=============================================================================

  double dEstiThre = EstiThreshold(kCol, kAddr, kN, aA, aH);
  if (dEstiThre<0.) return false;
//=============================================================================

  TF1 *fErr = new TF1("fErr", Erf, 0., 1500., 2);
  fErr->SetParameter(0, dEstiThre);
  fErr->SetParameter(1, 8.);

  TGraph *gErr = new TGraph(kN, aA, aH);
  gErr->Fit("fErr", "Q");

  dThre  = fErr->GetParameter(0);
  dNoise = fErr->GetParameter(1);
  dChiq  = fErr->GetChisquare() / fErr->GetNDF();

  delete gErr; gErr = 0;
  delete fErr; fErr = 0;
//=============================================================================

  if (dThre<0.)  ::Error(sFxn.Data(), "col = %d, address = %d, fitting failed, threshold = %f (<0)!!",kCol,kAddr,dThre);
  if (dNoise<0.) ::Error(sFxn.Data(), "col = %d, address = %d, fitting failed, noize     = %f (<0)!!",kCol,kAddr,dNoise);
  if (dChiq<0.)  ::Error(sFxn.Data(), "col = %d, address = %d, fitting failed, #chi^{2}  = %f (<0)!!",kCol,kAddr,dChiq);
  if ((dThre<0.) || (dNoise<0.) || (dChiq<0.)) return false;
//=============================================================================

  return true;
}

//_____________________________________________________________________________
double EstiThreshold(int kCol, int kAddr, const int kN, const int *aA, const int *aH)
{
  const TString sFxn = Form("%s::EstiThreshold",sPref.Data());
//=============================================================================

  if (kN<=0) {
    ::Warning(sFxn.Data(), "col = %d, address = %d, step = %d (<=0)!", kCol, kAddr, kN);
    return -1.;
  }
//=============================================================================

  double dUpper = -1.;
  for (int i=0; i<kN; i++) if (aH[i]==nInj) { dUpper = (double)aA[i]; break; }

  if (dUpper<0.) {
    ::Warning(sFxn.Data(), "col = %d, address = %d, step = %d, max # of hits = %d (<%d)!", kCol, kAddr, kN, TMath::MaxElement(kN,aH),nInj);
    return -1.;
  }
//=============================================================================

  double dLower = -1.;
  for (int i=kN-1; i>0; i--) if(aH[i]==0) { dLower = (double)aA[i]; break; }

  if (dLower<0.) {
    ::Warning(sFxn.Data(), "col = %d, address = %d, step = %d, min # of hits = %d (>0)!", kCol, kAddr, kN, TMath::MinElement(kN,aH));
    return -1.;
  }

  if (dUpper<dLower) {
    ::Warning(sFxn.Data(), "col = %d, address = %d, lower bound (= %f) > upper bond (= %f)!", kCol, kAddr, dLower/kELECTRONS_PER_DAC,
                                                                                                           dUpper/kELECTRONS_PER_DAC);
    return -1.;
  }
//=============================================================================

  return ((dUpper+dLower) / 2.);
}

//_____________________________________________________________________________
TString OutputFileName(const TString sPathIn)
{
  const TString sFxn = Form("%s::OutputFileName",sPref.Data());
//=============================================================================

  TObjArray *aPathIn = sPathIn.Tokenize("/");
  for (int i=0; i<aPathIn->GetEntriesFast(); i++) {
    TObjString *os = (TObjString*)aPathIn->At(i);
    if (bINFO) ::Info(sFxn.Data(), "path[%d] = %s", i, os->String().Data());
  }

  const TString sFileIn = ((TObjString*)aPathIn->Last())->String();
  if (sFileIn.IsNull()) {
    ::Fatal(sFxn.Data(), "Input file name is NULL!!!");
    exit(-1);
  }
//=============================================================================

  TString sFileOut;
  TObjArray *aFileIn = sFileIn.Tokenize("_");
  for (int i=0; i<aFileIn->GetEntriesFast(); i++) {
    TObjString *os = (TObjString*)aFileIn->At(i);
    if (bINFO) ::Info(sFxn.Data(), "file[%d] = %s", i, os->String().Data());

    if (i==0)
      sFileOut = Form("%sFit",os->String().Data());
    else
      sFileOut += Form("_%s", os->String().Data());
  }

  if (sFileOut.IsNull()) {
    ::Fatal(sFxn.Data(), "Output file name is NULL!!!");
    exit(-1);
  }
//=============================================================================

  return sFileOut;
}

//_____________________________________________________________________________
Double_t Erf(Double_t *x, Double_t *par)
{
  Double_t dNorm = nInj / 2.;
  Double_t dErrf = TMath::Erf((x[0]-par[0]) / (TMath::Sqrt(2.)*par[1]));

  return (dNorm*dErrf + dNorm);
}
