void PrintAnalysis(const TString sDataset="dataset.txt", const TString sPrefix="Results")
{
  if (gSystem->Load("$PA_ROOT/lib/libpAAnalysis")<0) {
    ::Error("PrintAnalysis.C::PrintAnalysis", "Can not load lib $PA_ROOT/lib/libpAAnalysis");
    return;
  }
//=============================================================================

  if (gSystem->AccessPathName(sDataset.Data())) {
    ::Error("PrintAnalysis.C::PrintAnalysis", "Dataset %s does not exist!", sDataset.Data());
    return;
  }
//=============================================================================

  gROOT->LoadMacro("utils/TUtilpAana.h");
//=============================================================================

  const Int_t   nScan   = 6;
  const TString sScan[] = { "Digital", "Analogue", "Threshold", "Source", "NoiseOCC", "DACscan" };

  TString sVersion = "";
  TString sWafer   = "";
  TString sChip    = "";

  TString  sDataFile;
  ifstream dataList(sDataset.Data(), ios::in);

  while (!dataList.eof()) {
    sDataFile.ReadLine(dataList,kFALSE);
    if (!sDataFile.EndsWith(".root")) continue;
    if (gSystem->AccessPathName(sDataFile.Data())) continue;

    TpAAnalysis *pa = new TpAAnalysis(sDataFile.Data()); if (!pa) continue;
    ::Info("PlotAnalysis.C::PlotAnalysis", "Chip ID: %s", sDataFile.Data());

    int kDAC = 0;
    pa->SetPrint();
    for (int i=0; i<=pa->GetNScans(); i++) {
      int kType = pa->GetScanType(i);
      TpAScan *pScan = pa->RetrieveScan(i); if (!pScan) continue;
      if (kType==5) { ((TpADACScan*)pScan)->SetIdentifier(Form("DACSCAN%d",kDAC)); kDAC += 1; }

      pScan->Plot();
      pScan->Summarise();
    } delete pa; pa = 0;
//=============================================================================

    CheckChipIdentifier(sDataFile, sVersion, sWafer, sChip);
    TString sOutPath = Form("%s/%s/%s/%s",sPrefix.Data(),sVersion.Data(),sWafer.Data(),sChip.Data());

    for (Int_t i=0; i<nScan; i++) {
      gSystem->Exec(Form("mkdir -p %s/%s", sOutPath.Data(), sScan[i].Data()));
      gSystem->Exec(Form("mv ch*_%s_*.pdf %s/%s", sScan[i].Data(), sOutPath.Data(), sScan[i].Data()));
    }
  } dataList.close();
//=============================================================================

  ::Info("PrintAnalysis.C::PrintAnalysis", "DONE");
//=============================================================================

  return;
}
