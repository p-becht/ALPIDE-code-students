#include "TpADigitalScan.h"

ClassImp(TpADigitalScan)

//_____________________________________________________________________________
TpADigitalScan::TpADigitalScan(TpAAnalysis *AAnalysis,
                               const char  *dataFile,
                               bool         Qualification,
                               const char  *Comment,
                               int          version) :
TpAHitmap(SCANTYPE_DIGITAL,AAnalysis,dataFile,Qualification,Comment,version)
{
//
//  TpADigitalScan::TpADigitalScan
//
}

//_____________________________________________________________________________
TpADigitalScan::TpADigitalScan(TpAAnalysis *AAnalysis, TConfig AConfig) :
TpAHitmap(AAnalysis,AConfig)
{
//
//  TpADigitalScan::TpADigitalScan
//

  Mapping((float)NHITDIGITAL, "nhits");
}

//_____________________________________________________________________________
TpADigitalScan::TpADigitalScan(const TpADigitalScan &src) :
TpAHitmap(src)
{
//
// TpADigitalScan::TpADigitalScan
//
}

//_____________________________________________________________________________
TpADigitalScan& TpADigitalScan::operator=(const TpADigitalScan &src)
{
//
// TpADigitalScan::operator=
//

  if (&src==this) return *this;

  TpAHitmap::operator=(src);

  return *this;
}

//_____________________________________________________________________________
TpADigitalScan::~TpADigitalScan()
{
//
//  TpADigitalScan::~TpADigitalScan
//
}
