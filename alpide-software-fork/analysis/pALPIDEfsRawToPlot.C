/* Written by Miljenko Suljic, m.suljic@cern.ch */

#include <Riostream.h>
#include <TString.h>
#include <TCanvas.h>
#include <TH2F.h>

#include "classes/helpers.h"

Bool_t pALPIDEfsRawToPlot(TString filepath) {
    // read a raw palpide file and convert it to a plot

    ifstream rawfile(filepath.Data());
    if(!rawfile.good()) {
        cout << "File not found!" << endl;
        return kFALSE;
    }

    TH2F *h = new TH2F("hplot", "pALPIDEfs generic plot;Column;Row", 1024, -0.5, 1024.-0.5, 512, -0.5, 512.-0.5);
    h->SetStats(0);
  
    UShort_t dblcol, adr, col, row;
    UInt_t nhits;

    while(rawfile >> dblcol >> adr >> nhits && rawfile.good()) {
        dblcol_adr_to_col_row(dblcol, adr, col, row);
        cout << dblcol << " " << adr << " " << col << " " << row << " " << nhits << endl;
        h->Fill(col, row, nhits);
    }
/*
    // mask noisy pixels
    Float_t avgz = h->Integral()/h->GetNbinsX()/h->GetNbinsY();
    cout << avgz << endl;
    for(Int_t ix=1; ix <= h->GetNbinsX(); ++ix)
        for(Int_t iy=1; iy <= h->GetNbinsX(); ++iy)
            if( h->GetBinContent(ix, iy) > 5*avgz)
               h->SetBinContent(ix, iy, 0);
*/
    TCanvas *c1 = new TCanvas("c1", "Canvas 1", 0, 0, 1024, 512);
    c1->cd();
    h->DrawCopy("COLZ");

    delete h;
  
    cout << "Done!" << endl;
    return kTRUE;
}
