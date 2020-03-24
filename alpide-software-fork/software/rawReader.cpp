#include "TConfig.h"
#include "TPalpidefs.h"
#include "TDaqboard.h"

#include "TH2F.h"

#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>


using std::cout;
using std::cerr;
using std::endl;
using std::vector;

int main(int argc, const char *argv[] ) {
    // RawHits file
    bool WriteHitsToFile = false;
    char  fNameRawHits[500];
    sprintf(fNameRawHits, "Data/RawHits_tmp.dat");
    FILE *fpRawHits = 0;
    if (WriteHitsToFile) {
        fpRawHits = fopen (fNameRawHits, "w");
        if (fpRawHits==0x0) { 
            cerr << "raw hit file could not be opened for writing, please check!!" << endl;
            return -1;    
        }
    }
    int Column;
    int Row;       
    // *NHits = 0;

    if (argc!=2) {
        cerr << "number of input arguments not correct; usage: ./rawReader <path_to_rawfile>" << endl;
        return -1;    
    }

    FILE* fp;
    fp = fopen(argv[1], "rb");
    if (fp==0x0) { 
        cerr << "file could not be opened, please check!!" << endl;
        return -1;    
    }
    cout << "Reading file: " <<  argv[1] << endl;


    TConfig * conf = new TConfig(TYPE_CHIP, 1);
    TpAlpidefs3* chip = new TpAlpidefs3((TTestSetup*)0x0, 0, conf->GetChipConfig(), true);
    TDAQBoard2* board = new TDAQBoard2(0x0, conf->GetBoardConfig());

    //TH2F* hHitmap = new TH2F("hitmap", "x;y;hits (a.u.)", 1024, 0., 1024, 512, 0., 512);

    // determine firmware version for header and trailer length
    uint32_t firmwareVersion=0;
    fread(&firmwareVersion, sizeof(uint32_t), 1, fp);
    board->SetFirmwareVersion(firmwareVersion);
    const int headerLength = board->GetEventHeaderLength();
    const int trailerLength = board->GetEventTrailerLength();

    unsigned long nEvts = 0;
    int NHits;
    std::vector<TPixHit> Hits;

    int length     = 0;
    int length_tmp = 0;
    int length_sum = 0;
    unsigned char data_buf[4000*1024];
    TEventHeader header;
    bool HeaderOK;
    bool EventOK;
    bool TrailerOK;
    unsigned int StrobeCounter = (unsigned int)-1;
    unsigned int BunchCounter = (unsigned int)-1;
    std::string str_tmp;
    vector <int> length_evts;

    // read all events
    //while(!feof(fp)) {
    while( (!feof(fp)) && (nEvts<1000) ) {
        if (fread(&length, sizeof(int), 1, fp)!=1) {
            if (!feof(fp)) cout << "Could not read length of the next event! Current event: " << nEvts << endl;
            //ERROR;
            break;
        }
        // check length > 4000*1024!
        if (fread(&data_buf, sizeof(unsigned char), length, fp)!=length) {
            cout << "Could not read the next event! Current event: " << nEvts << endl;
            //ERROR;
            break;
        }

        // -->> Decode event
        // check for several events in one packet (when running in event based mode)
        TrailerOK = board->DecodeEventTrailer(data_buf + length - trailerLength, &header);
        if ( TrailerOK && (header.EventSize*4)!=length ) { // eventsize in terms of 32 bit words, length in terms of bytes.?
            cout << "--- WARNING: eventsize does not match length ---" << endl;
            cout << "... checking whether can identify events" << endl;

            //str_tmp = "RAW dump Evt: \n";
            //for (int j=0; j<length; j++) {
            //    char buffer[20];
            //    sprintf(buffer, "%02x ", data_buf[j]);
            //    str_tmp += buffer;
            //    if (j%20==0 && j!=0) {
            //        str_tmp += "\n";
            //    }
            //}
            //str_tmp += "\n";
            //cout << str_tmp.data() << endl;

            // find number of events
            length_evts.clear();
            length_sum = 0;
            length_evts.push_back(header.EventSize*4);
            length_sum += (header.EventSize*4);
            //cout << header.EventSize*4 << endl;

            while( length!=length_sum ) {
                //cout << "length: " << length << endl;
                TrailerOK = board->DecodeEventTrailer(data_buf + length - trailerLength - length_sum, &header);
                if (TrailerOK) {
                    length_evts.push_back(header.EventSize*4);
                    length_sum += header.EventSize*4;
                    cout << header.EventSize*4 << endl;
                    //cout << "length sum: " << length_sum << endl;
                } 
                else {
                    cerr << "something wrong with trailer while identifying events" << endl;
                    return kFALSE;
                }
            }

            cout << "-->> " << length_evts.size() << " events found" << endl; 
            cout << "... reading them" << endl;

            length_sum = 0;
            for (int i=length_evts.size()-1; i>=0; i--) {
                //cout << i << endl; 
                //cout << length_evts[i] << endl;

                Hits.clear();
                HeaderOK  = board->DecodeEventHeader(data_buf+length_sum, &header);
                //cout << "lenght_sum: " << length_sum << endl;
                EventOK   = chip->DecodeEvent(data_buf+headerLength + length_sum, length_evts[i]-trailerLength-headerLength, &Hits, &StrobeCounter, &BunchCounter);
                TrailerOK = board->DecodeEventTrailer(data_buf + length_sum+length_evts[i] - trailerLength, &header);

                if ( !HeaderOK || !EventOK || !TrailerOK ) {
                    cerr << "something wrong with event!!" << endl;
                    continue;
                    //return -1;
                }
                else { 
                    cout << " --> event " << length_evts.size()-1-i << " fine" << endl;
                }

                NHits=Hits.size();
                cout << "event: " << nEvts << ", " << NHits << " hits found" << endl;

                if (WriteHitsToFile) {
                    for (int i=0; i<NHits; i++) {
                        Column = chip->AddressToColumn (Hits[i].region, Hits[i].doublecol, Hits[i].address);
                        Row    = chip->AddressToRow    (Hits[i].region, Hits[i].doublecol, Hits[i].address);
                        if (fpRawHits) fprintf(fpRawHits, "%lu %d %d\n", nEvts, Column, Row);
                    }
                }
                ++nEvts;
                length_sum += length_evts[i]; 

            }
            cout << "--- ---------------------------------------- ---" << endl;

            
            //return -1;
            continue;
        }
    
        Hits.clear();
        HeaderOK  = board->DecodeEventHeader(data_buf, &header);
        EventOK   = chip->DecodeEvent(data_buf+headerLength, length-trailerLength-headerLength, &Hits); // &StrobeCounter, &BunchCounter);
        
        if ( !HeaderOK || !EventOK || !TrailerOK ) {
            cerr << "something wrong with event!!" << endl;
            return -1;
        }

        NHits=Hits.size();
        cout << "event: " << nEvts << ", " << NHits << " hits found" << endl;
        

        if (WriteHitsToFile) {
            for (int i=0; i<NHits; i++) {
                Column = chip->AddressToColumn (Hits[i].region, Hits[i].doublecol, Hits[i].address);
                Row    = chip->AddressToRow    (Hits[i].region, Hits[i].doublecol, Hits[i].address);
                if (fpRawHits) fprintf(fpRawHits, "%lu %d %d\n", nEvts, Column, Row);
            }
        }
        ++nEvts;
    }
    fclose(fp);
    cout << "Number of events: " << nEvts << endl;


    delete chip;
    chip = 0x0;
    delete board;
    board = 0x0;
    delete conf;
    conf = 0x0;

    return 0;
}
