#ifndef TPAUTILS_H
#define TPAUTILS_H

#include "TString.h"

namespace TpAUtils {

  const int NCOLS = 1024;
  const int NROWS = 512;
  const int NPIX  = 512*1024;

  const int   NSEC        = 8;
  const int   NOISECUT    = 10;
  const int   NHITDIGITAL = 50;
  const float CHISQCUT    = 4.0;
//=============================================================================

  enum TScanType {
    SCANTYPE_DIGITAL   = 0,
    SCANTYPE_ANALOGUE  = 1,
    SCANTYPE_THRESHOLD = 2,
    SCANTYPE_SOURCE    = 3,
    SCANTYPE_NOISEOCC  = 4,
    SCANTYPE_DACSCAN   = 5 };
//=============================================================================

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
//=============================================================================

  TString GetScanName(TScanType aType);

  void EntryToColRow(int AEntry, int &ACol, int &ARow);

  int AddressToRow   (int AAddress);
  int AddressToColumn(int ADoubleCol, int AAddress);
  int AddressToColumn(int ARegion, int ADoubleCol, int AAddress);
  void DcolAddr2ColRow(int dcol, int addr, int &col, int &row);
  const char *CheckChipID(const TString sFile);
};

#endif
