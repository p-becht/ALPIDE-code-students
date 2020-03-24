#ifndef TPAANALYSIS_H
#define TPAANALYSIS_H

#include <iostream>

#include "TNamed.h"
#include "TFile.h"
#include "TTree.h"
#include "TpAUtils.h"
//=============================================================================

using namespace TpAUtils;
//=============================================================================

class TH1F;
class TH2F;
class TpAScan;
//=============================================================================

class TpAAnalysis : public TNamed {

 public :

  TpAAnalysis(const TString sName, const TString sTitle="pALPIDEfs analysis");
  virtual ~TpAAnalysis();
//=============================================================================

  TString GetChipID() const { return fChipID; }

  void  SetPrint() { fIsPrint = kTRUE; }
  Bool_t IsPrint() const { return fIsPrint; }

  Bool_t GetForceQualiState() const { return fIsForceQuali; }
  void SetForceQualiState(Bool_t b) { fIsForceQuali = b; }

  TTree *GetScanData    () const { return fScanData;    }
  TTree *GetScanConfigs () const { return fScanConfigs; }
  TTree *GetDACData     () const { return fDACData;     }
  TTree *GetNoisyPixels () const { return fNoisyPixels; }
  TTree *GetStuckPixels () const { return fStuckPixels; }
  int GetNScans() { return (fScanData->GetNbranches()-1); }
//=============================================================================

  TpAScan *AddScan(TScanType   AType,
                   const char *dataFile,
                   bool        Qualification = false,
                   const char *Comment       = "",
                   int         version       = 3);

  TpAScan *AddDACScan(const char *dataFile,
                      bool        Qualification = false,
                      const char *Comment       = "",
                      int         version       = 3) {
    return AddScan(SCANTYPE_DACSCAN, dataFile, Qualification, Comment, version);
  }

  TpAScan *AddDigitalScan(const char *dataFile,
                          bool        Qualification = false,
                          const char *Comment       = "",
                          int         version       = 3) {
    return AddScan(SCANTYPE_DIGITAL, dataFile, Qualification, Comment, version);
  }

  TpAScan *AddAnalogueScan(const char *dataFile,
                           bool        Qualification = false,
                           const char *Comment       = "",
                           int         version       = 3) {
    return AddScan(SCANTYPE_ANALOGUE, dataFile, Qualification, Comment, version);
  }

  TpAScan *AddThresholdScan(const char *dataFile,
                            bool        Qualification = false,
                            const char *Comment       = "",
                            int         version       = 3) {
    return AddScan(SCANTYPE_THRESHOLD, dataFile, Qualification, Comment, version);
  }

  TpAScan *AddNoiseoccScan(const char *dataFile,
                           bool        Qualification = false,
                           const char *Comment       = "",
                           int         version       = 3) {
    return AddScan(SCANTYPE_NOISEOCC, dataFile, Qualification, Comment, version);
  }

  TpAScan *AddSourceScan(const char *dataFile,
                         bool        Qualification = false,
                         const char *Comment       = "",
                         int         version       = 3) {
    return AddScan(SCANTYPE_SOURCE, dataFile, Qualification, Comment, version);
  }

  int      GetScanType (int kScan);
  TpAScan *RetrieveScan(int kScan);
  TpAScan *RetrieveScan(TConfig Config);
//TpAScan *RetrieveScan(TScanType AType, const char *AScan);
//=============================================================================

  void Summarise();
  void ListAllScans();
//=============================================================================

  void Write(const char *s="") {
    fScanData   ->Write(s, TObject::kOverwrite);
    fDACData    ->Write(s, TObject::kOverwrite);
    fScanConfigs->Write(s, TObject::kOverwrite);
//  fRootFile->Write();
  }

  bool ReadConfig(const char *AScan, TConfig *AConfig);

  void DumpAllConfigurations();
  void DumpConfiguration(TConfig AConfig);
//=============================================================================

  void GetMeanIDDD(double &dIb, double &dIbErr,
                   double &dIa, double &dIaErr,
                   double &dIc, double &dIcErr);

  void GetMeanIDDA(double &dIb, double &dIbErr,
                   double &dIa, double &dIaErr,
                   double &dIc, double &dIcErr);
//=============================================================================

//void NoiseMap();
//void Map(const char *AScan, const char *ALeaf);

  void NoiseAnalysis(const char *AScan, const char *ARefScan);
//=============================================================================

  void AddPixel(TTree *ATree, int AReg, int ADCol, int AAddress);
  void AddNoisyPixel(int AReg, int ADCol, int AAddress) { AddPixel(fNoisyPixels,AReg,ADCol,AAddress); }
  void AddStuckPixel(int AReg, int ADCol, int AAddress) { AddPixel(fStuckPixels,AReg,ADCol,AAddress); }

  void DumpPixels(TTree *APixelTree);
  void DumpStuckPixels() { std::cout << "Stuck Pixels: " << std::endl; DumpPixels(fStuckPixels); }
  void DumpNoisyPixels() { std::cout << "Noisy Pixels: " << std::endl; DumpPixels(fNoisyPixels); }
//=============================================================================

  void Correlation(const char *AScan1, const char *ALeaf1, const char *AScan2, const char *ALeaf2);
  void DiffHisto  (const char *AScan1, const char *ALeaf1, const char *AScan2, const char *ALeaf2);
//=============================================================================

 protected :
//=============================================================================

 private :

  TpAAnalysis(const TpAAnalysis &src);
  TpAAnalysis& operator=(const TpAAnalysis &src);

  void CreateDataTree();
  void ListScan(TConfig AConfig);
  void Compare(const char *AScan1, const char *ALeaf1, const char *AScan2, const char *ALeaf2, TH1F *Histo1D, TH2F *Histo2D);
//bool CreateRootFile(const char *fName);

  Bool_t CheckQualiState(const TScanType AType);
//=============================================================================

  TString fChipID;
  Bool_t fIsPrint;
  Bool_t fIsForceQuali;

  TFile *fRootFile;
  TTree *fScanData;
  TTree *fScanConfigs;
  TTree *fDACData;
  TTree *fNoisyPixels;
  TTree *fStuckPixels;
//=============================================================================

  ClassDef(TpAAnalysis, 3)
};

#endif
