#ifndef TPASCAN_H
#define TPASCAN_H

#include "TObject.h"
#include "TpAUtils.h"
//=============================================================================

using namespace TpAUtils;
//=============================================================================

class TpAAnalysis;

class TpAScan : public TObject {

 public :

  TpAScan(TScanType    AScanType,
          TpAAnalysis *AAnalysis,
          bool         Qualification,
          const char  *Comment,
          int          version);

  TpAScan(TpAAnalysis *AAnalysis, TConfig AConfig);

  TpAScan(const TpAScan &src);
  TpAScan& operator=(const TpAScan &src);
  virtual ~TpAScan();
//=============================================================================

//int  GetVersion      () const { return fConfig.VERSION; }
  bool IsQualification () const { return fConfig.Qualification; }

  bool IsDigitalScan   () const { return (fType==SCANTYPE_DIGITAL);   }
  bool IsAnalogueScan  () const { return (fType==SCANTYPE_ANALOGUE);  }
  bool IsThresholdScan () const { return (fType==SCANTYPE_THRESHOLD); }
  bool IsSourceScan    () const { return (fType==SCANTYPE_SOURCE);    }
  bool IsNoiseoccScan  () const { return (fType==SCANTYPE_NOISEOCC);  }
  bool IsDACScan       () const { return (fType==SCANTYPE_DACSCAN);   }

  virtual void Plot     () = 0;
  virtual void Summarise() = 0;
//=============================================================================

 protected :

  virtual void Init(const char *dataFile);

  virtual bool ReadConfigFile(const char *fName);
  virtual bool ReadDataFile  (const char *fName) = 0;

  TConfig      fConfig;
  TpAAnalysis *fAnalysis;
//=============================================================================

 private :

  void ParseLine(const char *Line);
  void FindIdentifier(const char *fName);
  void InitConfig(bool Qualification, int version, const char *Comment);

  TScanType fType;
//=============================================================================

  ClassDef(TpAScan, 3)
};

#endif
