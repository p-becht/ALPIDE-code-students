//
//  TDut.h
//  pALPIDEfs-software
//
//  Created by Carlo Puggioni and Markus Keil on 14/04/14.
//  Copyright (c) 2014 Carlo Puggioni, Markus Keil. All rights reserved.
//

#ifndef __pALPIDEfs_software__TDut__
#define __pALPIDEfs_software__TDut__

#include <iostream>

class TTestSetup;
class TDAQBoard;
class TDAQBoard2;
struct TChipConfig;

// DUT stuff
enum TDUTType    {DUT_UNKNOWN, DUT_PALPIDEFS, DUT_PALPIDEFS1, DUT_PALPIDEFS2, DUT_PALPIDEFS3, DUT_PALPIDEFS4};
enum TChipSignal {SIG_RESET, SIG_PRST, SIG_PULSE};

typedef struct PixelHit {
    int region;
    int doublecol;
    int address;
    inline bool operator == (const struct PixelHit &h) const    {
        return ( (h.region    == region) &&
                 (h.doublecol == doublecol) &&
                 (h.address   == address));
    }
    inline bool operator < (const struct PixelHit &h) const    {
        return ( (h.region    >= region) &&
                 (h.doublecol >= doublecol) &&
                 (h.address   >= address) &&
		  !(h == *this));
    }
} TPixHit;


const char *GetDACNameGen    (int ADAC, const char ADACNames[][10], int ANDacs);
int         GetDACDefaultGen (int ADAC, const int ADACDefaults[], int ANDacs);
int         FindDACGen       (const char ADACNames[][10], const char ADACName[10], int ANDacs);
//base class for chips in case we want to test different chips later (FSBB)
class TDUT {
private:
    static const int  NDACS = 0;
    static const char DACNames[1][10];
    static const int  DACDefaults[1];
protected:
    TTestSetup  *fSetup;               // Test Setup the DUT is attached to
    TChipConfig *fConfig;     
    int         fIndex;               // index of the DUT within the setup; identical to the index of the corresponding DAQ board
    int SendSignal    (TChipSignal ASignal, int ADuration);
public:
    TDUT (TTestSetup *ASetup, int AIndex, TChipConfig *AConfig);
    virtual const char *GetClassName           () { return "TDUT";};
    TChipConfig        *GetConfig   () {return fConfig;}; 
    TDUTType            GetChipType ();
    virtual int         ReadRegister  (int Address, int *Value);
    virtual int         WriteRegister (int Address, int Value);
    virtual int         GetNDacs      ()         {return NDACS;};
    virtual const char *GetDACName    (int ADAC) {return GetDACNameGen    (ADAC, DACNames, NDACS);};
    virtual int         GetDACDefault (int ADAC) {return GetDACDefaultGen (ADAC, DACDefaults, NDACS);};
    friend class TDAQBoard;
    friend class TDAQBoard2;
};


#endif /* defined(__pALPIDEfs_software__TDut__) */
