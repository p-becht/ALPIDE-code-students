#include <TFile.h>
#include <TTree.h>
#include <TH2F.h>

const int NCOLS = 1024;
const int NROWS = 512;
const int NPIX  = 512*1024;


class TScan;

enum TScanType {SCANTYPE_DIGITAL, SCANTYPE_ANALOGUE, SCANTYPE_THRESHOLD, SCANTYPE_SOURCE, SCANTYPE_NOISEOCC, SCANTYPE_DACSCAN};

typedef struct SConfigData {
  char  MASK       [80];
  char  Comment    [80];
  char  GitCommit  [50];
  char  IDENTSTRING[20];
  char  SCANNAME   [10];
  float TempInit;
  float TempFinal;
  float OnChipTemp;
  float IDDD;
  float IDDA;
  float IDDDPowerOn;
  float IDDAPowerOn;
  int   FWVERSION;
  int   SCANTYPE;
  int   NEvents; 
  int   VAUX;
  int   VRESET;
  int   VRESETD;
  int   VRESETP;
  int   VCASN;
  int   VCASN2;
  int   VCASP;
  int   VCLIP;
  int   VPULSEL;
  int   VPULSEH;
  int   IRESET; 
  int   IAUX2;
  int   IBIAS;
  int   IDB;
  int   ITHR;
  int   STROBEBLENGTH;
  int   STROBEBDELAY;
  int   READOUTDELAY;
  int   STROBELENGTH;
  int   PULSEMODE;
  int   PULSELENGTH;
  int   PULSEDELAY;
  int   Qualification;
  char  READOUTMODE;
  char  Dummy;
} TConfig;


class TAnalysis {
 private:
  TFile *fRootFile;
  TTree *fScanData;
  TTree *fScanConfigs;
  TTree *fDACData;
  TTree *fNoisyPixels;
  TTree *fStuckPixels;
  void   CreateDataTree();
  //bool   CreateRootFile(const char *fName);
  void   ListScan      (TConfig AConfig);
  void   Compare       (const char *AScan1, const char *ALeaf1, const char *AScan2, const char *ALeaf2, TH1F *Histo1D, TH2F *Histo2D);
 protected:
 public:
  TAnalysis(const char *fName);
  int    GetNScans      () {return fScanData->GetNbranches()-1;};
  TTree *GetScanData    () {return fScanData;};
  TTree *GetScanConfigs () {return fScanConfigs;};
  TTree *GetDACData     () {return fDACData;};
  TTree *GetNoisyPixels () {return fNoisyPixels;};
  TTree *GetStuckPixels () {return fStuckPixels;};
  TScan *AddScan     (TScanType AType, const char *dataFile, const char *cfgFile, bool Qualification = false, const char *Comment = "");
  TScan *AddDigitalScan  (const char *dataFile, bool Qualification = false, const char *Comment = "");
  TScan *AddAnalogueScan (const char *dataFile, bool Qualification = false, const char *Comment = "");
  TScan *AddThresholdScan(const char *dataFile, const char *cfgFile, bool Qualification = false, const char *Comment = "");
  TScan *AddSourceScan   (const char *dataFile, const char *cfgFile, bool Qualification = false, const char *Comment = "");
  TScan *AddDACScan      (const char *dataFile, bool Qualification = false, const char *Comment = "");
  TScan *AddNoiseoccScan (const char *dataFile, const char *cfgFile, bool Qualification = false, const char *Comment = "");
  TScan *RetrieveScan(TScanType AType, const char *AScan);
  bool   ReadConfig  (const char *AScan, TConfig *AConfig);
  void   Write       () {fScanData->Write("", TObject::kOverwrite); fScanConfigs->Write("", TObject::kOverwrite); fDACData->Write("", TObject::kOverwrite);};//fRootFile->Write();};
  void   NoiseMap    ();
  void   NoiseAnalysis (const char *AScan, const char *ARefScan);
  void   Map         (const char *AScan, const char *ALeaf);
  void   Correlation (const char *AScan1, const char *ALeaf1, const char *AScan2, const char *ALeaf2);
  void   DiffHisto   (const char *AScan1, const char *ALeaf1, const char *AScan2, const char *ALeaf2);
  void   DumpAllConfigurations();
  void   DumpConfiguration    (TConfig AConfig);
  void   ListAllScans         ();
  void   Summarise            (); 
  void   AddPixel             (TTree *ATree, int AReg, int ADCol, int AAddress);
  void   AddNoisyPixel        (int AReg, int ADCol, int AAddress) {AddPixel (fNoisyPixels, AReg, ADCol, AAddress);};
  void   AddStuckPixel        (int AReg, int ADCol, int AAddress) {AddPixel (fStuckPixels, AReg, ADCol, AAddress);};
  void   DumpPixels           (TTree *APixelTree);
  void   DumpStuckPixels      () {std::cout << "Stuck Pixels: " << std::endl; DumpPixels(fStuckPixels);};
  void   DumpNoisyPixels      () {std::cout << "Noisy Pixels: " << std::endl; DumpPixels(fNoisyPixels);};
};


class TScan {
 private:
  TScanType   fType;
  TAnalysis  *fAnalysis;
  int         fNValues;
  TConfig     fConfig;
  void        ParseLine           (const char *Line);
  bool        FindIdentifier      (const char *fName);
  void        InitConfig          (bool Qualification, const char *Comment);
 protected:
  TFile       *fRootFile;
  virtual bool ReadConfigFile(const char *fName, const char *ABranch);
  //  bool         ReadConfigTree(int AScanNumber);
  virtual bool ReadDataFile  (const char *fName, const char *ABranch) {(void)fName; (void) ABranch; return true;};
 public:
  TScan        (TScanType AScanType, TAnalysis *AAnalysis, bool Qualification, const char *Comment);
  void         Init           (int AScanNumber, const char *dataFile, const char *cfgFile);
  //void         Add (TFile *ARootFile);
  int          GetNValues     () {return fNValues;};  
  const char  *GetIdentifier  () {return fConfig.IDENTSTRING;};
  bool         IsQualification() {return fConfig.Qualification;};
  bool         IsDigitalScan  () {return (fType == SCANTYPE_DIGITAL);};
  bool         IsAnalogueScan () {return (fType == SCANTYPE_ANALOGUE);};
  bool         IsThresholdScan() {return (fType == SCANTYPE_THRESHOLD);};
  bool         IsSourceScan   () {return (fType == SCANTYPE_SOURCE);};
  bool         IsNoiseoccScan () {return (fType == SCANTYPE_NOISEOCC);};
  bool         IsDACScan      () {return (fType == SCANTYPE_DACSCAN);};
  void         DumpConfig     ();
  virtual void Plot           () {};
  virtual void Map            (const char *ALeaf);
  virtual void Summarise      () = 0;
};


class TThresholdScan : public TScan {
 protected: 
  virtual bool ReadDataFile (const char *fName, const char *ABranch);
 public:
  TThresholdScan (TAnalysis *AAnalysis, int AScanNumber, const char *dataFile, const char *cfgFile, bool Qualification, const char *Comment);
  TThresholdScan (TAnalysis *AAnalysis, TConfig AConfig);
  virtual void Plot      () { Map ("thresh"); };
  int          GetAverage(const char *ALeaf, float Av[4], float RMS[4]) ;
  virtual void Summarise ();
};


class THitmap : public TScan {
 protected:
  virtual bool ReadDataFile (const char *fName, const char *ABranch);
 public: 
  THitmap (TAnalysis *AAnalysis, TScanType AScantype, int AScanNumber, const char *dataFile, const char *cfgFile, bool Qualification, const char *Comment);
  THitmap (TAnalysis *AAnalysis, TScanType AScanType, TConfig AConfig);
  virtual void Plot () { Map ("nhits"); };
  int          CountPixels(int TargetValue, int &Equal, int &Above, int &Below) ;
};


class TNoiseocc : public THitmap {
 protected: 
 public:
  TNoiseocc (TAnalysis *AAnalysis, int AScanNumber, const char *dataFile, const char *cfgFile, bool Qualification, const char *Comment);
  TNoiseocc (TAnalysis *AAnalysis, TConfig AConfig);
  virtual void Summarise ();
};


class TDigitalScan : public THitmap {
 protected:
 public:
  TDigitalScan (TAnalysis *AAnalysis, int AScanNumber, const char *dataFile, bool Qualification, const char *Comment);
  TDigitalScan (TAnalysis *AAnalysis, TConfig AConfig);
  virtual void Summarise ();
};


class TAnalogueScan : public THitmap {
 protected:
 public:
  TAnalogueScan (TAnalysis *AAnalysis, int AScanNumber, const char *dataFile, bool Qualification, const char *Comment);
  TAnalogueScan (TAnalysis *AAnalysis, TConfig AConfig);
  virtual void Summarise ();
};

class TSourceScan : public THitmap {
 public:
  TSourceScan (TAnalysis *AAnalysis, int AScanNumber, const char *dataFile, const char *cfgFile, bool Qualification, const char *Comment);
  TSourceScan (TAnalysis *AAnalysis, TConfig AConfig);
  virtual void Summarise ();
};


class TDACScan : public TScan {
 private:
  virtual void PlotDAC        (const char *DACName);
 protected:
  virtual bool ReadConfigFile (const char *fName, const char *ABranch);
  virtual bool ReadDataFile   (const char *fName, const char *ABranch) {(void) fName; (void) ABranch; return true;}; 
  bool         ReadDataFile   (const char *fName); 
  bool         ReadDACFile    (const char *dataPath, const char *DACName);
  void         Init           (const char *dataPath);
 public:
  TDACScan (TAnalysis *AAnalysis, int AScanNumber, const char *dataPath, bool Qualification, const char *Comment);
  TDACScan (TAnalysis *AAnalysis, TConfig AConfig);
  virtual void Plot ();
  virtual void Map  (const char *ALeaf) {(void)ALeaf; std::cout << "Map not implemented for DAC scan" << std::endl; return;};
  virtual void Summarise () {std::cout << "DAC Scan: All OK" << std::endl;};
};


void EntryToColRow   (int AEntry, int &ACol, int &ARow);
int  AddressToColumn (int ADoubleCol, int AAddress);
int  AddressToColumn (int ARegion, int ADoubleCol, int AAddress);
int  AddressToRow    (int AAddress);



