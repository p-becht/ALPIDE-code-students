void PlotChipsMeanIDDA(const TString sDataset="dataset.txt")
{
  if (gSystem->Load("$PA_ROOT/lib/libpAAnalysis")<0) {
    ::Error("PlotChipsMeanIDDA.C::PlotChipsMeanIDDA", "Can not load lib $PA_ROOT/lib/libpAAnalysis");
    return;
  }
//=============================================================================

  if (gSystem->AccessPathName(sDataset.Data())) {
    ::Error("PlotChipsMeanIDDA.C::PlotChipsMeanIDDA", "Dataset %s does not exist!", sDataset.Data());
    return;
  }
//=============================================================================

  gROOT->LoadMacro("utils/TAliFigs.h");
  gROOT->LoadMacro("utils/TPlotWide.h");
  gROOT->LoadMacro("utils/TUtilpAana.h");
  gROOT->LoadMacro("utils/TUtilHisto.h"); SetStyle();
//=============================================================================

  UInt_t   n = 0;
  TString sID[1024];
  Double_t ds[1024], dsErr[1024];
  Double_t db[1024], dbErr[1024];
  Double_t da[1024], daErr[1024];
  Double_t dc[1024], dcErr[1024];

  TString  sDataFile;
  ifstream dataList(sDataset.Data(), ios::in);

  while (!dataList.eof()) {
    sDataFile.ReadLine(dataList,kFALSE);
    if (!sDataFile.EndsWith(".root")) continue;
    if (gSystem->AccessPathName(sDataFile.Data())) continue;
    TpAAnalysis *pa = new TpAAnalysis(sDataFile.Data()); if (!pa) continue;
    ::Info("PlotChipsMeanIDDA.C::PlotChipsMeanIDDA", "Chip ID%d: %s", n, sDataFile.Data());

    ds[n] = n; dsErr[n] = 0.;
    pa->GetMeanIDDA(db[n], dbErr[n], da[n], daErr[n], dc[n], dcErr[n]);
    sID[n] = CheckChipIdentifier(sDataFile); n += 1;

    delete pa; pa = 0;
  } dataList.close();

  if (n<=0) return;
  ::Info("PlotChipsMeanIDDA.C::PlotChipsMeanIDDA", "%d files have been analyzed", n);
//=============================================================================

  TGraph       *gc = new TGraph(n, ds, dc);
  TGraphErrors *gb = new TGraphErrors(n, ds, db, dsErr, dbErr);
  TGraphErrors *ga = new TGraphErrors(n, ds, da, dsErr, daErr);
//=============================================================================

  TH1F    *hfm = 0;
  TCanvas *can = 0;
  TLegend *leg = 0;
  TLatex  *tex = 0;
  TLine  *line = 0;

  const Float_t dflx = -0.5, dfux = -0.5+n;
  const Float_t dfly =  0.0, dfuy = 10.;

  const Float_t dlsx = 0.05, dlsy = 0.05;
  const Float_t dtsx = 0.06, dtsy = 0.06;
  const Float_t dtox = 1.10, dtoy = 0.50;

  const TString stnx = "";
  const TString stny = "Mean IDDA (mA)";
//=============================================================================

  Double_t dMin[] = { TMath::MinElement(n,db), TMath::MinElement(n,da) };
  Double_t dMax[] = { TMath::MaxElement(n,db), TMath::MaxElement(n,da) };

//Double_t dMin[] = { TMath::MinElement(n,db), TMath::MinElement(n,da), TMath::MinElement(n,dc) };
//Double_t dMax[] = { TMath::MaxElement(n,db), TMath::MaxElement(n,da), TMath::MaxElement(n,dc) };

  can = MakeCanvas("ChipsMeanIDDA"); can->SetGridx(); can->SetGridy();
  hfm = can->DrawFrame(dflx, 0.5*TMath::MinElement(2,dMin), dfux, 1.2*TMath::MaxElement(2,dMax));
//hfm = can->DrawFrame(dflx, 0.5*TMath::MinElement(3,dMin), dfux, 1.2*TMath::MaxElement(3,dMax));

  hfm->SetBins(n, dflx, dfux);
  for (Int_t i=0, k=1; i<n; i++, k++) hfm->GetXaxis()->SetBinLabel(k, sID[i]);

  SetupFrame(hfm, dlsx, dlsy, dtsx, dtsy, dtox, dtoy, stnx, stny);
  hfm->GetXaxis()->SetNdivisions(510);
  hfm->GetYaxis()->SetNdivisions(510);

//DrawGraph(gc, wcl[0], wmk[0], "P");
  DrawGraph(gb, wcl[1], wmk[0], "P");
  DrawGraph(ga, wcl[3], wmk[0], "P");

  leg = new TLegend(0.72, 0.18, 0.98, 0.34); SetupLegend(leg);
//leg->AddEntry(gc, "Combined", "LP")->SetTextSizePixels(24);
  leg->AddEntry(gb, "IDDA", "LP")->SetTextSizePixels(24);
  leg->AddEntry(ga, "IDDA w/ power on", "LP")->SetTextSizePixels(24);
  leg->Draw();

  CanvasEnd(can);
//=============================================================================

  ::Info("PlotChipsMeanIDDA.C::PlotChipsMeanIDDA", "DONE");
  return;
}
