#include "Riostream.h"
#include "TFile.h"
#include "TChain.h"
#include "TH2F.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TString.h"

#include "../classes/BinaryEvent.hpp"

TH2F* draw_cluster(BinaryCluster* cluster, Short_t cols, Short_t rows) {
    TH2F* h = NULL;
    BinaryPixel* pixels = cluster->GetPixelArray();
    Short_t max_x=0, max_y=0, min_x=cols, min_y=rows, dx, dy;
    for(Int_t ipix=0; ipix < cluster->GetMultiplicity(); ++ipix) {
        Short_t pix_x = pixels[ipix].GetCol();
        Short_t pix_y = pixels[ipix].GetRow();
        if( pix_x > max_x) max_x = pix_x;
        if( pix_y > max_y) max_y = pix_y;
        if( pix_x < min_x) min_x = pix_x;
        if( pix_y < min_y) min_y = pix_y;
    }
    if( (max_x-min_x)%2 ) dx = max_x-min_x+1;
    else                  dx = max_x-min_x;
    if( (max_y-min_y)%2 ) dy = max_y-min_y+1;
    else                  dy = max_y-min_y;
    Float_t x = TMath::Ceil(cluster->GetX());
    Float_t y = TMath::Ceil(cluster->GetY());
    h = new TH2F("hclust", "Cluster form;Colum;Row",
                 dx+6, x-dx/2.-3.5, x+dx/2.+2.5,
                 dy+6, y-dy/2.-3.5, y+dy/2.+2.5);
    for(Int_t ipix=0; ipix < cluster->GetMultiplicity(); ++ipix)
        h->Fill(pixels[ipix].GetCol(), pixels[ipix].GetRow());
    return h;
}


Bool_t interesting_events(
    const TString filepath_tree, // path to event tree file
    const TString filepath_ie,   // output path
    const Short_t min_prox = 10, // minimum distance to consider cluster fully separate
    const Short_t max_mult = 500 // max drawn cluster multiplicity
    ) {

    cout << "interesting_events() : Starting..." << endl;
    cout << " This can take a while depending on interesting criteria and number of events." << endl;

    const Short_t scols    = 256;
    const Short_t srows    = 512;
    const Short_t n_secs   = 4;   // number of sectors

    TChain *chain = new TChain("event_tree", "event_tree");
    if(!chain->Add(filepath_tree.Data())) {
        cerr << "interesting_events() : ERROR: Cannot find event tree in file! " << filepath_tree.Data() << endl;
        return kFALSE;
    }
    BinaryEvent* event = new BinaryEvent();
    chain->SetBranchAddress("event", &event);
    Long_t nentries = chain->GetEntries();
    Long_t cnt=0;

    TFile* file_ie = new TFile(filepath_ie.Data(), "RECREATE");
    if(!file_ie->IsOpen()) {
        cerr << "interesting_events() : ERROR: Cannot open interesting events file! " << filepath_ie.Data() << endl;
        return kFALSE;
    }
    // interesting reasons
    const Short_t n_reasons = 2;
    const TString reasons[n_reasons] = {"Empty core",
                                        Form("Proximity_%i", min_prox)};
    for(Short_t r=0; r<n_reasons; ++r) {
        file_ie->mkdir(reasons[r]);
        for(Short_t isec=0; isec<n_secs; ++isec) file_ie->mkdir(Form("%s/Sector %i", reasons[r].Data(), isec));
    }

    for(Long_t ientry=0; ientry < nentries; ++ientry) {
        chain->GetEntry(ientry);
        if( (ientry+1)%10000 == 0 )
            cout << "Processed events: " << ientry+1 << " / " << nentries << endl;
        for(Short_t isec=0; isec < n_secs; ++isec) {
            Int_t nclusters = event->GetPlane(isec)->GetNClustersSaved();
            
            // nearby clusters - proximity
            if(nclusters > 1) {
                for(Short_t i=0; i<nclusters; ++i) {
                    for(Short_t j=i+1; j<nclusters; ++j) {
                        Short_t cmx[2] = {event->GetPlane(isec)->GetCluster(i)->GetX(),
                                          event->GetPlane(isec)->GetCluster(j)->GetX()};
                        Short_t cmy[2] = {event->GetPlane(isec)->GetCluster(i)->GetY(),
                                          event->GetPlane(isec)->GetCluster(j)->GetY()};
                        if( TMath::Abs(cmx[0]-cmx[1]) < min_prox && TMath::Abs(cmy[0]-cmy[1]) < min_prox ) {
                            ++cnt;
                            file_ie->cd(Form("%s/Sector %i", reasons[1].Data(), isec));
                            TH2F* hClu1 = draw_cluster(event->GetPlane(isec)->GetCluster(i), scols*n_secs, srows);
                            hClu1->SetName(Form("ev_%li_sec_%i_clu_%i_prox_%i", ientry, isec, i, j));
                            file_ie->Write();
                            delete hClu1;
                            TH2F* hClu2 = draw_cluster(event->GetPlane(isec)->GetCluster(j), scols*n_secs, srows);
                            hClu2->SetName(Form("ev_%li_sec_%i_clu_%i_prox_%i", ientry, isec, j, i));
                            file_ie->Write();
                            delete hClu2;
                        }
                    }
                }
            }
                            
            for(Int_t iclu=0; iclu < nclusters; ++iclu) {
                Int_t mult = event->GetPlane(isec)->GetCluster(iclu)->GetMultiplicity();
                {   // empty core clusters
                    BinaryPixel* pixels = event->GetPlane(isec)->GetCluster(iclu)->GetPixelArray(); 
                    Short_t cmx[2] = {TMath::FloorNint(event->GetPlane(isec)->GetCluster(iclu)->GetX()),
                                      TMath::CeilNint(event->GetPlane(isec)->GetCluster(iclu)->GetX())};
                    Short_t cmy[2] = {TMath::FloorNint(event->GetPlane(isec)->GetCluster(iclu)->GetY()),
                                      TMath::CeilNint(event->GetPlane(isec)->GetCluster(iclu)->GetY())};
                    Bool_t flagCore = kFALSE;
                    for(Int_t ipix=0; ipix<mult && !flagCore; ++ipix)
                        for(Short_t i=0; i<2 && !flagCore; ++i)
                            for(Short_t j=0; j<2 && !flagCore; ++j)
                            flagCore = (pixels[ipix].GetCol() == cmx[i] &&
                                        pixels[ipix].GetRow() == cmy[j]);
                    if(!flagCore) {// draw
                        ++cnt;
                        file_ie->cd(Form("%s/Sector %i", reasons[0].Data(), isec));
                        TH2F* hClu = draw_cluster(event->GetPlane(isec)->GetCluster(iclu), scols*n_secs, srows);
                        hClu->SetName(Form("ev_%li_sec_%i_clu_%i_ec", ientry, isec, iclu));
                        file_ie->Write();
                        delete hClu;
                    }
                }
            }
        } // END FOR sectors
    } // END FOR entries
    
    cout << "Found " << cnt << " interesting events." << endl;
    
    file_ie->Close();
    
    cout << "interesting_events() : Done!" << endl;
    return kTRUE;

}
