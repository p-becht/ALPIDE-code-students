#include "TpASourceScan.h"

ClassImp(TpASourceScan)

//_____________________________________________________________________________
TpASourceScan::TpASourceScan(TpAAnalysis *AAnalysis,
                             const char  *dataFile,
                             bool         Qualification,
                             const char  *Comment,
                             int          version) :
TpAHitmap(SCANTYPE_SOURCE,AAnalysis,dataFile,Qualification,Comment,version)
{
//
//  TpASourceScan::TpASourceScan
//
}

//_____________________________________________________________________________
TpASourceScan::TpASourceScan(TpAAnalysis *AAnalysis, TConfig AConfig) :
TpAHitmap(AAnalysis,AConfig)
{
//
//  TpASourceScan::TpASourceScan
//

  Mapping(0., "nhits");
}

//_____________________________________________________________________________
TpASourceScan::TpASourceScan(const TpASourceScan &src) :
TpAHitmap(src)
{
//
// TpASourceScan::TpASourceScan
//
}

//_____________________________________________________________________________
TpASourceScan& TpASourceScan::operator=(const TpASourceScan &src)
{
//
// TpANoiseocc::operator=
//

  if (&src==this) return *this;

  TpAHitmap::operator=(src);

  return *this;
}

//_____________________________________________________________________________
TpASourceScan::~TpASourceScan()
{
//
//  TpASourceScan::~TpASourceScan
//
}

//_____________________________________________________________________________
void TpASourceScan::Summarise()
{
//
//  TpASourceScan::Summarise
//

  TString sName = GetScanName((TScanType)fConfig.SCANTYPE);
  Info(__func__, "%s: %s, ID: %s", fConfig.SCANNAME, sName.Data(), fConfig.IDENTSTRING);
  Info(__func__, "===========================================================");
//=============================================================================

  return;
}
