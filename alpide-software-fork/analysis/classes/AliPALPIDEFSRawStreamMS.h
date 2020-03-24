/// Written by Miljenko Suljic, m.suljic@cern.ch
// Based on Valerio Altini's AliMIMOSARawStreamVA class

#ifndef ALIPALPIDEFSRAWSTREAMMS_H
#define ALIPALPIDEFSRAWSTREAMMS_H

#include "Riostream.h"
#include "TObject.h"

class AliPALPIDEFSRawStreamMS: public TObject {
public:
    AliPALPIDEFSRawStreamMS();
    ~AliPALPIDEFSRawStreamMS();

    Bool_t SetInputFile(const char *filename);
    Bool_t ReadEvent();
    
    Bool_t IsLastEvent()     { return fLastEvent; }
    Int_t  GetEventCounter() { return fEventCounter; }
    Int_t  GetNumHits()      { return fHitCols.size(); }
    Int_t  GetHitPixels(Short_t *col, Short_t* row);
    Bool_t GetNextHit(Short_t *col, Short_t* row);
//    void   GetHitAt(Int_t i, Short_t *col, Short_t* row);
    
private:

    ifstream fFileInput;
    Bool_t   fFirstEvent;
    Int_t    fEventCounter;
    Bool_t   fLastEvent;
    Int_t    fHitIter;
    
    std::vector<Short_t> fHitCols;
    std::vector<Short_t> fHitRows;    
    
    ClassDef(AliPALPIDEFSRawStreamMS,1)
};

#endif
