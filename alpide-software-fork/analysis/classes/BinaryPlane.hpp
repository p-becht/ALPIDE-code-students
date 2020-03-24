// Modified by Miljenko Suljic, m.suljic@cern.ch
// Based on Jacobus W. van Hoorne Plane class


#ifndef BINARYPLANE_HPP
#define BINARYPLANE_HPP

#include "TObject.h"
#include "TClonesArray.h"
#include "TRef.h"

#include "BinaryCluster.hpp"


// coordinate system for plane (sensor) position
//__________________________________________________________________________
//
//						^ y  
//						|
//						|    /|
//						|  /  |    /
//						|/    |  /
//						|plane|/
//						|    /
//						|  /
//     <----------------|/------------------------------
//			z	        /			beam	
//				      /
//				    /
//				  / x
//		 		 V  
//__________________________________________________________________________

class BinaryPlane:public TObject {
  
private:
    char		fChipName[32];	// name of sensor
    Int_t		fNClusters;	// number of clusters found in plane
    Int_t		fNPixelsX;	// total number of pixels in x direction in plane (sensor), in terms of cols or rows
    Int_t		fNPixelsY;	// total number of pixels in y direction in plane (sensor), in terms of cols or rows
    Int_t		fPlaneID;	// plane ID number
    Float_t	fPosX; 		// x position of left lower corner of sensor seen in beam direction
    Float_t	fPosY;		// y position of left lower corner of sensor seen in beam direction
    Float_t	fPosZ; 		// z position of plane (z-axis pointing in beam direction)
    //Float_t	fTemperature;
    TClonesArray *fClusters; //-> array containg all clusters in one plane
   
public:
    BinaryPlane   		();
    BinaryPlane			(const BinaryPlane& orig);
    BinaryPlane& operator=(const BinaryPlane& orig);
    virtual ~BinaryPlane	() {delete fClusters;}

    char		*GetChipName		() {return fChipName;}
    BinaryCluster	*GetCluster		(Int_t idx); 
    TClonesArray 	*GetClusters	() const {return fClusters;}
    Int_t 	GetNClustersSaved	() const {return fNClusters;}
    Int_t 	GetNPixelsX   		() const {return fNPixelsX;}
    Int_t 	GetNPixelsY   		() const {return fNPixelsY;}
    Int_t 	GetPlaneID    		() const {return fPlaneID;}
    Float_t 	GetPosX    		() const {return fPosX;}
    Float_t 	GetPosY    		() const {return fPosY;}
    Float_t 	GetPosZ	 		() const {return fPosZ;}
  
    void AddCluster	 (BinaryCluster *cluster);	// add cluster to plane
    void Reset		 ();
  
    void SetChipName   	 (char *chipname)	 {strcpy(fChipName,chipname);}
    void SetNPixelsX   	 (Int_t npixelsx)	 {fNPixelsX=npixelsx;}
    void SetNPixelsY   	 (Int_t npixelsy)	 {fNPixelsY=npixelsy;}
    void SetPlaneID    	 (Int_t planeID)     {fPlaneID=planeID;}
    void SetPosX    	 (Float_t posx)		 {fPosX=posx;}
    void SetPosY    	 (Float_t posy)   	 {fPosY=posy;}
    void SetPosZ    	 (Float_t posz)	 	 {fPosZ=posz;}

    ClassDef(BinaryPlane,1);
};

#endif
