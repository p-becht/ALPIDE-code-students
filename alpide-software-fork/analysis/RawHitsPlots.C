/* Written by Miljenko Suljic, m.suljic@cern.ch */

#include <Riostream.h>
#include <TString.h>
#include <TCanvas.h>
#include <TH2F.h>
#include <TStyle.h>

#include "classes/helpers.h"

Bool_t RawHitsPlots(TString filepath) {
    // read a raw palpide file and convert it to a plot

    gStyle->SetOptFit(11);

    ifstream rawfile(filepath.Data());
    if(!rawfile.good()) {
        cout << "File not found!" << endl;
        return kFALSE;
    }

    TH2F *h = new TH2F("hplot", "pALPIDEfs generic plot;Column;Row", 256, -0.5, 1024.-0.5, 128, -0.5, 512.-0.5);
    TH1F *hf = new TH1F("hfreq", "Number of hits vs event number;Event;N. hits", 50000, -0.5, 49999.5);
    h->SetStats(0);
  
    UShort_t evt, col, row;

    while(rawfile >> evt >> col >> row && rawfile.good()) {
        //    cout << dblcol << " " << adr << " " << col << " " << row << " " << nhits << endl;
        h->Fill(col, row);
        hf->Fill(evt);
    }
  
    TCanvas *c1 = new TCanvas("cRawHits", "Canvas RawHits", 0, 0, 1024, 768);
    c1->Divide(1,2);
    c1->cd(1);
    h->DrawCopy("COLZ");
    c1->cd(2);
    hf->DrawCopy()->Fit("pol1");

    delete h;
    delete hf;
  
    cout << "Done!" << endl;
    return kTRUE;
}
