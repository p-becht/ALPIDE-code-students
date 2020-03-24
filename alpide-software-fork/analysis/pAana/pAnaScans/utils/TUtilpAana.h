TString CheckChipIdentifier(const TString sFile)
{
  TString sID = "";
  if (!sFile.EndsWith(".root")) {
    cout << "File " << sFile.Data() << " is Not a root file or is an empty line" << endl;
    return sID;
  }

  if (gSystem->AccessPathName(sFile.Data())) {
    cout << "File " sFile.Data() << " does NOT exist" << endl;
    return sID;
  }
//=============================================================================

  TString sPath = gSystem->DirName(sFile.Data());
  TObjArray *aDir = sPath.Tokenize("/");

  TObjString *ps = 0;
  TString sVersion="", sWafer="", sChip="";

  TIter next(aDir);
  while (ps = (TObjString*)next()) {
    TString sDir = ps->String();
    if (sDir.Contains("pALPIDE-2"))  sVersion = "2 ";
    if (sDir.Contains("pALPIDE-3"))  sVersion = "3 ";
    if (sDir.Contains("pALPIDE-3b")) sVersion = "3b";

    if (sDir.Contains("Wafer")) { sWafer = sDir; sWafer.ReplaceAll("Wafer", ""); }
    if (sDir.Contains("Chip"))  { sChip  = sDir;  sChip.ReplaceAll("Chip",  "");  }
  }
//=============================================================================

  if (sVersion.IsNull() ||  sWafer.IsNull() || sChip.IsNull()) {
    cout << "Invalidated inputs"  << endl;
    return sID;
  }
//=============================================================================

  sID = sVersion + "-" + "W" + sWafer + "R" + sChip;
  return sID;
}

//_____________________________________________________________________________
TString CheckChipIdentifier(const TString sFile, TString &sFormat)
{
  sFormat = "";
//=============================================================================

  TString sID = "";
  if (!sFile.EndsWith(".root")) {
    cout << "File " << sFile.Data() << " is Not a root file or is an empty line" << endl;
    return sID;
  }

  if (gSystem->AccessPathName(sFile.Data())) {
    cout << "File " sFile.Data() << " does NOT exist" << endl;
    return sID;
  }
//=============================================================================

  TString sPath = gSystem->DirName(sFile.Data());
  TObjArray *aDir = sPath.Tokenize("/");

  TObjString *ps = 0;
  TString sVersion="", sWafer="", sChip="";

  TIter next(aDir);
  while (ps = (TObjString*)next()) {
    TString sDir = ps->String();
    if (sDir.Contains("pALPIDE-2"))  sVersion = "2 ";
    if (sDir.Contains("pALPIDE-3"))  sVersion = "3 ";
    if (sDir.Contains("pALPIDE-3b")) sVersion = "3b";

    if (sDir.Contains("Wafer")) { sWafer = sDir; sWafer.ReplaceAll("Wafer", ""); }
    if (sDir.Contains("Chip"))  { sChip  = sDir;  sChip.ReplaceAll("Chip",  "");  }
  }
//=============================================================================

  if (sVersion.IsNull() ||  sWafer.IsNull() || sChip.IsNull()) {
    cout << "Invalidated inputs"  << endl;
    return sID;
  }
//=============================================================================

  sID = sVersion + "-" + "W" + sWafer + "R" + sChip;
//=============================================================================

  sFormat = Form("%3s : W%3s : R%3s", sVersion.Data(), sWafer.Data(), sChip.Data());
//=============================================================================

  return sID;
}

//_____________________________________________________________________________
void CheckChipIdentifier(const TString sFile, TString &sVersion,
                                              TString &sWafer,
                                              TString &sChip)
{
  sVersion = "";
  sWafer   = "";
  sChip    = "";
//=============================================================================

  TString sID = "";
  if (!sFile.EndsWith(".root")) {
    cout << "File " << sFile.Data() << " is Not a root file or is an empty line" << endl;
    return sID;
  }

  if (gSystem->AccessPathName(sFile.Data())) {
    cout << "File " sFile.Data() << " does NOT exist" << endl;
    return sID;
  }
//=============================================================================

  TString sPath = gSystem->DirName(sFile.Data());
  TObjArray *aDir = sPath.Tokenize("/");

  TObjString *ps = 0;

  TIter next(aDir);
  while (ps = (TObjString*)next()) {
    TString sDir = ps->String();
    if (sDir.Contains("pALPIDE-"))  sVersion = sDir;
    if (sDir.Contains("Wafer"))     sWafer   = sDir;
    if (sDir.Contains("Chip"))      sChip    = sDir;
  }
//=============================================================================

  return;
}
