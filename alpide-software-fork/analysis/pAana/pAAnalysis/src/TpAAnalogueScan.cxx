#include "TpAAnalogueScan.h"

ClassImp(TpAAnalogueScan)

//_____________________________________________________________________________
TpAAnalogueScan::TpAAnalogueScan(TpAAnalysis *AAnalysis,
                                 const char  *dataFile,
                                 bool         Qualification,
                                 const char  *Comment,
                                 int          version) :
TpAHitmap(SCANTYPE_ANALOGUE,AAnalysis,dataFile,Qualification,Comment,version)
{
//
//  TpAAnalogueScan::TpAAnalogueScan
//
}

//_____________________________________________________________________________
TpAAnalogueScan::TpAAnalogueScan(TpAAnalysis *AAnalysis, TConfig AConfig) :
TpAHitmap(AAnalysis,AConfig)
{
//
//  TpAAnalogueScan::TpAAnalogueScan
//

  Mapping((float)NHITDIGITAL, "nhits");
}

//_____________________________________________________________________________
TpAAnalogueScan::TpAAnalogueScan(const TpAAnalogueScan &src) :
TpAHitmap(src)
{
//
// TpAAnalogueScan::TpAAnalogueScan
//
}

//_____________________________________________________________________________
TpAAnalogueScan& TpAAnalogueScan::operator=(const TpAAnalogueScan &src)
{
//
// TpAAnalogueScan::operator=
//

  if (&src==this) return *this;

  TpAHitmap::operator=(src);

  return *this;
}

//_____________________________________________________________________________
TpAAnalogueScan::~TpAAnalogueScan()
{
//
//  TpAAnalogueScan::~TpAAnalogueScan
//
}
