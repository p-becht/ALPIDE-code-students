#include "TConfig.h"
#include "TPalpidefs.h"
#include "TDaqboard.h"

#include "TH2F.h"

#include <iostream>
#include <iomanip>
#include <vector>

#include <stdio.h>
#include <stdlib.h>


using std::cout;
using std::cerr;
using std::endl;
using std::vector;

//--------------------------------------------------------------------------------------------------
void dump_event(std::vector<unsigned char>& evt) {
  cerr << "Length: " << evt.size() << "Byte" << endl;
  cerr << "0x\t";
  for (int iByte=0; iByte < evt.size(); ++iByte) {
    cerr << std::setw(2) << std::setfill('0') << std::hex << (int)evt[iByte] << std::dec;
    cerr << ((iByte+1==evt.size()) ? "\n" : (iByte%4==3) ? "\n0x\t" : " ");
  }
  cerr << std::setfill(' ');
}


//--------------------------------------------------------------------------------------------------
int main(int argc, const char *argv[] ) {
  if (argc!=2) {
    cerr << "number of input arguments not correct; usage: ./debugReader <path_to_rawfile>" << endl;
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

  // determine firmware version for header and trailer length
  uint32_t firmwareVersion = 0x257E0611;
  board->SetFirmwareVersion(firmwareVersion);
  const int headerLength = board->GetEventHeaderLength();
  const int trailerLength = board->GetEventTrailerLength();

  unsigned long nEvts = 0;
  int NHits;
  std::vector<TPixHit> Hits;


  std::vector<unsigned char> event;
  std::vector<unsigned char> last_event;
  bool dump_next_event = false;
  uint32_t word = 0x0;
  TEventHeader header;
  bool HeaderOK;
  bool EventOK;
  bool TrailerOK;
  unsigned int StrobeCounter = (unsigned int)-1;
  unsigned int BunchCounter = (unsigned int)-1;
  std::string str_tmp;
  vector <int> length_evts;

  // read all events
  while(!feof(fp)) {
    do {
      if (fread(&word, sizeof(unsigned char), 4, fp)!= 4) {
        cout << "Could not read the next event! Current event: " << nEvts << endl;
        //ERROR;
        break;
      }
      for (int iByte=0; iByte<4; ++iByte) {
        event.push_back((word>>iByte*8)& 0xFF);
      }
    } while (word!=0xBFBFBFBF);

    if (dump_next_event) {
      if (event.size()>0) {
        cerr << "** Next event (raw data): " << endl;
        dump_event(event);
        cerr << endl;
      }
      dump_next_event = false;
    }

    TrailerOK = board->DecodeEventTrailer(&event[0] + event.size() - trailerLength, &header);
    if ( (TrailerOK && (header.EventSize*4)!=event.size()) || !TrailerOK) { // eventsize in terms of 32 bit words, length in terms of bytes.
      dump_next_event = true;
      if   (TrailerOK) cerr << "*** Event size does not match the trailer information:" << endl;
      else             cerr << "*** Failed to decode event trailer:" << endl;
      if (last_event.size() > 0) {
        cerr << "** Previous event (raw data): " << endl;
        dump_event(last_event);
        cerr << endl;
      }
      if (event.size()>0) {
        cerr << "** Current (broken) event (raw data): " << endl;
        dump_event(event);
        cerr << endl;
      }
    }
    else {
      HeaderOK  = board->DecodeEventHeader(&event[0], &header);
      EventOK   = chip->DecodeEvent(&event[0]+headerLength, event.size()-headerLength-trailerLength, &Hits, &StrobeCounter, &BunchCounter);
      if ( !HeaderOK || !EventOK || !TrailerOK ) {
        cerr << "something wrong with event!!" << endl;
        dump_event(event);
      }
      NHits=Hits.size();
      //cout << "event: " << nEvts << ", " << NHits << " hits found" << endl;
      Hits.clear();
    }
    ++nEvts;
    last_event.clear();
    last_event = event;
    event.clear();
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
