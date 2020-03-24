void PrintScanInfo(const Int_t kMode=0, const TString sDataset="dataset.txt")
{
  if (gSystem->Load("$PA_ROOT/lib/libpAAnalysis")<0) {
    ::Error("PrintScanInfo.C::PrintScanInfo", "Can not load lib $PA_ROOT/lib/libpAAnalysis !!");
    return;
  }
//=============================================================================

  if (gSystem->AccessPathName(sDataset.Data())) {
    ::Error("PrintScanInfo.C::PrintScanInfo", "Dataset %s does not exist !!", sDataset.Data());
    return;
  }
//=============================================================================

  if ((kMode<0) || (kMode>3)) {
    ::Error("PrintScanInfo.C::PrintScanInfo", "Print mode (= %d) is out of range [0, 3] !!", kMode);

    ::Info("PrintScanInfo.C::PrintScanInfo", " Print mode  = 0: All");
    ::Info("PrintScanInfo.C::PrintScanInfo", "             = 1: List scans");
    ::Info("PrintScanInfo.C::PrintScanInfo", "             = 2: Scan cfgs");
    ::Info("PrintScanInfo.C::PrintScanInfo", "             = 3: Scan summaries");
    return;
  }
//=============================================================================

  TString  sDataFile;
  ifstream dataList(sDataset.Data(), ios::in);

  UInt_t n = 0;
  while (!dataList.eof()) {
    sDataFile.ReadLine(dataList,kFALSE);
    if (!sDataFile.EndsWith(".root")) continue;
    if (gSystem->AccessPathName(sDataFile.Data())) continue;
    TpAAnalysis *pa = new TpAAnalysis(sDataFile.Data()); if (!pa) continue;
    ::Info("PrintScanInfo.C::PrintScanInfo", "Chip ID%d: %s", n, sDataFile.Data());

    if ((kMode==0) || (kMode==3)) pa->Summarise();
    if ((kMode==0) || (kMode==1)) pa->ListAllScans();
    if ((kMode==0) || (kMode==2)) pa->DumpAllConfigurations();

    n += 1;
    delete pa; pa = 0;
  } dataList.close();
//=============================================================================

  ::Info("PrintScanInfo.C::PrintScanInfo", "%d chips are analyzed", n);

  if (kMode==0) ::Info("PrintScanInfo.C::PrintScanInfo", "Print mode = 0: All");
  if (kMode==1) ::Info("PrintScanInfo.C::PrintScanInfo", "Print mode = 1: List scans");
  if (kMode==2) ::Info("PrintScanInfo.C::PrintScanInfo", "Print mode = 2: Scan cfgs");
  if (kMode==3) ::Info("PrintScanInfo.C::PrintScanInfo", "Print mode = 3: Scan summaries");
//=============================================================================

  ::Info("PrintScanInfo.C::PrintScanInfo", "DONE");
  return;
}
