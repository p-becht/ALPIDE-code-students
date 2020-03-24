// Modified by Miljenko Suljic, m.suljic@cern.ch
// Based on Jacobus W. van Hoorne Event class


#ifndef BINARYEVENT_HPP
#define BINARYEVENT_HPP

#include "TObject.h"
#include "TClonesArray.h"
#include "TRef.h"

#include "BinaryPlane.hpp"

class BinaryEvent:public TObject {
  
private:

    Int_t		fEvtID;		    // event ID number
    Int_t		fExtTrigCnt;	// external trigger counter (eg. from telescope)
    Int_t		fIntTrigCnt;	// internal trigger counter (eg. from internal event selector in Mimos32 readout)
    Int_t		fNPlanes;	    // number of planes in event
    //Float_t	fTemperature;	// temperature during data taking of event (in deg C)
    //Float_t	fTimeStamp;	    // time stamp for event (in format?)
    TClonesArray  *fPlanes; //-> array containing all planes in event

public:
    BinaryEvent   		();
    BinaryEvent			(const BinaryEvent& orig);
    BinaryEvent& operator=(const BinaryEvent& orig);
    virtual ~BinaryEvent	() {delete fPlanes;}
  
    Int_t 	GetEventID    	() const {return fEvtID;}
    Int_t 	GetExtTrigCnt  	() const {return fExtTrigCnt;}
    Int_t 	GetIntTrigCnt  	() const {return fIntTrigCnt;}
    Int_t 	GetNPlanes   	() const {return fNPlanes;}		// get number of planes in event
    BinaryPlane	 *GetPlane 	(Int_t idx);
    TClonesArray *GetPlanes	() const {return fPlanes;}
  
    void AddPlane      (BinaryPlane *plane); 				// add plane to event    
    void Reset         ();
    void SetEventID    (Int_t evtID)		 {fEvtID=evtID;}
    void SetExtTrigCnt (Int_t exttrigcnt)	 {fExtTrigCnt=exttrigcnt;}
    void SetIntTrigCnt (Int_t inttrigcnt) 	 {fIntTrigCnt=inttrigcnt;}
  
    ClassDef(BinaryEvent,1); // Event structure
};

#endif
