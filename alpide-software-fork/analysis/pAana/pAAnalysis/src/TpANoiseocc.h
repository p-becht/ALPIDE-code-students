#ifndef TPANOISEOCC_H
#define TPANOISEOCC_H

#include "TH1D.h"
#include "TpAHitmap.h"


class TpANoiseocc : public TpAHitmap {

 public :

  TpANoiseocc(TpAAnalysis *AAnalysis,
              const char  *dataFile,
              bool         Qualification,
              const char  *Comment,
              int          version);

  TpANoiseocc(TpAAnalysis *AAnalysis, TConfig AConfig);

  TpANoiseocc(const TpANoiseocc &src);
  TpANoiseocc& operator=(const TpANoiseocc &src);
  virtual ~TpANoiseocc();
//=============================================================================

  virtual void Plot();
  virtual void Summarise();
//=============================================================================

  int   GetNNoisiestPixels() const { return fNoisiestPixels;   }
  TH2D *GetNoisiestMap()     const { return fHistoNoisiestMap; }

  double GetNoisePixels     (int i) { if (fHistoNoise)        return fHistoNoise       ->GetBinContent(i+1); else return 0; }
  double GetNoiseFraction   (int i) { if (fHistoNoiseFrac)    return fHistoNoiseFrac   ->GetBinContent(i+1); else return 0; }
  double GetNoisiestPixels  (int i) { if (fHistoNoisiest)     return fHistoNoisiest    ->GetBinContent(i+1); else return 0; }
  double GetNoisiestFraction(int i) { if (fHistoNoisiestFrac) return fHistoNoisiestFrac->GetBinContent(i+1); else return 0; }

  TH1D *GetNoisePixels()      const { return fHistoNoise;        }
  TH1D *GetNoiseFraction()    const { return fHistoNoiseFrac;    }
  TH1D *GetNoisiestPixels()   const { return fHistoNoisiest;     }
  TH1D *GetNoisiestFraction() const { return fHistoNoisiestFrac; }

  void PrintNoisiestPixels();
//=============================================================================

 protected :

  virtual void Mapping(const float TargetValue, const char *ALeaf);

 private :

  int fNoisiestPixels;
  TH2D *fHistoNoisiestMap;

  TH1D *fHistoNoise;
  TH1D *fHistoNoiseFrac;

  TH1D *fHistoNoisiest;
  TH1D *fHistoNoisiestFrac;

  ClassDef(TpANoiseocc, 2);
};

#endif
