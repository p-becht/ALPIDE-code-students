//
//  TDut.cpp
//  pALPIDEfs-software
//
//  Created by Carlo Puggioni and Markus Keil on 14/04/14.
//  Copyright (c) 2014 Carlo Puggioni, Markus Keil. All rights reserved.
//

#include "TDut.h"
#include "TTestsetup.h"
#include "TDaqboard.h"
#include "TConfig.h"
#include <string.h>
#include <iostream>

//---------------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////
//                                                                    //
//                    class TDUT                                      //
//                                                                    //
////////////////////////////////////////////////////////////////////////

const char TDUT::DACNames[1][10] = {"VAUX"};
const int  TDUT::DACDefaults[1]  = {0};

TDUT::TDUT(TTestSetup *ASetup, int AIndex, TChipConfig *AConfig)
{
    fConfig = AConfig;
    fSetup  = ASetup;
    fIndex  = AIndex;
}


TDUTType TDUT::GetChipType () 
{
  return fConfig->ChipType;
}


int TDUT::WriteRegister (int Address, int Value) {
    TDAQBoard *myBoard = fSetup->GetDAQBoard(fIndex);
    std::cout << "TDUT: Writing value 0x" << std::hex << Value <<  " to register 0x" << Address << std::dec << std::endl;
    return myBoard->WriteChipRegister(Address,Value);
}


int TDUT::ReadRegister (int Address, int *Value) {
    TDAQBoard *myBoard = fSetup->GetDAQBoard(fIndex);
    return myBoard->ReadChipRegister(Address, Value);
}


int TDUT::SendSignal (TChipSignal ASignal, int ADuration)
{
    TDAQBoard *myBoard = fSetup->GetDAQBoard(fIndex);
    return myBoard->SendChipSignal(ASignal, ADuration);
}


const char *GetDACNameGen(int ADAC, const char ADACNames[][10], int ANDacs) {
  if (ADAC < ANDacs) {
    return ADACNames[ADAC]; 
  }
  else return "BAD DAC ID";
}


int GetDACDefaultGen (int ADAC, const int ADACDefaults[], int ANDacs) {
  if (ADAC < ANDacs) {
    return ADACDefaults[ADAC];
  }
  else {
    std::cout << "Bad DAC ID " << ADAC << std::endl;
    return -1;
  }
}


int FindDACGen (const char ADACNames[][10], const char ADACName[10], int ANDacs) {
  for (int i = 0; i < ANDacs; i++) {
    if (!strcmp (ADACNames[i], ADACName)) {
      return i;
    }
  }
  std::cout << "Invalid DAC name: " << ADACName << std::endl;
  return -1;
}
