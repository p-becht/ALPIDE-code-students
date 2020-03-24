#include "TSystem.h"
#include "TObjArray.h"
#include "TObjString.h"

#include "TpAUtils.h"

//#if !defined(R__ALPHA) && !defined(R__SOLARIS) && !defined(R__ACC) && !defined(R__FBSD)
//NamespaceImp(TpAUtils)
//#endif

//_____________________________________________________________________________
void TpAUtils::EntryToColRow(int AEntry, int &ACol, int &ARow)
{
  ACol = AEntry / NROWS;
  ARow = AEntry % NROWS;
  return;
}

//_____________________________________________________________________________
int TpAUtils::AddressToColumn(int ARegion, int ADoubleCol, int AAddress)
{
  return AddressToColumn((16*ARegion) + ADoubleCol, AAddress);
}

//_____________________________________________________________________________
int TpAUtils::AddressToColumn(int ADoubleCol, int AAddress)
{
  int LeftRight = (((AAddress%4)<2) ? 1 : 0);  // Left or right column within the double column
  int Column = (2*ADoubleCol) + LeftRight;     // Double columns before ADoubleCol

  return Column;
}

//_____________________________________________________________________________
int TpAUtils::AddressToRow(int AAddress)
{
  int Row = AAddress / 2;         // This is OK for the top-right and the bottom-left pixel within a group of 4
  if ((AAddress%4)==3) Row -= 1;  // adjust the top-left pixel
  if ((AAddress%4)==0) Row += 1;  // adjust the bottom-right pixel

  return Row;
}

//_____________________________________________________________________________
void TpAUtils::DcolAddr2ColRow(int dcol, int addr, int &col, int &row)
{
  int kBlock = addr / 4;
  int kRestd = addr % 4;

  int kc = 0, kr = 0;
  if (kRestd==0) { kc = 0; kr = 0; }
  if (kRestd==1) { kc = 1; kr = 0; }
  if (kRestd==2) { kc = 1; kr = 1; }
  if (kRestd==3) { kc = 0; kr = 1; }

  col = 2*dcol   + kc;
  row = 2*kBlock + kr;

  return;
}

//_____________________________________________________________________________
TString TpAUtils::GetScanName(TScanType aType)
{
  TString sName = "Unidentified";
  if (aType==SCANTYPE_DIGITAL)   sName = "Digital";
  if (aType==SCANTYPE_ANALOGUE)  sName = "Analogue";
  if (aType==SCANTYPE_THRESHOLD) sName = "Threshold";
  if (aType==SCANTYPE_SOURCE)    sName = "Source";
  if (aType==SCANTYPE_NOISEOCC)  sName = "NoiseOCC";
  if (aType==SCANTYPE_DACSCAN)   sName = "DACscan";

  return sName;
}

//_____________________________________________________________________________
const char* TpAUtils::CheckChipID(const TString sFile)
{
//
//  TpAUtils::CheckChipID
//

  new TSystem();
  TString sPath = gSystem->DirName(sFile.Data());

  TObjArray *aPath = sPath.Tokenize("/");

  TIter next(aPath);
  TObjString *pPath = 0;
  TString sWafer="", sChip="";
  while ((pPath = (TObjString*)next())) {
    TString sPath = pPath->GetString();

    if (sPath.Contains("Wafer")) sWafer = sPath;
    if (sPath.Contains("Chip"))  sChip  = sPath;
  }

  if (sWafer.IsNull() || sChip.IsNull()) return "";
//=============================================================================

  sWafer.ReplaceAll("afer", "");
  sChip.ReplaceAll("Chip", "R");

  return Form("%s_%s", sWafer.Data(), sChip.Data());
}
