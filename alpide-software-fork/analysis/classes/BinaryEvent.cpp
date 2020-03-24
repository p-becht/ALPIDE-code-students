#include "BinaryEvent.hpp"
#include <iostream>

using namespace std;

ClassImp(BinaryEvent);

// default constructor
//______________________________________________________________________
BinaryEvent::BinaryEvent()
 :TObject(),
  fEvtID(-1),
  fExtTrigCnt(-1),
  fIntTrigCnt(-1),
  //fLastPlane(0),
  fNPlanes(0),
  fPlanes(0)
{
  fPlanes=new TClonesArray(BinaryPlane::Class());
}

// copy constructor
//______________________________________________________________________
BinaryEvent::BinaryEvent(const BinaryEvent& orig)
 :TObject(orig),
  fEvtID(-1),
  fExtTrigCnt(-1),
  fIntTrigCnt(-1),
  //fLastPlane(0),
  fNPlanes(0),
  fPlanes(0)
{
  fPlanes=new TClonesArray(BinaryPlane::Class());
  cout << "copy constructor should not be used" << endl;
}

// assignment operator
//______________________________________________________________________
BinaryEvent& BinaryEvent::operator=(const BinaryEvent& orig) {
  if (this!=&orig) {
    fEvtID=orig.fEvtID;
    fExtTrigCnt=orig.fExtTrigCnt;
    fIntTrigCnt=orig.fIntTrigCnt;
    fNPlanes=orig.fNPlanes;
    *fPlanes=*orig.fPlanes;
  }
  return *this;
}

// add plane to event
//______________________________________________________________________
void BinaryEvent::AddPlane(BinaryPlane *plane) {
  BinaryPlane *add_plane = (BinaryPlane*)fPlanes->ConstructedAt(fNPlanes++);
  *add_plane = *plane;
  //Save reference to last Track in the collection of Clusters
  //fLastPlane = add_plane;  
}
   
// get specific plane with index idx from event
//______________________________________________________________________
BinaryPlane* BinaryEvent::GetPlane(Int_t idx) {
  return static_cast<BinaryPlane*>(fPlanes->UncheckedAt(idx));
}

// reset event
//______________________________________________________________________
void BinaryEvent::Reset() {
  fEvtID=-1;
  fExtTrigCnt=-1;
  fIntTrigCnt=-1;
  //fLastPlane=NULL;
  fNPlanes=0;
  fPlanes->Clear();
}

  
