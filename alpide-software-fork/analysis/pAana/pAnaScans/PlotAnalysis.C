void PlotAnalysis(const int kMode=2)
{
  if (kMode>5) {
    ::Error("PlotAnalysis.C::PlotAnalysis", "kMode = %d (>5)! Please correct", kMode);
    ::Info("PlotAnalysis.C::PlotAnalysis", "kMode < 0: all scans");
    ::Info("PlotAnalysis.C::PlotAnalysis", "      = 0: Digital    scan");
    ::Info("PlotAnalysis.C::PlotAnalysis", "      = 1: Analogue   scan");
    ::Info("PlotAnalysis.C::PlotAnalysis", "      = 2: Threshold  scan");
    ::Info("PlotAnalysis.C::PlotAnalysis", "      = 3: Source     scan");
    ::Info("PlotAnalysis.C::PlotAnalysis", "      = 4: Noise occ. scan");
    ::Info("PlotAnalysis.C::PlotAnalysis", "      = 5: DAC        scan");
    return;
  }
//=============================================================================

  if (gSystem->Load("$PA_ROOT/lib/libpAAnalysis")<0) return;
//=============================================================================

  const TString sFile = "/data1/xzhang/ITS-WP5-data/VTrubnikov/pALPIDE-3b/Wafer11/Chip3/AnalysisResults_Wafer11_Chip3.root";
//=============================================================================

  TpAAnalysis *pa = new TpAAnalysis(sFile.Data()); if (!pa) continue;
  pa->ListAllScans(); //return;
//=============================================================================

  int kDAC = 0;
  for (int i=0; i<=pa->GetNScans(); i++) {
    int kType = pa->GetScanType(i);
    if ((kMode>=0) && (kType!=kMode)) continue;
    TpAScan *pScan = pa->RetrieveScan(i); if (!pScan) continue;
    if (kType==5) { ((TpADACScan*)pScan)->SetIdentifier(Form("DACSCAN%d",kDAC)); kDAC += 1; }

    pScan->Plot();
    pScan->Summarise();
    if (kType==4) ((TpANoiseocc*)pScan)->PrintNoisiestPixels();
  }
//=============================================================================

  ::Info("PlotAnalysis.C::PlotAnalysis", "DONE");
//=============================================================================

  return;
}
