//
//  TestSystem.cpp
//  pALPIDEfs-software
//
//  Created by Carlo Puggioni and Markus Keil on 18/03/14.
//  Copyright (c) 2014 Carlo Puggioni, Markus Keil. All rights reserved.
//

#include "TTestsetup.h"
#include "stdio.h"
#include "chiptests.h"
#include <unistd.h>
#include <cstdlib>


bool StopADCData;

////////////////////////////////////////////////////////////////////////
//                                                                    //
//                    class TTestSetup                                //
//                                                                    //
////////////////////////////////////////////////////////////////////////



TTestSetup::TTestSetup () {
    InitLibUsb();
}


TTestSetup::~TTestSetup() {
    for (int i = 0; i < fNDAQBoards; i++) {
        delete fDAQBoards.at(i);
    }
    //libusb_exit(fContext);
}


int TTestSetup::InitLibUsb() {
    int err = libusb_init(&fContext);
    if (err) {
        std::cout << "Error " << err << " while trying to init libusb " << std::endl;
    }
    return err;
}


bool TTestSetup::IsDAQBoard(libusb_device *ADevice) {

    libusb_device_descriptor desc;
    libusb_get_device_descriptor(ADevice, &desc);

//    std::cout << std::hex << "Vendor id " << (int)desc.idVendor << ", Product id " << (int)desc.idProduct << std::dec << std::endl;

    if ((desc.idVendor == DAQ_BOARD_VENDOR_ID) && (desc.idProduct == DAQ_BOARD_PRODUCT_ID)) {
      //std::cout << "Serial number " << (int)desc.iSerialNumber << std::endl;
        return true;
    }

    return false;
}


int TTestSetup::AddDAQBoard (libusb_device *ADevice, TDAQBoardConfig *AConfig) {
    TDAQBoard *db;
    if (AConfig->BoardType == 1) {
        db = new TDAQBoard(ADevice, AConfig);
    }
    else if (AConfig->BoardType == 2) {
        db = new TDAQBoard2(ADevice, AConfig);
    }
    else {
      std::cout << "Unknown DAQ board type " << AConfig->BoardType << std::endl;
      return -1;
    }
    //std::cout << "created DAQ board: " << std::endl;
    //std::cout << "  Geographical address: " << db->GetBoardAddress() << std::endl;
    //std::cout << "  Firmware version:     " << std::hex << db->GetFirmwareVersion() << std::dec << std::endl;
    if (db) {
        fDAQBoards.push_back(db);
        fNDAQBoards ++;
        return 0;
    }
    else {
        return -1;
    }
}


// method for telescope setup (NDAQBoards > 1)
int TTestSetup::AddDAQBoard (libusb_device *ADevice, TConfig *AConfig) {
  TDAQBoard *dbtemp;
  if (AConfig->GetNBoards() == 1) return AddDAQBoard (ADevice, AConfig->GetBoardConfig(0));

  // if > 1 DAQ boards: 
  //  - create temporary DAQ board of type 1
  //  - read geographical address
  //  - get config for board with this address and construct correct board type  
  dbtemp = new TDAQBoard (ADevice, AConfig->GetBoardConfig(0));
  int GeoAdd = dbtemp->GetBoardAddress();
  int index = AConfig->GetIndexForGeoAdd (GeoAdd);  
  delete dbtemp;

  if (index == -1) {
    std::cout << "Warning, DAQ board with jumper address " << GeoAdd << " not found in config. Ignored." << std::endl;
    return 0;    
  }
  return AddDAQBoard (ADevice, AConfig->GetBoardConfig(index));
}


int TTestSetup::FindDAQBoards (TConfig *AConfig) {
    fNDAQBoards = 0;
    int err     = 0;
    libusb_device **list;

    ssize_t cnt = libusb_get_device_list(fContext, &list);

    if (cnt < 0) {
        std::cout << "Error getting device list" << std::endl;
        return -1;
    }

    for (ssize_t i = 0; i < cnt; i++) {
        libusb_device *device = list[i];
        if (IsDAQBoard(device)) {
    	    err = AddDAQBoard(device, AConfig);
            if (err) {
                std::cout << "Problem adding DAQ board" << std::endl;
                libusb_free_device_list(list, 1);
                return err;
            }
        }
    }
    libusb_free_device_list(list, 1);
    return err;
}


// Preliminary method ... in the end the dut type will come from the configuration, for the time being
// create a pAlpide for every daq board.

int TTestSetup::AddDUTs (TConfig *AConfig) {
    TDUT *dut;
    int  index; //chip configuration index
    fNDUTs = 0;
    for (int i = 0; i < fNDAQBoards; i++) {
      // index: treat chip and telescope here; module is handled by seperate class TModuleSetup
        if (AConfig->GetType() == TYPE_TELESCOPE) {   
          index = AConfig->GetIndexForGeoAdd (GetDAQBoard(i)->GetBoardAddress());  
        } 
        else {
          index = 0;
        }
        switch (AConfig->GetChipConfig(index)->ChipType) {
   	case DUT_PALPIDEFS:
 	    std::cout << "WARNING: DUT type PALPIDEFS is deprecated, constructing PALPIDE1." << std::endl;
            dut = new TpAlpidefs1(this, i, AConfig->GetChipConfig(index));
            break;
        case DUT_PALPIDEFS1:
	    dut = new TpAlpidefs1(this, i, AConfig->GetChipConfig(index));
            break;
        case DUT_PALPIDEFS2:
  	    dut = new TpAlpidefs2(this, i, AConfig->GetChipConfig(index));
            break;
        case DUT_PALPIDEFS3:
	    dut = new TpAlpidefs3(this, i, AConfig->GetChipConfig(index));
            break;
	case DUT_PALPIDEFS4:
	    dut = new TpAlpidefs4(this, i, AConfig->GetChipConfig(index));
            break;    
        case DUT_UNKNOWN:
	  std::cout << "WARNING, DUT type "<< AConfig->GetChipConfig(index)->ChipType<< " is UNKNOWN, doing nothing." << std::endl;
            break;
        }
        if (dut) {
            fDUTs.push_back(dut);
            fNDUTs ++;
        }
    }
    GetSetupSummary();
    return fNDUTs;
}


TDAQBoard *TTestSetup::GetDAQBoard  (int i) {
  if (i >= fNDAQBoards) {
    std::cout << "ERROR: Trying to access non existing DAQ board " << i << ". Exiting ... " << std::endl;
    exit (EXIT_FAILURE);
  }
  return fDAQBoards.at(i);
}


TDUT *TTestSetup::GetDUT (int ABoardID, int AChipID) {
  int index;
  if (AChipID == -1) {
    index = ABoardID;
  }
  else {
    index = AChipID;
  }
  if (index >= fNDUTs) {
    std::cout << "ERROR: Trying to access non existing DUT. Exiting ... " << std::endl;
    exit (EXIT_FAILURE);
  }
  return fDUTs.at(index);
}


int TTestSetup::GetBoardIndexByAddress (int AAddress) {
    for (int i = 0; i < GetNDAQBoards(); i++) {
      if (GetDAQBoard(i)->GetBoardAddress() == AAddress) return i;
    }
    return -1;
}


void TTestSetup::GetPlaneSummary (int index) {
  TDUT      *dut = GetDUT(index);
  TDAQBoard *db  = GetDAQBoard(index);
  int GeoAdd     = db->GetBoardAddress();
  int fw         = db->GetFirmwareVersion();

  if ((dut->GetChipType() == DUT_PALPIDEFS2) || (dut->GetChipType() == DUT_PALPIDEFS3) || (dut->GetChipType() == DUT_PALPIDEFS4)){
    int ChipId = ((TpAlpidefs2 *)dut)->GetChipId();
    std::cout << " - Plane " << index << ": " << db->GetClassName() << " ( GeoAdd: " << GeoAdd << ", firmware Version: 0x" << std::hex << fw <<std::dec << " ) with " << dut->GetClassName() << " ( Chip ID: " << ChipId << " ) " << std::endl;
  }
  else {
    std::cout << " - Plane " << index << ": " << db->GetClassName() << " ( GeoAdd: " << GeoAdd << ", firmware Version: 0x" << std::hex << fw << std::dec << " ) with " << dut->GetClassName() << std::endl;
  }
}


void TTestSetup::GetSetupSummary() {
  std::cout << std::endl << "Created Setup with " << GetNDAQBoards() << " DAQ Board(s):" << std::endl;
  for (int i = 0; i < GetNDAQBoards(); i++) {
    GetPlaneSummary(i);
  }
  std::cout << std::endl;
}


void TTestSetup::DoBulkloopTest (int DAQBoardID) {

    int           data_byte=0;
 	int           num_byte_send, num_byte_rcv;
	unsigned char data_in[1024];
    unsigned char data_out[1024];

    for(int k=0; k < 1024; k++){
        data_out[k] =(unsigned char)data_byte;
        data_byte++;
    }


    num_byte_send = fDAQBoards.at(DAQBoardID)->SendData    (0,data_out,1024);//(0,data_out,1);
    if (num_byte_send >0 ) {
        num_byte_rcv  = fDAQBoards.at(DAQBoardID)->ReceiveData (1,data_in,1024);//(1,data_in,1);

        printf("\nData Sent: %d bytes \n\n", num_byte_send);
        for(int k=0; k < num_byte_send; k++) {printf("%d\t",data_out[k]);}
        printf("\n\nData Received: %d bytes\n\n", num_byte_rcv);
        for(int k=0; k < num_byte_rcv; k++)  {printf("%d\t",data_in[k]);}
        printf ("\n");
    }

}


bool TTestSetup::PowerOnBoard(int ADAQBoardID, int &AOverflow)
{
  // Functionality has been moved to TDAQBoard::PowerOn(), this method has been kept for backward-compatibility
  std::cout << "TTestSetup::PowerOnBoard: Trying to power on DAQ board " << ADAQBoardID << std::endl;
    TDAQBoard *db = GetDAQBoard(ADAQBoardID);
    return db->PowerOn(AOverflow, (GetDUT(ADAQBoardID)->GetConfig()->ChipType == DUT_PALPIDEFS4) ? true : false);
}


void TTestSetup::PowerOffBoard (int ADAQBoardID) {
    if ((GetDUT(ADAQBoardID)->GetConfig()->ChipType == DUT_PALPIDEFS2) || 
        (GetDUT(ADAQBoardID)->GetConfig()->ChipType == DUT_PALPIDEFS3) || 
        (GetDUT(ADAQBoardID)->GetConfig()->ChipType == DUT_PALPIDEFS4)){
      TpAlpidefs2 *myAlpide = (TpAlpidefs2*) GetDUT (ADAQBoardID);
      myAlpide->PrintCMUErrors();
    }   
    TDAQBoard *db = GetDAQBoard(ADAQBoardID);
    if (GetDUT(ADAQBoardID)->GetConfig()->ChipType == DUT_PALPIDEFS1){
       db->PowerOff();
       }
    if (GetDUT(ADAQBoardID)->GetConfig()->ChipType == DUT_PALPIDEFS2){
       ((TDAQBoard2 *)db)->PowerOff(1);
       }
    if (GetDUT(ADAQBoardID)->GetConfig()->ChipType == DUT_PALPIDEFS3){
       ((TDAQBoard2 *)db)->PowerOff(0);
       }
    if (GetDUT(ADAQBoardID)->GetConfig()->ChipType == DUT_PALPIDEFS4){
       ((TDAQBoard2 *)db)->PowerOff(0);
       }          
}


bool TTestSetup::InitialiseChip (int ADAQBoardID, int &AOverflow, bool Reset) {

    TpAlpidefs *myAlpide = (TpAlpidefs *) GetDUT(ADAQBoardID);
    TDAQBoard  *db       = GetDAQBoard(ADAQBoardID);
    std::vector <SFieldReg> ADCConfigReg1=db->GetADCConfigReg1(); // Get Descriptor Register ADCConfigReg1

    // Initialise chip
    db->SendADCConfigReg1 (ADCConfigReg1, db->CurrentToADC(db->GetConfig()->LimitAnalogInit));
    myAlpide->Init(Reset);
    db->SendADCConfigReg1 (ADCConfigReg1, db->CurrentToADC(db->GetConfig()->LimitAnalog));

    sleep(1);  // if delay < 1s measurements will be incorrect
    std::cout << std::endl <<  "Reading ADCs after init: " << std::endl;
    db->ReadAllADCs();
    return db->GetLDOStatus(AOverflow);
}


double TTestSetup::ReadDacMonV (int ADAQBoardID, TAlpideDacmonV AVmon) {
    TpAlpidefs* myAlpide = (TpAlpidefs*) GetDUT(ADAQBoardID);
    myAlpide->MonitorDACs(AVmon, DACI_NONE);
    usleep(100000);
    int    Reading = GetDAQBoard(ADAQBoardID)->ReadMonV();
    double Voltage = (double) Reading;
    Voltage       *= 3.3;
    Voltage       /= (1.8 * 4096);
    return Voltage;
}


double TTestSetup::ReadDacMonI (int ADAQBoardID, TAlpideDacmonI AImon) {
    TpAlpidefs* myAlpide = (TpAlpidefs*) GetDUT(ADAQBoardID);
    myAlpide->MonitorDACs(DACV_NONE, AImon);
    usleep(100000);
    int    Reading = GetDAQBoard(ADAQBoardID)->ReadMonI();
    double Current = (double) Reading;
    Current       *= (1e9 * 3.3);
    Current       /= (5100 * 4096 * 6);// 101);
    Current       /= 10;
    return Current;

}


double TTestSetup::ReadTempOnChip (int ADAQBoardID)
{
    int Value1, Value2;
    double Voltage = 0;
    TpAlpidefs* myAlpide = (TpAlpidefs*) GetDUT (ADAQBoardID);
    const char* dacName =
        (myAlpide->GetChipType() == DUT_PALPIDEFS2) ? "VAUX"  :
        (myAlpide->GetChipType() == DUT_PALPIDEFS3) ? "VTEMP" : "";
	(myAlpide->GetChipType() == DUT_PALPIDEFS4) ? "VTEMP" : "";
    if ((myAlpide->GetChipType() == DUT_PALPIDEFS2) || (myAlpide->GetChipType() == DUT_PALPIDEFS3) || (myAlpide->GetChipType() == DUT_PALPIDEFS4)) {
      TpAlpidefs2* myAlpide2 = (TpAlpidefs2*)myAlpide;
      myAlpide2->GetTemperature (Value1, Value2);
      myAlpide2->SetDAC(dacName, Value1);
      Voltage += ReadDacMonV(ADAQBoardID, SWCNTL_VAUX);
      myAlpide2->SetDAC(dacName, Value2);
      Voltage += ReadDacMonV(ADAQBoardID, SWCNTL_VAUX);
      Voltage /= 2;
      std::cout << "On-chip temp. measurement: voltage = " << Voltage << std::endl;
      return Voltage;
    }
    else {
      std::cout << "Temperature reading not implemented for chip type " << myAlpide->GetChipType() << std::endl;
      return -1;
    }
}


void TTestSetup::ReadAllChipDacs(int ADAQBoardID) {
    std::cout << "VAux       = " << ReadDacMonV(ADAQBoardID, SWCNTL_VAUX)       << " V" << std::endl;
    std::cout << "VCasn      = " << ReadDacMonV(ADAQBoardID, SWCNTL_VCASN)      << " V" << std::endl;
    std::cout << "VCasp      = " << ReadDacMonV(ADAQBoardID, SWCNTL_VCASP)      << " V" << std::endl;
    std::cout << "VPulsehigh = " << ReadDacMonV(ADAQBoardID, SWCNTL_VPLSE_HIGH) << " V" << std::endl;
    std::cout << "VPulselow  = " << ReadDacMonV(ADAQBoardID, SWCNTL_VPLSE_LOW)  << " V" << std::endl;
    std::cout << "VReset     = " << ReadDacMonV(ADAQBoardID, SWCNTL_VRESET)     << " V" << std::endl;

    std::cout << "IReset     = " << ReadDacMonI(ADAQBoardID, SWCNTL_IRESET) / 400  << " pA" << std::endl;
    std::cout << "IDB        = " << ReadDacMonI(ADAQBoardID, SWCNTL_IDB)    / 256  << " nA" << std::endl;
    std::cout << "IThr       = " << ReadDacMonI(ADAQBoardID, SWCNTL_ITHR)   / 4096 << " nA" << std::endl;
    std::cout << "IBias      = " << ReadDacMonI(ADAQBoardID, SWCNTL_IBIAS)  / 128  << " nA" << std::endl;
}


void TTestSetup::ReadAllChipDacsCSV(int ADAQBoardID) {
    std::cout << "VAux;" << ReadDacMonV(ADAQBoardID, SWCNTL_VAUX)       << ";V" << std::endl;
    std::cout << "VCasn;" << ReadDacMonV(ADAQBoardID, SWCNTL_VCASN)      << ";V" << std::endl;
    std::cout << "VCasp;" << ReadDacMonV(ADAQBoardID, SWCNTL_VCASP)      << ";V" << std::endl;
    std::cout << "VPulsehigh;" << ReadDacMonV(ADAQBoardID, SWCNTL_VPLSE_HIGH) << ";V" << std::endl;
    std::cout << "VPulselow;" << ReadDacMonV(ADAQBoardID, SWCNTL_VPLSE_LOW)  << ";V" << std::endl;
    std::cout << "VReset;" << ReadDacMonV(ADAQBoardID, SWCNTL_VRESET)     << ";V" << std::endl;
    std::cout << "IReset;" << ReadDacMonI(ADAQBoardID, SWCNTL_IRESET) / 400  << ";pA" << std::endl;
    std::cout << "IDB;" << ReadDacMonI(ADAQBoardID, SWCNTL_IDB)    / 256  << ";nA" << std::endl;
    std::cout << "IThr;" << ReadDacMonI(ADAQBoardID, SWCNTL_ITHR)   / 4096 << ";nA" << std::endl;
    std::cout << "IBias;" << ReadDacMonI(ADAQBoardID, SWCNTL_IBIAS)  / 128  << ";nA" << std::endl;
}

void TTestSetup::scanVoltageDAC (int ADAQBoardID, int ADAC, TAlpideDacmonV AVmon, int SampleDistance) {
    TpAlpidefs* myAlpide = (TpAlpidefs*) GetDUT(ADAQBoardID);
    int         Nominal  = myAlpide->GetConfig()->DACS[ADAC];
    double      Voltage;
    char        fName[100];

    sprintf(fName, "%s/VDAC%s.dat", OutputPath, myAlpide->GetDACName(ADAC));
    FILE *fp = fopen (fName, "w");

    std::cout << "Scanning DAC " << ADAC << std::endl;
    for (int i = 0; i < 256; i+=SampleDistance) {
        myAlpide->SetDAC(ADAC, i);
        Voltage = ReadDacMonV(ADAQBoardID, AVmon);
        fprintf(fp, "%d %.3f\n", i, Voltage);
        // std::cout << "   Value = " << i << ", Voltage = " << Voltage << std::endl;
    }
    myAlpide->SetDAC(ADAC, Nominal);
    fclose(fp);
}


void TTestSetup::scanCurrentDAC (int ADAQBoardID, int ADAC, TAlpideDacmonI AImon, int SampleDistance) {
    TpAlpidefs* myAlpide = (TpAlpidefs*) GetDUT(ADAQBoardID);
    int         Nominal  = myAlpide->GetConfig()->DACS[ADAC];
    double      Current;
    char        fName[100];

    sprintf(fName, "%s/IDAC%s.dat", OutputPath, myAlpide->GetDACName(ADAC));
    FILE *fp = fopen (fName, "w");

    myAlpide->MonitorDACs(DACV_NONE, AImon);

    std::cout << "Scanning DAC " << ADAC << std::endl;
    for (int i = 0; i < 256; i+=SampleDistance) {
        myAlpide->SetDAC(ADAC, i);
        Current = ReadDacMonI(ADAQBoardID, AImon);
        fprintf(fp, "%d %.1f\n", i, Current);
        //std::cout << "   Value = " << i << ", Current = " << Current << " nA" << std::endl;
    }
    fclose(fp);
    myAlpide->SetDAC(ADAC, Nominal);
}


void TTestSetup::ScanAllChipDacs(int ADAQBoardID, int SampleDistance) {
    TpAlpidefs* myAlpide = (TpAlpidefs*) GetDUT(ADAQBoardID);
    scanVoltageDAC(ADAQBoardID, myAlpide->FindDAC("VCASN"),   SWCNTL_VCASN, SampleDistance);
    scanVoltageDAC(ADAQBoardID, myAlpide->FindDAC("VCASP"),   SWCNTL_VCASP, SampleDistance);
    scanVoltageDAC(ADAQBoardID, myAlpide->FindDAC("VPULSEL"), SWCNTL_VPLSE_LOW, SampleDistance);
    scanVoltageDAC(ADAQBoardID, myAlpide->FindDAC("VPULSEH"), SWCNTL_VPLSE_HIGH, SampleDistance);
    if (myAlpide->GetChipType() == DUT_PALPIDEFS3 || myAlpide->GetChipType() == DUT_PALPIDEFS4) {
      scanVoltageDAC(ADAQBoardID, myAlpide->FindDAC("VRESETP"), SWCNTL_VRESETP, SampleDistance);
      scanVoltageDAC(ADAQBoardID, myAlpide->FindDAC("VRESETD"), SWCNTL_VRESETD, SampleDistance);
      scanVoltageDAC(ADAQBoardID, myAlpide->FindDAC("VCASN2"),  SWCNTL_VCASN2,  SampleDistance);
      scanVoltageDAC(ADAQBoardID, myAlpide->FindDAC("VCLIP"),   SWCNTL_VCLIP,   SampleDistance);
      scanVoltageDAC(ADAQBoardID, myAlpide->FindDAC("VTEMP"),   SWCNTL_VTEMP,   SampleDistance);
      scanCurrentDAC(ADAQBoardID, myAlpide->FindDAC("IAUX2"),   SWCNTL_IAUX2, SampleDistance);
      //      scanCurrentDAC(ADAQBoardID, myAlpide->FindDAC("IREF"),    SWCNTL_IREF, SampleDistance);
    }
    else {
      scanVoltageDAC(ADAQBoardID, myAlpide->FindDAC("VAUX"),    SWCNTL_VAUX, SampleDistance);
      scanVoltageDAC(ADAQBoardID, myAlpide->FindDAC("VRESET"),  SWCNTL_VRESET, SampleDistance);
    }
    scanCurrentDAC(ADAQBoardID, myAlpide->FindDAC("IRESET"),  SWCNTL_IRESET, SampleDistance);
    scanCurrentDAC(ADAQBoardID, myAlpide->FindDAC("IBIAS"),   SWCNTL_IBIAS, SampleDistance);
    scanCurrentDAC(ADAQBoardID, myAlpide->FindDAC("IDB"),     SWCNTL_IDB,   SampleDistance);
    scanCurrentDAC(ADAQBoardID, myAlpide->FindDAC("ITHR"),    SWCNTL_ITHR,  SampleDistance);
}


void TTestSetup::PrepareEmptyReadout (int AChipID, int AStrobeLength, bool APacketBased) {
    int DAQBoardID = GetBoardID(AChipID);
    int Delay =0x1a;
    GetDAQBoard(DAQBoardID)->ConfigureReadout (1, true, APacketBased);       // buffer depth = 1, sampling on rising edge
    GetDAQBoard(DAQBoardID)->ConfigureTrigger (0, AStrobeLength, 1, 1,Delay);
}


void TTestSetup::PrepareDigitalInjection (int AChipID) {
    int         DAQBoardID = GetBoardID(AChipID);
    TpAlpidefs *myAlpide   = (TpAlpidefs*) GetDUT(DAQBoardID, AChipID);

    PrepareEmptyReadout      (AChipID, GetDAQBoard(DAQBoardID)->GetConfig()->StrobeLength);

    if (myAlpide->GetChipType() == DUT_PALPIDEFS3) {
      TpAlpidefs3 *myAlpide3 = (TpAlpidefs3*) myAlpide;
      myAlpide3->SetPulseType(PT_DIGITAL);
    }
    if (myAlpide->GetChipType() == DUT_PALPIDEFS4) {
       TpAlpidefs4 *myAlpide4 = (TpAlpidefs4*) myAlpide;
       myAlpide4->SetPulseType(PT_DIGITAL);
    }
    myAlpide->PrepareReadout (fConfigGeneral       ->StrobeBlengthDigital, 
                              myAlpide->GetConfig()->ReadoutDelay,
                              myAlpide->GetConfig()->ReadoutMode);

}


void TTestSetup::PrepareDigitalNew(int AChipID, int NumEvt, bool APacketBased) {
    int         PulseMode  = 2;
    //int         NumEvt     = 1;
    int         Delay      =0x1a;
    int         TrigMode   = 2;
    int         TriggerPulseDelay =0xfff; 
    int         DAQBoardID = GetBoardID (AChipID);
    TDAQBoard  *myDAQBoard = GetDAQBoard(DAQBoardID);
    TpAlpidefs *myAlpide   = (TpAlpidefs *) GetDUT (DAQBoardID, AChipID);

    myAlpide->SetChipMode(MODE_ALPIDE_CONFIG);
    if (myAlpide->GetChipType() == DUT_PALPIDEFS3) {
      TpAlpidefs3 *myAlpide3 = (TpAlpidefs3*) myAlpide;
      myAlpide3->SetPulseType(PT_DIGITAL);
    }
    if (myAlpide->GetChipType() == DUT_PALPIDEFS4) {
       TpAlpidefs4 *myAlpide4 = (TpAlpidefs4*) myAlpide;
       myAlpide4->SetPulseType(PT_DIGITAL);
    }
    myDAQBoard->ConfigureReadout (1, true, APacketBased);       // buffer depth = 1, sampling on rising edge
    if (myDAQBoard->GetFirmwareDate() <  0x7E0912) NumEvt   = 1;
    if (myDAQBoard->GetFirmwareDate() >= 0x7E0912) TrigMode = 1;
    myDAQBoard->ConfigureTrigger(0, myDAQBoard->GetConfig()->StrobeLength, TrigMode, NumEvt,Delay);
    if (myDAQBoard->GetFirmwareDate() >= 0x7E0912) myDAQBoard->WriteTriggerPulseDelay(TriggerPulseDelay);
    ConfigurePulse (AChipID, fConfigGeneral->PulsedelayDigital, fConfigGeneral->PulselengthDigital, PulseMode);

    //if (PulseMode == 2) myDAQBoard->StartTrigger();
    myAlpide->PrepareReadout (fConfigGeneral->StrobeBlengthDigital, 
                              myAlpide->GetConfig()->ReadoutDelay, 
                              myAlpide->GetConfig()->ReadoutMode);

}


void TTestSetup::ConfigurePulse (int AChipID, int ADelay, int APulseLength, int APulseMode) {
    int         DAQBoardID = GetBoardID(AChipID);
    TDAQBoard  *myDAQBoard = GetDAQBoard(DAQBoardID);
    TpAlpidefs *myAlpide   = (TpAlpidefs*) GetDUT(DAQBoardID, AChipID);

    myDAQBoard->ConfigurePulse (fConfigGeneral->PulsedelayAnalogue, fConfigGeneral->PulselengthAnalogue, APulseMode);
    myAlpide  ->ConfigurePulse (fConfigGeneral->PulselengthAnalogue);

}


void TTestSetup::PrepareAnalogueInjection (int AChipID, int AStep, int PulseMode, int NumEvt, bool APacketBased)
{
    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    int         DAQBoardID = GetBoardID (AChipID);
    TDAQBoard  *myDAQBoard = GetDAQBoard(DAQBoardID);
    TpAlpidefs *myAlpide   = (TpAlpidefs *) GetDUT (DAQBoardID, AChipID);
    int         TrigMode   = 2;
    int         TriggerPulseDelay =0xfff;

    if (myAlpide->GetChipType() == DUT_PALPIDEFS3) {
      TpAlpidefs3 *myAlpide3 = (TpAlpidefs3*) myAlpide;
      myAlpide->SetChipMode(MODE_ALPIDE_CONFIG);
      myAlpide3->SetPulseType(PT_ANALOGUE);
    }
    
    if (myAlpide->GetChipType() == DUT_PALPIDEFS4) {
      TpAlpidefs4 *myAlpide4 = (TpAlpidefs4*) myAlpide;
      myAlpide4->SetChipMode(0);
      myAlpide4->WriteControlRegister();
      myAlpide4->SetPulseType(PT_ANALOGUE);
    }
     
    myDAQBoard->ConfigureReadout (1, true, APacketBased);       // buffer depth = 1, sampling on rising edge
    if (myDAQBoard->GetFirmwareDate() <  0x7E0912) NumEvt   = 1;
    if (myDAQBoard->GetFirmwareDate() >= 0x7E0912) TrigMode = 1;
    if (myDAQBoard->GetFirmwareDate() >= 0x7E0912) myDAQBoard->WriteTriggerPulseDelay(TriggerPulseDelay);
    int Delay =0x1a;
    if (PulseMode == 2) {  // Set external trigger
        myDAQBoard->ConfigureTrigger(0, myDAQBoard->GetConfig()->StrobeLength, TrigMode, NumEvt,Delay);
    }
    else {                 // Internal trigger
        myDAQBoard->ConfigureTrigger(0, myDAQBoard->GetConfig()->StrobeLength, TrigMode, NumEvt,Delay);
    }

    ConfigurePulse (AChipID, fConfigGeneral->PulsedelayAnalogue, fConfigGeneral->PulselengthAnalogue, PulseMode);

    //if (PulseMode == 2) myDAQBoard->StartTrigger();
    int LowValue;

    if (AStep < 171) LowValue = 170 - (int) AStep;
    else             LowValue = 0;

    myAlpide->SetDAC(myAlpide->FindDAC("VPULSEH"), 170);
    myAlpide->SetDAC(myAlpide->FindDAC("VPULSEL"), LowValue);

    myAlpide->PrepareReadout (fConfigGeneral->StrobeBlengthStandard, 
                              myAlpide->GetConfig()->ReadoutDelay, 
                              myAlpide->GetConfig()->ReadoutMode);
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> TimeSpan = duration_cast<duration<double>>(t2 - t1);
    PrepTime += TimeSpan.count();

}


void TTestSetup::FinaliseAnalogueInjection (int AChipID, int PulseMode)
{
  if (PulseMode == 2) {
    GetDAQBoard(GetBoardID(AChipID))->StopTrigger();
  }
}


// Triggers a single event and adds the hits to the vector Hits
bool TTestSetup::TriggerAndReadEvent (int ADAQBoardID, TpAlpidefs *myAlpide, std::vector<TPixHit> *Hits) {
    unsigned char  data_buf[1024*100];
    TEventHeader   Header;
    int            Length;
    bool           EventOK = false, HeaderOK, TrailerOK;

    const int headerLength = GetDAQBoard(ADAQBoardID)->GetEventHeaderLength();
    const int trailerLength = GetDAQBoard(ADAQBoardID)->GetEventTrailerLength();

    if (GetDAQBoard(ADAQBoardID)->GetFirmwareDate() <  0x7E0912) GetDAQBoard(ADAQBoardID)->StartTrigger();

    TpAlpidefs3 *myAlpide3 = (TpAlpidefs3*) myAlpide;

    //myAlpide3->SendCommand(0xb1);

    //if (!GetDAQBoard(ADAQBoardID)->ReadChipEvent(data_buf, &Length, sizeof(data_buf))) return false; 
Length =0;
     while (Length == 0){
       if (!GetDAQBoard(ADAQBoardID)->ReadChipEvent(data_buf, &Length, sizeof(data_buf))) return false;
       }      
    HeaderOK                = GetDAQBoard(ADAQBoardID)->DecodeEventHeader(data_buf, &Header);

    //        std::cout << "Received event " << Header.EventId << ", time stamp = " << Header.TimeStamp << std::endl;

    if (HeaderOK) EventOK   = myAlpide->DecodeEvent(data_buf+headerLength, Length-headerLength-trailerLength, Hits);
    if (EventOK)  TrailerOK = GetDAQBoard(ADAQBoardID)->DecodeEventTrailer(data_buf + Length - trailerLength, &Header);

    if (!EventOK) {
        std::cout << "EVENT NOT OK! Dumping raw data" << std::endl;
        DumpRawData(data_buf, Length);
        std::cout << std::endl;
        GetDAQBoard(ADAQBoardID)->ReadMonitorRegisters();
        std::cout << std::endl;
        GetDAQBoard(ADAQBoardID)->ReadAllRegisters();
        std::cout << std::endl;
        uint64_t eventID = (uint64_t)-1;
        uint32_t tmp_value = 0;
        GetDAQBoard(ADAQBoardID)->ReadRegister(0x202, &tmp_value); // event id low
        eventID = tmp_value & 0xffffff;
        GetDAQBoard(ADAQBoardID)->ReadRegister(0x202, &tmp_value); // event id high
        eventID |= (tmp_value & 0xffffff) << 24;
        std::cout << "DAQ board event ID: " << eventID << std::endl;
        GetDAQBoard(ADAQBoardID)->ReadRegister(0x306, &tmp_value);
        std::cout << "Strobe count :      " << tmp_value << std::endl;
        std::cout << std::endl;
        TpAlpidefs3* myAlpide3 = dynamic_cast<TpAlpidefs3*>(myAlpide);
        if (myAlpide3) {
            int StrobeCounter = myAlpide3->ReadStrobeCounter();
            std::cout << "pALPIDE-3 strobe counter: " << StrobeCounter << std::endl;
        }
        exit(0);
    }
    if (GetDAQBoard(ADAQBoardID)->GetFirmwareDate() <  0x7E0912) GetDAQBoard(ADAQBoardID)->StopTrigger    ();
    return true;
}


bool TTestSetup::TriggerAndReadEvent (int AChipID, std::vector <TPixHit> *Hits) {
  int         DAQBoardID = GetBoardID(AChipID);
  TpAlpidefs* myAlpide   = (TpAlpidefs*) GetDUT (DAQBoardID, AChipID);
  return TriggerAndReadEvent (DAQBoardID, myAlpide, Hits);
}


// Special function first send a pulse and then a strobe
// in this case the event is triggered by a pulse signal
// Trigger a single event and adds the hits to the vector Hits
bool TTestSetup::PulseAndReadEvent (int AChipID, int APulseLength, std::vector<TPixHit> *Hits, int NTriggers, TEventHeader *Header) {
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  int         DAQBoardID = GetBoardID (AChipID);
  TpAlpidefs *myAlpide = (TpAlpidefs*) GetDUT (DAQBoardID, AChipID);
  TDAQBoard  *DAQBoard = GetDAQBoard (DAQBoardID);
  bool Result = PulseAndReadEvent (DAQBoard, myAlpide, APulseLength, Hits, NTriggers, Header);
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  duration<double> TimeSpan = duration_cast<duration<double>>(t2 - t1);
  PulseReadTime += TimeSpan.count();
  return Result;
}

bool TTestSetup::PulseAndReadEvent (TDAQBoard *myDAQBoard, TpAlpidefs *myAlpide, int APulseLength, std::vector<TPixHit> *Hits, int NTriggers, TEventHeader *Header /*=0x0*/) {
    unsigned char  data_buf[1024*100];
    int            Length;
    bool           EventOK = false, HeaderOK, TrailerOK;
    if (!Header) Header = new TEventHeader();

    const int headerLength = myDAQBoard->GetEventHeaderLength();
    const int trailerLength = myDAQBoard->GetEventTrailerLength();
    uint32_t tmp_value = 0;
    
    if (myDAQBoard->GetFirmwareDate() >= 0x7E0912)myDAQBoard->StartTrigger();
    /*std::cout << "AFTER START TRIGGER :      " << std::endl;
    myDAQBoard->ReadRegister(0x306, &tmp_value);
    std::cout << "Strobe count :      " << tmp_value << std::endl;
    myDAQBoard->ReadMonitorRegisters();
    std::cout << std::endl;*/
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    for (int itrig = 0; itrig < NTriggers; itrig ++) {
      //printf("Triggering...\n");
        if (myDAQBoard->GetFirmwareDate() < 0x7E0912) myDAQBoard->StartTrigger();
        if (myDAQBoard->GetFirmwareDate() < 0x7E0912) myAlpide->Pulse(APulseLength);
	if (myDAQBoard->GetFirmwareDate() < 0x7E0912) myDAQBoard->StopTrigger();
    }
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> TimeSpan = duration_cast<duration<double>>(t2 - t1);
    PulseTime += TimeSpan.count();

    //if (myDAQBoard->GetFirmwareVersion() == 0x347E0912) myDAQBoard->StopTrigger();
    for (int itrig = 0; itrig < NTriggers; itrig ++) {
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        Length =0;
        while (Length == 0){
            if (!myDAQBoard->ReadChipEvent(data_buf, &Length, sizeof(data_buf))) return false;
            }
        /*tmp_value = 0;
        myDAQBoard->ReadRegister(0x306, &tmp_value);
        std::cout << "Strobe count :      " << tmp_value << std::endl;
        myDAQBoard->ReadMonitorRegisters();
        std::cout << std::endl;*/
	
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
        duration<double> TimeSpan = duration_cast<duration<double>>(t2 - t1);
        ReadTime += TimeSpan.count();
        t1 = high_resolution_clock::now();
	
        HeaderOK                = myDAQBoard->DecodeEventHeader(data_buf, Header);
        if (HeaderOK) EventOK   = myAlpide->DecodeEvent(data_buf+headerLength, Length-headerLength-trailerLength, Hits);
        if (EventOK)  TrailerOK = myDAQBoard->DecodeEventTrailer(data_buf+Length-trailerLength, Header);
        if (!EventOK) {
            std::cout<<"HeaderOK"<<HeaderOK<<std::endl;
            std::cout<<"EventOK"<<EventOK<<std::endl;
            std::cout << "EVENT NOT OK! Dumping raw data" << std::endl;
            DumpRawData(data_buf, Length);
            std::cout << std::endl;
            myDAQBoard->ReadMonitorRegisters();
            std::cout << std::endl;
            myDAQBoard->ReadAllRegisters();
            std::cout << std::endl;
            uint64_t eventID = (uint64_t)-1;
            uint32_t tmp_value = 0;
            myDAQBoard->ReadRegister(0x202, &tmp_value); // event id low
            eventID = tmp_value & 0xffffff;
            myDAQBoard->ReadRegister(0x202, &tmp_value); // event id high
            eventID |= (tmp_value & 0xffffff) << 24;
            std::cout << "DAQ board event ID: " << eventID << std::endl;
            myDAQBoard->ReadRegister(0x306, &tmp_value);
            std::cout << "Strobe count :      " << tmp_value << std::endl;
            std::cout << std::endl;
            TpAlpidefs3* myAlpide3 = dynamic_cast<TpAlpidefs3*>(myAlpide);
            if (myAlpide3) {
                int StrobeCounter = myAlpide3->ReadStrobeCounter();
                std::cout << "pALPIDE-3 strobe counter: " << StrobeCounter << std::endl;
            }
            //exit(0);
        }

        t2 = high_resolution_clock::now();
        TimeSpan = duration_cast<duration<double>>(t2 - t1);
        DecodeTime += TimeSpan.count();

    }

    return true;
    //myAlpide->DumpHitData(Hits);
}
void TTestSetup::DumpRawData(unsigned char *data_buf, int Length) {
    FILE *fp = fopen ("RawDataDump.dat", "w");

    for (int i = 0; i < Length; i++) {
        fprintf(fp, "%02X ", data_buf[i]);
    }
    fprintf(fp, "\n\n");
    fclose(fp);
}


void TTestSetup::WriteConfigToFile (const char *fName) {
    FILE *fp = fopen (fName, "a");

    ReadFinalTemp(USB_id);
#ifndef XCODE
    fprintf(fp, "Git commit: %s\n", VERSION);
#endif
    fprintf(fp, "Firmware version: %d\n\n", GetDAQBoard(USB_id)->GetFirmwareVersion());

    for (int idut = 0; idut < fNDUTs; idut ++) {
        TpAlpidefs *myAlpide = (TpAlpidefs *) GetDUT(idut);
        for (int idac = 0; idac < myAlpide->GetNDacs(); idac ++)  {
            fprintf(fp, "%s\t%d\n", myAlpide->GetDACName(idac), myAlpide->GetDAC (idac));
        }
    }

    fprintf(fp, "\nTempI\t\%.1f\n", fTempInitial);
    fprintf(fp, "TempF\t\%.1f\n", fTempFinal);

    fclose (fp);
}


void TTestSetup::WriteConfigToFile (const char *fName, TConfig *AConfig) {
    FILE *fp = fopen (fName, "a");
    AConfig->WriteToFile(fp); 
    fclose (fp);
}


void TTestSetup::WriteConfigToFileCSV (const char *fName) {
    FILE *fp = fopen (fName, "a");

#ifndef XCODE
    fprintf(fp, "Git commit;%s\n", VERSION);
#endif
    for (int idut = 0; idut < fNDUTs; idut ++) {
        TpAlpidefs *myAlpide = (TpAlpidefs *) GetDUT(idut);
        for (int idac = 0; idac < myAlpide->GetNDacs(); idac ++)  {
            fprintf(fp, "%s;%d\n", myAlpide->GetDACName(idac), myAlpide->GetDAC (idac));
        }
    }
    fclose (fp);
}


const char* TTestSetup::GetGITVersion()
{
#ifndef XCODE
    return VERSION;
#else
    return "";
#endif
}


void TTestSetup::ReadADCData() {
    TDAQBoard *db = GetDAQBoard(0);
    unsigned char data_in[1024];
    int num_byte_rcv;
    int nreads = 0;
    TADCData ADCData;
    std::vector <SFieldReg> ADCConfigReg0 = db->GetADCConfigReg0(); // Get Descriptor Register ADCConfigReg0

    std::cout << "Warning: method ReadADCData not working yet" << std::endl;

    db->SendStartStreamDataADC(ADCConfigReg0);  // This seems to switch off the regulators

    StopADCData = false;

    FILE *fp = fopen("ADC.dat", "w");

    while(!StopADCData) {
      num_byte_rcv = db->ReceiveData( TDAQBoard::ENDPOINT_READ_ADC, data_in, 1024);

        //printf("\n\nData Received: %d bytes\n\n", num_byte_rcv);
        //for(int k=0; k < num_byte_rcv; k++)  {fprintf(fp, "%x\t",data_in[k]);}
        //fprintf (fp,"\n");

        db->DecodeADCData(data_in, ADCData);

        float time = (float)ADCData.TimeStamp * 0.0125e-3;
        fprintf (fp, "%d %f %f %f %f %f\n", ADCData.LDOOn?1:0, time, ADCData.Temp - 273.15, ADCData.IDig, ADCData.IDigIO, ADCData.IAna);
        //printf("LDO Status: %d\n", ADCData.LDOOn?1:0);
        //std::cout << "Time stamp: " << ADCData.TimeStamp << std::endl;
        //printf("Temp:   %f\n", ADCData.Temp);
        //printf("MonV:   %f\n", ADCData.MonV);
        //printf("MonI:   %f\n", ADCData.MonI);
        //printf("IDig:   %f\n", ADCData.IDig);
        //printf("IDigIO: %f\n", ADCData.IDigIO);
        //printf("IAna:   %f\n", ADCData.IAna);

        nreads ++;
    }
    db->SendEndStreamDataADC(ADCConfigReg0);

    fclose (fp);

}
