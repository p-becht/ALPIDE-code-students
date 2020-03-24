// Modified by Miljenko Suljic, m.suljic@cern.ch
// Based on Jacobus W. van Hoorne Cluster class

#ifndef BINARYCLUSTER_HPP
#define BINARYCLUSTER_HPP

#include <TObject.h>
#include "BinaryPixel.hpp"

class BinaryCluster: public TObject {
  
private:

    Int_t        fClusterID; // cluster ID number
    Int_t        fSectorID;  // sector ID number, can be used for explorer, palpidefs
    Int_t        fNPixels;	 // number of pixels in cluster
    BinaryPixel* fPixels;    //[fNPixels] // array only containing the signal of pixels which are assigned to cluster (are above threshold)
    
public:

    BinaryCluster();
    BinaryCluster(Int_t npixels);
    BinaryCluster(const BinaryCluster& );
    BinaryCluster& operator=(const BinaryCluster& );
    virtual ~BinaryCluster();
    
    Int_t        GetClusterID	() const {return fClusterID;}
    Int_t        GetSectorID	() const {return fSectorID;}
    Int_t        GetMultiplicity() const {return fNPixels;}
    Int_t        GetNPixels		() const {return fNPixels;} // redundancy for easier use
    BinaryPixel* GetPixelArray	() const {return fPixels;}		
    Float_t      GetX  	  		();	// X position of c.m. of cluster signal
    Float_t      GetY  	  		();	// Y position of c.m. of cluster signal
    Int_t        GetXSpread     (); // get cluster width
    Int_t        GetYSpread     (); // get cluster height
    Float_t      GetMaxSpread   (); // get maximum distance between two pixels
    UShort_t     GetPixelFlags  (); // return OR of all pixel flags.

    Bool_t HasHotPixels();          // contains hot pixels?
    Bool_t HasBorderPixels();       // contains border pixels?

    virtual void Print(Option_t *option="") const;
    void Reset();
    
    void SetClusterID (Int_t clustID);
    void SetSectorID  (Int_t sectorID);
    void SetPixelArray(Int_t npixels, BinaryPixel* pixels);
//    void SetPixelArray(Int_t npixels, Short_t* cols, Short_t* rows);

    ClassDef(BinaryCluster,1);
};

#endif


