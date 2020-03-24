#include "AliPALPIDEFSRawStreamMS.h"

//__________________________________________________________
AliPALPIDEFSRawStreamMS::AliPALPIDEFSRawStreamMS() :
    fEventCounter(0),
    fFirstEvent(1),
    fLastEvent(0),
    fHitIter(0)
{
// Construct
    fHitCols.reserve(50);
    fHitRows.reserve(50);
}

//__________________________________________________________
AliPALPIDEFSRawStreamMS::~AliPALPIDEFSRawStreamMS()
{
    fFileInput.close();
    fHitCols.clear();
    fHitRows.clear();
}
//__________________________________________________________
Bool_t AliPALPIDEFSRawStreamMS::SetInputFile(const char *filename)
{
    fFileInput.open(filename);
    return fFileInput.is_open();
}

//__________________________________________________________
Bool_t AliPALPIDEFSRawStreamMS::ReadEvent()
{
    Int_t   evt;
    Short_t col, row;

    fHitIter = 0;
    fHitCols.clear();
    fHitRows.clear();

    if(!fFileInput.good()) {
        if(!IsLastEvent())
            cerr << "AliPALPIDEFSRawStreamMS::ReadEvent() : Error Input File : 1" << endl;
        return kFALSE;
    }

    if(fFirstEvent) {
        fFileInput >> evt;
        fEventCounter = evt;
        fFirstEvent = kFALSE;
    }
    else {
        evt = fEventCounter;
    }

    while(fFileInput.good() && evt == fEventCounter) {
        fFileInput >> col >> row;
        fHitCols.push_back(col);
        fHitRows.push_back(row);
        if(!fFileInput.good()) {
            cerr << "AliPALPIDEFSRawStreamMS::ReadEvent() : Error Input File : 2" << endl;
            return kFALSE;
        }
        fFileInput >> evt;
    }

    if(!fFileInput.good())
        fLastEvent = kTRUE;

    fEventCounter = evt;

    return kTRUE;
}

//__________________________________________________________
Int_t AliPALPIDEFSRawStreamMS::GetHitPixels(Short_t *col, Short_t* row) {
    // not working
    col = &fHitCols[0];
    row = &fHitRows[0];
    return fHitCols.size();
}

//__________________________________________________________
Bool_t AliPALPIDEFSRawStreamMS::GetNextHit(Short_t *col, Short_t* row) {
    if( fHitIter < GetNumHits() ) {
        *col = fHitCols.at(fHitIter);
        *row = fHitRows.at(fHitIter);
        ++fHitIter;
        return kTRUE;
    }
    else {
        *col = -1;
        *row = -1;
        return kFALSE;
    }
}
