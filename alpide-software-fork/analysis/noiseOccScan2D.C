/* Written by Miljenko Suljic, m.suljic@cern.ch */

#include <Riostream.h>
#include <TString.h>
#include <TCanvas.h>
#include <TH2F.h>

#include "classes/helpers.h"

const int NEVTS = 1000;
//_______________________________________________________________________________________
Bool_t noiseOccScan2D(TString filepath) {
    // read a raw palpide file and convert it to a plot
    const Short_t dacc=256; // max dac counts

    ifstream rawfile(filepath.Data());
    if(!rawfile.good()) {
        cout << "File not found!" << endl;
        return kFALSE;
    }

    TH2F *h[4];
    for (Int_t isec=0; isec<4; ++isec) {
        h[isec] = new TH2F(Form("h_sec_%i", isec+1),
                           Form("Noise Occupancy as a function of VCASN and ITHR, Sector %i;VCASN [DAC];ITHR [DAC];Number of fake hits per event per pixel", isec+1),
                           dacc, -0.5, dacc-0.5, dacc, -0.5, dacc-0.5);
        h[isec]->SetStats(0);
    }
  
    UInt_t vcasn, ithr, nhits[4];

    while(rawfile >> vcasn >> ithr >> nhits[0] >> nhits[1] >> nhits[2] >> nhits[3]
          && rawfile.good()) {
      cout << "read " << vcasn << " " << ithr << " " << nhits[0] << " " << nhits[1] << " " << nhits[2] << " " << nhits[3] << endl;
        for (Int_t isec=0; isec<4; ++isec)
            h[isec]->Fill(vcasn, ithr, nhits[isec]);
    }
    rawfile.close();
    
    TCanvas *c1 = new TCanvas("c1", "Canvas 1", 0, 0, 1024, 512);
    c1->Divide(2,2);
    for (Int_t isec=0; isec<4; ++isec) {
        c1->cd(isec+1);
        c1->GetPad(isec+1)->SetRightMargin(0.15);
        c1->GetPad(isec+1)->SetLogz();
        h[isec]->Scale(1./NEVTS/512./256.);
        zoom_th2((TH2*)h[isec]->DrawCopy("COLZ"), 1);
        delete h[isec];
    }
  
    cout << "Done! (Warning: all values assuming 1000 triggers)" << endl;
    return kTRUE;
}
