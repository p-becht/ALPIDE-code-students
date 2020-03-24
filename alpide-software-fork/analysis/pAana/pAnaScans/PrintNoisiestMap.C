void PrintNoisiestMap(const TString sDataset="dataset.txt")
{
  if (gSystem->Load("$PA_ROOT/lib/libpAAnalysis")<0) {
    ::Error("PrintNoisiestMap.C::PrintNoisiestMap", "Can not load lib $PA_ROOT/lib/libpAAnalysis !!");
    return;
  }
//=============================================================================

  if (gSystem->AccessPathName(sDataset.Data())) {
    ::Error("PrintNoisiestMap.C::PrintNoisiestMap", "Dataset %s does not exist !!", sDataset.Data());
    return;
  }
//=============================================================================

  gROOT->LoadMacro("utils/TUtilpAana.h");
//=============================================================================

  TString sID     = "";
  TString sFormat = "";

  TString  sDataFile;
  ifstream dataList(sDataset.Data(), ios::in);

  UInt_t n = 0;
  while (!dataList.eof()) {
    sDataFile.ReadLine(dataList,kFALSE);
    if (!sDataFile.EndsWith(".root")) continue;
    if (gSystem->AccessPathName(sDataFile.Data())) continue;
    TpAAnalysis *pa = new TpAAnalysis(sDataFile.Data()); if (!pa) continue;
    ::Info("PrintNoisiestMap.C::PrintNoisiestMap", "Chip ID%d: %s", n, sDataFile.Data());
    sID = CheckChipIdentifier(sDataFile, sFormat);

    for (int i=0; i<=pa->GetNScans(); i++) {
      if (pa->GetScanType(i)!=TpAUtils::SCANTYPE_NOISEOCC) continue;
      TpANoiseocc *pScan = (TpANoiseocc*)pa->RetrieveScan(i); if (!pScan) continue;

      ::Info("PrintNoisiestMap.C::PrintNoisiestMap", "%s : %s", sFormat.Data(), pScan->GetIdentifier());
      pScan->PrintNoisiestPixels(); cout << endl;
    }

    n += 1;
    delete pa; pa = 0;
  } dataList.close();
//=============================================================================

  ::Info("PrintNoisiestMap.C::PrintNoisiestMap", "%d chips are analyzed", n);
//=============================================================================

  ::Info("PrintNoisiestMap.C::PrintNoisiestMap", "DONE");
  return;
}
