TString sDigitalId  = "";
TString sAnalogueId = "";
TString sThreId     = "";
TString sSourceId   = "";
TString sNoiseoccId = "";
//=============================================================================

// By default the prefix of the file for the threshold fit is "ThresholdScanFit"
// One has to change it to the preference
//nst TString sPrefThreFit = "ThresholdScanFit";
const TString sPrefThreFit = "FitValues";
//=============================================================================

void ReadAnalysis(const TString sPath, const TString sFile="AnalysisResults.root")
{
  if (gSystem->Load("$PA_ROOT/lib/libpAAnalysis")<0) return;
//=============================================================================

  CheckScanID(sPath);
//=============================================================================

  TpAAnalysis *pa = new TpAAnalysis(sFile.Data());
//=============================================================================

  if (!sPath.IsNull()) {
    TpADACScan *pDAC = (TpADACScan*)pa->AddDACScan(sPath.Data());
  }
//=============================================================================

  if (!sDigitalId.IsNull()) {
    const TString sDigital = Form("%s/DigitalScan_%s.dat", sPath.Data(), sDigitalId.Data());
    TpADigitalScan *pDigital = (TpADigitalScan*)pa->AddDigitalScan(sDigital.Data());
  }
//=============================================================================

  if (!sAnalogueId.IsNull()) {
    const TString sAnalogue = Form("%s/AnalogueScan_%s.dat", sPath.Data(), sAnalogueId.Data());
    TpAAnalogueScan *pAnalogue = (TpAAnalogueScan*)pa->AddAnalogueScan(sAnalogue.Data());
  }
//=============================================================================

  if (!sThreId.IsNull()) {
    const TString sThreFit = Form("%s/%s_%s.dat", sPath.Data(), sPrefThreFit.Data(), sThreId.Data());
    TpAThresholdScan *pThre = (TpAThresholdScan*)pa->AddThresholdScan(sThreFit.Data());
  }
//=============================================================================

  if (!sSourceId.IsNull()) {
    const TString sSource = Form("%s/SourceScan_%s.dat", sPath.Data(), sSourceId.Data());
    TpASourceScan *pSource = (TpASourceScan*)pa->AddSourceScan(sSource.Data());
  }
//=============================================================================

  if (!sNoiseoccId.IsNull()) {
    const TString sNoiseocc = Form("%s/NoiseOccupancy_%s.dat", sPath.Data(), sNoiseoccId.Data());
    TpANoiseocc *pNoiseocc = (TpANoiseocc*)pa->AddNoiseoccScan(sNoiseocc.Data());
  }
//=============================================================================

//pa->DumpAllConfigurations();
//pa->ListAllScans();
//pa->Summarise();
//=============================================================================

  return;
}

//_____________________________________________________________________________
void CheckScanID(const TString sPath)
{
  TString sFile = "Temp" + sPath + ".log";
  sFile.ReplaceAll("/", "_");
  gSystem->Exec(Form("ls %s/*.dat > %s 2>&1",sPath.Data(),sFile.Data()));
//=============================================================================

  TString sData;
  ifstream dataList(sFile.Data(), ios::in);
  while (!dataList.eof()) {
    sData.ReadLine(dataList,kFALSE);
    if (!sData.EndsWith(".dat")) continue;
    if (gSystem->AccessPathName(sData.Data())) continue;

    sData.ReplaceAll(".dat", "");
    sData.ReplaceAll(sPath,  "");

    if (sData.Contains("/DigitalScan_")) {
      sData.ReplaceAll("/DigitalScan_", "");
      sDigitalId = sData;

      ::Info("ReadAnalysis::CheckScanID", "Id of Digital    scan = %s", sDigitalId.Data());
    }

    if (sData.Contains("/AnalogueScan_")) {
      sData.ReplaceAll("/AnalogueScan_", "");
      sAnalogueId = sData;

      ::Info("ReadAnalysis::CheckScanID", "Id of Analogue   scan = %s", sAnalogueId.Data());
    }

    if (sData.Contains("/ThresholdScan_")) {
      sData.ReplaceAll("/ThresholdScan_", "");
      sThreId = sData;

      ::Info("ReadAnalysis::CheckScanID", "Id of Threshold  scan = %s", sThreId.Data());
    }

    if (sData.Contains("/SourceScan_")) {
      sData.ReplaceAll("/SourceScan_", "");
      sSourceId = sData;

      ::Info("ReadAnalysis::CheckScanID", "Id of Source     scan = %s", sSourceId.Data());
    }

    if (sData.Contains("/NoiseOccupancy_")) {
      sData.ReplaceAll("/NoiseOccupancy_", "");
      sNoiseoccId = sData;

      ::Info("ReadAnalysis::CheckScanID", "Id of Noise occ. scan = %s", sNoiseoccId.Data());
    }
  } dataList.close();
//=============================================================================

  gSystem->Exec(Form("rm -f %s",sFile.Data()));
//=============================================================================

  return;
}
