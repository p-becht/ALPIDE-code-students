#ifndef __pALPIDEfs_software__TModuleSetup__
#define __pALPIDEfs_software__TModuleSetup__

#include "TTestsetup.h"

enum TModuleType {MODULE_TYPE_IB, MODULE_TYPE_OB};

class TModuleSetup : public TTestSetup {
 private:
  TModuleType fType;
 protected:
 public:
  TModuleSetup ();
  virtual TDAQBoard *GetDAQBoard (int i);
  virtual int    FindDAQBoards     (TConfig *AConfig);
  virtual int    AddDUTs           (TConfig *AConfig);
  virtual int    GetBoardID        (int AChipID);
  int            GetChipID         (int index);
  int            GetChipIndexByID  (int AChipID);
  //  virtual bool PowerOnBoard      (int ADAQBoardID, int &Overflow);
  virtual void   PowerOffBoard     (int ADAQBoardID);
  virtual bool   InitialiseChip    (int AChipID, int &AOverflow, bool Reset = true);
  bool           InitialiseAllChips(int &AOverflow);
  virtual double ReadDacMonV     (int ADAQBoardID, TAlpideDacmonV AVmon);
  virtual double ReadDacMonI     (int ADAQBoardID, TAlpideDacmonI AImon);
  virtual void   ReadAllChipDacs           (int ADAQBoardID);
  virtual void   ReadAllChipDacsCSV        (int ADAQBoardID);
  virtual void   scanVoltageDAC            (int ADAQBoardID, int ADAC, TAlpideDacmonV AVmon, int SampleDistance = 1);
  virtual void   scanCurrentDAC            (int ADAQBoardID, int ADAC, TAlpideDacmonI AImon, int SampleDistance = 1);
  virtual void   ScanAllChipDacs           (int ADAQBoardID, int SampleDistance = 1);
  virtual double ReadTempOnChip            (int ADAQBoardID);
};

#endif  // #ifndef __pALPIDEfs_software__TModuleSetup__
