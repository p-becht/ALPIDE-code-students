#ifndef TPASOURCESCAN_H
#define TPASOURCESCAN_H

#include "TpAHitmap.h"
//=============================================================================

class TpASourceScan : public TpAHitmap {

 public :

  TpASourceScan(TpAAnalysis *AAnalysis,
                const char  *dataFile,
                bool         Qualification,
                const char  *Comment,
                int          version);

  TpASourceScan(TpAAnalysis *AAnalysis, TConfig AConfig);

  TpASourceScan(const TpASourceScan &src);
  TpASourceScan& operator=(const TpASourceScan &src);
  virtual ~TpASourceScan();
//=============================================================================

  virtual void Summarise();
  virtual void Plot() { Info(__func__, "Not Implimented yet"); return; }
//=============================================================================

  ClassDef(TpASourceScan, 1)
};

#endif
