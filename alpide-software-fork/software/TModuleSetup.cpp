#include "TModuleSetup.h"
#include "chiptests.h"
#include <unistd.h>

TModuleSetup::TModuleSetup () 
  : TTestSetup () 
{
  fType = MODULE_TYPE_IB; // for the time being, later -> config
}


int TModuleSetup::FindDAQBoards (TConfig *AConfig) 
{
  int err = TTestSetup::FindDAQBoards(AConfig);
  if (err) return err;
  
  if ((fType == MODULE_TYPE_IB) && (fNDAQBoards != 1)) {
    std::cout << "Error: found " << fNDAQBoards << " DAQ boards for IB Module. Something is wrong..." << std::endl;
    return -1;
  }
  if ((fType == MODULE_TYPE_OB) && (fNDAQBoards != 2)) {
    std::cout << "Error: found " << fNDAQBoards << " DAQ boards for OB Module. Something is wrong..." << std::endl;
    return -1;
  }
  return err;
}


// Same as in TTestSetup, but all DUTs get the same DAQ board index (0) in case of IB modules, 0 or 1 in the case of OB modules

// to be checked for OB modules: which chip to which DAQ board (chip IDs)
int TModuleSetup::AddDUTs (TConfig     *AConfig)
{
    int NChips = AConfig->GetNChips(); 
    TDUT *dut;
    fNDUTs = 0;  
    for (int i = 0; i < NChips; i++) {
        dut = 0;
        switch (AConfig->GetChipConfig(i)->ChipType) {
   	case DUT_PALPIDEFS:
 	    std::cout << "WARNING: DUT type PALPIDEFS is deprecated, constructing PALPIDE1." << std::endl;
            dut = new TpAlpidefs1(this, GetBoardID(i), AConfig->GetChipConfig(i));
            break;
        case DUT_PALPIDEFS1:
	  std::cout << "WARNING: Found chip type PALPIDE1 for a module. Chip #" << i << std::endl;
            dut = new TpAlpidefs1(this, GetBoardID(i), AConfig->GetChipConfig(i));
            break;
        case DUT_PALPIDEFS2:
  	    std::cout << "Adding pALPIDE2 as DUT number " << i << std::endl;
 	    dut = new TpAlpidefs2(this, GetBoardID(i), AConfig->GetChipConfig(i));
            break;
        case DUT_PALPIDEFS3:
  	    std::cout << "Adding pALPIDE3 as DUT number " << i << std::endl;
 	    dut = new TpAlpidefs3(this, GetBoardID(i), AConfig->GetChipConfig(i));
            break;
	case DUT_PALPIDEFS4:
  	    std::cout << "Adding pALPIDE4 as DUT number " << i << std::endl;
 	    dut = new TpAlpidefs4(this, GetBoardID(i), AConfig->GetChipConfig(i));
            break;    
        case DUT_UNKNOWN:
  	    std::cout << "WARNING, DUT type is UNKNOWN, doing nothing." << std::endl;
            break;
        }
        if (dut) {
  	    fNDUTs ++;
            fDUTs.push_back(dut);
        }
    }
    return fNDUTs;
}


int TModuleSetup::GetBoardID (int AChipID) {
  if (fType == MODULE_TYPE_IB) {
    return 0;
  }
  else if (fType == MODULE_TYPE_OB) {
    int index = GetChipIndexByID (AChipID);
    return index / 7; // or similar
  }
  else {
    std::cout << "Unknown module type " << fType << std::endl;
    return -1;
  }
}


TDAQBoard *TModuleSetup::GetDAQBoard (int i) {
  return TTestSetup::GetDAQBoard(GetBoardID(i));
}


int TModuleSetup::GetChipID(int index) {
  TpAlpidefs2 *myDUT = (TpAlpidefs2*) GetDUT (index);
  return myDUT->GetChipId();
}


int TModuleSetup::GetChipIndexByID(int AChipID) {
  for (int i = 0; i < fNDUTs; i++) {
    if (GetChipID(i) == AChipID) return i;
  }
  return -1;
}


//bool TModuleSetup::PowerOnBoard (int ADAQBoardID, int &Overflow) {
    //The argument passed as ADAQBOardID is USB_id, which corresponds to the chip not the board 
    //-> Call GetBoardID first
//  return TTestSetup::PowerOnBoard (GetBoardID (ADAQBoardID), Overflow);
//}


void TModuleSetup::PowerOffBoard (int ADAQBoardID) {
    if ((GetDUT(ADAQBoardID)->GetConfig()->ChipType == DUT_PALPIDEFS2) ||
        (GetDUT(ADAQBoardID)->GetConfig()->ChipType == DUT_PALPIDEFS3) ||
        (GetDUT(ADAQBoardID)->GetConfig()->ChipType == DUT_PALPIDEFS4)) {
      for (int i = 0; i < fNDUTs; i ++) {
        TpAlpidefs2 *myAlpide = (TpAlpidefs2*) GetDUT (i);
	std::cout << "Chip " << i << ", ";
        myAlpide->PrintCMUErrors();
      }
    }   
    //The argument passed as ADAQBOardID is USB_id, which corresponds to the chip not the board 
    //-> Call GetBoardID first
    TDAQBoard *db = GetDAQBoard(ADAQBoardID);
    db->PowerOff();
}


bool TModuleSetup::InitialiseChip (int AChipID, int &AOverflow, bool Reset) {

    TpAlpidefs *myAlpide = (TpAlpidefs *) GetDUT(AChipID);
    TDAQBoard  *db       = GetDAQBoard(0);
    std::vector <SFieldReg> ADCConfigReg1=db->GetADCConfigReg1(); // Get Descriptor Register ADCConfigReg1

    // Initialise chip
    db->SendADCConfigReg1 (ADCConfigReg1, db->CurrentToADC(db->GetConfig()->LimitAnalogInit));
    myAlpide->Init(Reset);
    //if (AChipID != USB_id) {
    //  TpAlpidefs2* myAlpide2 = (TpAlpidefs2*) myAlpide;
    //  std::cout << "chip ID = " << AChipID << ", setting drvr to 0" << std::endl;
    //  myAlpide2->WriteCMUCurrents (0, 0, 10);
    //}
    db->SendADCConfigReg1 (ADCConfigReg1, db->CurrentToADC(db->GetConfig()->LimitAnalog));

    sleep(1);  // if delay < 1s measurements will be incorrect
    std::cout << std::endl <<  "Reading ADCs after init: " << std::endl;
    db->ReadAllADCs();
    return db->GetLDOStatus(AOverflow);
}


bool TModuleSetup::InitialiseAllChips (int &AOverflow) {
  // Initialise first chip with global reset, 
  // all following ones without
  if (fNDUTs == 0) return false;
  if (!InitialiseChip(0, AOverflow)) return false;  
  for (int i = 1; i < fNDUTs; i++) {
    if (!InitialiseChip(i, AOverflow, false)) return false;
  }
  return true;
}


double TModuleSetup::ReadDacMonV (int ADAQBoardID, TAlpideDacmonV AVmon) {
  std::cout << "Chip DAC monitoring not possible with module setup" << std::endl;
  return -1;
}


double TModuleSetup::ReadDacMonI (int ADAQBoardID, TAlpideDacmonI AImon) {
  std::cout << "Chip DAC monitoring not possible with module setup" << std::endl;
  return -1;
}


void TModuleSetup::ReadAllChipDacs (int ADAQBoardID) {
  std::cout << "Chip DAC monitoring not possible with module setup" << std::endl;
}


void TModuleSetup::ReadAllChipDacsCSV (int ADAQBoardID){
  std::cout << "Chip DAC monitoring not possible with module setup" << std::endl;
}


void TModuleSetup::scanVoltageDAC (int ADAQBoardID, int ADAC, TAlpideDacmonV AVmon, int SampleDistance) {
  std::cout << "Chip DAC monitoring not possible with module setup" << std::endl;
}


void TModuleSetup::scanCurrentDAC (int ADAQBoardID, int ADAC, TAlpideDacmonI AImon, int SampleDistance) {
  std::cout << "Chip DAC monitoring not possible with module setup" << std::endl;
}


void TModuleSetup::ScanAllChipDacs (int ADAQBoardID, int SampleDistance) {
  std::cout << "Chip DAC monitoring not possible with module setup" << std::endl;
}


double TModuleSetup::ReadTempOnChip (int ADAQBoardID)
{
    int Value1, Value2;
    double Voltage = 0;
    TpAlpidefs* myAlpide = (TpAlpidefs*) GetDUT (ADAQBoardID);
    if ((myAlpide->GetChipType() == DUT_PALPIDEFS2) || (myAlpide->GetChipType() == DUT_PALPIDEFS3) || (myAlpide->GetChipType() == DUT_PALPIDEFS4)) {
      TpAlpidefs2* myAlpide2 = (TpAlpidefs2*) myAlpide;
      myAlpide2->GetTemperature (Value1, Value2);
      Voltage = ((double) Value1 + (double) Value2) / 2;
      std::cout << "On-chip temp. measurement: voltage = " << Voltage << std::endl;
      return Voltage;
    }
    else {
      std::cout << "Temperature reading not implemented for chip type " << myAlpide->GetChipType() << std::endl;
      return -1;
    }
}

// Prepare DigitalInjection / Prepare EmptyReadout can stay, if it is sure that they are called with the correct parameters...

