void PlotChipsPixelAnalogue(const TString sDataset="dataset.txt")
{
  if (gSystem->Load("$PA_ROOT/lib/libpAAnalysis")<0) {
    ::Error("PlotChipsPixelAnalogue.C::PlotChipsPixelAnalogue", "Can not load lib $PA_ROOT/lib/libpAAnalysis");
    return;
  }
//=============================================================================

  if (gSystem->AccessPathName(sDataset.Data())) {
    ::Error("PlotChipsPixelAnalogue.C::PlotChipsPixelAnalogue", "Dataset %s does not exist!", sDataset.Data());
    return;
  }
//=============================================================================

  gROOT->LoadMacro("utils/TAliFigs.h");
  gROOT->LoadMacro("utils/TPlotWide.h");
  gROOT->LoadMacro("utils/TUtilpAana.h");
  gROOT->LoadMacro("utils/TUtilHisto.h"); SetStyle();
//=============================================================================

  UInt_t   ns = 0;
  UInt_t   nc = 0;
  TString  sn[1024];
  TString  sf[1024];
  Double_t ds[1024];
  Double_t dd[1024], di[1024], dn[1024];

  TString sID     = "";
  TString sFormat = "";

  TString  sDataFile;
  ifstream dataList(sDataset.Data(), ios::in);

  while (!dataList.eof()) {
    sDataFile.ReadLine(dataList,kFALSE);
    if (!sDataFile.EndsWith(".root")) continue;
    if (gSystem->AccessPathName(sDataFile.Data())) continue;
    TpAAnalysis *pa = new TpAAnalysis(sDataFile.Data()); if (!pa) continue;
    ::Info("PlotChipsPixelAnalogue.C::PlotChipsPixelAnalogue", "Chip ID%d: %s", nc, sDataFile.Data());
    sID = CheckChipIdentifier(sDataFile, sFormat);

    for (int i=0; i<=pa->GetNScans(); i++) {
      if (pa->GetScanType(i)!=TpAUtils::SCANTYPE_ANALOGUE) continue;
      TpAAnalogueScan *pScan = (TpAAnalogueScan*)pa->RetrieveScan(i); if (!pScan) continue;

      ds[ns] = ns;
      dd[ns] = pScan->GetNDeadPixels();
      di[ns] = pScan->GetNInefficientPixels();
      dn[ns] = pScan->GetNNoisyPixels();

      sn[ns] = Form("%s:%s",  sID.Data(),    pScan->GetIdentifier());
      sf[ns] = Form("%s : %s",sFormat.Data(),pScan->GetIdentifier());

      ns += 1;
      delete pScan; pScan = 0;
    }

    nc += 1;
    delete pa; pa = 0;
  } dataList.close();

  if (ns<=0) return;
  ::Info("PlotChipsPixelAnalogue.C::PlotChipsPixelAnalogue", "%d files have been analyzed", nc);
//=============================================================================

  TGraph *gd = new TGraph(ns, ds, dd);
  TGraph *gi = new TGraph(ns, ds, di);
  TGraph *gn = new TGraph(ns, ds, dn);
//=============================================================================

  TH1F    *hfm = 0;
  TCanvas *can = 0;
  TLegend *leg = 0;
  TLatex  *tex = 0;
  TLine  *line = 0;

  const Float_t dflx = -0.5, dfux = -0.5+ns;
  const Float_t dfly =  0.0, dfuy = 10.;

  const Float_t dlsx = 0.05, dlsy = 0.05;
  const Float_t dtsx = 0.06, dtsy = 0.06;
  const Float_t dtox = 1.10, dtoy = 0.05;

  const TString stnx = "";
  const TString stny = "";
//=============================================================================

  can = MakeCanvas("ChipsPixelAnalogueScanDead");
  can->SetRightMargin(0.12);
  can->SetBottomMargin(0.2);
  can->SetGridx();
  can->SetGridy();

  hfm = can->DrawFrame(dflx, 0.5*TMath::MinElement(ns,dd), dfux, 1.2*TMath::MaxElement(ns,dd));

  hfm->SetBins(ns, dflx, dfux);
  for (Int_t i=0, k=1; i<ns; i++, k++) hfm->GetXaxis()->SetBinLabel(k, sn[i]);

  SetupFrame(hfm, dlsx, dlsy, dtsx, dtsy, dtox, dtoy, stnx, stny);
  hfm->GetXaxis()->SetNdivisions(510);
  hfm->GetYaxis()->SetNdivisions(510);

  DrawGraph(gd, wcl[0], wmk[0], "P");

  tex = new TLatex();
  tex->SetNDC();
  tex->SetTextSizePixels(24);
  tex->DrawLatex(0.1, 0.92, "Analogue scan, # of dead pixels");
  CanvasEnd(can);
//=============================================================================

  can = MakeCanvas("ChipsPixelAnalogueScanInefficient");
  can->SetRightMargin(0.12);
  can->SetBottomMargin(0.2);
  can->SetGridx();
  can->SetGridy();

  hfm = can->DrawFrame(dflx, 0.5*TMath::MinElement(ns,di), dfux, 1.2*TMath::MaxElement(ns,di));

  hfm->SetBins(ns, dflx, dfux);
  for (Int_t i=0, k=1; i<ns; i++, k++) hfm->GetXaxis()->SetBinLabel(k, sn[i]);

  SetupFrame(hfm, dlsx, dlsy, dtsx, dtsy, dtox, dtoy, stnx, stny);
  hfm->GetXaxis()->SetNdivisions(510);
  hfm->GetYaxis()->SetNdivisions(510);

  DrawGraph(gi, wcl[0], wmk[0], "P");

  tex = new TLatex();
  tex->SetNDC();
  tex->SetTextSizePixels(24);
  tex->DrawLatex(0.1, 0.92, "Analogue scan, # of inefficient pixels");
  CanvasEnd(can);
//=============================================================================

  can = MakeCanvas("ChipsPixelAnalogueScanNoisy");
  can->SetRightMargin(0.12);
  can->SetBottomMargin(0.2);
  can->SetGridx();
  can->SetGridy();

  hfm = can->DrawFrame(dflx, 0.5*TMath::MinElement(ns,dn), dfux, 1.2*TMath::MaxElement(ns,dn));

  hfm->SetBins(ns, dflx, dfux);
  for (Int_t i=0, k=1; i<ns; i++, k++) hfm->GetXaxis()->SetBinLabel(k, sn[i]);

  SetupFrame(hfm, dlsx, dlsy, dtsx, dtsy, dtox, dtoy, stnx, stny);
  hfm->GetXaxis()->SetNdivisions(510);
  hfm->GetYaxis()->SetNdivisions(510);

  DrawGraph(gn, wcl[0], wmk[0], "P");

  tex = new TLatex();
  tex->SetNDC();
  tex->SetTextSizePixels(24);
  tex->DrawLatex(0.1, 0.92, "Analogue scan, # of noisy pixels");
  CanvasEnd(can);
//=============================================================================

  cout << endl;
  ::Info("PlotChipsPixelAnalogue.C::PlotChipsPixelAnalogue", "Markdown table");

  cout << Form("| %35s | Dead pixels | Inefficient pixels | Noisy pixels |", "Identifier") << endl;
  cout << "|------------------------------------:|" <<
                       "------------:|-------------------:|-------------:|" << endl;

  for (Int_t i=0; i<gd->GetN(); i++) cout << Form("| %35s | %11d | %18d | %12d |", sf[i].Data(),
                                                                                   (int)gd->GetY()[i],
                                                                                   (int)gi->GetY()[i],
                                                                                   (int)gn->GetY()[i]) << endl;


  ::Info("PlotChipsPixelAnalogue.C::PlotChipsPixelAnalogue", "End of markdonw table");
  cout << endl;
//=============================================================================

  ::Info("PlotChipsPixelAnalogue.C::PlotChipsPixelAnalogue", "DONE");

  return;
}
