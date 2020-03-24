#ifndef TpADigitalScan_H
#define TpADigitalScan_H

#include "TpAHitmap.h"
//=============================================================================

class TpADigitalScan : public TpAHitmap {

 public :

  TpADigitalScan(TpAAnalysis *AAnalysis,
                 const char  *dataFile,
                 bool         Qualification,
                 const char  *Comment,
                 int          version);

  TpADigitalScan(TpAAnalysis *AAnalysis, TConfig AConfig);

  TpADigitalScan(const TpADigitalScan &src);
  TpADigitalScan& operator=(const TpADigitalScan &src);
  virtual ~TpADigitalScan();
//=============================================================================

 protected :
//=============================================================================

  ClassDef(TpADigitalScan, 2)
};

#endif
