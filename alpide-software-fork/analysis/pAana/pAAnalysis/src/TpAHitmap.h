#ifndef TPAHITMAP_H
#define TPAHITMAP_H

#include "TpAScan.h"
//=============================================================================

class TH2D;
class TpAAnalysis;

class TpAHitmap : public TpAScan {

 public :

  TpAHitmap(TScanType    AScanType,
            TpAAnalysis *AAnalysis,
            const char  *dataFile,
            bool         Qualification,
            const char  *Comment,
            int          version);

  TpAHitmap(TpAAnalysis *AAnalysis, TConfig AConfig);

  TpAHitmap(const TpAHitmap &src);
  TpAHitmap& operator=(const TpAHitmap &src);
  virtual ~TpAHitmap();

  virtual void Plot();
  virtual void Summarise();
//=============================================================================

  const char *GetIdentifier() const { return fConfig.IDENTSTRING; }

  int GetNPixels()            const { return fNPixels;            }
  int GetNDeadPixels()        const { return fNDeadPixels;        }
  int GetNInefficientPixels() const { return fNInefficientPixels; }
  int GetNNoisyPixels()       const { return fNoisyPixels;        }

  TH2D *GetPixelMap()        const { return fHistoPixelMap;       }
  TH2D *GetDeadMap()         const { return fHistoDeadMap;        }
  TH2D *GetInefficientMap()  const { return fHistoInefficientMap; }
  TH2D *GetNoisyMap()        const { return fHistoNoisyMap;       }
//=============================================================================

 protected :

  virtual bool ReadDataFile(const char *fName);
  virtual void Mapping(const float TargetValue, const char *ALeaf);

  void Map(const char *ALeaf);
  int  CountPixels(int TargetValue, int &Equal, int &Above, int &Below, const char *ALeaf="nhits");
//=============================================================================

  int fNPixels;
  int fNDeadPixels;
  int fNInefficientPixels;
  int fNoisyPixels;

  TH2D *fHistoPixelMap;
  TH2D *fHistoDeadMap;
  TH2D *fHistoInefficientMap;
  TH2D *fHistoNoisyMap;
//=============================================================================

 private :


  ClassDef(TpAHitmap, 5)
};

#endif
