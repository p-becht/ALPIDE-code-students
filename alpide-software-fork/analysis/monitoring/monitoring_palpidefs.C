/// Developer: Valerio Altini  Contact: valerio.altini@cern.ch  ///
//  Modified for palpide by Miljenko Suljic, m.suljic@cern.ch

#include <iostream>
using namespace std;
#include <sstream>
#include <TH2.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TColor.h>
#include <TMath.h>
#include <TROOT.h>
#include <../classes/AliPALPIDEFSRawStreamMS.h>
#include <TPad.h>
#include <TFile.h>
#include <TSystem.h>
#include <TGClient.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TGLayout.h>
#include <TGWindow.h>
#include <TGLabel.h>
#include <TGNumberEntry.h>
#include <TApplication.h>
#include <TGTextEntry.h>
#include <TGComboBox.h>
#include <TRootCanvas.h>

class Monitoring {

protected:

    virtual ~Monitoring();

private:

    Int_t totevt;
    Bool_t fStop;
    char *fFile;
    Bool_t flagInitHistos;	

    TCanvas *c1; //single event
    TCanvas *c2; //summary canvas

    AliPALPIDEFSRawStreamMS *palpidefs;

    TGMainFrame *fMain;
    //TGMainFrame *PedM;
    TGGroupFrame *fGframe;  
    TGNumberEntry *fNumber; 
    TGLabel *fLabel;
    TGGroupFrame *fGframe2;
    TGNumberEntry *fNumber2;
    TGLabel *fLabel2;
    TGTextEntry *tentry;
    // Histo to fill
    TH2F *map;
    TH2F *maptot;

    // Functions
  
    Bool_t Init();
    Bool_t InitHistos();
    Bool_t ProcessSingleEvent();
    Bool_t DrawSE();
    Bool_t ProcessSingleEventFast();
    Bool_t FinishEventFast();

public:

    void   SetFile(char *filename="data.txt"){fFile = filename;};
    Bool_t Terminate();
    Int_t  Run();
    Bool_t NextEvt();
    Bool_t End();
    Bool_t EndSilent();
    void   Stop();
    void   DisplayInt();
    void   DisplayFloat();
    void   MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h);
  
    ClassDef(Monitoring,0)

};
//_______________________________________________________________
Monitoring::~Monitoring() {

    delete palpidefs;
    fMain->Cleanup();
    // PedM->Cleanup();
    delete fMain;
    //delete PedM;
    delete fGframe;
    delete fNumber;
    delete fGframe2;
    delete fNumber2;
    delete fLabel;
    delete tentry;
    delete c1;
    delete c2;
    delete map;
    delete maptot;

}
//_______________________________________________________________
Bool_t Monitoring::Init(){

    printf("\n\n"
           "************* MONITORING AND OFFLINE DATA ANALYSIS GUI *************\n"
           "*************  CURRENT VERSION v0.1 February 23, 2015  *************\n"
           "************* for any request contact m.suljic@cern.ch *************\n"
           "\n\n");

    gStyle->SetOptStat(111111);

    totevt=0;
    fStop = kFALSE;
    flagInitHistos=kFALSE;

    palpidefs = new AliPALPIDEFSRawStreamMS();
    palpidefs->SetInputFile(fFile);

    MyMainFrame(gClient->GetRoot(),200,200);
 
    return kTRUE;

}

//_______________________________________________________________
Bool_t Monitoring::InitHistos(){

    /// Histo part
    const Int_t NRGBs = 5;
    const Int_t NCont = 255;
    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);

    TRootCanvas *c1i;
    TRootCanvas *c2i;

    c1 = new TCanvas("pALPIDEfsSnglEvt","pALPIDEfs Single Event",1200,600);
    c1i=(TRootCanvas*)c1->GetCanvasImp();
    c1i->DontCallClose();

    c2 = new TCanvas("pALPIDEfsAllEvt","pALPIDEfs Cumulative Events",1200,600);
    c2i=(TRootCanvas*)c2->GetCanvasImp();
    c2i->DontCallClose();
    
    map = new TH2F("snglevt", "pALPIDEfs Event;Column;Row", 1024, -0.5, 1023.5, 512, -0.5, 512);
    maptot = new TH2F("allevt", "pALPIDEfs Multiple Events;Column;Row", 1024, -0.5, 1023.5, 512, -0.5, 512);
   
    map->SetStats(0);
    maptot->SetStats(0);

// End histo part

    return kTRUE;

}

//_______________________________________________________________
Bool_t Monitoring::ProcessSingleEvent(){

    gSystem->ProcessEvents();

    if(fStop)return kFALSE;

    ++totevt;  // no effect for eth, foundamentl for DATE monitoring

    UInt_t evt = palpidefs->GetEventCounter();

    //*****************************************************
    if(!palpidefs->ReadEvent()) {
        cout << "Monitoring::problem Reading event:" 
             << palpidefs->GetEventCounter() << endl;
        return kFALSE;
    }

    if(!flagInitHistos){
        cout << "evt :" << evt << endl;
        if(!InitHistos()){
            cout << "Problem in initializing histograms" << endl;
            return kFALSE;
        }
        flagInitHistos=kTRUE;
    }

    map->Reset();
    map->SetTitle(Form("pALPIDEfs Event %i", totevt));
/*
    maptot->Scale(totevt-1.);
    maptot->Scale(1./totevt);
*/
    Short_t col;
    Short_t row;

    for(Int_t ihit=0; palpidefs->GetNextHit(&col, &row); ++ihit) {
        map->Fill(col, row);
        maptot->Fill(col, row);
    }

    return kTRUE; 

}
//_______________________________________________________________
Bool_t Monitoring::FinishEventFast(){

    map->Reset();

    return kTRUE;

}
//_______________________________________________________________
Bool_t Monitoring::ProcessSingleEventFast(){

    if(fStop)return kFALSE;

    ++totevt; // no effect for eth, foundamentl for DATE monitoring
    UInt_t evt = palpidefs->GetEventCounter();

    //*****************************************************
    if(!palpidefs->ReadEvent()) {
        cout << "Monitoring::problem Reading event:" 
             << palpidefs->GetEventCounter() << endl;
        return kFALSE;
    }

    if(!flagInitHistos){
        cout << "evt :" << evt << endl;
        if(!InitHistos()){
            cout << "Problem in initializing histograms" << endl;
            return kFALSE;
        }
        flagInitHistos=kTRUE;
    }

    Short_t col, row;
    for(Int_t ihit=0; palpidefs->GetNextHit(&col, &row); ++ihit) {
        maptot->Fill(col, row);
    }

    return kTRUE;

}
//_______________________________________________________________
Bool_t Monitoring::Terminate(){
    cout << "unused" << endl;
    // used fo saving plots before exiting
    return kTRUE;

}
//_______________________________________________________________
Bool_t Monitoring::DrawSE(){

    c1->cd();
    map->DrawCopy("COLZ");
    c1->Update();

    c2->cd();
    maptot->DrawCopy("COLZ");
    c2->Update();
  
    return kTRUE;
  
}
//_______________________________________________________________
Int_t Monitoring::Run(){

    if(!Init()) cout<<"Init procedure failure"<<endl;
    ProcessSingleEvent();
    DrawSE();

    return 0;

}
//_______________________________________________________________
Bool_t Monitoring::NextEvt(){
    fStop = kFALSE;
    if(!ProcessSingleEvent()) Terminate();
    DrawSE();

    return kTRUE;
}
//_______________________________________________________________
Bool_t Monitoring::End(){

    fStop = kFALSE;
    while(ProcessSingleEvent()){
        DrawSE();
    }
    if(!fStop)Terminate();

    return kTRUE;

}
//_______________________________________________________________
Bool_t Monitoring::EndSilent(){

    fStop = kFALSE;
    while(ProcessSingleEventFast());
    cout << "after processing events" << endl;
    FinishEventFast();
    cout << "after filling histos" << endl;
    DrawSE();
    cout << "after draw" << endl;
    Terminate();
    cout << "after terminate" << endl;

    return kTRUE;

}
//_______________________________________________________________
void Monitoring::Stop(){

    fStop = kTRUE;

}
//_______________________________________________________________
void Monitoring::DisplayInt(){
    // Slot method connected to the ValueSet(Long_t) signal.
    // It displays the value set in TGNumberEntry widget.
    fLabel->SetText(Form("%i",(Int_t)fNumber->GetNumberEntry()->GetIntNumber()));
    // Parent frame Layout() method will redraw the label showing the new value.
    fGframe->Layout();
}
void Monitoring::DisplayFloat(){
    fLabel2->SetText(Form("%f",(Float_t)fNumber2->GetNumberEntry()->GetNumber()));
    // Parent frame Layout() method will redraw the label showing the new value.
    fGframe2->Layout();
}

//_______________________________________________________________
void Monitoring::MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h) {

// Create a main frame
    fMain = new TGMainFrame(p,w,h);
// Create a horizontal frame widget with buttons
    TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain,200,200);
    TGTextButton *draw = new TGTextButton(hframe,"&Next Event");
    draw->Connect("Clicked()","Monitoring",this,"NextEvt()");
    hframe->AddFrame(draw, new TGLayoutHints(kLHintsCenterX,15,15,15,15));
    TGTextButton *draw2 = new TGTextButton(hframe,"Scan &All");
    draw2->Connect("Clicked()","Monitoring",this,"End()");
    hframe->AddFrame(draw2, new TGLayoutHints(kLHintsCenterX,15,2,15,15));
    TGTextButton *stop = new TGTextButton(hframe,"&Stop");
    stop->Connect("Clicked()","Monitoring",this,"Stop()");
    hframe->AddFrame(stop, new TGLayoutHints(kLHintsCenterX,2,15,15,15));
    TGTextButton *draw3 = new TGTextButton(hframe,"To the &End");
    draw3->Connect("Clicked()","Monitoring",this,"EndSilent()");
    hframe->AddFrame(draw3, new TGLayoutHints(kLHintsCenterX,15,15,15,15));
// TGTextButton *draw4 = new TGTextButton(hframe,"&Use Pedestal");
// draw4->Connect("Clicked()","Monitoring",this,"MenuPedestal()");
//hframe->AddFrame(draw4, new TGLayoutHints(kLHintsCenterX,15,15,15,15));
    TGTextButton *draw5 = new TGTextButton(hframe,"&Terminate");
    draw5->Connect("Clicked()","Monitoring",this,"Terminate()");
    hframe->AddFrame(draw5, new TGLayoutHints(kLHintsCenterX,15,15,15,15));
    TGTextButton *exit = new TGTextButton(hframe,"&Exit","gApplication->Terminate(0)");
    hframe->AddFrame(exit, new TGLayoutHints(kLHintsCenterX,35,15,15,15));
    fMain->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX,2,2,2,2));
// Set a name to the main frame
    fMain->SetWindowName("MENU");
// Map all subwindows of main frame
    fMain->MapSubwindows();
// Initialize the layout algorithm
    fMain->Resize(fMain->GetDefaultSize());
// Map main frame
    fMain->MapWindow();

}

