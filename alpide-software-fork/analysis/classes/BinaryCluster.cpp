#include "BinaryCluster.hpp"
#include "Riostream.h"
#include "TMath.h"

using namespace std;

ClassImp(BinaryCluster)

// default constructor, signal array size 0
//______________________________________________________________________
BinaryCluster::BinaryCluster()
:TObject(),
    fClusterID(-1),
    fSectorID(-1),
    fNPixels(0),
    fPixels(0x0)
{
}

// constructor signal & noise array with size nxn set to 0
//______________________________________________________________________
BinaryCluster::BinaryCluster(Int_t npixels)
    :TObject(),
     fClusterID(-1),
     fSectorID(-1),
     fNPixels(npixels)
{
    fPixels = new BinaryPixel[fNPixels];
}

// copy constructor
//______________________________________________________________________
BinaryCluster::BinaryCluster(const BinaryCluster &orig)
    :TObject(),
     fClusterID(orig.fClusterID),
     fSectorID(orig.fSectorID),
     fNPixels(orig.fNPixels)
{
    fPixels = new BinaryPixel[fNPixels];
    for (Int_t i=0; i<fNPixels; ++i) {
        fPixels[i]=orig.fPixels[i];
    }
}

// assignment operator
//______________________________________________________________________
BinaryCluster& BinaryCluster::operator=(const BinaryCluster &orig) {
    if (this!=&orig) {
        if (fNPixels!=orig.fNPixels) {
            delete[] fPixels;
            fNPixels=orig.fNPixels;
            fPixels = new BinaryPixel[fNPixels];
        }
        for (Int_t i=0; i<fNPixels; ++i) {
            fPixels[i]=orig.fPixels[i];
        }
        fClusterID=orig.fClusterID;
        fSectorID=orig.fSectorID;
    }
    return *this;
}

// destructor
//______________________________________________________________________
BinaryCluster::~BinaryCluster() {
    delete[] fPixels; fPixels=NULL;
}

// get x position of center of mass of cluster signal
//______________________________________________________________________
Float_t BinaryCluster::GetX() {
    Float_t sx=0.;
    for (Int_t i=0; i<fNPixels; ++i)
        sx += fPixels[i].GetCol();
    return sx/fNPixels;
}

// get y position of center of mass of cluster signal
//______________________________________________________________________
Float_t BinaryCluster::GetY() {
    Float_t sy=0.;
    for (Int_t i=0; i<fNPixels; ++i)
        sy += fPixels[i].GetRow();
    return sy/fNPixels;
}

// reset hole cluster, but keep the array size
//______________________________________________________________________
void BinaryCluster::Reset() {
    fClusterID=-1;
    fSectorID=-1;
    for (Int_t i=0; i<fNPixels; ++i)
        fPixels[i].Reset();
}

// set ClusterID
//______________________________________________________________________
void BinaryCluster::SetClusterID(Int_t clustID) {fClusterID=clustID;}

// set SectorID
//______________________________________________________________________
void BinaryCluster::SetSectorID(Int_t sectorID) {fSectorID=sectorID;}

// set pixel array
//______________________________________________________________________
void BinaryCluster::SetPixelArray(Int_t npixels, BinaryPixel *pixels) {
    if(fPixels) delete[] fPixels;
    fNPixels = npixels;
    fPixels = new BinaryPixel[fNPixels];    
    for (Int_t i=0; i<fNPixels; i++) {
        fPixels[i]=pixels[i];
    }
}

// get cluster width
//______________________________________________________________________
Int_t BinaryCluster::GetXSpread() {
    Int_t min = 1024, max = 0; // make it more general
    for(Int_t i=0; i<fNPixels; ++i) {
        Int_t col = fPixels[i].GetCol();
        if(col > max) max = col;
        if(col < min) min = col;
    }
    return max-min+1;
}

// get cluster height
//______________________________________________________________________
Int_t BinaryCluster::GetYSpread() {
    Int_t min = 1024, max = 0; // make it more general
    for(Int_t i=0; i<fNPixels; ++i) {
        Int_t row = fPixels[i].GetRow();
        if(row > max) max = row;
        if(row < min) min = row;
    }
    return max-min+1;
}

// get maximum distance between two pixels
//______________________________________________________________________
Float_t BinaryCluster::GetMaxSpread() {
    Float_t max = 0.;
    for(Int_t i=0; i<fNPixels; ++i) {
        for(Int_t j=i+1; j<fNPixels; ++j) {
            Float_t d = TMath::Sqrt(
                (fPixels[i].GetCol()-fPixels[j].GetCol())*(fPixels[i].GetCol()-fPixels[j].GetCol()) +
                (fPixels[i].GetRow()-fPixels[j].GetRow())*(fPixels[i].GetRow()-fPixels[j].GetRow()) );
            if(d > max) max = d;
        }
    }
    return max;
}


// get OR of pixel flags
//______________________________________________________________________
UShort_t BinaryCluster::GetPixelFlags() {
    UShort_t flags = 0;
    for(Int_t i=0; i<fNPixels; ++i)
        flags = flags | fPixels[i].GetFlags();
    return flags;
}

// contains hot pixels?
//______________________________________________________________________
Bool_t BinaryCluster::HasHotPixels() {
    for(Int_t i=0; i<fNPixels; ++i)
        if(fPixels[i].IsHot())
            return kTRUE;
    return kFALSE;
}

// contains border pixels?
//______________________________________________________________________
Bool_t BinaryCluster::HasBorderPixels() {
    for(Int_t i=0; i<fNPixels; ++i)
        if(fPixels[i].IsBorder())
            return kTRUE;
    return kFALSE;
}

//______________________________________________________________________
void BinaryCluster::Print(Option_t *) const {
    cout << "********************BinaryCluster********************" << endl;
    cout << " Multiplicity: " << fNPixels << endl;
    for(Short_t i=0; i<fNPixels; ++i)
        fPixels[i].Print();
    cout << "*****************************************************" << endl;
}
