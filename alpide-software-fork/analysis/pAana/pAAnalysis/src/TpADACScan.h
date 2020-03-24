#ifndef TPADACSCAN_H
#define TPADACSCAN_H

#include "TpAScan.h"
//=============================================================================

class TGraph;
class TpAAnalysis;

class TpADACScan : public TpAScan {

 public :

  TpADACScan(TpAAnalysis *AAnalysis,
             const char  *dataPath,
             bool         Qualification,
             const char  *Comment,
             int          version);

  TpADACScan(TpAAnalysis *AAnalysis, TConfig AConfig);

  TpADACScan(const TpADACScan &src);
  TpADACScan& operator=(const TpADACScan &src);
  virtual ~TpADACScan();
//=============================================================================

  virtual void Plot();
  virtual void Summarise();
//=============================================================================

  void SetIdentifier(const char *c) { sprintf(fConfig.IDENTSTRING, "%s", c); }
//=============================================================================

 protected :

  virtual bool ReadDataFile(const char *fName);
//=============================================================================

 private :

  TGraph *PlotDAC(const char *DACName);
  TGraph *PlotLinearity(const char *DACName);
  bool ReadDACFile(const char *dataPath, const char *DACName);
//=============================================================================

  ClassDef(TpADACScan, 4)
};

#endif
