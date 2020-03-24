//
//  decodeADCData.cpp
//  pALPIDEfs-software
//
//  Created by Carlo Puggioni on 20/11/14.
//  Copyright (c) 2014 Carlo Puggioni. All rights reserved.
//

#include <iostream>
#include <fstream> 
#include "stdio.h"
#include <stdint.h>
#include <stdio.h>      /* printf */
#include <time.h>       /* time_t, struct tm, time, localtime, asctime */
#include <unistd.h>
#include <stdlib.h>
#include <cstring>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

char myArgv[10][50];
int myArgc=0;
std::ifstream fFileData;
std::ofstream fADCFileCSV_RO;
unsigned long long fTimeStampPrev=0;
int fMode = 0;
struct SADCDataDecoder {
   bool 	     LDOStatus;
   unsigned int      ModuleAddress;
   unsigned short    DataType;
   uint32_t	     TimeStamp1;
   uint32_t	     TimeStamp2;
   unsigned short    Temp_adc0;
   unsigned short    MonV_adc1;
   unsigned short    MonI_adc2;
   unsigned short    IDig_adc3;
   unsigned short    IDigIO_adc4;
   unsigned short    IAna_adc5;
   };   
SADCDataDecoder fADCEvent; 

uint32_t GetADCLDOstatus()     {return fADCEvent.LDOStatus;}   
uint32_t GetADCModuleAddress() {return fADCEvent.ModuleAddress;}
uint32_t GetADCDataType()      {return fADCEvent.DataType;}
uint32_t GetADCTimeStamp1()    {return fADCEvent.TimeStamp1;}
uint32_t GetADCTimeStamp2()    {return fADCEvent.TimeStamp2;}
uint32_t GetADC0()  	       {return fADCEvent.Temp_adc0;}
uint32_t GetADC1()  	       {return fADCEvent.MonV_adc1;}
uint32_t GetADC2()  	       {return fADCEvent.MonI_adc2;}
uint32_t GetADC3()  	       {return fADCEvent.IDig_adc3;}
uint32_t GetADC4()  	       {return fADCEvent.IDigIO_adc4;}
uint32_t GetADC5()  	       {return fADCEvent.IAna_adc5;}
 unsigned long long GetADCTimeStamp() {return (((unsigned long long)(GetADCTimeStamp2())) << 24) + GetADCTimeStamp1();}    
// 
//---------------------------------------------------------------------------------------------
bool SetPathFile(const char * fileName){
    fFileData.open(fileName, std::ifstream::binary); //open the file 
//Check if the File is open.    
    if (fFileData.is_open()){
        std::cout << " INFO:Load ADC Raw Data File:" << fileName << std::endl; 
        return true;
	}
    std::cout <<"ERROR: The ADC Raw Data File " << fileName << "don't exsist !!! " << std::endl;
    return false;   
    }
    
// Size of the File
// Return -1 if the file not exisist otherwise return the size of File (Number of Byte)
//---------------------------------------------------------------------------------------------
int GetSizeFile(){
    int size;
    if (fFileData.is_open()){
        fFileData.seekg (0, fFileData.end);
        size = (int) fFileData.tellg();
        fFileData.seekg (0, fFileData.beg);
	std::cout << "INFO: Size Data File: " << size << std::endl;
	return size;
	}
    return -1;	
    }
    
// 
//---------------------------------------------------------------------------------------------
uint32_t GetIntFromBinaryStringReversed(int numByte, unsigned char *str){
    uint32_t number = 0;
    int      pos    = 0;
    while (pos < numByte){
    	number= number + (uint32_t)(str[pos] << 8*pos);
    	pos++;
    }
    return number;
}
    
// 
//---------------------------------------------------------------------------------------------
void checkTimeStamp(int num_event){
   if (fTimeStampPrev != 0 && (GetADCTimeStamp() - fTimeStampPrev) !=67 && fMode == 1 ){ 
      std::cout << "WARNING TIMESTAMP: Event number " << num_event << " Difference between timestamp " << GetADCTimeStamp() - fTimeStampPrev << std::endl;
      }
   fTimeStampPrev=GetADCTimeStamp();	    
}
// 
//---------------------------------------------------------------------------------------------
void CreateCSVFileADC_RO(const char *fileName){
    fADCFileCSV_RO.open (fileName, std::ofstream::out);
    fADCFileCSV_RO << "rownum;Time_Stamp[47:0](dec);ADC0[11:0](dec);ADC1[11:0](dec);ADC2[11:0](dec);ADC3[11:0](dec);ADC4[11:0](dec);ADC5[11:0](dec);"; 
    fADCFileCSV_RO <<"LDOStat[0];DataType;ModuleAddress(dec);";
    fADCFileCSV_RO << "Time_Stamp[47:0](hex);ADC0[11:0](hex);ADC1[11:0](hex);ADC2[11:0](hex);ADC3[11:0](hex);ADC4[11:0](hex);ADC5[11:0](hex)";
    fADCFileCSV_RO << std::endl;
    }
// 
//---------------------------------------------------------------------------------------------    
void AddEventCSVFileADC_RO(int num_event){
    fADCFileCSV_RO << std::dec << num_event <<";" << GetADCTimeStamp() << ";";
    fADCFileCSV_RO << std::dec << GetADC0() << ";" << GetADC1() << ";" << GetADC2()  << ";" << GetADC3() << ";" << GetADC4() << ";" << GetADC5() << ";";
    fADCFileCSV_RO << std::dec << GetADCLDOstatus() << ";" << GetADCDataType() << ";" << GetADCModuleAddress() << ";";
    fADCFileCSV_RO << std::hex << GetADCTimeStamp() << ";";
    fADCFileCSV_RO << std::hex << GetADC0() << ";" << GetADC1() << ";" << GetADC2()  << ";" << GetADC3() << ";" << GetADC4() << ";" << GetADC5() << ";";
    fADCFileCSV_RO << std::endl;
    checkTimeStamp(num_event);
    }
// 
//---------------------------------------------------------------------------------------------     
void DumpADCWords (){
     std::cout <<  "LDO Status "      << std::hex <<  GetADCLDOstatus();
     std::cout <<  " Module Address " << std::hex <<  GetADCModuleAddress();
     std::cout <<  " DataType "       << std::hex <<  GetADCDataType();
     std::cout <<  " Time stamp "     << std::hex <<  GetADCTimeStamp();
     std::cout <<  " Time stamp 1 "   << std::hex <<  GetADCTimeStamp1();
     std::cout <<  " Time stamp 2 "   << std::hex <<  GetADCTimeStamp2();
     std::cout <<  " ADC0 "	      << std::hex <<  GetADC0();
     std::cout <<  " ADC1 "	      << std::hex <<  GetADC1();
     std::cout <<  " ADC2 "	      << std::hex <<  GetADC2();
     std::cout <<  " ADC3 "	      << std::hex <<  GetADC3();
     std::cout <<  " ADC4 "	      << std::hex <<  GetADC4();
     std::cout <<  " ADC5 "	      << std::hex <<  GetADC5();
     std::cout << std::endl;
    } 
// 
//---------------------------------------------------------------------------------------------    
bool DecodeADCEvent(unsigned char * data_buf){
     int i=0;
     int num_ADC_word=6;
     int size_word =4; //byte
     uint32_t ADCWord[num_ADC_word];
     int currentPos=0;
 
     for (i=0; i<num_ADC_word;i++){
         ADCWord[i]=GetIntFromBinaryStringReversed(size_word, data_buf + currentPos);
         currentPos +=size_word;
	 //std::cout << "word[" << i << "] " << AADCWord[i] << std::endl;
         }
      fADCEvent.LDOStatus     = (ADCWord[0] & 0x00000040) >> 6;
      fADCEvent.ModuleAddress = (ADCWord[0] & 0x0000003C) >> 2;
      fADCEvent.DataType      = ADCWord[0]  & 0x00000003;
      fADCEvent.TimeStamp1    = ADCWord[1]  & 0x00ffffff;
      fADCEvent.TimeStamp2    = ADCWord[2]  & 0x00ffffff;
      fADCEvent.Temp_adc0     = ADCWord[3]  & 0x00000fff;
      fADCEvent.MonV_adc1     = (ADCWord[3] & 0x00fff000) >> 12;
      fADCEvent.MonI_adc2     = ADCWord[4]  & 0x00000fff;
      fADCEvent.IDig_adc3     = (ADCWord[4] & 0x00fff000) >> 12;
      fADCEvent.IDigIO_adc4   = ADCWord[5]  & 0x00000fff;
      fADCEvent.IAna_adc5     = (ADCWord[5] & 0x00fff000) >> 12;
      
      /*std::cout <<  " LDO Status "	<< ADCEvent.LDOStatus;
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
      return true;
   }
// 
//---------------------------------------------------------------------------------------------
bool DecodeADCData(const char* adcCSVfile){
   unsigned char buffer[24];
   int size = GetSizeFile();
   int indexFile=0;
   int count_event=0;
   CreateCSVFileADC_RO(adcCSVfile);
   std::cout << "Wait ..." << std::endl;  
   while ( indexFile < size -4){
       count_event++;
       fFileData.seekg (indexFile);
       fFileData.read ((char *)buffer,24);
       if (DecodeADCEvent(buffer) == false) std::cout << "Event Number " << count_event << " is corrupted !!!" << std::endl; 
       AddEventCSVFileADC_RO(count_event);
       indexFile += 24;   
       } 
    std::cout << "Number of Event decodified: " << count_event  << std::endl;    
   return true;   
   }
// 
//---------------------------------------------------------------------------------------------       
void printUsage() {
    std::cout << "------------------------------------------------------------------" << std::endl << std::endl;
    std::cout << "| Convert the ADC Data from binary file format to .csv format.   |" << std::endl;
    std::cout << "| PAR1 Path of ADC Data binary format file.                      |" << std::endl;
    std::cout << "| PAR2 Path of ADC Data csv format file.                         |" << std::endl;
    std::cout << "| PAR3 Mode (0 --> Normal 1 --> Debug).                          |" << std::endl;
    std::cout << "------------------------------------------------------------------" << std::endl << std::endl;
}

// 
//---------------------------------------------------------------------------------------------
int main(int argc, const char * argv[]){

    for (int i = 0; i < argc-1; i ++) {
        sprintf(myArgv[i], "%s", argv[i+1]);
        std::cout << myArgv[i] << std::endl;
    } 
    myArgc = argc - 1;  
    if ( myArgc != 3){
        printUsage();
	}
    else{ 
         std::cout << "Decodify Data from File: " << argv[1] << std::endl;
	 std::cout << "Create the file : " << argv[2] << std::endl;
	 fMode = atoi(argv[3]);
	 SetPathFile(argv[1]);
	 DecodeADCData(argv[2]);
         remove(argv[1]);
	 }
   return 0;
}
