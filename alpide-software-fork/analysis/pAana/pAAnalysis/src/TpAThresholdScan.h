#ifndef TPATHRESHOLDSCAN_H
#define TPATHRESHOLDSCAN_H

#include "TH1D.h"
#include "TpAHitmap.h"
//=============================================================================

class TH2D;
class TpAAnalysis;

class TpAThresholdScan : public TpAHitmap {

 public :

  TpAThresholdScan(TpAAnalysis *AAnalysis,
                   const char  *dataFile,
                   bool         Qualification,
                   const char  *Comment,
                   int          version);

  TpAThresholdScan(TpAAnalysis *AAnalysis, TConfig AConfig);

  TpAThresholdScan(const TpAThresholdScan &src);
  TpAThresholdScan& operator=(const TpAThresholdScan &src);
  virtual ~TpAThresholdScan();
//=============================================================================

  virtual void Plot();
  virtual void Summarise();
//=============================================================================

  double GetThresholdMean(const int i) { if (fHistoThreshMean) return fHistoThreshMean->GetBinContent(i+1); else return 0.; }
  double GetNoiseMean    (const int i) { if (fHistoNoiseMean)  return fHistoNoiseMean ->GetBinContent(i+1); else return 0.; }
  double GetChisqMean    (const int i) { if (fHistoChisqMean)  return fHistoChisqMean ->GetBinContent(i+1); else return 0.; }

  double GetThresholdRMS(const int i) { if (fHistoThreshMean) return fHistoThreshMean->GetBinError(i+1); else return 0.; }
  double GetNoiseRMS    (const int i) { if (fHistoNoiseMean)  return fHistoNoiseMean ->GetBinError(i+1); else return 0.; }
  double GetChisqRMS    (const int i) { if (fHistoChisqMean)  return fHistoChisqMean ->GetBinError(i+1); else return 0.; }

  TH1D *GetThresholdMean() const { return fHistoThreshMean; }
  TH1D *GetNoiseMean()     const { return fHistoNoiseMean;  }
  TH1D *GetChisqMean()     const { return fHistoChisqMean;  }

  TH2D *GetThreshold() const { return fHistoThreshDist; }
  TH2D *GetNoise()     const { return fHistoNoiseDist;  }
  TH2D *GetChisq()     const { return fHistoChisqDist;  }
//=============================================================================

 protected :

  virtual void Mapping(const float TargetValue, const char *ALeaf);
//=============================================================================

 private :

  TH1D *fHistoThreshMean;
  TH1D *fHistoNoiseMean;
  TH1D *fHistoChisqMean;

  TH2D *fHistoThreshDist;
  TH2D *fHistoNoiseDist;
  TH2D *fHistoChisqDist;
//=============================================================================

  ClassDef(TpAThresholdScan, 2)
};

#endif
