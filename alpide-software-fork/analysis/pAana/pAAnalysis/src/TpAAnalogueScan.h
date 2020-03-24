#ifndef TPAANALOGUESCAN_H
#define TPAANALOGUESCAN_H

#include "TpAHitmap.h"
//=============================================================================

class TpAAnalogueScan : public TpAHitmap {

 public :

  TpAAnalogueScan(TpAAnalysis *AAnalysis,
                  const char  *dataFile,
                  bool         Qualification,
                  const char  *Comment,
                  int          version);

  TpAAnalogueScan(TpAAnalysis *AAnalysis, TConfig AConfig);

  TpAAnalogueScan(const TpAAnalogueScan &src);
  TpAAnalogueScan& operator=(const TpAAnalogueScan &src);
  virtual ~TpAAnalogueScan();
//=============================================================================

 protected :
//=============================================================================

  ClassDef(TpAAnalogueScan, 3)
};

#endif
