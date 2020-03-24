//
//  TDaqboard.cpp
//  pALPIDEfs-software
//
//  Created by Carlo Puggioni and Markus Keil on 14/04/14.
//  Copyright (c) 2014 Carlo Puggioni, Markus Keil. All rights reserved.
//

#include "TDaqboard.h"
#include "stdio.h"
#include <math.h>
#include "stdint.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include "chiptests.h"
#include <chrono>

//#define MYDEBUG
//#define CPDEBUG // Caterina: Current Profile Debug Mode

//---------------------------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////
//                                                                    //
//                    class TDAQBoard                                 //
//                                                                    //
////////////////////////////////////////////////////////////////////////

#define ASSERTBUFFERLENGTH(maxLength, current, additional) if (current + additional > maxLength) { std::cerr << "Buffer too small (current = " << current << ", add = " << additional << ", max = " << maxLength << "). Exiting!" << std::endl; return false; }

TDAQBoard::TDAQBoard(libusb_device *ADevice, TDAQBoardConfig *AConfig) : TUSBBoard(ADevice), fReadoutMode(0), fEventBuffer() {
    // set object variables

    fConfig        = AConfig;
    fResetDuration = fConfig->ResetDuration;
    fPulseDuration = fConfig->PulseLength;
    fPrstDuration  = fConfig->PrstDuration;
    fPulseDelay    = fConfig->PulseDelay;
    fPulseMode     = fConfig->PulseMode;

    fNumADCEvent     = 0;
    fNumLastADCWords = 0;

    fHeaderFormat = 0;

    DefineAckHeader     ();
    DefineAckData       ();
    DefineADCHeader     ();
    DefineADCConfigReg0 ();
    DefineADCConfigReg1 ();
    DefineADCRead0      ();
    DefineADCRead1      ();
    DefineADCRead2      ();
    DefineIDReg         ();
    DefineIDChip        ();
    DefineIDFirmware    ();
    DefineEventIDReg    ();

    fTrailerWord = 0xabfeabfe;
    if (ADevice) {
        ReadFirmwareVersion ();
        //if (GetFWChipVersion() != DUT_PALPIDEFS1) {
        //  std::cout << "Warning, wrong firmware version detected for DAQBoard 1" << std::endl;
        //}
        fDummy = false;

        WriteResetDuration();
    }
    else {
        fFirmwareVersion = 0;
        fDummy = true;
    }
}


void TDAQBoard::DefineAckHeader(){
    SFieldReg field;
// DataType
    field.name = "header";
    field.size = 32;
    field.min_value = 0;
    field.max_value = 0xFFFFFFFF;
    field.setValue = 0;
    fAckHeader.push_back(field);
}

void TDAQBoard::DefineAckData(){
    SFieldReg field;
// DataType
    field.name = "Data";
    field.size = 32;
    field.min_value = 0;
    field.max_value = 0xFFFFFFFF;
    field.setValue = 0;
    fAckData.push_back(field);
}

void TDAQBoard::DefineADCConfigReg0(){
    SFieldReg field;
    int id=0;
    field.addr = ADC_CONFIG0 + (MODULE_ADC << SIZE_ADDR_REG);
// Iref0
    field.name = "Iref0";
    field.size = 12;
    field.min_value = 0;
    field.max_value = 4095;
    field.setValue = 100;
    fADCConfigReg0.push_back(field);
    if (CheckNameExist(field.name)){
        fMapNameId[field.name]=id;
        id++;
    }
// Iref1
    field.name = "Iref1";
    field.size = 12;
    field.min_value = 0;
    field.max_value = 4095;
    field.setValue = 500;
    fADCConfigReg0.push_back(field);
    if (CheckNameExist(field.name)){
        fMapNameId[field.name]=id;
        id++;
    }
// Enable or Disable LDOs self shutdown
    field.name = "EnLDOSelfShtdn";
    field.size = 1;
    field.min_value = 0;
    field.max_value = 1;
    field.setValue = 1;
    fADCConfigReg0.push_back(field);
    if (CheckNameExist(field.name)){
        fMapNameId[field.name]=id;
        id++;
    }
// Enable or Disable LDO shutdown
    field.name = "EnLDOff";
    field.size = 1;
    field.min_value = 0;
    field.max_value = 1;
    field.setValue = 1;
    fADCConfigReg0.push_back(field);
    if (CheckNameExist(field.name)){
        fMapNameId[field.name]=id;
        id++;
    }
// Enable or Disable ADC Stream Data.
    field.name = "EnStreamADCData";
    field.size = 1;
    field.min_value = 0;
    field.max_value = 1;
    field.setValue = 0;
    fADCConfigReg0.push_back(field);
    if (CheckNameExist(field.name)){
        fMapNameId[field.name]=id;
        id++;
    }
// Enable or Disable ADC Stream Data.
    field.name = "ADCSelfStop";
    field.size = 1;
    field.min_value = 0;
    field.max_value = 1;
    field.setValue = 0;
    fADCConfigReg0.push_back(field);
    if (CheckNameExist(field.name)){
        fMapNameId[field.name]=id;
        id++;
    }
// Enable or Disable ADC Stream Data.
    field.name = "ADCDisableResetTimeStamp";
    field.size = 1;
    field.min_value = 0;
    field.max_value = 1;
    field.setValue = 0;
    fADCConfigReg0.push_back(field);
    if (CheckNameExist(field.name)){
        fMapNameId[field.name]=id;
        id++;
    }
// Enable or Disable ADC Stream Data.
    field.name = "ADCEnablePacketBased";
    field.size = 1;
    field.min_value = 0;
    field.max_value = 1;
    field.setValue = 0;
    fADCConfigReg0.push_back(field);
    if (CheckNameExist(field.name)){
        fMapNameId[field.name]=id;
        id++;
    }
}

void TDAQBoard::DefineADCConfigReg1(){
    int id=0;
    SFieldReg field;
    field.addr = ADC_CONFIG1 + (MODULE_ADC << SIZE_ADDR_REG);
// Iref2
    field.name = "Iref2";
    field.size = 12;
    field.min_value = 0;
    field.max_value = 4095;
    field.setValue = 100;
    fADCConfigReg1.push_back(field);
    if (CheckNameExist(field.name)){
        fMapNameId[field.name]=id;
        id++;
    }
}




void TDAQBoard::DefineADCRead0(){
    int id=0;
    SFieldReg field;
    field.addr = ADC_READ0 + (MODULE_ADC << SIZE_ADDR_REG);
// ADC_NTC
    field.name = "ADC_NTC";
    field.size = 12;
    field.min_value = 0;
    field.max_value = 4095;
    field.readValue = 0;
    fADCRead0.push_back(field);
    if (CheckNameExist(field.name)){
        fMapNameId[field.name]=id;
        id++;
    }
// ADC_MONV
    field.name = "ADC_MONV";
    field.size = 12;
    field.min_value = 0;
    field.max_value = 4095;
    field.readValue = 0;
    fADCRead0.push_back(field);
    if (CheckNameExist(field.name)){
        fMapNameId[field.name]=id;
        id++;
    }
}

void TDAQBoard::DefineADCRead1(){
    int id=0;
    SFieldReg field;
    field.addr = ADC_READ1 + (MODULE_ADC << SIZE_ADDR_REG);
// ADC_MONI
    field.name = "ADC_MONI";
    field.size = 12;
    field.min_value = 0;
    field.max_value = 4095;
    field.readValue = 0;
    fADCRead1.push_back(field);
    if (CheckNameExist(field.name)){
        fMapNameId[field.name]=id;
        id++;
    }
// ADC_V18D
    field.name = "ADC_V18D";
    field.size = 12;
    field.min_value = 0;
    field.max_value = 4095;
    field.readValue = 0;
    fADCRead1.push_back(field);
    if (CheckNameExist(field.name)){
        fMapNameId[field.name]=id;
        id++;
    }
}

void TDAQBoard::DefineADCRead2(){
    int id=0;
    SFieldReg field;
    field.addr = ADC_READ2 + (MODULE_ADC << SIZE_ADDR_REG);
// ADC_V18O
    field.name = "ADC_V18O";
    field.size = 12;
    field.min_value = 0;
    field.max_value = 4095;
    field.readValue = 0;
    fADCRead2.push_back(field);
    if (CheckNameExist(field.name)){
        fMapNameId[field.name]=id;
        id++;
    }
// ADC_V18A
    field.name = "ADC_V18A";
    field.size = 12;
    field.min_value = 0;
    field.max_value = 4095;
    field.readValue = 0;
    fADCRead2.push_back(field);
    if (CheckNameExist(field.name)){
        fMapNameId[field.name]=id;
        id++;
    }
}

void TDAQBoard::DefineADCHeader(){
    SFieldReg field;
// DataType
    field.name = "DataType";
    field.size = 2;
    field.min_value = 0;
    field.max_value = 1;
    field.setValue = 0;
    fADCHeader.push_back(field);
// Module Address
    field.name = "ModuleAddr";
    field.size = 4;
    field.min_value = 0;
    field.max_value = 4;
    field.setValue = 0;
    fADCHeader.push_back(field);
// Error Bits
    field.name = "ErrorBits";
    field.size = 4;
    field.min_value = 0;
    field.max_value = 15;
    field.setValue = 0;
    fADCHeader.push_back(field);
// ADC Active
    field.name = "ADC Active";
    field.size = 6;
    field.min_value = 0;
    field.max_value = 63;
    field.setValue = 0;
    fADCHeader.push_back(field);
// Spare
    field.name = "Spare";
    field.size = 16;
    field.min_value = 0;
    field.max_value = 0;
    field.setValue = 0;
    fADCHeader.push_back(field);
    // TimeStamp1
    field.name = "TimeStamp1";
    field.size = 32;
    field.min_value = 0;
    field.max_value = 16777215;
    field.setValue = 0;
    fADCHeader.push_back(field);
    // TimeStamp2
    field.name = "TimeStamp2";
    field.size = 32;
    field.min_value = 0;
    field.max_value = 16777215;
    field.setValue = 0;
    fADCHeader.push_back(field);
}



void TDAQBoard::DefineIDReg(){
    int id=0;
    SFieldReg field;
    field.addr = IDENT_ADDRESS + (MODULE_IDENT << SIZE_ADDR_REG);

    field.name = "BoardAddress";
    field.size = 8;
    field.min_value = 0;
    field.max_value = 0xff;
    field.setValue = 0;
    fIDReg.push_back(field);
    if (CheckNameExist(field.name)){
        fMapNameId[field.name]=id;
        id++;
    }
}

void TDAQBoard::DefineIDChip(){
    int id=0;
    SFieldReg field;
    field.addr = IDENT_CHIP + (MODULE_IDENT << SIZE_ADDR_REG);

    field.name = "ChipAddress";
    field.size = 32;
    field.min_value = 0;
    field.max_value = 0xffffffff;
    field.setValue = 0;
    fIDChip.push_back(field);
    if (CheckNameExist(field.name)){
        fMapNameId[field.name]=id;
        id++;
    }
}

void TDAQBoard::DefineIDFirmware(){
    int id=0;
    SFieldReg field;
    field.addr = IDENT_FIRMWARE + (MODULE_IDENT << SIZE_ADDR_REG);

    field.name = "Day";
    field.size = 8;
    field.min_value = 0;
    field.max_value = 0xff;
    field.setValue = 0;
    fIDFirmware.push_back(field);
    if (CheckNameExist(field.name)){
        fMapNameId[field.name]=id;
        id++;
    }

    field.name = "Month";
    field.size = 4;
    field.min_value = 0;
    field.max_value = 0xc;
    field.setValue = 0;
    fIDFirmware.push_back(field);
    if (CheckNameExist(field.name)){
        fMapNameId[field.name]=id;
        id++;
    }

    field.name = "Year";
    field.size = 12;
    field.min_value = 0;
    field.max_value = 0xfff;
    field.setValue = 0;
    fIDFirmware.push_back(field);
    if (CheckNameExist(field.name)){
        fMapNameId[field.name]=id;
        id++;
    }

    field.name = "FirmwareType";
    field.size = 8;
    field.min_value = 0;
    field.max_value = 0xff;
    field.setValue = 0;
    fIDFirmware.push_back(field);
    if (CheckNameExist(field.name)){
        fMapNameId[field.name]=id;
        id++;
    }
}

void TDAQBoard::DefineEventIDReg(){
    int id=0;
    SFieldReg field;
    field.addr = READOUT_EVTID1 + (MODULE_READOUT << SIZE_ADDR_REG);

    field.name = "EventID1";
    field.size = 24;
    field.min_value = 0;
    field.max_value = 0xffffff;
    field.setValue = 0;
    fEvtIDReg1.push_back(field);
    if (CheckNameExist(field.name)){
        fMapNameId[field.name]=id;
        id++;
    }

    id=0;
    field.addr = READOUT_EVTID2 + (MODULE_READOUT << SIZE_ADDR_REG);

    field.name = "EventID2";
    field.size = 24;
    field.min_value = 0;
    field.max_value = 0xffffff;
    field.setValue = 0;
    fEvtIDReg2.push_back(field);
    if (CheckNameExist(field.name)){
        fMapNameId[field.name]=id;
        id++;
    }

}

bool TDAQBoard::SendFieldValue(std::string name, std::vector <SFieldReg>& reg, uint32_t value){
    int id=-1;
    bool err;
    uint32_t value_reg=-1;
    id=GetIdField(name);
    if (id ==-1) return false;
    err=SetFieldValue(reg, id, value);
    if(err==false) return false;
    err=GetRegValue(reg,&value_reg);
    if(err==false || value_reg==-1) return false;
    //WriteADCRegister(reg.at(id).addr,value_reg);
    WriteRegister(reg);
    return true;
}

bool TDAQBoard::SendIrefValues(std::vector <SFieldReg>& ADCreg0, std::vector <SFieldReg>& ADCreg1, uint32_t iref0,uint32_t iref1, uint32_t iref2){
    bool err;
    uint32_t value_reg=-1;
    err=SetFieldValue(ADCreg0, 0, iref0);
    if(err==false) return false;
    err=SetFieldValue(ADCreg0, 1, iref1);
    if(err==false) return false;
    err=SetFieldValue(ADCreg1, 0, iref2);
    if(err==false) return false;
    err=GetRegValue(ADCreg0,&value_reg);
    if(err==false || value_reg==-1) return false;
    WriteRegister(ADCreg0);
    err=GetRegValue(ADCreg1,&value_reg);
    if(err==false || value_reg==-1) return false;
    WriteRegister(ADCreg1);
    return true;
}

bool TDAQBoard::SendADCControlReg(std::vector <SFieldReg>& ADCreg0, uint32_t LDOSelfShtdn, uint32_t LDOff){
    bool err;
    uint32_t value_reg=-1;
    err=SetFieldValue(ADCreg0, 2, LDOSelfShtdn);
    if(err==false) return false;
    err=GetRegValue(ADCreg0,&value_reg);
    if(err==false || value_reg==-1) return false;
    err=SetFieldValue(ADCreg0, 3, LDOff);
    if(err==false) return false;
    err=GetRegValue(ADCreg0,&value_reg);
    if(err==false || value_reg==-1) return false;
    WriteRegister(ADCreg0);
    return true;
}

bool TDAQBoard::SendADCConfigReg0(std::vector <SFieldReg>& ADCreg0,uint32_t iref0,uint32_t iref1,uint32_t LDOSelfShtdn, uint32_t LDOff,uint32_t StreamADC, uint32_t ADCSelfStop, uint32_t DisableResetTimeStamp, uint32_t EnablePacketBased){
    bool err;
    uint32_t value_reg=-1;
    err=SetFieldValue(ADCreg0, 0, iref0);
    if(err==false) return false;
    err=SetFieldValue(ADCreg0, 1, iref1);
    if(err==false) return false;
    err=SetFieldValue(ADCreg0, 2, LDOSelfShtdn);
    if(err==false) return false;
    err=SetFieldValue(ADCreg0, 3, LDOff);
    if(err==false) return false;
    err=SetFieldValue(ADCreg0, 4, StreamADC);
    if(err==false) return false;
    err=SetFieldValue(ADCreg0, 5, ADCSelfStop);
    if(err==false) return false;
    err=SetFieldValue(ADCreg0, 6, DisableResetTimeStamp);
    if(err==false) return false;
    err=SetFieldValue(ADCreg0, 7, EnablePacketBased);
    if(err==false) return false;
    err=GetRegValue(ADCreg0,&value_reg);
    if(err==false || value_reg==-1) return false;
    WriteRegister(ADCreg0);
    return true;
}

bool TDAQBoard::SendADCConfigReg1(std::vector <SFieldReg>& ADCreg1,uint32_t iref2){
    bool err;
    uint32_t value_reg=-1;
    err=SetFieldValue(ADCreg1, 0, iref2);
    if(err==false) return false;
    err=GetRegValue(ADCreg1,&value_reg);
    if(err==false || value_reg==-1) return false;
    WriteRegister(ADCreg1);
    return true;
}

bool TDAQBoard::ReadoutADCPacketRawData(int *ANumEv, int *ALength, int AMaxLength, bool *endRun, std::ofstream * AFileRawData,bool write, bool countEV){
    unsigned char data_buf[AMaxLength];
    int length=0;
    int num_ADC_word=6;
    int size_word =4; //byte
    uint32_t lastWord=0;
    *ALength = ReceiveData(ENDPOINT_READ_ADC, data_buf, AMaxLength);
   /* for (int i=0; i< *ALength; i++){
       printf("%02X ",data_buf[i]);
       }
    printf("\n\n");  */
    if(write == true){
        AFileRawData->write((const char * )data_buf,*ALength);
	}
    if(countEV == true){	
        length=*ALength+(fNumLastADCWords*size_word);
        *ANumEv=length / (num_ADC_word*size_word);
        //std::cout <<"Length " <<*ALength << " AMaxLength "<<" fNumLastADCWords "<< fNumLastADCWords << AMaxLength <<std::endl;
        fNumLastADCWords=(length % (num_ADC_word*size_word))/size_word;
        }     
    lastWord = GetIntFromBinaryStringReversed(size_word, data_buf + *ALength - size_word);    
    if (lastWord == 0xcafefade) *endRun = true;
    else *endRun = false;
    return true;
}

bool TDAQBoard::ReadoutADCPacket(int *ANumEv, int *ALength, int AMaxLength, bool *endRun){
    int i=0;
    int currentPos=0;
    unsigned char data_buf[AMaxLength];
    int num_ADC_word=6;
    int size_word =4; //byte
    int count_ev;
    int numLastADCWord=0;
    uint32_t lastWord=0;
    uint32_t AADCWord[num_ADC_word];
    SADCCountData ADCEvent;
    fADCPacket.clear();
    numLastADCWord=fNumLastADCWords;
    *ALength = ReceiveData(ENDPOINT_READ_ADC, data_buf, AMaxLength);
    //std::cout << std::dec << "  Length " << *ALength << "  Max Length " << AMaxLength << std::endl;
    fNumADCEvent=*ALength / (num_ADC_word*size_word);
    fNumLastADCWords=(*ALength % (num_ADC_word*size_word))/size_word;
    //std::cout << "Num Event " <<  fNumADCEvent << " Num Last Word " << fNumLastADCWords << std::endl;
    for (count_ev=0; count_ev<fNumADCEvent; count_ev++){
        if (numLastADCWord >0){
            for (i=0;i<numLastADCWord;i++){
                AADCWord[i]=fLastADCWords[i];
                //std::cout << "Last Word " << AADCWord[i] <<" i " << i << " numLastADCWords "<<fNumLastADCWords <<std::endl;
            }
            for (i=numLastADCWord;i<num_ADC_word;i++){
                AADCWord[i]=GetIntFromBinaryStringReversed(size_word, data_buf + currentPos);
                currentPos +=size_word;
            }
            fNumADCEvent=((*ALength-((num_ADC_word -numLastADCWord)*size_word))/(num_ADC_word*size_word))+1;
            fNumLastADCWords=((*ALength-((num_ADC_word -numLastADCWord)*size_word)) % (num_ADC_word*size_word))/size_word;
            numLastADCWord=0;
        }
        else{
            for (i=0; i<num_ADC_word;i++){
                AADCWord[i]=GetIntFromBinaryStringReversed(size_word, data_buf + currentPos);
                currentPos +=size_word;
                //std::cout << "word[" << i << "] " << AADCWord[i] << std::endl;
            }
        }
        ADCEvent.LDOStatus     = (AADCWord[0] & 0x00000040) >> 6;
        ADCEvent.ModuleAddress = (AADCWord[0] & 0x0000003C) >> 2;
        ADCEvent.DataType      = AADCWord[0]  & 0x00000003;
        ADCEvent.TimeStamp1    = AADCWord[1]  & 0x00ffffff;
        ADCEvent.TimeStamp2    = AADCWord[2]  & 0x00ffffff;
        ADCEvent.Temp_adc0     = AADCWord[3]  & 0x00000fff;
        ADCEvent.MonV_adc1     = (AADCWord[3] & 0x00fff000) >> 12;
        ADCEvent.MonI_adc2     = AADCWord[4]  & 0x00000fff;
        ADCEvent.IDig_adc3     = (AADCWord[4] & 0x00fff000) >> 12;
        ADCEvent.IDigIO_adc4   = AADCWord[5]  & 0x00000fff;
        ADCEvent.IAna_adc5     = (AADCWord[5] & 0x00fff000) >> 12;
        fADCPacket.push_back(ADCEvent);
        /*std::cout <<  " LDO Status "    << ADCEvent.LDOStatus;
          std::cout <<  " ModuleAddress " << ADCEvent.ModuleAddress;
          std::cout <<  " DataType "      << ADCEvent.DataType;
          std::cout <<  " TimeStamp1 "    << ADCEvent.TimeStamp1;
          std::cout <<  " TimeStamp2 "    << ADCEvent.TimeStamp2;
          std::cout <<  " Temp_adc0 "     << ADCEvent.Temp_adc0;
          std::cout <<  " MonV_adc1 "     << ADCEvent.MonV_adc1;
          std::cout <<  " MonI_adc2 "     << ADCEvent.MonI_adc2;
          std::cout <<  " IDig_adc3 "     << ADCEvent.IDig_adc3;
          std::cout <<  " IDigIO_adc4 "   << ADCEvent.IDigIO_adc4;
          std::cout <<  " IAna_adc5 "     << ADCEvent.IAna_adc5 << std::endl;*/
    }

    if (fNumLastADCWords>0){
        for (i=0; i< fNumLastADCWords; i++){
            fLastADCWords[i]=GetIntFromBinaryStringReversed(size_word, data_buf + currentPos);
            //std::cout << "Last Word " << std::hex << fLastADCWords[i] << std::dec << " i " << i << " fNumLastADCWords "<<fNumLastADCWords <<std::endl;
            currentPos +=size_word;
        }
    }
    fNumADCEvent=count_ev;
    *ANumEv=fNumADCEvent;
    //std::cout << "Num Events  " << fNumADCEvent << std::endl;
    lastWord = GetIntFromBinaryStringReversed(size_word, data_buf + *ALength - size_word);
    if (lastWord == 0xcafefade) *endRun = true;
    else *endRun = false;
    return true;
}

void TDAQBoard::CreateCSVFileADC_RO(const char * fileName){
    fADCFileCSV_RO.open (fileName, std::ofstream::out);
    fADCFileCSV_RO << "rownum;Time_Stamp[47:0](dec);ADC0[11:0](dec);ADC1[11:0](dec);ADC2[11:0](dec);ADC3[11:0](dec);ADC4[11:0](dec);ADC5[11:0](dec);";
    fADCFileCSV_RO <<"LDOStat[0];DataType;ModuleAddress(dec);";
    fADCFileCSV_RO << "Time_Stamp[47:0](hex);ADC0[11:0](hex);ADC1[11:0](hex);ADC2[11:0](hex);ADC3[11:0](hex);ADC4[11:0](hex);ADC5[11:0](hex)";
    fADCFileCSV_RO << std::endl;
}

void TDAQBoard::AddEventCSVFileADC_RO(int num_event){
    num_event +=1;
    for(int i=0;i< fNumADCEvent; i++){
        fADCFileCSV_RO << std::dec << num_event <<";" << GetADCTimeStamp(i) << ";";
        fADCFileCSV_RO << std::dec << GetADC0(i) << ";" << GetADC1(i) << ";" << GetADC2(i)  << ";" << GetADC3(i) << ";" << GetADC4(i) << ";" << GetADC5(i) << ";";
        fADCFileCSV_RO << std::dec << GetADCLDOstatus(i) << ";" << GetADCDataType(i) << ";" << GetADCModuleAddress(i) << ";";
        fADCFileCSV_RO << std::hex << GetADCTimeStamp(i) << ";";
        fADCFileCSV_RO << std::hex << GetADC0(i) << ";" << GetADC1(i) << ";" << GetADC2(i)  << ";" << GetADC3(i) << ";" << GetADC4(i) << ";" << GetADC5(i) << ";";
        //std::cout << "num_event  " << num_event << " fNumADCEvent2  " << fNumADCEvent << " i "<< i << std::endl ;
        fADCFileCSV_RO << std::endl;
        num_event++;
    }
}

void TDAQBoard::DumpADCWords (int id_event){
    std::cout <<  "LDO Status "      << std::hex <<  GetADCLDOstatus(id_event);
    std::cout <<  " Module Address " << std::hex <<  GetADCModuleAddress(id_event);
    std::cout <<  " DataType "       << std::hex <<  GetADCDataType(id_event);
    std::cout <<  " Time stamp "     << std::hex <<  GetADCTimeStamp(id_event);
    std::cout <<  " Time stamp 1 "   << std::hex <<  GetADCTimeStamp1(id_event);
    std::cout <<  " Time stamp 2 "   << std::hex <<  GetADCTimeStamp2(id_event);
    std::cout <<  " ADC0 "	      << std::hex <<  GetADC0(id_event);
    std::cout <<  " ADC1 "	      << std::hex <<  GetADC1(id_event);
    std::cout <<  " ADC2 "	      << std::hex <<  GetADC2(id_event);
    std::cout <<  " ADC3 "	      << std::hex <<  GetADC3(id_event);
    std::cout <<  " ADC4 "	      << std::hex <<  GetADC4(id_event);
    std::cout <<  " ADC5 "	      << std::hex <<  GetADC5(id_event);
    std::cout << std::dec << std::endl;
}

bool TDAQBoard::SendStartStreamDataADC(std::vector <SFieldReg>& ADCreg0){
    bool err;
    err= SendFieldValue("EnStreamADCData",ADCreg0,1);
    return err;
}


bool TDAQBoard::SendEndStreamDataADC(std::vector <SFieldReg>& ADCreg0){
    bool err;
    err = SendFieldValue("EnStreamADCData",ADCreg0,0);
    return err;
}

bool TDAQBoard::CheckNameExist(std::string name){
    std::map<std::string,int>::iterator iter = fMapNameId.find(name);
    if (iter == fMapNameId.end()){
	    return true;
	}
	std::cout << "Error: The name of field already eixst !!!" << std::endl;
    return false;
}

int TDAQBoard::GetIdField(std::string name){
    int id=-1;
    std::map<std::string,int>::iterator iter = fMapNameId.find(name);
    if (iter != fMapNameId.end()) id= iter->second;
    else std::cout << "Error: Field "<< name <<" don't found !!!" << std::endl;
    return id;
}

void TDAQBoard::DumpMap(){

    std::cout << "Dump Map Name Id" << std::endl;
    for (std::map<std::string,int>::iterator iter = fMapNameId.begin(); iter!=fMapNameId.end(); ++iter){
        std::cout << iter->first << " => " << iter->second << std::endl;
    }
}


/*
 * The function divides an integer in bytes and put the result in a string. Reverts the byte order
 */
void TDAQBoard::GetBinaryStringFromInt(unsigned char * binStr, uint32_t number, int sizeWord){
/*	int numByte=0;
	uint32_t byteValue=0;
	uint32_t tmpvalue=0;
	unsigned char databin[sizeWord];
	int exp=0;
	int pos=0;
	int pos_zero=0;
	numByte=GetNumberByte(number);
	exp=numByte-1;
	pos_zero=sizeWord;
	while(pos_zero > numByte){
	databin[pos]=0;
	pos++;
	pos_zero--;
	}
	while (exp!=-1){
    byteValue=((number - tmpvalue) >> (8*exp));
    tmpvalue=(tmpvalue + byteValue) << (8*exp);
    exp--;
    databin[pos]=(unsigned int)byteValue;
    pos++;
    }
    databin[pos]='\0';
// Invert Byte
int k=3;
for(int i=0;i<sizeWord;i++){
binStr[k]=databin[i];
k--;
}
databin[pos]='\0';*/


    for (int i=0; i<sizeWord; i++) {
        binStr[i] = number & 0xff;
        number >>= 8;
    }

}


/*
 * The function converts a string 32 bit in integer. Keeps the byte order
 */
uint32_t TDAQBoard::GetIntFromBinaryString(int numByte, unsigned char *str){
    uint32_t number=0;
    int pos=0;
    int exp = numByte -1;
    while (pos < numByte){
    	number= number + (uint32_t)(str[pos] << 8*exp);
    	exp--;
    	pos++;
    }
    return number;
}


uint32_t TDAQBoard::GetIntFromBinaryStringReversed(int numByte, unsigned char *str){
    uint32_t number = 0;
    int      pos    = 0;
    while (pos < numByte){
    	number= number + (uint32_t)(str[pos] << 8*pos);
    	pos++;
    }
    return number;
}


/*
 * Get number of byte to rappresent a decimal number in a binary string.
 */
int TDAQBoard::GetNumberByte(int number){
    int numByte = 1;
    uint32_t num;
    num=255;
    if (number > 0 ){
        while (number > num){
            num = 255 + (num * 255);
            numByte++;
        }
        return numByte;
    }
    return -1;
}

bool TDAQBoard::SetFieldValue(std::vector <SFieldReg>& reg, int id, uint32_t value_reg){
    if (value_reg < reg.at(id).min_value || value_reg > reg.at(id).max_value){
        std::cout << "Error: the value of" << reg.at(id).name <<  "must be a value between" << reg.at(id).min_value << "and" << reg.at(id).max_value << "(requested was " << value_reg << ")"<< std::endl;
        return false;
    }
    if (id < 0 || id > reg.size()) {
        std::cout << "Error: the id value of" << reg.at(id).name <<  "must be a value between 0 and" << reg.size() << std::endl;
        return false;
    }
    reg.at(id).setValue=value_reg;
    return true;
}

// value will be contains the integer value of register
bool TDAQBoard::GetRegValue(std::vector <SFieldReg> reg,uint32_t * value){
    uint32_t calc_value=-1;
    int size=0;
    if(reg.size() < 1) return false;
    size=0;
    for (int i = 0; i<reg.size(); i++){
        if(i==0){
            calc_value=reg.at(i).setValue;
        }
        if(i>0){
            size=size+reg.at(i-1).size;
            calc_value=calc_value + (reg.at(i).setValue << size);
        }
        *value= calc_value;
    }
    return true;
}

uint32_t TDAQBoard::GetMaskBit(int sizeField, int numBitBefore){
    uint32_t bitMask=1;
    if (sizeField <=0 || numBitBefore < 0){
        std::cout << "Error: The size of field or the number of bit is not correct !!!."<< std::endl;
        return 0;
    }
    for (int i=1;i<sizeField;i++){
        bitMask= (bitMask << 1) +1;
    }
    bitMask=bitMask << numBitBefore;
    return bitMask;
}

bool TDAQBoard::DecodeStringRead(std::vector <SFieldReg>& reg,uint32_t value){
    int numBitBefore=0;
    uint32_t value_field=0;
    for (int i=0; i< reg.size(); i++){
        value_field = (value & GetMaskBit(reg.at(i).size, numBitBefore)) >> numBitBefore;
        numBitBefore += reg.at(i).size;
        reg.at(i).readValue = value_field;
#ifdef MYDEBUG
        std::cout << std::dec<<reg.at(i).name << " : \t" << reg.at(i).readValue << std::endl;
#endif
        value_field=0;
    }
    return true;
}

void TDAQBoard::SplitStringRead(unsigned char *string_read, unsigned char *header, unsigned char *data){
    int i,k;
#ifdef MYDEBUG
    printf ("\nWordHeader:");
#endif
    k=0;
    for (i=3; i>=0; i--){
        header[k]=string_read[i];
#ifdef MYDEBUG
        printf ("%02X", header[k]);
#endif
        k++;
    }
    //header[k]='\0';
    k=0;
#ifdef MYDEBUG
    printf ("\nWordData:");
#endif
    for (i=7; i>=4; i--){
        data[k]=string_read[i];
#ifdef MYDEBUG
        printf ("%02X", data[k]);
#endif
        k++;
    }
#ifdef MYDEBUG
    printf("\n");
#endif
    //data[k]='\0';
}

bool TDAQBoard::SendWord(uint32_t value){
    int err;
    unsigned char data_buf[SIZE_WORD];
#ifdef MYDEBUG
    std::cout << std::hex << "SendWord: Send Value: 0x" << value << std::dec << std::endl;
#endif
    GetBinaryStringFromInt(data_buf, value,SIZE_WORD);
    err=SendData (ENDPOINT_WRITE_REG,data_buf,SIZE_WORD);
    if(err<0) return false;
    return true;
}

bool TDAQBoard::ReadAck(){
    uint32_t receiveValueHeader=-1;
    uint32_t receiveValueData=-1;
    unsigned char data_buf[SIZE_WORD*2];
    unsigned char header[SIZE_WORD];
    unsigned char data[SIZE_WORD];
    int err;

    err=ReceiveData(ENDPOINT_READ_REG, data_buf,SIZE_WORD*2);
    if(err<0) return false;
    SplitStringRead(data_buf,header,data);
    receiveValueHeader=GetIntFromBinaryString(SIZE_WORD,header);
#ifdef MYDEBUG
    std::cout << std::hex << "ReadAck: Header Value: 0x" << receiveValueHeader << std::dec << std::endl;
#endif
    receiveValueData=GetIntFromBinaryString(SIZE_WORD,data);
#ifdef MYDEBUG
    std::cout << std::hex << "ReadAck: Data Value: 0x" << receiveValueData << std::dec << std::endl;
#endif
    DecodeStringRead(fAckHeader,receiveValueHeader);
    DecodeStringRead(fAckData,receiveValueData);
    return true;
}

bool TDAQBoard::ReadRegister(std::vector <SFieldReg>& reg, uint32_t *ReadValue){
    uint32_t receiveValueHeader=-1;
    uint32_t receiveValueData=-1;
    unsigned char data_buf[SIZE_WORD*2];
    unsigned char header[SIZE_WORD];
    unsigned char data[SIZE_WORD];
    uint32_t command;
    int err;
    command = reg.at(0).addr + (1 << (SIZE_ADDR_REG + SIZE_ADDR_MODULE));
    // std::cout << std::hex << "Send Value: 0x" << command << std::endl;
    GetBinaryStringFromInt(data_buf, command,SIZE_WORD);
    err=SendData (ENDPOINT_WRITE_REG,data_buf,SIZE_WORD);
    if(err<0) return false;
    err=ReceiveData(ENDPOINT_READ_REG, data_buf,SIZE_WORD*2);
    if(err<0) return false;
    SplitStringRead(data_buf,header,data);
    receiveValueHeader=GetIntFromBinaryString(SIZE_WORD,header);
    //std::cout << std::hex << "Header Value: 0x" << receiveValueHeader << std::endl;
    receiveValueData=GetIntFromBinaryString(SIZE_WORD,data);
    //std::cout << std::hex << "Read Value: 0x" << receiveValueData << std::endl;
    *ReadValue = receiveValueData;
    DecodeStringRead(fAckHeader,receiveValueHeader);
    DecodeStringRead(reg,receiveValueData);
    return true;
}


bool TDAQBoard::ReadRegister(int AAddress, uint32_t *AValue){
    uint32_t receiveValueHeader=-1;
    uint32_t receiveValueData=-1;
    unsigned char data_buf[SIZE_WORD*2];
    unsigned char header[SIZE_WORD];
    unsigned char data[SIZE_WORD];
    uint32_t command;
    int err;
    command =  AAddress + (1 << (SIZE_ADDR_REG + SIZE_ADDR_MODULE));
    // std::cout << std::hex << "Send Value: 0x" << command << std::dec << std::endl;
    GetBinaryStringFromInt(data_buf, command,SIZE_WORD);
    err=SendData (ENDPOINT_WRITE_REG,data_buf,SIZE_WORD);
    if(err<0) return false;
    err=ReceiveData(ENDPOINT_READ_REG, data_buf,SIZE_WORD*2);
    if(err<0) return false;
    SplitStringRead(data_buf,header,data);
    receiveValueHeader=GetIntFromBinaryString(SIZE_WORD,header);
    //std::cout << std::hex << "Header Value: 0x" << receiveValueHeader << std::dec << std::endl;
    receiveValueData=GetIntFromBinaryString(SIZE_WORD,data);
    //std::cout << std::hex << "Read Value: 0x" << receiveValueData << std::dec << std::endl;
    *AValue = receiveValueData;
    return true;
}


void TDAQBoard::PrintVersionFirmware(){
    uint32_t readValue;
    ReadRegister(fIDFirmware, &readValue);
    std::cout << "***************************** FIRMWARE VERSION *******************************"<< std::endl;
    std::cout << "* Code:    " << std::hex <<  readValue <<  "                                                           *" << std::dec << std::endl;
    if (fIDFirmware.at(3).readValue == 0)  std::cout << "* Type:    Event Based with Onboard Clock                                    *" <<std::endl;
    if (fIDFirmware.at(3).readValue == 1)  std::cout << "* Type:    Event Based with External Clock                                   *" <<std::endl;
    if (fIDFirmware.at(3).readValue == 2)  std::cout << "* Type:    Packet Based with Onboard Clock                                   *" <<std::endl;
    if (fIDFirmware.at(3).readValue == 3)  std::cout << "* Type:    Packet Based with External Clock                                  *" <<std::endl;
    if (fIDFirmware.at(3).readValue == 4)  std::cout << "* Type:    Marged Event Based / Packet Based (selectable) with Onboard Clock *" <<std::endl;
    if (fIDFirmware.at(3).readValue == 5)  std::cout << "* Type:    Marged Event Based / Packet Based (selectable) with Onboard Clock *" <<std::endl;
    std::cout <<  std::dec << "* Date:    "  <<  fIDFirmware.at(0).readValue<< "/" << fIDFirmware.at(1).readValue << "/" <<  fIDFirmware.at(2).readValue<<  "                                                          *" << std::endl;
    std::cout <<"******************************************************************************" <<std::endl;
}


std::string TDAQBoard::GetFirmwareName() {
    char buffer[100];
    uint32_t readValue;
    ReadRegister(fIDFirmware, &readValue);

    sprintf(buffer, "%x %d/%d/%d", readValue, fIDFirmware.at(0).readValue, fIDFirmware.at(1).readValue, fIDFirmware.at(2).readValue);
    return buffer;
}


void TDAQBoard::ReadFirmwareVersion() {
    uint32_t readValue;
    ReadRegister(fIDFirmware, &readValue);
    fFirmwareVersion = readValue;
}


TDUTType TDAQBoard::GetFWChipVersion() {
    int Version = (fFirmwareVersion >> 28) & 0xf;
    if (Version == 0) {
        return DUT_PALPIDEFS1;
    }
    else if (Version == 1) {
        return DUT_PALPIDEFS2;
    }
    else if (Version == 2) {
        return DUT_PALPIDEFS3;
    }
    else if (Version == 3) {
        return DUT_PALPIDEFS4;
    }
    else return DUT_UNKNOWN;
}


bool TDAQBoard::WriteRegister(std::vector <SFieldReg>& reg){
    uint32_t command[2];
    bool err;
    uint32_t value;//-1;

    err=GetRegValue(reg,&value);
    if(err==false) return false; // || value==-1) return false;
    command[0] = reg.at(0).addr;
    command[1] = value;
    //std::cout << "[FPGA] ADDRESS: " << std::hex <<  command[0] << " VALUE " << command[1] << std::dec << std::endl;

    #ifdef CPDEBUG
        //std::cout << "Write_1 [FPGA] ADDRESS: " << std::hex <<  command[0] << " VALUE " << command[1] << std::dec << std::endl;
        //std::cout <<"Press Enter to proceed _ " << std::endl;
        //std::cin.get();
    #endif

//Send Instruction Word
    err=SendWord(command[0]);
    if(err==false) return false;
// Send Data Word
    err=SendWord(command[1]);
    if(err==false) return false;
// Read ack Word
    err=ReadAck();
    if(err==false) return false;
    return true;
}


bool TDAQBoard::WriteRegister(int AAddress, int AValue) {
    bool err;
    //std::cout << "[FPGA] ADDRESS: " << std::hex <<  AAddress << " VALUE " << AValue << std::dec << std::endl;

    #ifdef CPDEBUG
        /*
        std::cout << "Write_2 [FPGA] ADDRESS: " << std::hex <<  AAddress << " VALUE " << AValue << std::endl;
        // added debug ALPIDE3
        if (
            //( (AAddress==0x504) and (AValue==0x10) ) or (AAddress==0x302) or (AAddress==0x303)
            //or (AAddress==0x501)
            //or (AAddress==0x502)
            //or (AAddress==0x503)
            (AAddress==0x504)
            or (AAddress==0x302)
            //or (AAddress==0x505)
            //or (AAddress==0x506)
            or (AAddress==0x400)
            ){
                std::cout <<"Press Enter to proceed _ " << std::endl;
                //std::cin.get();
            }
         */
    #endif

    err = SendWord(AAddress);
    if (!err) return false;
    err = SendWord(AValue);
    if (!err) return false;
    err = ReadAck();
    if (!err) return false;

    return true;
}


int TDAQBoard::GetBoardAddress() {
    uint32_t ReadValue;
    bool err = ReadRegister(fIDReg, &ReadValue);
    if (err) return (~ReadValue) & 0x0f;
    return -1;
}


bool TDAQBoard::GetLDOStatus(int &AOverflow) {
    uint32_t ReadValue;
    bool     err, reg0, reg1, reg2;

    err  = ReadRegister (fADCRead0, &ReadValue);
    reg0 = ((ReadValue & 0x1000000) != 0);
    err  = ReadRegister (fADCRead1, &ReadValue);
    reg1 = ((ReadValue & 0x1000000) != 0);
    err  = ReadRegister (fADCRead2, &ReadValue);
    reg2 = ((ReadValue & 0x1000000) != 0);

    err = ReadRegister((MODULE_ADC << SIZE_ADDR_REG) + ADC_OVERFLOW, &ReadValue);

    AOverflow = (int) ReadValue;

    if (! (reg0 & reg1 & reg2))
        std::cout << "GetLDOStatus, LDO status = " << reg0 << ", " << reg1 << ", " << reg2 << std::endl;
    return ( reg0& reg1 & reg2);
}


void TDAQBoard::DecodeOverflow  (int AOverflow) {
    if (AOverflow & 0x1) {
        std::cout << "Overflow in digital current" << std::endl;
    }
    if (AOverflow & 0x2) {
        std::cout << "Overflow in digital I/O current" << std::endl;
    }
    if (AOverflow & 0x4) {
        std::cout << "Overflow in analogue current" << std::endl;
    }
}


uint32_t TDAQBoard::ReadADC(std::vector <SFieldReg>& reg, int id){
    uint32_t readValue;
    bool err;
    err=ReadRegister(reg, &readValue);
    if (err== false) return -1;
    return reg.at(id).readValue;

}


void TDAQBoard::ReadAllADCs()
{
    std::cout << std::dec;
    std::cout << "Read ADC: NTC              = " << GetTemperature() - 273.15 << " deg C" << std::endl;
    //    std::cout << "Read ADC: MONV             = " << ReadMonV() << " ADC counts" << std::endl;
    //    std::cout << "Read ADC: MONI             = " << ReadMonI() << " ADC counts" << std::endl;
    std::cout << "Read ADC: I(1.8 V Digital) = " << ReadDigitalI() << " mA" << std::endl;
    std::cout << "Read ADC: I(1.8 V Output)  = " << ReadOutputI() << " mA" << std::endl;
    std::cout << "Read ADC: I(1.8 V Analog)  = " << ReadAnalogI() << " mA" << std::endl;
}


uint32_t TDAQBoard::ReadMonI() {
    return ReadADC(fADCRead1,0);
}


uint32_t TDAQBoard::ReadMonV() {
    return ReadADC(fADCRead0,1);
}

float TDAQBoard::ReadDigitalI() {
    return ADCToCurrent(ReadADC(fADCRead1,1));
}

float TDAQBoard::ReadOutputI() {
    return ADCToCurrent(ReadADC(fADCRead2,0));
}

float TDAQBoard::ReadAnalogI() {
    return ADCToCurrent(ReadADC(fADCRead2,1));
}

float TDAQBoard::GetTemperature() {
    uint32_t Reading = ReadADC(fADCRead0,0);
    //printf("NTC ADC: 0x%08X\n",Reading);
    return ADCToTemperature (Reading);
}


float TDAQBoard::ADCToTemperature (int AValue) {
    float    Temperature, R;
    float    AVDD = 1.8;
    float    R2   = 5100;
    float    B    = 3900;
    float    T0   = 273.15 + 25;
    float    R0   = 10000;

    float Voltage = (float) AValue;
    Voltage       *= 3.3;
    Voltage       /= (1.8 * 4096);

    R           = (AVDD/Voltage) * R2 - R2;   // Voltage divider between NTC and R2
    Temperature = B / (log (R/R0) + B/T0);

    return Temperature;
}


float TDAQBoard::ADCToCurrent (int AValue)
{
    float Result = (float) AValue * 3.3 / 4096.;   // reference voltage 3.3 V, full range 4096
    Result /= 0.1;    // 0.1 Ohm resistor
    Result *= 10;     // / 100 (gain) * 1000 (conversion to mA);
    return Result;
}


int TDAQBoard::CurrentToADC (int ACurrent)
{
    float Result = (float) ACurrent / 100. * 4096. / 3.3;
    //std::cout << "Current to ADC, Result = " << Result << std::endl;
    return (int) Result;
}


int TDAQBoard::WriteChipRegister(int Address, int Value) {
    int32_t command[4];
    bool err;

    #ifdef CPDEBUG
        std::cout << "Write [CHIP] ADDRESS: " << std::hex << Address << " VALUE " << Value <<std::endl;

    if (
        // ALPIDE4
        (Address==0x1109c) // Command Register
        or (Address==0x14109c) // DTU Configuration Register
        ){
            std::cout <<"Press Enter to proceed _ " << std::endl;
            std::cin.get();
        }
    
        // added debug ALPIDE3
        /*
        if (
            // ALPIDE3
            (Address==0xe109c) or (Address==0xf109c) or (Address==0x109c) or (Address==0x4109c) or
            ( (Address==0x500109c) and  (Value==0xe00) )  or
            ( (Address==0x500109c) and  (Value==0x200) )  or
            ( (Address==0x500109c) and (Value==0x600) )  or
            ( (Address==0x501109c) and (Value==0x000) )  or
            ( (Address==0x501109c) and (Value==0x001) )  or
            //( (Address==0x501109c) and (Value==0xc00) ) or
            //(Address==0x501109c) or
            ( (Address==0x1109c) and (Value==0x31) ) )
        {
                std::cout <<"Press Enter to proceed _ " << std::endl;
                //std::cin.get();
        } */
    
    #endif

    //std::cout << "[CHIP] ADDRESS: " << std::hex << Address << " VALUE " << Value << std::dec << std::endl;
    command[0] = DAQ_WRITE_DATA_REG + (MODULE_JTAG << SIZE_ADDR_REG);
    command[1] = Value;
    command[2] = DAQ_WRITE_INSTR_REG + (MODULE_JTAG << SIZE_ADDR_REG);
    command[3] = Address;
    SendWord((int)command[0]);
    SendWord((int)command[1]);
    err=ReadAck();
    if(err==false) return -1;
    SendWord((int)command[2]);
    SendWord((int)command[3]);
    err=ReadAck();
    if(err==false) return -1;
    
    #ifdef CPDEBUG
        // ALPIDE4
        if (
            (Address==0x1109c) // Command Register
            or (Address==0x14109c) // DTU Configuration Register
        ){
            std::cout <<"Done! Press Enter to proceed _ " << std::endl;
            std::cin.get();
        }
    #endif
    
    
    return 1;
}


// This is the old ReadChipRegister function, which should be called for TpAlpide1
int TDAQBoard::ReadChipRegister(int Address,int *Value) {
    int NewAddress = 0x10000 | Address;
    return ReadChipRegisterLL (NewAddress, Value);
}


// This is the (private) low level function, which should be common for Alpide1 and 2
// This function should be called by ReadChipRegister of TDAQBoard / TDAQBoard2
int TDAQBoard::ReadChipRegisterLL(int Address,int *Value) {
    int32_t command[2];
    uint32_t receiveValueHeader=-1;
    uint32_t receiveValueData=-1;
    unsigned char data_buf[SIZE_WORD*2];
    unsigned char header[SIZE_WORD];
    unsigned char data[SIZE_WORD];
    int err;
    command[0] = DAQ_WRITE_INSTR_REG + (MODULE_JTAG << SIZE_ADDR_REG);
    command[1] = Address;
    SendWord((int)command[0]);
    SendWord((int)command[1]);
    err=ReadAck();

    SendWord(0x00001401);   // Word to read the value back from the FPGA

    //std::cout << std::hex << "Send Value: 0x" << command[1] << std::dec << std::endl;
    //GetBinaryStringFromInt(data_buf, command[1],SIZE_WORD);
    //err=SendData (ENDPOINT_WRITE_REG,data_buf,SIZE_WORD);

    if(err<0) return -1;
    //std::cout << "before 2nd ReadAck" << std::endl;
    //ReadAck();
    err=ReceiveData(ENDPOINT_READ_REG, data_buf,SIZE_WORD*2);
    if(err<0) return -1;
    SplitStringRead(data_buf,header,data);
    receiveValueHeader=GetIntFromBinaryString(SIZE_WORD,header);
    //std::cout << std::hex << "Header Value: 0x" << receiveValueHeader << std::dec << std::endl;
    receiveValueData=GetIntFromBinaryString(SIZE_WORD,data);
    //std::cout << std::hex << "Read Value: 0x" << receiveValueData << std::dec << std::endl;
    *Value = receiveValueData;

    #ifdef CPDEBUG
        //std::cout << "READ data --> [CHIP] ADDRESS: " << std::hex << Address << " VALUE_data " << Value <<std::endl;
        //std::cout <<"Press Enter to proceed _ " << std::endl;
        //std::cin.get();
    #endif


    DecodeStringRead(fAckHeader,receiveValueHeader);
    return 1;
}

// Method that sends one of the signals contained in enum TChipSignal to the chip
// ADuration is the number of clock cycles during which the signal is active

int  TDAQBoard::SendChipSignal (TChipSignal ASignal, int ADuration)
{
    bool     err;
    uint32_t PulseAddress;
    bool     rewrite = false;

    // write pulse duration and calculate address for pulsing
    if (ASignal == SIG_PRST) {
        if (ADuration != fPrstDuration) {
            fPrstDuration = ADuration;
            rewrite       = true;
        }
        PulseAddress = (MODULE_RESET << SIZE_ADDR_REG) + RESET_PRST;
    }
    else if (ASignal == SIG_PULSE) {
        if (ADuration != fPulseDuration) {
            fPulseDuration = ADuration;
            rewrite       = true;
        }
        fPulseDuration = ADuration;
        PulseAddress = (MODULE_RESET << SIZE_ADDR_REG) + RESET_PULSE;
    }
    else {
        if (ADuration != fResetDuration) {
            fResetDuration = ADuration;
            rewrite       = true;
        }
        PulseAddress = (MODULE_RESET << SIZE_ADDR_REG) + RESET_DRST;
    }

    if (rewrite) WriteResetDuration();

    // perform actual pulse
    err = WriteRegister(PulseAddress, 16);  // write anything to the pulse register...
    return err;
}


bool TDAQBoard::WriteResetDuration()
{
    uint32_t NewDate  = 0x7DEB05;  // Date when register format was changed in the firmware
    uint32_t NewDate2 = 0x7DEB1C;  // Date when max. length was increased to 65536 clock cycles
    uint32_t Address  = (MODULE_RESET << SIZE_ADDR_REG) + RESET_DURATION;
    uint32_t Value    = 0;

    if (fFirmwareVersion & 0xf0000000) {
        // Not writing reset duration register for pALPIDE-2/3
        return true;
    }

    if (GetFirmwareDate() >= NewDate2) {
        Value |= (fPrstDuration & 0xff) | ((fResetDuration & 0xff) << 8) | ((fPulseDuration & 0xffff) << 16);
    }
    else if (GetFirmwareDate() >= NewDate) {
        if (fPulseDuration > 2047) {
            std::cout << "Warning, pulse duration > 2047 not supported by firmware, set to 2047" << std::endl;
            fPulseDuration = 2047;
        }
        Value |= (fPrstDuration & 0xff) | ((fResetDuration & 0xff) << 8) | ((fPulseDuration & 0x7ff) << 16);
    }
    else {
        if (fPulseDuration > 255) {
            std::cout << "Warning, pulse duration > 255 not supported by firmware, set to 255" << std::endl;
            fPulseDuration = 255;
        }
        Value |= (fPrstDuration & 0xff) | ((fPulseDuration & 0xff) << 8) | ((fResetDuration & 0xff) << 16);
    }
    if (!WriteRegister (Address, Value)) return false;
    return true;
}


bool TDAQBoard::WritePulseDelay()
{
    uint32_t Address = (MODULE_RESET << SIZE_ADDR_REG) + RESET_PULSE_DELAY;
    uint32_t Value = (fPulseDelay & 0xffff) | ((fPulseMode & 0x3) << 16);

    return WriteRegister(Address, Value);
}


// Set all delays for power on sequence in units of 51.2 us
bool TDAQBoard::SetPowerOnSequence (int ADelayShtdn, int ADelayClk, int ADelaySig, int ADelayDrst) {
    int  Value = 0;
    bool err;

    Value |= (ADelayDrst  & 0xff) << 24;
    Value |= (ADelaySig   & 0xff) << 16;
    Value |= (ADelayClk   & 0xff) <<  8;
    Value |= (ADelayShtdn & 0xff);

    err = WriteRegister((MODULE_RESET << SIZE_ADDR_REG) + RESET_DELAYS, Value);
    return err;
}


bool TDAQBoard::PowerOn (int &AOverflow, bool disablePOR)
{
    std::cout << "Voltages off, setting current limits " << std::endl;

    SendADCConfigReg0 (fADCConfigReg0,
                       CurrentToADC(fConfig->LimitDigital),
                       CurrentToADC(fConfig->LimitIO),
                       1,    // LDOSelfShtdn  //1
                       0,    // LDOOff
                       0,    // StreamADC
                       0,    // ADC Self Stop
                       1,    // Disable Reset Time Stamp
                       0);   // Disable ADC Packet Based.
    SendADCConfigReg1 (fADCConfigReg1,
                       CurrentToADC(fConfig->LimitAnalog));

    // Set Delays for power on (in units of 51.2 us): AutoShutdown, Clock enable, Signal enable, Drst-deassertion
    SetPowerOnSequence(fConfig->AutoShutdownTime,
                       fConfig->ClockEnableTime,
                       fConfig->SignalEnableTime,
                       fConfig->DrstTime);

    if (disablePOR) WriteRegister(0x506, 0x1); // Disable power-on reset

    std::cout << std::endl << "Switching on voltages...: " << std::endl;
    SendADCControlReg(fADCConfigReg0, 1, 1);   // register 0, self shutdown = 1, off = 0

    std::cout << std::endl <<  "Reading all ADCs: " << std::endl;
    sleep(1);     // if delay < 1s measurements will be incorrect
    ReadAllADCs();
    return GetLDOStatus(AOverflow);
}

void TDAQBoard::PowerOff ()
{
    SetPowerOnSequence (1, 0, 0, 0);
    SendADCControlReg  (fADCConfigReg0, 1, 0);   // register 0, self shutdown = 1, off = 0
}

bool TDAQBoard::DisableTimeStampReset(bool ADisableTimeStamp)
{
    return SendADCConfigReg0 (fADCConfigReg0,
                              CurrentToADC(fConfig->LimitDigital),
                              CurrentToADC(fConfig->LimitIO),
                              1,
                              0,
                              0,
                              0,
                              ADisableTimeStamp,
                              0);
}


bool TDAQBoard::ConfigurePulse (int ADelay, int ADuration, int AMode)
{
    fPulseDelay    = ADelay;
    fPulseDuration = ADuration;
    fPulseMode     = AMode;

    bool err = WriteResetDuration();
    if (!err) return false;
    else return WritePulseDelay();
}


bool TDAQBoard::ConfigureReadout (int ABufferDepth, bool AFallingEdgeSampling, bool AEnPacketBasedReadout)
{
    int Value = 0;
    if(AEnPacketBasedReadout) fReadoutMode = 1;
    else                      fReadoutMode = 0;

    if (fEventBuffer.size()>0) {
        std::cerr << "ERROR: some data is still in the buffer: " << fEventBuffer.size() << ", 0x" << std::endl;
        for (int iByte=0; iByte<fEventBuffer.size(); ++iByte) {
            std::cout << std::hex << (int)fEventBuffer[iByte] << std::dec << " ";
        }
        std::cout << std::endl;
    }
    fEventBuffer.clear();

    Value |= ((AEnPacketBasedReadout ? 1:0) << 5);
    Value |= ((AFallingEdgeSampling ? 1:0)  << 4);
    Value |= (ABufferDepth & 0xf);

    return WriteRegister((MODULE_READOUT << SIZE_ADDR_REG) + READOUT_CHIP_DATA, Value);
}


bool TDAQBoard::ConfigureTrigger (int ABusyDuration, int ATriggerWidth, int ATriggerMode, int ANumTrigger, int ADelay)
{
    int Value = 0;
    bool err;

    err = WriteRegister((MODULE_TRIGGER << SIZE_ADDR_REG) + TRIG_BUSY_DURATION, ABusyDuration);
    if (!err) return false;

    Value |= (ATriggerWidth & 0xff) << 19;
    Value |= (ATriggerMode  & 0x7)  << 16;
    Value |= (ANumTrigger   & 0xffff);

    err = WriteRegister((MODULE_TRIGGER << SIZE_ADDR_REG) + TRIG_TRIGGER_CONFIG, Value);
    if (!err) return false;
    err = WriteRegister((MODULE_TRIGGER << SIZE_ADDR_REG) + TRIG_DELAY, ADelay);
    return err;
}

bool TDAQBoard::WriteTriggerPulseDelay (int ADelay)
{
    int Value = 0;
    Value = (ADelay  & 0xffffffff);

    return WriteRegister((MODULE_TRIGGER << SIZE_ADDR_REG) + TRIG_PULSE_DELAY, Value);

}

bool TDAQBoard::StartTrigger()
{
    return WriteRegister((MODULE_TRIGGER << SIZE_ADDR_REG) + TRIG_START, 13);
}


bool TDAQBoard::StopTrigger ()
{
    return WriteRegister((MODULE_TRIGGER << SIZE_ADDR_REG) + TRIG_STOP, 13);
}


int TDAQBoard::ReadChipEvent(unsigned char *data_buf, int *ALength, int AMaxLength, int *error /*=0x0*/, unsigned char **debug /*=0x0*/, int *debug_length /*=0x0*/)
{
    //  1: no error
    // -1: general errror, not specifically treated
    // -2: USB timeout
    // -3: stop trigger marker
    if (fReadoutMode == 0) {
        // event based
        *ALength = ReceiveData(ENDPOINT_READ_DATA, data_buf, AMaxLength, error);
        //std::cout << "Received " << *ALength << " bytes." <<std::endl;

        if (*ALength < 1) {
            std::cout << "Error, receive data returned with " << *ALength << std::endl;
            return -1;
        }
    } else if (fReadoutMode == 1) {
        // packet based
        // each packet may contain more than one event. nevertheless the function returns one event at a time. this class acts as intermediate storage for the remaining buffer

        *ALength = 0; // no data read so far
        bool foundMagicWord = false;
        const int nMagicWords = 5;
        unsigned char magicWords[nMagicWords][4] = { { 0xbf, 0xbf, 0xbf, 0xbf },   // pALPIDE-2/3 event trailer
                                                     { 0xaf, 0xaf, 0xaf, 0xaf },   // pALPIDE-2/3 event trailer for truncated event
                                                     { 0xfe, 0xeb, 0xfe, 0xeb },   // stop-trigger marker in the packet-based readout mode
                                                     { 0xef, 0xeb, 0xef, 0xeb },   // stop-trigger marker in the packet-based readout mode (inconsistent timestamp and data fifo)
                                                     { 0xfe, 0xab, 0xfe, 0xab } }; // pALPIDE-1 event trailer
        bool timeout = false;
        int length_tmp = 0;
        do {
            while (length_tmp+4<=fEventBuffer.size() && !foundMagicWord) {
                for (int iMagicWord=0; iMagicWord<nMagicWords; ++iMagicWord) {
                    if (magicWords[iMagicWord][0]==fEventBuffer[length_tmp+0] &&
                        magicWords[iMagicWord][1]==fEventBuffer[length_tmp+1] &&
                        magicWords[iMagicWord][2]==fEventBuffer[length_tmp+2] &&
                        magicWords[iMagicWord][3]==fEventBuffer[length_tmp+3]) {
                        foundMagicWord=true;
                        switch (iMagicWord) {
                        case 1:
                            std::cerr << "Truncated pALPIDE-2/3 event found!" << std::endl;
                            break;
                        case 3:
                            std::cout << "Inconsistent timestamp and data FIFO detected!" << std::endl;
                        case 2:
                            std::cout << "Stop-trigger marker received." << std::endl;
                            //EndOfRun();
                            fEventBuffer.clear();
                            return -3;
                            break;
                        }
                    }
                }
                length_tmp+=4;
            }
            if (!timeout && !foundMagicWord) { // read new data
                unsigned char tmp[1024];
                int tmp_error = 0;
                int packetLength = ReceiveData(ENDPOINT_READ_DATA, tmp, 1024, &tmp_error);

#if 0
                std::cout << "USB RAW (length " << std::dec << packetLength << "): ";
                for (int j=0; j<packetLength; j++)
                    printf("%02x ", tmp[j]);
                std::cout << std::endl;
#endif

                if (debug && debug_length) {
                    *debug = new unsigned char[packetLength];
                    memcpy(*debug, tmp, packetLength);
                    *debug_length = packetLength;
                }
                if (error) {
                    *error = tmp_error;
                }
                if (tmp_error==-7) { // USB timeout
                    timeout=true;
#ifdef MYDEBUG
                    for (int iByte=0; iByte<fEventBuffer.size(); ++iByte) {
                        std::cout << std::hex << (int)fEventBuffer[iByte] << std::dec;
                    }
                    std::cout << std::endl;
#endif
                    return -2;
                }

                if (packetLength < 1) {
                    std::cout << "Error, receive data returned with " << packetLength << std::endl;
                    return -1;
                }
                if (packetLength%4!=0) {
                    std::cout << "Error, received data was not a multiple of 32 bit! Packet length: " << packetLength << " byte" << std::endl;
                    return -1;
                }

                for (int i=0; i<packetLength; i++) {
                    fEventBuffer.push_back(tmp[i]);
                }
            }
        } while (length_tmp<fEventBuffer.size() && !foundMagicWord);

        if (!foundMagicWord) return -1; // did not achieve to read a full event

        *ALength = length_tmp;

        if (*ALength>AMaxLength) {
            std::cerr << "Event to large (" << *ALength << "Byte) to be read with a buffer of " << AMaxLength << "Byte!" << std::endl;
            *ALength=0;
            return -1;
        }

        for (int i=0; i<*ALength; ++i) {
            data_buf[i] = fEventBuffer.front();
            fEventBuffer.pop_front();
        }
    }

#ifdef MYDEBUG
    std::cout << "ReadChipEvent: Received " << *ALength << " bytes, dumping data below:" << std::endl << std::endl;

    std::cout << std::hex;
    for (int i = 0; i < *ALength; i++)
    {
        std::cout << (int) data_buf[i] << " ";
    }
    std::cout << std::dec << std::endl << std::endl;
#endif
    return 1;
}


int TDAQBoard::GetEventHeaderLength() {
    switch(fFirmwareVersion) {
    case 0x257E030A:
    case 0x247E030A:
    case 0x257E031D:
    case 0x247E031D:
        return 36;
        break;
    case 0:
        return -1;
        break;
    case 0x257E0602:
    case 0x247E0602:
    case 0x257E0610:
    case 0x247E0610:
        return 12;
        break;
    case 0x247E0611:
    case 0x347E0803:
    case 0x247E0912:
    case 0x347E0912:
        return (fHeaderFormat==0) ? 12 : 36 ;
        break;
    default:
        return 20;
        break;
    }
    return 20;
}


bool TDAQBoard::DecodeEventHeader  (unsigned char *data_buf, TEventHeader *AHeader) {
    const int length = GetEventHeaderLength()/4;
    int Header[length];
    for (int i = 0; i < length; i++) {
        Header[i] = GetIntFromBinaryStringReversed(4, data_buf + i*4);
#ifdef MYDEBUG
        std::cout << "Header word: 0x" << std::hex << Header[i] << std:: dec << std::endl;
#endif
    }

    return DecodeEventHeader(Header, length, AHeader);
}

bool TDAQBoard::DecodeEventTrailer (unsigned char *data_buf, TEventHeader *AHeader) {
    int Trailer[2];
    for (int i = 0; i < 2; i++) {
        Trailer[i] = GetIntFromBinaryStringReversed(4, data_buf + i*4);
#ifdef MYDEBUG
        std::cout << "Trailer word: 0x" << std::hex << Trailer[i] << std:: dec << std::endl;
#endif
    }

    return DecodeEventTrailer(Trailer, AHeader);
}

bool  TDAQBoard::DecodeEventHeader  (int *Header, int length, TEventHeader *AHeader){
    // all header words are supposed to have a zero MSB
    for (int i=0; i<length; ++i) {
        if (0x80000000 & Header[i]) {
            std::cout << "Corrupt header data, MSB of header word active!" << std::endl;
            std::cout << std::hex << "0x" << Header[i] << "\t0x" << (0x80000000 & Header[i]) << std::dec << std::endl;
            return false;
        }
    }

    bool    AFull       = false;
    int     TrigType    = -1;
    int     BufferDepth = -1;
    uint64_t Event_ID   = (uint64_t)-1;
    uint64_t TimeStamp  = (uint64_t)-1;
    int StrobeCountTotal  = (length>5) ? Header[5] : -1;
    int TrigCountChipBusy = -1;
    int TrigCountDAQbusy  = -1;
    int ExtTrigCounter    = -1;
    if (length==3) {
        switch(fFirmwareVersion) {
        case 0x257E0602:
        case 0x247E0602:
            Event_ID         = (uint64_t)Header[0] & 0x7fffffff;
            TimeStamp        = (uint64_t)Header[1] & 0x7fffffff;
            break;
        case 0x257E0610:
        case 0x247E0610:
        default:
            Event_ID         = (uint64_t)Header[0] & 0x00ffffff;
            //TimeStamp        = (uint64_t)Header[1] & 0x7fffffff | ((uint64_t)Header[0] & 0x7f000000) << 7; // Original
            TimeStamp        = ((uint64_t)Header[1] & 0x7fffffff) | ((uint64_t)Header[0] & 0x7f000000) << 7; // Caterina: added ()
        }
        TrigCountDAQbusy = (Header[2] & 0x7fff0000)>>8;
        StrobeCountTotal = (Header[2] & 0x00007fff);
    }
    else if (length==5 || length==9) {
        AFull       = (bool) (Header[0] & 0x40);
        TrigType    = (Header[0] & 0x1c00) >> 10;
        BufferDepth = (Header[0] & 0x1e000) >> 13;
        Event_ID   = ((uint64_t) Header[1] & 0xffffff) | ( ((uint64_t) Header[2] & 0xffffff) << 24 );
        TimeStamp  = ((uint64_t) Header[3] & 0xffffff) | ( ((uint64_t) Header[4] & 0xffffff) << 24 );
        StrobeCountTotal  = (length>5) ? Header[5] : -1;
        TrigCountChipBusy = (length>6) ? Header[6] : -1;
        TrigCountDAQbusy  = (length>7) ? Header[7] : -1;
        ExtTrigCounter    = (length>8) ? Header[8] : -1;

        // few consistency checks:
        if ((Header[0] & 0xfffe03bf) != 0x8) {
            std::cout << "Corrupt header word 0: 0x" << std::hex << Header[0] << std::dec << std::endl;
            return false;
        }
        if ((Header[1] & 0xff000000) || (Header[2] & 0xff000000) || (Header[3] & 0xff000000)) {
            std::cout << "Corrupt header, missing at least one of the leading 0s in word 1-4" << std::endl;
            return false;
        }
        if ((TrigType < 1) || (TrigType > 2)) {
            std::cout << "Bad Trigger Type " << TrigType << std::endl;
            return false;
        }
    }  
#ifdef MYDEBUG
    std::cout << "Header: Trigger type = " << TrigType << std::endl;
    std::cout << "Header: Almost full  = " << (int) AFull << std::endl;
    std::cout << "Header: Event ID     = " << Event_ID << std::endl;
    std::cout << "Header: Time stamp   = " << TimeStamp << std::endl;
    std::cout << "             in sec  = " << (float)TimeStamp / 8e7 << std::endl;
    std::cout << "Header: Total Strobe Count       = " << StrobeCountTotal << std::endl;
    std::cout << "Header: Trigger Count Chip Busy  = " << TrigCountChipBusy << std::endl;
    std::cout << "Header: Trigger Count DAQ Busy   = " << TrigCountDAQbusy << std::endl;
    std::cout << "Header: External Trigger Count   = " << ExtTrigCounter << std::endl;
#endif
    if (AHeader) {
        AHeader->AFull       = AFull;
        AHeader->BufferDepth = BufferDepth;
        AHeader->EventId     = Event_ID;
        AHeader->TimeStamp   = TimeStamp;
        AHeader->TrigType    = TrigType;
        AHeader->StrobeCountTotal  = StrobeCountTotal;
        AHeader->TrigCountChipBusy = TrigCountChipBusy;
        AHeader->TrigCountDAQbusy  = TrigCountDAQbusy;
        AHeader->ExtTrigCount      = ExtTrigCounter;
    }
    return true;
}

bool TDAQBoard::DecodeEventTrailer (int * Trailer, TEventHeader *ATrailer) {
    if (Trailer[1] != fTrailerWord) {
        std::cout << "Corrupt trailer, expecting 0x " << std::hex << fTrailerWord << ", found 0x" << Trailer[1] << std::dec << std::endl;
        return false;
    }
    int EventSize = Trailer[0];

    if (ATrailer) {
        ATrailer->EventSize = EventSize;
    }
#ifdef MYDEBUG
    std::cout << "Trailer: Event size = " << EventSize << std::endl;
    std::cout << std::hex<< "Trailer: 2 word = " << Trailer[1] <<  std::dec << std::endl;
#endif
    return true;
}

uint64_t TDAQBoard::GetNextEventId  ()
{
    bool     err;
    uint32_t Value1, Value2;
    uint64_t Dummy;
    uint64_t Result = 0;

    err = ReadRegister(fEvtIDReg1, &Value1);
    if (!err) return -1;
    err = ReadRegister(fEvtIDReg2, &Value2);
    if (!err) return -1;

    Value1 &= 0xffffff;
    Value2 &= 0xffffff;

    Dummy  = Value2;
    Result = (Dummy << 24) + Value1;
    return Result;
}


bool TDAQBoard::ResetBoardFPGA (int ADuration)
{
    bool err;
    err = WriteRegister((MODULE_SOFTRESET << SIZE_ADDR_REG) + SOFTRESET_DURATION, ADuration);
    if (!err) return false;
    return WriteRegister((MODULE_SOFTRESET << SIZE_ADDR_REG) + SOFTRESET_FPGA_RESET, 13);
}

bool TDAQBoard::ResetBoardFX3 (int ADuration)
{
    bool err;
    err = WriteRegister((MODULE_SOFTRESET << SIZE_ADDR_REG) + SOFTRESET_DURATION, ADuration);
    if (!err) return false;
    return WriteRegister((MODULE_SOFTRESET << SIZE_ADDR_REG) + SOFTRESET_FX3_RESET, 13);
}


bool TDAQBoard::WriteBusyOverrideReg(bool ABusyOverride)
{
    bool err;
    err = WriteRegister((MODULE_TRIGGER << SIZE_ADDR_REG) + TRIG_BUSY_OVERRIDE, ABusyOverride);
    if (!err) return false;
    return err;
}

bool TDAQBoard::WriteChipIdReg(uint32_t AChipId)
{
    bool err;
    err = WriteRegister((MODULE_IDENT << SIZE_ADDR_REG) + IDENT_CHIP, AChipId);
    if (!err) return false;
    return err;
}


uint32_t TDAQBoard::GetChipId ()
{
    bool err;
    uint32_t Value;
    err = ReadRegister(fIDChip, &Value);
    if (!err) return -1;
    return Value;
}



void TDAQBoard::DecodeADCData (unsigned char *data_buf, TADCData &ADCData) {
    int Data[6];
    for (int i = 0; i < 6; i++) {
        Data[i] = GetIntFromBinaryStringReversed(4, data_buf + i*4);
    }
    ADCData.TimeStamp  = ((uint64_t) Data[1] & 0xffffff) | ( ((uint64_t) Data[2] & 0xffffff) << 24 );

    ADCData.LDOOn      = (bool) (Data[0] & 0x40);

    ADCData.Temp   = ADCToTemperature(Data[3] & 0x000fff);  //  Temperature
    ADCData.MonV   = (Data[3] & 0xfff000) >> 12;  //  MonV
    ADCData.MonI   = Data[4] & 0x000fff;  //  MonI
    ADCData.IDig   = ADCToCurrent((Data[4] & 0xfff000) >> 12);  //  I Digital
    ADCData.IDigIO = ADCToCurrent(Data[5] & 0x000fff);  //  I DigIO
    ADCData.IAna   = ADCToCurrent((Data[5] & 0xfff000) >> 12);  //  I I Analog


}


bool TDAQBoard::ReadMonitorRegisters(){
    ReadMonitorReadoutRegister();
    ReadMonitorTriggerRegister();
    return true; // added Caterina
}

bool TDAQBoard::ReadMonitorReadoutRegister(){
    uint32_t value;
    int addr;
    addr = READOUT_MONITOR1 + (MODULE_READOUT << SIZE_ADDR_REG);
    ReadRegister(addr, &value);
    std::cout << "READOUT_MONITOR1 (0x" << std::hex << addr << "): 0x" << value << std::endl;
    std::cout << " READOUT SM 2: 0x"          << (value & 0x7)         << std::endl;
    std::cout << " EOT SM: 0x"                << ((value >> 3) & 0x7)  << std::endl;
    std::cout << " EOT COUNTER: 0x"           << ((value >> 6) & 0xff) << std::endl;
    std::cout << " TIMESTAMP FIFO EMPTY: "    << ((value >> 14) & 0x1) << std::endl;
    std::cout << " PACKET BASED FLAG: "       << ((value >> 15) & 0x1) << std::endl;
    std::cout << " FIFO 33 BIT EMPTY: "       << ((value >> 16) & 0x1) << std::endl;
    std::cout << " FIFO 32 BIT ALMOST FULL: " << ((value >> 17) & 0x1) << std::endl;
    std::cout << " SM READOUT 1: 0x"          << ((value >> 18) & 0x7) << std::endl;
    std::cout << " FIFO 9 BIT EMPTY: "        << ((value >> 21) & 0x1) << std::endl;
    std::cout << " SM READOUT 3: "            << ((value >> 22) & 0x1) << std::endl;
    std::cout << " FIFO 33 BIT FULL: "        << ((value >> 23) & 0x1) << std::endl;
    std::cout << " SM CTRL WORD DECODER: 0x"  << ((value >> 24) & 0x7) << std::endl;
    std::cout << " FIFO 32 BIT EMPTY: "       << ((value >> 27) & 0x1) << std::dec << std::endl;
    return true;
}


bool TDAQBoard::ReadMonitorTriggerRegister(){
    uint32_t value;
    int addr;
    addr = TRIG_MONITOR1 + (MODULE_TRIGGER<< SIZE_ADDR_REG);
    WriteRegister(addr,0xaaa);
    ReadRegister(addr, &value);
    std::cout <<"TRIG_MONITOR1 (0x" << std::hex << addr <<"): 0x" << value << std::endl;
    std::cout <<" TRIGGER SM: 0x"            << (value & 0x7) << std::endl;
    std::cout <<" BUSY SM: 0x"               << ((value >> 3) & 0x3) << std::endl;
    std::cout <<" STOP TRIGGER COUNTER: 0x"  << ((value >> 5) & 0xff) << std::endl;
    std::cout <<" START TRIGGER COUNTER: 0x" << ((value >> 13) & 0xff) << std::endl;
    std::cout <<" BUSY IN: "                 << ((value >> 21) & 0x1) << std::endl;
    std::cout <<" BUSY PALPIDE: "            << ((value >> 22) & 0x1) << std::endl;
    std::cout <<" CONTROL WORD BUSY: "       << ((value >> 23) & 0x1) << std::endl;
    std::cout <<" BUSY EVENT BUILDER: "      << ((value >> 24) & 0x1) << std::endl;
    std::cout <<" BUSY OVERRIDE FLAG: "      << ((value >> 25) & 0x1) << std::endl;
    std::cout <<" BUSY : "                   << ((value >> 26) & 0x1) << std::dec << std::endl;
    return true;
}


////////////////////////////////////////////////////////////////////////
//                                                                    //
//                    class TDAQBoard2                                //
//              (DAQ Board for pALPIDEfs2)                            //
////////////////////////////////////////////////////////////////////////


TDAQBoard2::TDAQBoard2 (libusb_device *ADevice, TDAQBoardConfig *AConfig) : TDAQBoard (ADevice, AConfig)
{
    if (ADevice && AConfig) {
        if (GetFWChipVersion() == DUT_PALPIDEFS3) {
            //std::cout << "Warning, using DAQBoard2 with ALPIDE-3 firmware" << std::endl;
        }
        else if (GetFWChipVersion() != DUT_PALPIDEFS2) {
            std::cout << "Warning, wrong firmware version detected for DAQBoard 2" << std::endl;
        }

        fDisableManchester = true;
        fCMUFallingEdge    = false;
        fInvertCMUBus      = AConfig->InvertCMU;
        fSetChipSlave      = false;
        fEmulationMode     = 0;

        WriteCMUConfigReg();
        fEnableDoubleDataRate = AConfig->EnableDDR;

        fDataPort = PORT_NONE;
        ConfigureReadout(1,false,false);
        fDataPort  = AConfig->DataPort;
    }

    fTrailerWord = 0xbfbfbfbf;
}


int TDAQBoard2::WriteChipRegister (int Address, int Value, int ChipId) {
    //std::cout << "Writing value 0x" << std::hex << Value << " to chip register 0x" << Address << std::dec << std::endl;
    //std::cout << "[CHIP2-3] ADDRESS: " << std::hex << Address << " VALUE " << Value << std::dec << std::endl;
    int NewAddress = ((Address & 0xffff) << 16) | ((ChipId & 0xff) << 8) | ( TpAlpidefs2::OPCODE_WROP & 0xff);
    return TDAQBoard::WriteChipRegister (NewAddress, Value);
}


int TDAQBoard2::ReadChipRegister (int Address, int *Value, int ChipId) {
    int NewAddress = ((Address & 0xffff) << 16) | ((ChipId & 0xff) << 8) | ( TpAlpidefs2::OPCODE_RDOP & 0xff);
    return ReadChipRegisterLL (NewAddress, Value);
}


int TDAQBoard2::WriteChipRegister (int Address, int Value) {
    std::cout << "Error, method not compatible with pALPIDE-2, call TDut::WriteRegister instead!" << std::endl;
    exit (EXIT_FAILURE);
}


int TDAQBoard2::ReadChipRegister (int Address, int *Value) {
    std::cout << "Error, method not compatible with pALPIDE-2, call TDut::ReadRegister instead!" << std::endl;
    exit (EXIT_FAILURE);
}


int TDAQBoard2::WriteChipCommand  (int OpCode) {
    uint32_t Address = DAQ_WRITE_INSTR_REG + (MODULE_JTAG << SIZE_ADDR_REG);
    return WriteRegister (Address, OpCode);
}


// in DAQBoard2, this should be called only for the PULSE signal, no longer for PRST or GRST
// Also, the duration has to be programmed previously into the corresponding chip register,
// ADuration is kept only for compatibility with TDAQBoard

int TDAQBoard2::SendChipSignal (TChipSignal ASignal, int ADuration) {
    bool     err;
    uint32_t PulseAddress;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    if (ASignal == SIG_PULSE) {
        PulseAddress = (MODULE_RESET << SIZE_ADDR_REG) + RESET_PULSE;
        err          = WriteRegister(PulseAddress, 16);  // write anything to the pulse register...
    }
    else {
        std::cout << "SendChipSignal called for signal " << ASignal << ", something went wrong... " << std::endl;
        err = -1;
    }
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> TimeSpan = duration_cast<duration<double>>(t2 - t1);
    SignalTime += TimeSpan.count();
    return err;
}


bool TDAQBoard2::ConfigurePulse   (int ADelay, int ADuration, int AMode)
{
    fPulseDelay    = ADelay;
    fPulseMode     = AMode;

    return WritePulseDelay();
}


bool TDAQBoard2::WriteCMUConfigReg ()
{
    uint32_t Address, Value;
    Address = (MODULE_JTAG << SIZE_ADDR_REG) + DAQ_CONFIG_REG;
    Value   = (fDisableManchester?1:0) | ((fCMUFallingEdge?1:0) << 1) | ((fInvertCMUBus?1:0) << 2) | ((fSetChipSlave?1:0) << 3);

    return WriteRegister(Address, Value);
}


void TDAQBoard2::WriteI2C (int SlaveAddress, int Command, int Data) {
    uint32_t NAck;
    SlaveAddress *= 2;   // 7 bit address + read/write bit = 0;
    int Value = ((Data & 0xff) << 16) | ((Command & 0xff) << 8) | (SlaveAddress & 0xfe);
    //std::cout << "Writing I2C, Value = 0x"<< std::hex << Value << std::dec << std::endl;
    WriteRegister((MODULE_IDENT << SIZE_ADDR_REG) + IDENT_CHIP, Value);

    usleep (400000);
    ReadRegister((MODULE_IDENT << SIZE_ADDR_REG) + IDENT_ACKCOUNT, &NAck);
    //std::cout << "Counted " << NAck << " Acknowledges" << std::endl;
}


void TDAQBoard2::SetChipIdI2C (int AId) {
    int I2COutputPort  = 0x1;
    int I2CConfig      = 0x3;
    int SlaveAddress   = 0x21;  // for chip carrier card;
    int SlaveAddressPS = 0x20; // for probe card;

    int AddressCC = AId * 2; // carrier card: address shifted by one bit
    int AddressPS = 0;

    if (((AId & 0x7) != 0) && (AId > 15))  {  // smallest outer barrel chip ID = 16
        fSetChipSlave = true;
        WriteCMUConfigReg();
    }
    else {   // chip is either outer barrel master or inner barrel chip
        fSetChipSlave = false;
    }
    for(int i = 0; i < 7; i++)   // bit order is reversed on probe card
        AddressPS |= ((AId>>i) & 1)<<(6-i);

    // Do the same operation twice, once for carrier board, once for the probe station
    // Since the SlaveAddresses are different, the "wrong" version should be ignored by either card

    WriteI2C (SlaveAddress, I2COutputPort, AddressCC);   // set value of output port
    WriteI2C (SlaveAddress, I2CConfig,     0x0);         // define all pins as outputs;

    WriteI2C (SlaveAddressPS, I2COutputPort, AddressPS);   // set value of output port
    WriteI2C (SlaveAddressPS, I2CConfig,     0x0);         // define all pins as outputs;
}


void TDAQBoard2::SelectDataOutI2C (int AChip)   // for IB Stave with adapter board only
{
    int I2COutputPort  = 0x1;
    int I2CConfig      = 0x3;
    int SlaveAddress   = 0x21;
    int Settings[9]    = {3, 2, 1, 0, 4, 5, 6, 7, 8};    // Multiplexer addressing as implemened on adapter card
    if ((AChip > 8) || (AChip < 0)) {
        std::cout << "Error, invalid chip #:" << AChip << std::endl;
        return;
    }
    else {
        WriteI2C (SlaveAddress, I2COutputPort, Settings[AChip]);   // set value of output port
        WriteI2C (SlaveAddress, I2CConfig,     0x0);         // define all pins as outputs;
    }

}


void TDAQBoard2::PowerOff (int i2c)
{
    // Stop DAQ board from reading data port
    fDataPort = PORT_NONE;
    WriteRegister((MODULE_READOUT << SIZE_ADDR_REG) + READOUT_ENDOFRUN, 0xd);
    ConfigureReadout (1, false, false);
    // set chip id lines to low
    if (i2c==1){
        SetChipIdI2C(0);
	}
    // power off
    TDAQBoard::PowerOff();
}


bool TDAQBoard2::ConfigureReadout (int ABufferDepth, bool AFallingEdgeSampling, bool AEnPacketBasedReadout)
{
    int Value = 0;
    if(AEnPacketBasedReadout) fReadoutMode = 1;
    else                      fReadoutMode = 0;

    if (fEventBuffer.size()>0) {
        std::cerr << "ERROR: some data is still in the buffer: " << fEventBuffer.size() << ", 0x" << std::endl;
        for (int iByte=0; iByte<fEventBuffer.size(); ++iByte) {
            std::cout << std::hex << (int)fEventBuffer[iByte] << std::dec << " ";
        }
        std::cout << std::endl;
    }
    fEventBuffer.clear();

    Value |= ((AEnPacketBasedReadout ? 1:0) << 5);
    Value |= ((AFallingEdgeSampling  ? 1:0) << 4);
    Value |= ((fEnableDoubleDataRate ? 1:0) << 6);
    Value |= (((int) fDataPort)             << 7);
    Value |= (ABufferDepth    & 0xf);
    Value |= ((fEmulationMode & 0x3)        << 9);
    if (fFirmwareVersion>=0x247E0611) {
        if (fHeaderFormat==0)         Value |= 0x0800; // short header
        if (fConfig->BoardVersion==3) Value |= 0x1000; // DAQ board v3
    }

    //std::cout << "Configuring readout, writing event builder config, value = 0x" << std::hex << Value << std::dec << std::endl;
    if (fDataPort == PORT_SERIAL) ResyncReadout();

    return WriteRegister((MODULE_READOUT << SIZE_ADDR_REG) + READOUT_CHIP_DATA, Value);
}


bool TDAQBoard2::ResyncReadout ()
{
    return WriteRegister((MODULE_READOUT << SIZE_ADDR_REG) + READOUT_RESYNC, 0x0);
}


bool TDAQBoard2::WriteSlaveDataEmulator(int AWord) {
    AWord &= 0xffffffff;
    return WriteRegister((MODULE_READOUT << SIZE_ADDR_REG) + READOUT_SLAVE_DATA_EMULATOR, AWord);
}

void TDAQBoard2::ReadAllRegisters() {
    for (int i_module=0; i_module<8; ++i_module) {
        for (int i_reg=0; i_reg<8; ++i_reg) {
            uint32_t value = -1;
            int address = (i_module&0xf)<<8 | (i_reg&0xff);
            ReadRegister(address, &value);
            std::cout << i_module << '\t' << i_reg << "\t0x" << std::hex << address << ":\t0x" << value << std::dec << std::endl;
        }
    }
}
