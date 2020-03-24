/* Written by Miljenko Suljic, m.suljic@cern.ch */
#include "Riostream.h"
#include <TTree.h>
#include <TFile.h>
#include <TH2F.h>
#include <TMath.h>

#include "../classes/AliPALPIDEFSRawStreamMS.h"
#include "../classes/BinaryEvent.hpp"
#include "../classes/helpers.h"

#define MAX_CS 1000 // maximum pixels in an event

using namespace std;

// check if <pix1> and <pix2> are neighbours in the <crown> cro
//_______________________________________________________________________________________________
Bool_t IsNeighbour(Short_t crown, BinaryPixel pix1, BinaryPixel pix2) {
    Short_t dcol = pix1.GetCol() - pix2.GetCol();
    Short_t drow = pix1.GetRow() - pix2.GetRow();
    if(-crown <= dcol && dcol <= crown) {
        if(-crown <= drow && drow <= crown) { return kTRUE; }
        else { return kFALSE; }
    }
    else { return kFALSE; }
}

// check if pixel <pix> is border pixel. <scols> and <srows> is number of columns, rows in sector
//_______________________________________________________________________________________________
Bool_t IsBorderPixel(BinaryPixel pix, Short_t scols, Short_t srows) {
    if( !(pix.GetRow() % srows) || !((pix.GetRow()+1) % srows) )
        return kTRUE;
    else if( !(pix.GetCol() % scols) || !((pix.GetCol()+1) % scols) )
        return kTRUE;
    else
        return kFALSE;
}


//_______________________________________________________________________________________________
Bool_t csa(
    const TString filepath_raw,                      // path to RawHits_*_*.dat file
    const TString filepath_tree,                     // output tree path
    const TString filepath_mask="",                  // path to mask file
    const Short_t crown = 2,                         // consider neighbours all pixels in <crown> crown
    const Bool_t  flagRmSingleHotPixClusters = kTRUE // do not save clusters with just one pixels which is hot
    ) {

    cout << "csa() : Starting..." << endl;

    // pALPIDEfs constants
    const Short_t n_secs = 4;  // number of sectors
    const Short_t scols = 256; // number of columns in a sector
    const Short_t srows = 512; // number of rows in a sector

    // raw file reader
    AliPALPIDEFSRawStreamMS *palpidefsRaw = new AliPALPIDEFSRawStreamMS();
    if( !palpidefsRaw->SetInputFile(filepath_raw.Data()) ) {
        cerr << "csa() : Cannot open raw file!" << endl
             << "\tFile path: " << filepath_raw.Data() << endl;
        return kFALSE;
    }

    // Event tree variables
    TFile* file_tree = new TFile(filepath_tree.Data(), "RECREATE");
    BinaryEvent* event = new BinaryEvent();
    TTree* tree = new TTree("event_tree", "event_tree"); 
    tree->Branch("event", "BinaryEvent", &event);
    
    // CSA variables
    BinaryPlane* plane[n_secs];
    for(Short_t i=0; i<n_secs; ++i) plane[i] = new BinaryPlane(); 
    BinaryCluster* cluster = new BinaryCluster();
    vector<BinaryPixel> pix_vec;
    BinaryPixel*  pix_arr = new BinaryPixel[MAX_CS];
    BinaryPixel   pix_tmp;

    // TODO: make the code nicer
    // hot pixels variables
    Bool_t flagHot = filepath_mask.EqualTo("") ? kFALSE : kTRUE;
    Bool_t hot_map[n_secs*scols][srows]; 
    if(flagHot) {
        for(Int_t i=0; i<n_secs*scols; ++i) // initialise hotmap
            for(Int_t j=0; j<srows; ++j)
                hot_map[i][j] = kFALSE;
        ifstream file_mask(filepath_mask.Data()); // read hot pixels from mask file
        if(!file_mask.is_open()) {
            cout << "csa() : ERROR: Unable to open mask file: "
                 << filepath_mask.Data() << endl;
            return kFALSE;
        }
        UShort_t reg, dblcol, addr, col, row, cnt=0;
        file_mask >> reg >> dblcol >> addr;
        while(file_mask.good()) {
            dblcol += reg*16;
            dblcol_adr_to_col_row(dblcol, addr, col, row);
            hot_map[col][row] = kTRUE;
            ++cnt;
            file_mask >> reg >> dblcol >> addr;
        }
        file_mask.close();
        cout << "csa() : Successfully loaded " << cnt << " hot pixels from mask file!" << endl;
    }
        
    //-------------------------------------------
    cout << "csa() : Started reading RawHits file." << endl;
    Long_t evts = 0;
    while(palpidefsRaw->ReadEvent()) {
        if( (evts+1)%10000 == 0 )
            cout << "csa() : Processed events: " << evts+1 << endl;
        event->Reset();
        event->SetEventID(evts);
        event->SetIntTrigCnt(palpidefsRaw->GetEventCounter());
        for(Short_t i=0; i<n_secs; ++i) {
            plane[i]->Reset();
            plane[i]->SetPlaneID(i);
        }

        Int_t   nhits = palpidefsRaw->GetNumHits();
        Int_t   j=0;
        Short_t col, row;
        Bool_t  flagPixAdded = kTRUE;

        // read which pixels are hit and re-construct first cluster
        while(palpidefsRaw->GetNextHit(&col, &row)) {
            pix_tmp.Reset();
            pix_tmp.Set(col, row);
            // check if there are conditions that require setting a flag for this pixel
            if(flagHot) if(hot_map[col][row])        pix_tmp.SetFlag(0, kTRUE);
            if(IsBorderPixel(pix_tmp, scols, srows)) pix_tmp.SetFlag(1, kTRUE);
            // 1st cluster reconstruction
            if(j==0) {
                pix_arr[j] = pix_tmp;
                ++j; --nhits;
            }
            else {
                Bool_t flagNeigh = kFALSE;
                for(Int_t k=0; k<j && !flagNeigh; ++k) {
                    if(IsNeighbour(crown, pix_tmp, pix_arr[k])) {
                        pix_arr[j] = pix_tmp;
                        ++j; --nhits;
                        flagNeigh = kTRUE;
                    }
                }
                if(!flagNeigh) pix_vec.push_back(pix_tmp);
            }
        }
        // some pixels excluded from cluster at the beginning of previous loop
        // might be neighbours with pixels included at the end
        flagPixAdded = kTRUE;
        while(flagPixAdded && nhits) {
            flagPixAdded = kFALSE;
            for(Int_t i=0; i < nhits; ++i) {
                for(Int_t k=0; k<j; ++k) {
                    if(IsNeighbour(crown, pix_vec[i], pix_arr[k])) {
                        pix_arr[j] = pix_vec[i];
                        pix_vec.erase(pix_vec.begin()+i);
                        ++j; --nhits; --i;
                        flagPixAdded = kTRUE;
                        break;
                    }
                }
            }
        }
        cluster->SetPixelArray(j, pix_arr);
        if(flagHot && flagRmSingleHotPixClusters
           && cluster->GetNPixels()==1 && cluster->HasHotPixels() )
            cluster->Reset();
        else
            plane[TMath::FloorNint(cluster->GetX() / scols)]->AddCluster(cluster);
        
        // reconstruct other clusters
        while(nhits) {
            pix_arr[0] = pix_vec[0];
            pix_vec.erase(pix_vec.begin());
            j=1; --nhits;
            for(Int_t i=0; i < nhits; ++i) {
                for(Int_t k=0; k<j; ++k) {
                    if(IsNeighbour(crown, pix_vec[i], pix_arr[k])) {
                        pix_arr[j] = pix_vec[i];
                        pix_vec.erase(pix_vec.begin()+i);
                        ++j; --nhits; --i;
                    }
                }
            }
            // some pixels excluded from cluster at the beginning of previous loop
            // might be neighbours with pixels included at the end
            flagPixAdded = kTRUE;
            while(flagPixAdded && nhits) {
                flagPixAdded = kFALSE;
                for(Int_t i=0; i < nhits; ++i) {
                    for(Int_t k=0; k<j; ++k) {
                        if(IsNeighbour(crown, pix_vec[i], pix_arr[k])) {
                            pix_arr[j] = pix_vec[i];
                            pix_vec.erase(pix_vec.begin()+i);
                            ++j; --nhits; --i;
                            flagPixAdded = kTRUE;
                            break;
                        }
                    }
                }
            }
            cluster->SetPixelArray(j, pix_arr);
            if(flagHot && flagRmSingleHotPixClusters
               && cluster->GetNPixels()==1 && cluster->HasHotPixels() )
                cluster->Reset();
            else
                plane[TMath::FloorNint(cluster->GetX() / scols)]->AddCluster(cluster);
        }

        // fill the tree (if at least one plane is hit)
        Bool_t flagFill = kFALSE;
        for(Short_t i=0; i<n_secs; ++i) {
            event->AddPlane(plane[i]);
            if(plane[i]->GetNClustersSaved()) flagFill = kTRUE;
        }
        if(flagFill) tree->Fill();
        ++evts;    
    } // End while ReadEvent()
    cout << "csa() : Finished reading RawHits file." << endl;

//    tree->Print();

    file_tree->Write();
    tree->ResetBranchAddresses();
    file_tree->Close();

    delete file_tree;
    delete palpidefsRaw;
    delete event;
    for(Short_t i=0; i<n_secs; ++i) delete plane[i];
    delete cluster;
    delete[] pix_arr;

    cout << "csa() : Done!" << endl;
    return kTRUE;
}
