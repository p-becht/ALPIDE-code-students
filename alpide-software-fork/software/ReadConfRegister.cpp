//
//  main.cpp
//  pALPIDEfs-software
//
//  Created by Carlo Puggioni on 27/11/14.
//  Copyright (c) 2014 Carlo Puggioni. All rights reserved.
//  This main contains some example how set the register of ADC.

#include <iostream>
#include <libusb-1.0/libusb.h>
#include "USB.h"
#include "TTestsetup.h"
#include "TDaqboard.h"
#include "TPalpidefs.h"
#include "stdio.h"
#include <stdio.h>      /* printf */
#include <time.h>       /* time_t, struct tm, time, localtime, asctime */
#include <unistd.h>
#include <stdlib.h>
#include <cstring>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

using namespace std;

#define IREF0 4000
#define IREF1 4000
#define IREF2 4000

// Global Variables
//--------------------------------------------------------------------------------------------- 
TTestSetup *fTS = new TTestSetup ();     // Object Test Setup
TpAlpidefs *fAlpide;
TConfig *fConfigObj =  new TConfig("Config.cfg");
std::vector <SFieldReg> fADCConfigReg0;  // Descriptor ConfigADCReg0
std::vector <SFieldReg> fADCConfigReg1;  // Descriptor ConfigADCRg1
int fChipType =0;
  //  static const int  DEFAULT_RESET_DURATION = 2;
 //   static const int  DEFAULT_PRST_DURATION  = 32;
    static const bool DEFAULT_PULSE_MODE     = 0;
    //Endpoints
    static const int NEndpoints = 4;
    static const int ENDPOINT_WRITE_REG =0;
    static const int ENDPOINT_READ_REG  =1;
    static const int ENDPOINT_READ_ADC  =2;
    static const int ENDPOINT_READ_DATA =3;
    // Schema Instruction Word
    static const int SIZE_WORD        = 4; // byte
    static const int SIZE_ADDR_REG    = 8; // bit
    static const int SIZE_ADDR_MODULE = 4; // bit
    static const int SIZE_READ_WRITE  = 1; // bit 
    // Address Module
    static const int MODULE_FPGA      = 0x0;
    static const int MODULE_ADC       = 0x1;
    static const int MODULE_READOUT   = 0x2;
    static const int MODULE_TRIGGER   = 0x3;
    static const int MODULE_JTAG      = 0x4;
    static const int MODULE_RESET     = 0x5;
    static const int MODULE_IDENT     = 0x6;
    static const int MODULE_SOFTRESET = 0x7;

    // ADC Module: Register
    static const int ADC_CONFIG0  = 0x0;
    static const int ADC_CONFIG1  = 0x1;
    static const int ADC_CONFIG2  = 0x2;
    static const int ADC_READ0    = 0x3; // Read only
    static const int ADC_READ1    = 0x4; // Read only
    static const int ADC_READ2    = 0x5; // Read only
    static const int ADC_OVERFLOW = 0x9; // Read only

    // JTAG Module: Register
    static const int DAQ_WRITE_INSTR_REG  = 0x0;
    static const int DAQ_WRITE_DATA_REG   = 0x1;

    // TRIGGER Module: Register
    static const int TRIG_BUSY_DURATION    = 0x0;
    static const int TRIG_TRIGGER_CONFIG   = 0x1;
    static const int TRIG_START            = 0x2;
    static const int TRIG_STOP             = 0x3;
    static const int TRIG_DELAY            = 0x4;
    static const int TRIG_BUSY_OVERRIDE    = 0x5;

    // READOUT Module: Register
    static const int READOUT_CHIP_DATA     = 0x0;
    static const int READOUT_ENDOFRUN      = 0x1;
    static const int READOUT_EVTID1        = 0x2;
    static const int READOUT_EVTID2        = 0x3;
    static const int READOUT_SERIAL_RESYNC = 0x4; // pAlpide 2 only write
    static const int READOUT_SLAVE_DATA_EM = 0x5; // pAlpide 2 only write

    // RESET Module: Register
    static const int RESET_DURATION      = 0x0;
    static const int RESET_DELAYS        = 0x1;
    static const int RESET_DRST          = 0x2;
    static const int RESET_PRST          = 0x3;
    static const int RESET_PULSE         = 0x4;
    static const int RESET_PULSE_DELAY   = 0x5;
    static const int RESET_POWRON_EN_RES = 0x6; // pAlpide2 

    // IDENTIFICATION Module: Register
    static const int IDENT_ADDRESS     = 0x0;
    static const int IDENT_CHIP        = 0x1;
    static const int IDENT_FIRMWARE    = 0x2;
    static const int IDENT_I2C_ACK     = 0x3; //pAlpide2

    // SOFTRESET Module Register
    static const int SOFTRESET_DURATION = 0x0;
    static const int SOFTRESET_RESET    = 0x1; 
    
    // Chip Register	
    static const int REG_CONTROL	   = 0x000;
    static const int REG_CMD_REG	   = 0x000; // pAlpide 3
    static const int REG_REGION_DISABLE1   = 0x001;
    static const int REG_PERIPHERY_CONTROL = 0x001; // pAlpide 3
    static const int REG_REGION_DISABLE2   = 0x002;
    static const int REG_REGION_DISABLE1_3 = 0x002; // pAlpide 3
    static const int REG_STROBE_TIMING     = 0x003;
    static const int REG_REGION_DISABLE2_3 = 0x003; // pAlpide 3
    static const int REG_COMANDS	   = 0x004; // pAlpide 2
    static const int REG_FROMU_CONFIG_1    = 0x004; // pAlpide 3
    static const int REG_PULSE_RESET	   = 0x005; // pAlpide 2
    static const int REG_FROMU_CONFIG_2    = 0x005; // pAlpide 3
    static const int REG_FROMU_PULSE_1     = 0x006; // pAlpide 3
    static const int REG_FROMU_PULSE_2     = 0x007; // pAlpide 3
    static const int REG_FROMU_STATUS_1    = 0x008; // pAlpide 3
    static const int REG_FROMU_STATUS_2    = 0x009; // pAlpide 3
    static const int REG_DAC_DCLK_MCLK     = 0x00a; // pAlpide 3
    static const int REG_DAC_CMU	   = 0x00b; // pAlpide 3
    static const int REG_CMU_DMU	   = 0x00c; // pAlpide 3
    static const int REG_CMU_ERR	   = 0x00d; // pAlpide 3
    static const int REG_DTU_CONF	   = 0x00e; // pAlpide 3
    static const int REG_DTU_DAC	   = 0x00f; // pAlpide 3
    static const int REG_DTU_PLL1	   = 0x010; // pAlpide 3
    static const int REG_DTU_PLL2	   = 0x011; // pAlpide 3
    static const int REG_DTU_TEST1	   = 0x012; // pAlpide 3
    static const int REG_DTU_TEST2	   = 0x013; // pAlpide 3
    static const int REG_DTU_TEST3	   = 0x014; // pAlpide 3
    static const int REG_BUSY_MIN_WIDTH    = 0x015; // pAlpide 3
    static const int REG_FUSES_W_LSB	   = 0x016; // pAlpide 3
    static const int REG_FUSES_W_MSB	   = 0x017; // pAlpide 3
    static const int REG_FUSES_R_LSB	   = 0x018; // pAlpide 3
    static const int REG_FUSES_R_MSB	   = 0x019; // pAlpide 3
    static const int REG_TEMPERATUR_SENSOR = 0x01a; // pAlpide 3
    static const int REG_SIZEEVENTFIFOBASE = 0x100; 
    static const int REG_DATAFIFOBASE	   = 0x200; 
    static const int REG_COLDISABLEBASE    = 0x300;
    static const int REG_DMU_TRU_STATE     = 0x400; // pAlpide 3 
    static const int REG_PIXELCONFIG1	   = 0x500;
    static const int REG_PIXELCONFIG2	   = 0x501;
    static const int REG_PIXELCONFIG3	   = 0x502; // pAlpide 3 
    static const int REG_VAUX_VRESET	   = 0x600;
    static const int REG_VCASP_VCASN	   = 0x601;
    static const int REG_VPULSEH_VPULSEL   = 0x602;
    static const int REG_IAUX2_IRESET	   = 0x603;
    static const int REG_IDB_IBIAS	   = 0x604;
    static const int REG_ITHR		   = 0x605;
    static const int REG_MONITORING	   = 0x606;
    static const int REG_SET_CMU_CURRENT   = 0x607; // pAlpide 2
    static const int REG_SET_DMU_CURRENT   = 0x608; // pAlpide 2
    static const int REG_CONF_CMU_DMU	   = 0x609; // pAlpide 2
    static const int REG_WRITE_FUSES1	   = 0x60a; // pAlpide 2
    static const int REG_WRITE_FUSES2	   = 0x60b; // pAlpide 2
    static const int REG_READ_FUSES1	   = 0x60c; // pAlpide 2  
    static const int REG_READ_FUSES2	   = 0x60d; // pAlpide 2
    static const int CMU_ERRORS_COUNTER    = 0x60e; // pAlpide 2
    static const int CMU_TEMPERATUR_SENSOR = 0x60f; // pAlpide 2
    static const int REG_MONITORING_3	   = 0x600; // pAlpide 3
    static const int REG_VRESETP	   = 0x601; // pAlpide 3
    static const int REG_VRESETD	   = 0x602; // pAlpide 3
    static const int REG_VCASP  	   = 0x603; // pAlpide 3
    static const int REG_VCASN  	   = 0x604; // pAlpide 3
    static const int REG_VPULSEH	   = 0x605; // pAlpide 3
    static const int REG_VPULSEL	   = 0x606; // pAlpide 3
    static const int REG_VCASN2 	   = 0x607; // pAlpide 3
    static const int REG_VCLIP  	   = 0x608; // pAlpide 3
    static const int REG_VTEMP  	   = 0x609; // pAlpide 3
    static const int REG_IAUX2  	   = 0x60a; // pAlpide 3
    static const int REG_IRESET 	   = 0x60b; // pAlpide 3
    static const int REG_IDB		   = 0x60c; // pAlpide 3
    static const int REG_IBIAS  	   = 0x60d; // pAlpide 3
    static const int REG_ITHR_3 	   = 0x60e; // pAlpide 3
    static const int REG_BUFF_BYPASS	   = 0x60f; // pAlpide 3
    static const int REG_RGNSTATUSBASE     = 0x700;

//Alpide 4
    static const int  REG_CMD_REG_4	    = 0x000;
    static const int  REG_CONTROL_4	    = 0x001;
    static const int  REG_REGION_DISABLE1_4 = 0x002;
    static const int  REG_REGION_DISABLE2_4 = 0x003;
    static const int  REG_FROMUCONFIG1_4    = 0x004;
    static const int  REG_FROMUCONFIG2_4    = 0x005;
    static const int  REG_FROMUCONFIG3_4    = 0x006;
    static const int  REG_FROMUPULSING1_4   = 0x007;
    static const int  REG_FROMUPULSING2_4   = 0x008;
    static const int  REG_FROMUSTATUS1_4    = 0x009;
    static const int  REG_FROMUSTATUS2_4    = 0x00a;
    static const int  REG_FROMUSTATUS3_4    = 0x00b;
    static const int  REG_FROMUSTATUS4_4    = 0x00c;
    static const int  REG_FROMUSTATUS5_4    = 0x00d;
    static const int  REG_CLKCURRENTS_4     = 0x00e;
    static const int  REG_CMUCURRENTS_4     = 0x00f;
    static const int  REG_CMUDMU_CONFIG_4   = 0x010;
    static const int  REG_CMUERRORS_4	    = 0x011;
    static const int  REG_DMUFIFO1_4	    = 0x012;
    static const int  REG_DMUFIFO2_4	    = 0x013;
    static const int  REG_DTUCONFIG_4	    = 0x014;
    static const int  REG_DTUDACS_4	    = 0x015;
    static const int  REG_DTU_PLLLOCKREG1_4 = 0x016;
    static const int  REG_DTU_PLLLOCKREG2_4 = 0x017;
    static const int  REG_DTU_TESTREG1_4    = 0x018;
    static const int  REG_DTU_TESTREG2_4    = 0x019;
    static const int  REG_DTU_TESTREG3_4    = 0x01a;
    static const int  REG_BUSYWIDTH_4	    = 0x01b;
    
    static const int REG_MONITORING_4	    = 0x600;
    static const int  REG_VRESETP_4	    = 0x601;
    static const int  REG_VRESETD_4	    = 0x602;
    static const int  REG_VCASN_4	    = 0x603;
    static const int  REG_VCASP_4	    = 0x604;
    static const int  REG_VPULSEH_4	    = 0x605;
    static const int  REG_VPULSEL_4	    = 0x606;
    static const int  REG_VCASN2_4	    = 0x607;
    static const int  REG_VCLIP_4	    = 0x608;
    static const int  REG_VTEMP_4	    = 0x609;
    static const int  REG_IAUX2_4	    = 0x60a;
    static const int  REG_IRESET_4	    = 0x60b;
    static const int  REG_IDB_4 	    = 0x60c;
    static const int  REG_IBIAS_4	    = 0x60d;
    static const int  REG_ITHR_4	    = 0x60e;
    static const int  REG_BUFF_BYPASS_4     = 0x60f;
    static const int  REG_ADCCONTROL_4      = 0x610;
    static const int  REG_ADCDAC_4	    = 0x611;
    static const int  REG_ADCCALIB_4	    = 0x612;
    static const int  REG_ADCAVSS_4	    = 0x613;
    static const int  REG_ADCDVSS_4	    = 0x614;
    static const int  REG_ADCAVDD_4	    = 0x615;
    static const int  REG_ADCDVDD_4	    = 0x616;
    static const int  REG_ADCVCASN_4	    = 0x617;
    static const int  REG_ADCVCASP_4	    = 0x618;
    static const int  REG_ADCVPULSEH_4      = 0x619;
    static const int  REG_ADCVPULSEL_4      = 0x61a;
    static const int  REG_ADCVRESETP_4      = 0x61b;
    static const int  REG_ADCVRESETD_4      = 0x61c;
    static const int  REG_ADCVCASN2_4	    = 0x61d;
    static const int  REG_ADCVCLIP_4	    = 0x61d;
    static const int  REG_ADCVTEMP_4	    = 0x61f;
    static const int  REG_ADCITHR_4	    = 0x620;
    static const int  REG_ADCIREF_4	    = 0x621;
    static const int  REG_ADCIDB_4	    = 0x622;
    static const int  REG_ADCIBIAS_4	    = 0x623;
    static const int  REG_ADCIAUX2_4	    = 0x624;
    static const int  REG_ADCIRESET_4	    = 0x625;
    static const int  REG_ADCBG2V_4	    = 0x626;
    static const int  REG_ADCT2V_4	    = 0x627;
    	
    // DAC VOLTAGE
    static const int NO_VDAC	= 0x0;
    static const int VAUX	= 0x1;
    static const int VCASN      = 0x2;
    static const int VCASP      = 0x3;
    static const int VPLSE_HIGH = 0x4;
    static const int VPLSE_LOW  = 0x5;
    static const int VRESET     = 0x6;
    
   // DAC CURRENT
    static const int NO_IDAC    = 0x0;
    static const int IRESET     = 0x1;
    static const int IAUX2      = 0x2;
    static const int IBIAS      = 0x3;
    static const int IDB        = 0x4;
    static const int IREF       = 0x5;
    static const int IRHR       = 0x6;
    
// Functions
//
//---------------------------------------------------------------------------------------------                    
void cleanExit() {
    //SoftwareReset (fTS, 10);
    struct libusb_context *context = fTS->GetContext();
    delete fTS;
    libusb_exit(context);
    //exit (AExitValue);
}
//
//---------------------------------------------------------------------------------------------     
void SetDescriptorRegister(){
    if (fConfigObj->GetChipConfig()->ChipType == DUT_PALPIDEFS1) fChipType = 1;
    if (fConfigObj->GetChipConfig()->ChipType == DUT_PALPIDEFS2) fChipType = 2;
    if (fConfigObj->GetChipConfig()->ChipType == DUT_PALPIDEFS3) fChipType = 3; 
    if (fConfigObj->GetChipConfig()->ChipType == DUT_PALPIDEFS4) fChipType = 4; 	
    fADCConfigReg0=fTS->GetDAQBoard(0)->GetADCConfigReg0(); // Get Descriptor Register ADCConfigReg0
    fADCConfigReg1=fTS->GetDAQBoard(0)->GetADCConfigReg1(); // Get Descriptor Register ADCConfigReg1
    if ( fChipType == 1) fAlpide = (TpAlpidefs  *) fTS->GetDUT(0);
    if ( fChipType == 2) fAlpide = (TpAlpidefs2 *) fTS->GetDUT(0);
    if ( fChipType == 3) fAlpide = (TpAlpidefs3 *) fTS->GetDUT(0);
    if ( fChipType == 4) fAlpide = (TpAlpidefs4 *) fTS->GetDUT(0);
    }
//
//---------------------------------------------------------------------------------------------    
int CreateTestSetup(){
    int numDAQBoard=0;
    std::cout << "Creating test setup " << std::endl;
    std::cout << "Searching for DAQ boards " << std::endl;
    fTS->FindDAQBoards(fConfigObj);
    std::cout << "Found " << fTS->GetNDAQBoards() << " DAQ boards." << std::endl;
    numDAQBoard= fTS->GetNDAQBoards();
    if ( numDAQBoard > 0) {
        //fTS->GetDAQBoard(0)->DumpDeviceInfo();
	fTS->AddDUTs(fConfigObj);
        }
    return numDAQBoard;
    }   
//
//--------------------------------------------------------------------------------------------- 
void ReadFPGARegister1(){
    uint32_t data;
    int addr = 0;
    uint32_t value=0;
    std::cout << "**********   FPGA REGISTER PALPIDE 1   **********" <<std::endl;
    fTS->GetDAQBoard(0)->ReadRegister(fADCConfigReg0,&data);
    std::cout << "FPGA;" << "ADC_CONFIG0;" << "Iref0;"<< std::hex << fADCConfigReg0.at(0).readValue <<std::endl;
    std::cout << "FPGA;" << "ADC_CONFIG0;" << "Iref1;"<< std::hex << fADCConfigReg0.at(1).readValue <<std::endl;
    std::cout << "FPGA;" << "ADC_CONFIG0;" << "Enable Self Shoutdown;" << std::hex << fADCConfigReg0.at(2).readValue <<std::endl;
    std::cout << "FPGA;" << "ADC_CONFIG0;" << "Enable LDOFF;"<<  std::hex <<fADCConfigReg0.at(3).readValue   <<std::endl;
    std::cout << "FPGA;" << "ADC_CONFIG0;" << "Enable Stream ADC Data;"<< std::hex << fADCConfigReg0.at(4).readValue   <<std::endl;
    std::cout << "FPGA;" << "ADC_CONFIG0;" << "Enable ADC Self Stop;"<< std::hex << fADCConfigReg0.at(5).readValue   <<std::endl;
    std::cout << "FPGA;" << "ADC_CONFIG0;" << "ADC Disable Reset Time Stamp;"<< std::hex << fADCConfigReg0.at(6).readValue   <<std::endl;
    std::cout << "FPGA;" << "ADC_CONFIG0;" << "ADC Enable Packet Based;"<< std::hex << fADCConfigReg0.at(7).readValue   <<std::endl;
    
    fTS->GetDAQBoard(0)->ReadRegister(fADCConfigReg1,&data);
    std::cout << "FPGA;" << "ADC_CONFIG1;"<< "Iref2;" << std::hex << fADCConfigReg1.at(0).readValue <<std::endl;
    
    addr = READOUT_CHIP_DATA + (MODULE_READOUT << SIZE_ADDR_REG);
    fTS->GetDAQBoard(0)->ReadRegister(addr,&data);
    value = data & 0xf;
    std::cout << "FPGA;" << "READOUT_CHIP_DATA;"<< "Buffer Depth;" << std::hex << value  <<std::endl;
    value = (data >> 4) & 0x1;
    std::cout << "FPGA;" << "READOUT_CHIP_DATA;"<< "Falling Edge Sampling;" << std::hex << value  <<std::endl;
    value = (data >> 5) & 0x1;
    std::cout << "FPGA;" << "READOUT_CHIP_DATA;"<< "Enable Packet Based Readout;" << std::hex << value <<std::endl;
    
    addr = READOUT_ENDOFRUN + (MODULE_READOUT << SIZE_ADDR_REG);
    fTS->GetDAQBoard(0)->ReadRegister(addr,&data);
    std::cout << "FPGA;" << "READOUT_ENDOFRUN;"<< "Readout end of run;" << std::hex << data <<std::endl;
    
    addr = READOUT_EVTID1 + (MODULE_READOUT << SIZE_ADDR_REG);
    fTS->GetDAQBoard(0)->ReadRegister(addr,&data);
    std::cout << "FPGA;" << "READOUT_EVTID1;"<< "Readout Id Event 1;" << std::hex << data <<std::endl;
    
    addr = READOUT_EVTID2 + (MODULE_READOUT << SIZE_ADDR_REG);
    fTS->GetDAQBoard(0)->ReadRegister(addr,&data);
    std::cout << "FPGA;" << "READOUT_EVTID2;"<< "Readout Id Event 2;" << std::hex << data <<std::endl;
    
    addr = TRIG_BUSY_DURATION + (MODULE_TRIGGER << SIZE_ADDR_REG);
    fTS->GetDAQBoard(0)->ReadRegister(addr,&data);
    std::cout << "FPGA;" << "TRIG_BUSY_DURATION;"<< "Trigger Busy Duration;" << std::hex << data <<std::endl;
    
    addr = TRIG_TRIGGER_CONFIG + (MODULE_TRIGGER << SIZE_ADDR_REG);
    fTS->GetDAQBoard(0)->ReadRegister(addr,&data);
    value = (data & 0xffff);
    std::cout << "FPGA;" << "TRIG_TRIGGER_CONFIG;"<< "Number of Trigger;" << std::hex << value <<std::endl;
    value = (data >> 16) & 0x7;
    std::cout << "FPGA;" << "TRIG_TRIGGER_CONFIG;"<< "Trigger Mode [00] Disabled [01] Auto Trigger [10] External Trigger;" << std::hex << value <<std::endl;
    value = (data >> 19) & 0xff;
    std::cout << "FPGA;" << "TRIG_TRIGGER_CONFIG;"<< "Trigger Width;" << std::hex << value <<std::endl;
    
    addr = TRIG_START + (MODULE_TRIGGER << SIZE_ADDR_REG);
    fTS->GetDAQBoard(0)->ReadRegister(addr,&data);
    std::cout << "FPGA;" << "TRIG_START;"<< "Trigger Start;" << std::hex << data <<std::endl;
    
    addr = TRIG_STOP + (MODULE_TRIGGER << SIZE_ADDR_REG);
    fTS->GetDAQBoard(0)->ReadRegister(addr,&data);
    std::cout << "FPGA;" << "TRIG_STOP;"<< "Trigger Stop;" << std::hex << data <<std::endl;
    
    addr = TRIG_DELAY + (MODULE_TRIGGER << SIZE_ADDR_REG);
    fTS->GetDAQBoard(0)->ReadRegister(addr,&data);
    std::cout << "FPGA;" << "TRIG_DELAY;"<< "Trigger Delay;" << std::hex << data <<std::endl;
    
    addr = TRIG_BUSY_OVERRIDE + (MODULE_TRIGGER << SIZE_ADDR_REG);
    fTS->GetDAQBoard(0)->ReadRegister(addr,&data);
    std::cout << "FPGA;" << "TRIG_BUSY_OVERRIDE;"<< "Trigger Busy Override;" << std::hex << data <<std::endl;
    
    addr = RESET_DURATION + (MODULE_RESET << SIZE_ADDR_REG);
    fTS->GetDAQBoard(0)->ReadRegister(addr,&data);
    value = (data & 0xff);
    std::cout << "FPGA;" << "RESET_DURATION;"<< "Pulse Reset Duration;" << std::hex << value <<std::endl;
    value = (data >> 8) & 0xff;
    std::cout << "FPGA;" << "RESET_DURATION;"<< "Reset Duration;" << std::hex << value <<std::endl;
    value = (data >> 16) & 0x7ff;
    std::cout << "FPGA;" << "RESET_DURATION;"<< "Pulse Duration;" << std::hex << value <<std::endl;
    
    addr = RESET_DELAYS + (MODULE_RESET << SIZE_ADDR_REG);
    fTS->GetDAQBoard(0)->ReadRegister(addr,&data);
    value = (data & 0xff);
    std::cout << "FPGA;" << "RESET_DELAYS;"<< "Delay Shout Down;" << std::hex << value <<std::endl;
    value = (data >> 8) & 0xff;
    std::cout << "FPGA;" << "RESET_DELAYS;"<< "Delay Clk;" << std::hex << value <<std::endl;
    value = (data >> 16) & 0xff;
    std::cout << "FPGA;" << "RESET_DELAYS;"<< "Delay Signal;" << std::hex << value <<std::endl;
    value = (data >> 24) & 0xff;
    std::cout << "FPGA;" << "RESET_DELAYS;"<< "Delay Duration Reset;" << std::hex << value <<std::endl;
    
    addr = RESET_DRST + (MODULE_RESET << SIZE_ADDR_REG);
    fTS->GetDAQBoard(0)->ReadRegister(addr,&data);
    std::cout << "FPGA;" << "RESET_DRST;"<< "Reset Duration;" << std::hex << data <<std::endl;
    
    addr = RESET_PRST + (MODULE_RESET << SIZE_ADDR_REG);
    fTS->GetDAQBoard(0)->ReadRegister(addr,&data);
    std::cout << "FPGA;" << "RESET_PRST;"<< "Reset Pulse;" << std::hex << data <<std::endl;
    
    addr = RESET_PULSE + (MODULE_RESET << SIZE_ADDR_REG);
    fTS->GetDAQBoard(0)->ReadRegister(addr,&data);
    std::cout << "FPGA;" << "RESET_PULSE;"<< "Pulse;" << std::hex << data <<std::endl;
    
    addr = RESET_PULSE_DELAY + (MODULE_RESET << SIZE_ADDR_REG);
    fTS->GetDAQBoard(0)->ReadRegister(addr,&data);
    value = (data & 0xffff);
    std::cout  << "RESET_PULSE_DELAY;"<< "Pulse Delay;" << std::hex << value <<std::endl;
    value = (data >> 16) & 0x3;
    std::cout  << "RESET_PULSE_DELAY;"<< "Pulse Mode;" << std::hex << value <<std::endl;
    
    addr = IDENT_ADDRESS + (MODULE_IDENT << SIZE_ADDR_REG);
    fTS->GetDAQBoard(0)->ReadRegister(addr,&data);
    value = (data & 0xff);
    std::cout  << "IDENT_ADDRESS;"<< "Address;" << std::hex << value <<std::endl;
    
    addr = IDENT_CHIP + (MODULE_IDENT << SIZE_ADDR_REG);
    fTS->GetDAQBoard(0)->ReadRegister(addr,&data);
    std::cout  << "IDENT_CHIP;"<< "Chip;" << std::hex << data <<std::endl;
    
    addr = IDENT_FIRMWARE + (MODULE_IDENT << SIZE_ADDR_REG);
    fTS->GetDAQBoard(0)->ReadRegister(addr,&data);
    std::cout  << "IDENT_FIRMWARE;"<< "Firmware;" << std::hex << data <<std::endl;
    
    addr = SOFTRESET_DURATION + (MODULE_SOFTRESET << SIZE_ADDR_REG);
    fTS->GetDAQBoard(0)->ReadRegister(addr,&data);
    std::cout  << "SOFTRESET_DURATION;"<< "Software Reset Duration;" << std::hex << data <<std::endl;
    
    addr = SOFTRESET_RESET + (MODULE_SOFTRESET << SIZE_ADDR_REG);
    fTS->GetDAQBoard(0)->ReadRegister(addr,&data);
    std::cout  << "SOFTRESET_RESET;"<< "Software Reset;" << std::hex << data <<std::endl;    
    }
//
//---------------------------------------------------------------------------------------------
void ReadCHIPRegister1(){
    int Value;        
    fTS->GetDAQBoard(0)->ReadChipRegister(REG_CONTROL,&Value); 
    std::cout << "**********   PALPIDE 1 REGISTER   **********" <<std::endl;    
    std::cout << "PALPIDE;" << "CONTROL_REG;"<< "ChipMode;"	           << std::hex << (Value & 0x3)         <<std::endl;
    std::cout << "PALPIDE;" << "CONTROL_REG;"<< "Clustering Enabled;"      << std::hex << ((Value >> 2) & 0x1)  << std::endl;
    std::cout << "PALPIDE;" << "CONTROL_REG;"<< "Start Self Test;"         << std::hex << ((Value >> 3) & 0x1)  << std::endl;	
    std::cout << "PALPIDE;" << "CONTROL_REG;"<< "Strobe of Pixel Config;"  << std::hex << ((Value >> 4) & 0x1)  << std::endl;	 
    std::cout << "PALPIDE;" << "CONTROL_REG;"<< "Data Port1 Enabled;"      << std::hex << ((Value >> 5) & 0x1)  << std::endl;	   
    std::cout << "PALPIDE;" << "CONTROL_REG;"<< "Data Port2 Enabled;"      << std::hex << ((Value >> 6) & 0x1)  << std::endl;	   
    std::cout << "PALPIDE;" << "CONTROL_REG;"<< "Readout Delay;"           << std::hex << ((Value >> 8) & 0xff) << std::endl;
    fTS->GetDAQBoard(0)->ReadChipRegister(REG_REGION_DISABLE1,&Value);
    std::cout << "PALPIDE;" << "REGION_DISABLE_1;"<< "Region Disable 1;"   << std::hex << (Value & 0xffff)      << std::endl;
    fTS->GetDAQBoard(0)->ReadChipRegister(REG_REGION_DISABLE2,&Value);
    std::cout << "PALPIDE;" << "REGION_DISABLE_2;"<< "Region Disable 2;"   << std::hex << (Value & 0xffff)      << std::endl;
    fTS->GetDAQBoard(0)->ReadChipRegister(REG_STROBE_TIMING,&Value);			   
    std::cout << "PALPIDE;" << "STROBE_B_TIMING;"<< "Strobe Timing;"       << std::hex << (Value & 0xffff)      << std::endl;
    fTS->GetDAQBoard(0)->ReadChipRegister(REG_SIZEEVENTFIFOBASE,&Value);
    std::cout << "PALPIDE;" << "DATA_FIFO_BASE;"    << "Memory Location of the event length FIFO;" << std::hex << (Value & 0xffff)<< std::endl; 
    fTS->GetDAQBoard(0)->ReadChipRegister(REG_DATAFIFOBASE,&Value);
    std::cout << "PALPIDE;" << "DATA_FIFO_BASE;"    << "Memory Location of data FIFO;" << std::hex << (Value & 0xffff) << std::endl; 
    fTS->GetDAQBoard(0)->ReadChipRegister(REG_COLDISABLEBASE,&Value);
    std::cout << "PALPIDE;" << "COLUMN_DISABLE_BASE;"    << "Column Disable Register;" << std::hex << (Value & 0xffff) << std::endl;
    fTS->GetDAQBoard(0)->ReadChipRegister(REG_PIXELCONFIG1,&Value);
    std::cout << "PALPIDE;" << "PIXEL_CFG_REG_1;"<< "Pixel row select address;"                  << std::hex << (Value & 0x1ff)      << std::endl;
    std::cout << "PALPIDE;" << "PIXEL_CFG_REG_1;"<< "Set all rows for configuration;"            << std::hex << ((Value >> 9)& 0x1)  << std::endl;
    std::cout << "PALPIDE;" << "PIXEL_CFG_REG_1;"<< "Selection of the in-pixel register;"        << std::hex << ((Value >> 10)& 0x1) << std::endl;
    std::cout << "PALPIDE;" << "PIXEL_CFG_REG_1;"<< "Data to be written in the target register;" << std::hex << ((Value >> 11)& 0x1) << std::endl;
    fTS->GetDAQBoard(0)->ReadChipRegister(REG_PIXELCONFIG2,&Value);
    std::cout << "PALPIDE;" << "PIXEL_CFG_REG_2;"<< "Pixel column select address;"               << std::hex << (Value & 0x3ff)      << std::endl;
    std::cout << "PALPIDE;" << "PIXEL_CFG_REG_2;"<< "All pixel columns selected;"                << std::hex << ((Value >> 10)& 0x1) << std::endl;
    std::cout << "PALPIDE;" << "PIXEL_CFG_REG_2;"<< "Type of pulsing (0 digital 1 analog);"      << std::hex << ((Value >> 11)& 0x1) << std::endl;
    fTS->GetDAQBoard(0)->ReadChipRegister(REG_VAUX_VRESET,&Value);
    std::cout << "PALPIDE;" << "VRESET_VAUX_REG;"    << "VAUX;"        << std::hex << (Value & 0xff)        << std::endl;			
    std::cout << "PALPIDE;" << "VRESET_VAUX_REG;"    << "VRESET;"      << std::hex << ((Value >> 8) & 0xff) << std::endl;
    fTS->GetDAQBoard(0)->ReadChipRegister(REG_VCASP_VCASN,&Value);
    std::cout << "PALPIDE;" << "VCASP_VCASN_REG;"   << "VCASN;"        << std::hex << (Value & 0xff)        << std::endl;
    std::cout << "PALPIDE;" << "VCASP_VCASN_REG;"   << "VCASP;"        << std::hex << ((Value >> 8) & 0xff) << std::endl;
    fTS->GetDAQBoard(0)->ReadChipRegister(REG_VPULSEH_VPULSEL,&Value);
    std::cout << "PALPIDE;" << "VPULSEH_VPULSEL_REG;"<< "VPULSEL;"     << std::hex << (Value & 0xff)        << std::endl;
    std::cout << "PALPIDE;" << "VPULSEH_VPULSEL_REG;"<< "VPULSEH;"     << std::hex << ((Value >> 8) & 0xff) << std::endl;
    fTS->GetDAQBoard(0)->ReadChipRegister(REG_IAUX2_IRESET,&Value);
    std::cout << "PALPIDE;" << "IAUX2_IRESET_REG;"   << "IRESET;"      << std::hex << (Value & 0xff)        << std::endl;
    std::cout << "PALPIDE;" << "IAUX2_IRESET_REG;"   << "IAUX2;"       << std::hex << ((Value >> 8) & 0xff) << std::endl;
    fTS->GetDAQBoard(0)->ReadChipRegister(REG_IDB_IBIAS,&Value);
    std::cout << "PALPIDE;" << "IDB_IBIAS_REG;"      << "IBIAS;"       << std::hex << (Value & 0xff)        << std::endl;
    std::cout << "PALPIDE;" << "IDB_IBIAS_REG;"      << "IDB;"         << std::hex << ((Value >> 8) & 0xff) << std::endl;
    fTS->GetDAQBoard(0)->ReadChipRegister(REG_ITHR,&Value);
    std::cout << "PALPIDE;" << "ITHR_REG;"<< std::hex << Value << std::endl;
    std::cout << "PALPIDE;" << "ITHR_REG;"           << "ITHR;"        << std::hex << (Value & 0xff)        << std::endl;    
    fTS->GetDAQBoard(0)->ReadChipRegister(REG_MONITORING,&Value);
    std::cout << "PALPIDE;" << "MONITORING_OVERRIDE_CTRL_REG;"<<"Voltage DAC Selection;"            << std::hex << (Value & 0x7)         << std::endl;
    std::cout << "PALPIDE;" << "MONITORING_OVERRIDE_CTRL_REG;"<<"Current DAC Selection;"            << std::hex << ((Value >> 3) & 0x7)  << std::endl;
    std::cout << "PALPIDE;" << "MONITORING_OVERRIDE_CTRL_REG;"<<"Configure the current DAC block;"  << std::hex << ((Value >> 6) & 0x1)  << std::endl;
    std::cout << "PALPIDE;" << "MONITORING_OVERRIDE_CTRL_REG;"<<"Configure the voltage DAC block;"  << std::hex << ((Value >> 7) & 0x1)  << std::endl;  
    fTS->GetDAQBoard(0)->ReadChipRegister(REG_RGNSTATUSBASE,&Value);      
    std::cout << "PALPIDE;" << "STATUS_BASE_REG;" <<"Region Pixel Self Test;"                               << std::hex << (Value & 0x1)        << std::endl;     
    std::cout << "PALPIDE;" << "STATUS_BASE_REG;" <<"Region Memory Self Test;"                              << std::hex << ((Value >> 1) & 0x1) << std::endl;
    std::cout << "PALPIDE;" << "STATUS_BASE_REG;" <<"Asserted if all columns of region are disabled;"       << std::hex << ((Value >> 2) & 0x1) << std::endl;
    std::cout << "PALPIDE;" << "STATUS_BASE_REG;" <<"Region Readout SM State;"                              << std::hex << ((Value >> 3) & 0x7) << std::endl;
    std::cout << "PALPIDE;" << "STATUS_BASE_REG;" <<"Region Memory SM State;"                               << std::hex << ((Value >> 6) & 0x3) << std::endl;
    std::cout << "PALPIDE;" << "STATUS_BASE_REG;" <<"Periphery Readout SM State;"                           << std::hex << ((Value >> 8) & 0xf) << std::endl;
    }
//
//--------------------------------------------------------------------------------------------- 
void ReadFPGARegister2(){
    uint32_t data;
    int addr = 0;
    uint32_t value=0;
    std::cout << "**********   FPGA REGISTER PALPIDE 2  **********" <<std::endl;
    TDAQBoard2 *myBoard2 = (TDAQBoard2*) fTS->GetDAQBoard(0);
    myBoard2->ReadRegister(fADCConfigReg0,&data);
    std::cout << "FPGA;" << "ADC_CONFIG0;" << "Iref0;"<< std::hex << fADCConfigReg0.at(0).readValue <<std::endl;
    std::cout << "FPGA;" << "ADC_CONFIG0;" << "Iref1;"<< std::hex << fADCConfigReg0.at(1).readValue <<std::endl;
    std::cout << "FPGA;" << "ADC_CONFIG0;" << "Enable Self Shoutdown;" << std::hex << fADCConfigReg0.at(2).readValue <<std::endl;
    std::cout << "FPGA;" << "ADC_CONFIG0;" << "Enable LDOFF;"<<  std::hex <<fADCConfigReg0.at(3).readValue   <<std::endl;
    std::cout << "FPGA;" << "ADC_CONFIG0;" << "Enable Stream ADC Data;"<< std::hex << fADCConfigReg0.at(4).readValue   <<std::endl;
    std::cout << "FPGA;" << "ADC_CONFIG0;" << "Enable ADC Self Stop;"<< std::hex << fADCConfigReg0.at(5).readValue   <<std::endl;
    std::cout << "FPGA;" << "ADC_CONFIG0;" << "ADC Disable Reset Time Stamp;"<< std::hex << fADCConfigReg0.at(6).readValue   <<std::endl;
    std::cout << "FPGA;" << "ADC_CONFIG0;" << "ADC Enable Packet Based;"<< std::hex << fADCConfigReg0.at(7).readValue   <<std::endl;
    
    myBoard2->ReadRegister(fADCConfigReg1,&data);
    std::cout << "FPGA;" << "ADC_CONFIG1;"<< "Iref2;" << std::hex << fADCConfigReg1.at(0).readValue <<std::endl;
    
    addr = READOUT_CHIP_DATA + (MODULE_READOUT << SIZE_ADDR_REG);
    myBoard2->ReadRegister(addr,&data);
    value = data & 0xf;
    std::cout << "FPGA;" << "READOUT_CHIP_DATA;"<< "Buffer Depth;" << std::hex << value  <<std::endl;
    value = (data >> 4) & 0x1;
    std::cout << "FPGA;" << "READOUT_CHIP_DATA;"<< "Falling Edge Sampling;" << std::hex << value  <<std::endl;
    value = (data >> 5) & 0x1;
    std::cout << "FPGA;" << "READOUT_CHIP_DATA;"<< "Enable Packet Based Readout;" << std::hex << value <<std::endl;
    value = (data >> 6) & 0x1;
    std::cout << "FPGA;" << "READOUT_CHIP_DATA;"<< "DDR Enable for parallel port readout;" << std::hex << value <<std::endl;
    value = (data >> 7) & 0x3;
    std::cout << "FPGA;" << "READOUT_CHIP_DATA;"<< "Select Data port for Readout (01 Serial 10 Parallel);" << std::hex << value <<std::endl;
    value = (data >> 9) & 0x3;
    std::cout << "FPGA;" << "READOUT_CHIP_DATA;"<< "FPGA Emulation Mode [00] Default [01] FPGA Master Chip Slave [10] FPGA Slave Chip Master;" << std::hex << value <<std::endl;
    
    addr = READOUT_ENDOFRUN + (MODULE_READOUT << SIZE_ADDR_REG);
    myBoard2->ReadRegister(addr,&data);
    std::cout << "FPGA;" << "READOUT_ENDOFRUN;"<< "Readout end of run;" << std::hex << data <<std::endl;
    
    addr = READOUT_EVTID1 + (MODULE_READOUT << SIZE_ADDR_REG);
    myBoard2->ReadRegister(addr,&data);
    value = data & 0xffffff;
    std::cout << "FPGA;" << "READOUT_EVTID1;"<< "Readout Id Event 1;" << std::hex << value <<std::endl;
    
    addr = READOUT_EVTID2 + (MODULE_READOUT << SIZE_ADDR_REG);
    myBoard2->ReadRegister(addr,&data);
    value = data & 0xffffff;
    std::cout << "FPGA;" << "READOUT_EVTID2;"<< "Readout Id Event 2;" << std::hex << value <<std::endl;
    
    
    addr = TRIG_BUSY_DURATION + (MODULE_TRIGGER << SIZE_ADDR_REG);
    myBoard2->ReadRegister(addr,&data);
    std::cout << "FPGA;" << "TRIG_BUSY_DURATION;"<< "Trigger Busy Duration;" << std::hex << data <<std::endl;
    
    addr = TRIG_TRIGGER_CONFIG + (MODULE_TRIGGER << SIZE_ADDR_REG);
    myBoard2->ReadRegister(addr,&data);
    value = (data & 0xffff);
    std::cout << "FPGA;" << "TRIG_TRIGGER_CONFIG;"<< "Number of Trigger;" << std::hex << value <<std::endl;
    value = (data >> 16) & 0x7;
    std::cout << "FPGA;" << "TRIG_TRIGGER_CONFIG;"<< "Trigger Mode[00] Disabled [01] Auto Trigger [10] External Trigger;" << std::hex << value <<std::endl;
    value = (data >> 19) & 0xff;
    std::cout << "FPGA;" << "TRIG_TRIGGER_CONFIG;"<< "Trigger Width;" << std::hex << value <<std::endl;
    
    addr = TRIG_START + (MODULE_TRIGGER << SIZE_ADDR_REG);
    myBoard2->ReadRegister(addr,&data);
    std::cout << "FPGA;" << "TRIG_START;"<< "Trigger Start;" << std::hex << data <<std::endl;
    
    addr = TRIG_STOP + (MODULE_TRIGGER << SIZE_ADDR_REG);
    myBoard2->ReadRegister(addr,&data);
    std::cout << "FPGA;" << "TRIG_STOP;"<< "Trigger Stop;" << std::hex << data <<std::endl;
    
    addr = TRIG_DELAY + (MODULE_TRIGGER << SIZE_ADDR_REG);
    myBoard2->ReadRegister(addr,&data);
    std::cout << "FPGA;" << "TRIG_DELAY;"<< "Trigger Delay;" << std::hex << data <<std::endl;
    
    addr = TRIG_BUSY_OVERRIDE + (MODULE_TRIGGER << SIZE_ADDR_REG);
    myBoard2->ReadRegister(addr,&data);
    std::cout << "FPGA;" << "TRIG_BUSY_OVERRIDE;"<< "Trigger Busy Override;" << std::hex << data <<std::endl;
    
    addr = RESET_DURATION + (MODULE_RESET << SIZE_ADDR_REG);
    myBoard2->ReadRegister(addr,&data);
    value = (data & 0xff);
    std::cout << "FPGA;" << "RESET_DURATION;"<< "Pulse Reset Duration;" << std::hex << value <<std::endl;
    value = (data >> 8) & 0xff;
    std::cout << "FPGA;" << "RESET_DURATION;"<< "Reset Duration;" << std::hex << value <<std::endl;
    value = (data >> 16) & 0x7ff;
    std::cout << "FPGA;" << "RESET_DURATION;"<< "Pulse Duration;" << std::hex << value <<std::endl;
    
    addr = RESET_DELAYS + (MODULE_RESET << SIZE_ADDR_REG);
    myBoard2->ReadRegister(addr,&data);
    value = (data & 0xff);
    std::cout << "FPGA;" << "RESET_DELAYS;"<< "Delay Shout Down;" << std::hex << value <<std::endl;
    value = (data >> 8) & 0xff;
    std::cout << "FPGA;" << "RESET_DELAYS;"<< "Delay Clk;" << std::hex << value <<std::endl;
    value = (data >> 16) & 0xff;
    std::cout << "FPGA;" << "RESET_DELAYS;"<< "Delay Signal;" << std::hex << value <<std::endl;
    value = (data >> 24) & 0xff;
    std::cout << "FPGA;" << "RESET_DELAYS;"<< "Delay Duration Reset;" << std::hex << value <<std::endl;
    
    addr = RESET_DRST + (MODULE_RESET << SIZE_ADDR_REG);
    myBoard2->ReadRegister(addr,&data);
    std::cout << "FPGA;" << "RESET_DRST;"<< "Reset Duration;" << std::hex << data <<std::endl;
    
    addr = RESET_PRST + (MODULE_RESET << SIZE_ADDR_REG);
    myBoard2->ReadRegister(addr,&data);
    std::cout << "FPGA;" << "RESET_PRST;"<< "Reset PRST;" << std::hex << data <<std::endl;
    
    addr = RESET_PULSE + (MODULE_RESET << SIZE_ADDR_REG);
    myBoard2->ReadRegister(addr,&data);
    std::cout << "FPGA;" << "RESET_PULSE;"<< "Reset Pulse;" << std::hex << data <<std::endl;
    
    addr = RESET_PULSE_DELAY + (MODULE_RESET << SIZE_ADDR_REG);
    myBoard2->ReadRegister(addr,&data);
    value = (data & 0xffff);
    std::cout  << "RESET_PULSE_DELAY;"<< "Pulse Delay;" << std::hex << value <<std::endl;
    value = (data >> 16) & 0x3;
    std::cout  << "RESET_PULSE_DELAY;"<< "Pulse Mode[00] Pulse after a write access [01] Strobe Delay Pulse [10] Pulse Delay Strobe;" << std::hex << value <<std::endl;
    
    addr = RESET_POWRON_EN_RES + (MODULE_RESET << SIZE_ADDR_REG);
    myBoard2->ReadRegister(addr,&data);
    value = (data & 0x1);
    std::cout  << "RESET_POWRON_EN_RES;"<< "Power On Reset [0] Enable [1] Disable;" << std::hex << value <<std::endl;
    
    addr = IDENT_ADDRESS + (MODULE_IDENT << SIZE_ADDR_REG);
    myBoard2->ReadRegister(addr,&data);
    value = (data & 0xff);
    std::cout  << "IDENT_ADDRESS;"<< "Address;" << std::hex << value <<std::endl;
    
    addr = IDENT_CHIP + (MODULE_IDENT << SIZE_ADDR_REG);
    myBoard2->ReadRegister(addr,&data);
    value = (data & 0x1);
    std::cout  << "IDENT_CHIP;"<< "Mode [0] Write [1] read;" << std::hex << value <<std::endl;
    value = (data >> 1 ) & 0x7f;
    std::cout  << "IDENT_CHIP;"<< "I2C Device Address" << std::hex << value <<std::endl;
    value = (data >> 8 ) & 0xff;
    std::cout  << "IDENT_CHIP;"<< "I2C Comand Byte" << std::hex << value <<std::endl;
    value = (data >> 16 ) & 0xff;
    std::cout  << "IDENT_CHIP;"<< "I2C Data Byte" << std::hex << value <<std::endl;
    
    addr = IDENT_FIRMWARE + (MODULE_IDENT << SIZE_ADDR_REG);
    fTS->GetDAQBoard(0)->ReadRegister(addr,&data);
    std::cout  << "IDENT_FIRMWARE;"<< "Firmware;" << std::hex << data <<std::endl;
    
    addr = IDENT_I2C_ACK  + (MODULE_IDENT << SIZE_ADDR_REG);
    fTS->GetDAQBoard(0)->ReadRegister(addr,&data);
    value = data & 0x7 ;
    std::cout  << "IDENT_I2C_ACK;"<< "Ack;" << std::hex << value <<std::endl;
    
    addr = SOFTRESET_DURATION + (MODULE_SOFTRESET << SIZE_ADDR_REG);
    myBoard2->ReadRegister(addr,&data);
    std::cout  << "SOFTRESET_DURATION;"<< "Software Reset Duration;" << std::hex << data <<std::endl;
    
    addr = SOFTRESET_RESET + (MODULE_SOFTRESET << SIZE_ADDR_REG);
    myBoard2->ReadRegister(addr,&data);
    std::cout  << "SOFTRESET_RESET;"<< "Software Reset;" << std::hex << data <<std::endl;    
    }
//
//---------------------------------------------------------------------------------------------
void ReadCHIPRegister2(){
    int Value;        
    fAlpide->ReadRegister(REG_CONTROL,&Value); 
    std::cout << "**********   PALPIDE 2 REGISTER   **********" <<std::endl;    
    std::cout << "PALPIDE2;" << "CONTROL_REG;"<< "ChipMode [0] Config [1] A [2] B [3] Auto;" << std::hex << (Value & 0x3)         <<std::endl;
    std::cout << "PALPIDE2;" << "CONTROL_REG;"<< "Clustering  [0] Disabled [1] Enabled;"     << std::hex << ((Value >> 2) & 0x1)  << std::endl;
    std::cout << "PALPIDE2;" << "CONTROL_REG;"<< "Start Self Test;"         << std::hex << ((Value >> 3) & 0x1)  << std::endl;	
    std::cout << "PALPIDE2;" << "CONTROL_REG;"<< "Strobe of Pixel Config;"  << std::hex << ((Value >> 4) & 0x1)  << std::endl;	 
    std::cout << "PALPIDE2;" << "CONTROL_REG;"<< "MEB memories;"            << std::hex << ((Value >> 7) & 0x1)  << std::endl;	     
    std::cout << "PALPIDE2;" << "CONTROL_REG;"<< "Readout Delay;"           << std::hex << ((Value >> 8) & 0xff) << std::endl;
    
    fAlpide->ReadRegister(REG_REGION_DISABLE1,&Value);
    std::cout << "PALPIDE2;" << "REGION_DISABLE_1;"<< "Region Disable 1;"   << std::hex << (Value & 0xffff)      << std::endl;
    
    fAlpide->ReadRegister(REG_REGION_DISABLE2,&Value);
    std::cout << "PALPIDE2;" << "REGION_DISABLE_2;"<< "Region Disable 2;"   << std::hex << (Value & 0xffff)      << std::endl;
    
    fAlpide->ReadRegister(REG_STROBE_TIMING,&Value);			   
    std::cout << "PALPIDE2;" << "STROBE_B_TIMING;"<< "Strobe Timing;"       << std::hex << (Value & 0xffff)      << std::endl;
    
    fAlpide->ReadRegister(REG_SIZEEVENTFIFOBASE,&Value);    
    std::cout << "PALPIDE2;" << "DATA_FIFO_BASE;"    << "Memory Location of the event length FIFO;" << std::hex << (Value & 0xffff)<< std::endl; 
    
    fAlpide->ReadRegister(REG_DATAFIFOBASE,&Value);
    std::cout << "PALPIDE2;" << "DATA_FIFO_BASE;"    << "Memory Location of data FIFO;" << std::hex << (Value & 0xffff) << std::endl; 
    
    fAlpide->ReadRegister(REG_COLDISABLEBASE,&Value);
    std::cout << "PALPIDE2;" << "COLUMN_DISABLE_BASE;"    << "Column Disable Register;" << std::hex << (Value & 0xffff) << std::endl;
    
    fAlpide->ReadRegister(REG_PIXELCONFIG1,&Value);
    std::cout << "PALPIDE2;" << "PIXEL_CFG_REG_1;"<< "Pixel row select address;"                  << std::hex << (Value & 0x1ff)      << std::endl;
    std::cout << "PALPIDE2;" << "PIXEL_CFG_REG_1;"<< "Set all rows for configuration;"            << std::hex << ((Value >> 9)& 0x1)  << std::endl;
    std::cout << "PALPIDE2;" << "PIXEL_CFG_REG_1;"<< "Selection of the in-pixel register;"        << std::hex << ((Value >> 10)& 0x1) << std::endl;
    std::cout << "PALPIDE2;" << "PIXEL_CFG_REG_1;"<< "Data to be written in the target register;" << std::hex << ((Value >> 11)& 0x1) << std::endl;
    
    fAlpide->ReadRegister(REG_PIXELCONFIG2,&Value);
    std::cout << "PALPIDE2;" << "PIXEL_CFG_REG_2;"<< "Pixel column select address;"               << std::hex << (Value & 0x3ff)      << std::endl;
    std::cout << "PALPIDE2;" << "PIXEL_CFG_REG_2;"<< "All pixel columns selected;"                << std::hex << ((Value >> 10)& 0x1) << std::endl;
    std::cout << "PALPIDE2;" << "PIXEL_CFG_REG_2;"<< "Type of pulsing [0] digital [1] analog;"    << std::hex << ((Value >> 11)& 0x1) << std::endl;
    
    fAlpide->ReadRegister(REG_VAUX_VRESET,&Value);
    std::cout << "PALPIDE2;" << "VRESET_VAUX_REG;"    << "VAUX;"        << std::hex << (Value & 0xff)        << std::endl;			
    std::cout << "PALPIDE2;" << "VRESET_VAUX_REG;"    << "VRESET;"      << std::hex << ((Value >> 8) & 0xff) << std::endl;
    
    fAlpide->ReadRegister(REG_VCASP_VCASN,&Value);
    std::cout << "PALPIDE2;" << "VCASP_VCASN_REG;"   << "VCASN;"        << std::hex << (Value & 0xff)        << std::endl;
    std::cout << "PALPIDE2;" << "VCASP_VCASN_REG;"   << "VCASP;"        << std::hex << ((Value >> 8) & 0xff) << std::endl;
    
    fAlpide->ReadRegister(REG_VPULSEH_VPULSEL,&Value);
    std::cout << "PALPIDE2;" << "VPULSEH_VPULSEL_REG;"<< "VPULSEL;"     << std::hex << (Value & 0xff)        << std::endl;
    std::cout << "PALPIDE2;" << "VPULSEH_VPULSEL_REG;"<< "VPULSEH;"     << std::hex << ((Value >> 8) & 0xff) << std::endl;
    
    fAlpide->ReadRegister(REG_IAUX2_IRESET,&Value);
    std::cout << "PALPIDE2;" << "IAUX2_IRESET_REG;"   << "IRESET;"      << std::hex << (Value & 0xff)        << std::endl;
    std::cout << "PALPIDE2;" << "IAUX2_IRESET_REG;"   << "IAUX2;"       << std::hex << ((Value >> 8) & 0xff) << std::endl;
    
    fAlpide->ReadRegister(REG_IDB_IBIAS,&Value);
    std::cout << "PALPIDE2;" << "IDB_IBIAS_REG;"      << "IBIAS;"       << std::hex << (Value & 0xff)        << std::endl;
    std::cout << "PALPIDE2;" << "IDB_IBIAS_REG;"      << "IDB;"         << std::hex << ((Value >> 8) & 0xff) << std::endl;
    
    fAlpide->ReadRegister(REG_ITHR,&Value);
    std::cout << "PALPIDE2;" << "ITHR_REG;"<< std::hex << Value << std::endl;
    std::cout << "PALPIDE2;" << "ITHR_REG;"           << "ITHR;"        << std::hex << (Value & 0xff)        << std::endl;    
    
    fAlpide->ReadRegister(REG_MONITORING,&Value);
    std::cout << "PALPIDE2;" << "MONITORING_OVERRIDE_CTRL_REG;"<<"Voltage DAC Selection;"            << std::hex << (Value & 0x7)         << std::endl;
    std::cout << "PALPIDE2;" << "MONITORING_OVERRIDE_CTRL_REG;"<<"Current DAC Selection;"            << std::hex << ((Value >> 3) & 0x7)  << std::endl;
    std::cout << "PALPIDE2;" << "MONITORING_OVERRIDE_CTRL_REG;"<<"Configure the current DAC block;"  << std::hex << ((Value >> 6) & 0x1)  << std::endl;
    std::cout << "PALPIDE2;" << "MONITORING_OVERRIDE_CTRL_REG;"<<"Configure the voltage DAC block;"  << std::hex << ((Value >> 7) & 0x1)  << std::endl;
    
    fAlpide->ReadRegister(REG_SET_CMU_CURRENT,&Value);
    std::cout << "PALPIDE2;" << "SET_CMU_CURRENT;" <<"DCLK  Receiver Current ;"   << std::hex << (Value & 0xf)        << std::endl;
    std::cout << "PALPIDE2;" << "SET_CMU_CURRENT;" <<"DCTRL Driver Current   ;"   << std::hex << ((Value >> 4) & 0xf) << std::endl;
    std::cout << "PALPIDE2;" << "SET_CMU_CURRENT;" <<"DCTRL  Receiver Current;"   << std::hex << ((Value >> 8) & 0xf) << std::endl;
    
    fAlpide->ReadRegister(REG_SET_DMU_CURRENT,&Value);
    std::cout << "PALPIDE2;" << "SET_DMU_CURRENT;" <<"DATAPORT Driver Current   ;"<< std::hex << (Value & 0xf)        << std::endl;
    std::cout << "PALPIDE2;" << "SET_DMU_CURRENT;" <<"DATAPORT  Receiver Current;"<< std::hex << ((Value >> 4) & 0xf) << std::endl;
    
    fAlpide->ReadRegister(REG_CONF_CMU_DMU,&Value);
    std::cout << "PALPIDE2;" << "REG_CONF_CMU_DMU;"<<"Previous Chip Id;"                                   << std::hex << (Value & 0xf)         << std::endl;
    std::cout << "PALPIDE2;" << "REG_CONF_CMU_DMU;"<<"Manchester Encoding [0] Enabled [1] Disabled;"       << std::hex << ((Value >> 6) & 0x1)  << std::endl;
    std::cout << "PALPIDE2;" << "REG_CONF_CMU_DMU;"<<"Disable Double Data Rate [0] Enabled [1] Disabled;"  << std::hex << ((Value >> 7) & 0x1)  << std::endl;   
    
    fAlpide->ReadRegister(REG_WRITE_FUSES1,&Value);
    std::cout << "PALPIDE2;" << "REG_WRITE_FUSES1;"<<"Fuses Write [15:0];"   << std::hex << (Value & 0xffff )  << std::endl;
    
    fAlpide->ReadRegister(REG_WRITE_FUSES2,&Value);
    std::cout << "PALPIDE2;" << "REG_WRITE_FUSES2;"<<"Fuses Write [23:16];"  << std::hex << (Value & 0xff )  << std::endl;
    
    fAlpide->ReadRegister(REG_READ_FUSES1,&Value);
    std::cout << "PALPIDE2;" << "REG_READ_FUSES1;" <<"Fuses Read [15:0];"    << std::hex << (Value & 0xffff )  << std::endl;
    
    fAlpide->ReadRegister(REG_READ_FUSES2,&Value);
    std::cout << "PALPIDE2;" << "REG_READ_FUSES2;" <<"Fuses Read [23:16];"   << std::hex << (Value & 0xff )  << std::endl;
    
    fAlpide->ReadRegister(CMU_ERRORS_COUNTER,&Value);
    std::cout << "PALPIDE2;" << "CMU_ERRORS_COUNTER;" <<"CMU Error Counter;"   << std::hex << ((Value >> 4) & 0xfff)  << std::endl;
    
    fAlpide->ReadRegister(CMU_TEMPERATUR_SENSOR,&Value);   
    std::cout << "PALPIDE2;" << "MU_TEMPERATUR_SENSOR;" <<"Temperature Sensor;"   << std::hex << (Value & 0x1 )  << std::endl;
       
    fAlpide->ReadRegister(REG_RGNSTATUSBASE,&Value);      
    std::cout << "PALPIDE2;" << "STATUS_BASE_REG;" <<"Region Pixel Self Test;"                               << std::hex << (Value & 0x1)        << std::endl;     
    std::cout << "PALPIDE2;" << "STATUS_BASE_REG;" <<"Region Memory Self Test;"                              << std::hex << ((Value >> 1) & 0x1) << std::endl;
    std::cout << "PALPIDE2;" << "STATUS_BASE_REG;" <<"Asserted if all columns of region are disabled;"       << std::hex << ((Value >> 2) & 0x1) << std::endl;
    std::cout << "PALPIDE2;" << "STATUS_BASE_REG;" <<"Region Readout SM State;"                              << std::hex << ((Value >> 3) & 0xf) << std::endl;
    std::cout << "PALPIDE2;" << "STATUS_BASE_REG;" <<"Region Memory SM State;"                               << std::hex << ((Value >> 7) & 0x3) << std::endl;
    }

void ReadCHIPRegister3(){
    int Value;
    std::cout << "**********   PALPIDE 3 REGISTER   **********" <<std::endl; 
         
    fAlpide->ReadRegister(REG_CMD_REG,&Value); 
    std::cout << "PALPIDE3;" << "COMMAND_REG;"<< "OPCODE of a Control Transaction;" << std::hex << (Value) <<std::endl;
    
    fAlpide->ReadRegister(REG_PERIPHERY_CONTROL,&Value); 
    std::cout << "PALPIDE3;" << "CONTROL_REG;"<< "ChipMode [0] Config [1] Readout Triggered Mode [2] Readout Continuos Mode;" << std::hex << (Value & 0x3)         <<std::endl;
    std::cout << "PALPIDE3;" << "CONTROL_REG;"<< "Clustering  [0] Disabled [1] Enabled;"                         << std::hex << ((Value >> 2) & 0x1)  << std::endl;
    std::cout << "PALPIDE3;" << "CONTROL_REG;"<< "XOff [0] DMU Enabled [1] DMU Disabled;"                        << std::hex << ((Value >> 3) & 0x1)  << std::endl;	
    std::cout << "PALPIDE3;" << "CONTROL_REG;"<< "Matrix Readout Speed: [0] 10 MHz [1] 20 MHz;"                  << std::hex << ((Value >> 4) & 0x1)  << std::endl;	 
    std::cout << "PALPIDE3;" << "CONTROL_REG;"<< "Force Busy [0] disbaled(default) [1] Busy line is forced;"     << std::hex << ((Value >> 7) & 0x1)  << std::endl;	     
    std::cout << "PALPIDE3;" << "CONTROL_REG;"<< "Force Busy Value;"           					 << std::hex << ((Value >> 8) & 0xff) << std::endl;
        
    fAlpide->ReadRegister(REG_REGION_DISABLE1_3,&Value);
    std::cout << "PALPIDE3;" << "REGION_DISABLE_1;"<< "Region Disable 1;"   << std::hex << (Value & 0xffff)      << std::endl;
        
    fAlpide->ReadRegister(REG_REGION_DISABLE2_3,&Value);
    std::cout << "PALPIDE3;" << "REGION_DISABLE_2;"<< "Region Disable 2;"   << std::hex << (Value & 0xffff)      << std::endl;
    
    fAlpide->ReadRegister(REG_FROMU_CONFIG_1,&Value); 
    std::cout << "PALPIDE3;" << "REG_FROMU_CONFIG_1;"<< "Pixel MEB Mask: [bit0-1-2 0/1] En/Dis Slice 1-2-3;"                    << std::hex << (Value & 0x7)            << std::endl;
    std::cout << "PALPIDE3;" << "REG_FROMU_CONFIG_1;"<< "Internal Strobe: [0] Disabled [1] strobe pulse in continuous mode;"     << std::hex << ((Value >> 3)& 0x1)      << std::endl;
    std::cout << "PALPIDE3;" << "REG_FROMU_CONFIG_1;"<< "Busy Monitoring: [0] Disabled [1] monitoring external BUSY line;"       << std::hex << ((Value >> 4)& 0x1)      << std::endl; 
    std::cout << "PALPIDE3;" << "REG_FROMU_CONFIG_1;"<< "Test Pulse Mode: [0] Digital(Default) [1] Analog;"                      << std::hex << ((Value >> 5)& 0x1)      << std::endl;
    std::cout << "PALPIDE3;" << "REG_FROMU_CONFIG_1;"<< "Test Strobe:     [0] Disabled [1] Enabled;"                             << std::hex << ((Value >> 6)& 0x1)      << std::endl;
    std::cout << "PALPIDE3;" << "REG_FROMU_CONFIG_1;"<< "SM States: State machine bits;"                             << std::hex << ((Value >> 8)& 0x1f)     << std::endl; 
     
    fAlpide->ReadRegister(REG_FROMU_CONFIG_2,&Value);			   
    std::cout << "PALPIDE3;" << "REG_FROMU_CONFIG_2;"<< "Strobe Timing 25ns-1638ns(step 25ns);"       << std::hex << (Value & 0xffff)      << std::endl;
    
    fAlpide->ReadRegister(REG_FROMU_PULSE_1,&Value);			   
    std::cout << "PALPIDE3;" << "REG_FROMU_PULSE_1;"<< "Delay from the test PULSE to the STROBE 0ns-1638ns (step 25ns);"       << std::hex << (Value & 0xffff)      << std::endl;
     
    fAlpide->ReadRegister(REG_FROMU_PULSE_2,&Value);			   
    std::cout << "PALPIDE3;" << "REG_FROMU_PULSE_2;"<< "Test Pulse Duration  0ns-1638ns (step 25ns);"       << std::hex << (Value & 0xffff)      << std::endl; 
    
    fAlpide->ReadRegister(REG_FROMU_STATUS_1,&Value);			   
    std::cout << "PALPIDE3;" << "REG_FROMU_STATUS_1;"<< "Status1: STROBE Counter;"       << std::hex << (Value & 0xffff)      << std::endl;
     
    fAlpide->ReadRegister(REG_FROMU_STATUS_2,&Value);			   
    std::cout << "PALPIDE3;" << "REG_FROMU_STATUS_2;"<< "Status2: BUNCH Counter;"       << std::hex << (Value & 0xfff)      << std::endl; 
    std::cout << "PALPIDE3;" << "REG_FROMU_STATUS_2;"<< "Status2: EVENT Counter;"       << std::hex << ((Value >> 12)& 0x3)      << std::endl;  
      
    fAlpide->ReadRegister(REG_DAC_DCLK_MCLK,&Value);
    std::cout << "PALPIDE3;" << "SET_DCLK_MCLK_CURRENT;" <<"DCLK  Receiver;"   << std::hex << (Value & 0xf)	   << std::endl;
    std::cout << "PALPIDE3;" << "SET_DCLK_MCLK_CURRENT;" <<"DCLK Driver;"   << std::hex << ((Value >> 4) & 0xf) << std::endl;
    std::cout << "PALPIDE3;" << "SET_DCLK_MCLK_CURRENT;" <<"MCLK  Receiver;"   << std::hex << ((Value >> 8) & 0xf) << std::endl;
        
    fAlpide->ReadRegister(REG_DAC_CMU,&Value);
    std::cout << "PALPIDE3;" << "SET_DAC_CMU;" <<"DCTRL Receiver;"<< std::hex << (Value & 0xf)	     << std::endl;
    std::cout << "PALPIDE3;" << "SET_DAC_CMU;" <<"DCTRL Driver;"  << std::hex << ((Value >> 4) & 0xf) << std::endl;  
    
    fAlpide->ReadRegister(REG_CMU_DMU,&Value);
    std::cout << "PALPIDE3;" << "REG_CMU_DMU;"<<"Previous Chip Id;"				     << std::hex << (Value & 0xf)	  << std::endl;
    std::cout << "PALPIDE3;"<< "REG_CMU_DMU;"<<"Initial Token;"				             << std::hex << ((Value >> 5) & 0x1)  << std::endl;
    std::cout << "PALPIDE3;" << "REG_CMU_DMU;"<<"Manchester Encoding [0] Enabled [1] Disabled;"       << std::hex << ((Value >> 6) & 0x1)  << std::endl;
    std::cout << "PALPIDE3;" << "REG_CMU_DMU;"<<"Disable Double Data Rate [0] Enabled [1] Disabled;"  << std::hex << ((Value >> 7) & 0x1)  << std::endl;  
     
    fAlpide->ReadRegister(REG_CMU_ERR,&Value);
    std::cout << "PALPIDE3;" << "REG_CMU_ERR;" <<"Deserializer Error Counter;"	<< std::hex << (Value & 0xf)  << std::endl; 
    std::cout << "PALPIDE3;" << "REG_CMU_ERR;" <<"Time Out Error Counter;"	<< std::hex << ((Value >> 4) & 0xf)  << std::endl;
    std::cout << "PALPIDE3;" << "REG_CMU_ERR;" <<"Unknown Error Counter;"	<< std::hex << ((Value >> 8) & 0xf)  << std::endl;
    
    fAlpide->ReadRegister(REG_DTU_CONF,&Value);
    std::cout << "PALPIDE3;" << "REG_DTU_CONF;" <<"PLL Configuration;" << std::hex << (Value & 0xf)  << std::endl; 
    std::cout << "PALPIDE3;" << "REG_DTU_CONF;" <<"Serializer Phase;"  << std::hex << ((Value >> 4) & 0xf)  << std::endl;
    std::cout << "PALPIDE3;" << "REG_DTU_CONF;" <<"PLL Reset;"	      << std::hex << ((Value >> 8) & 0x1)  << std::endl;
    std::cout << "PALPIDE3;" << "REG_DTU_CONF;" <<"Error Counter;"     << std::hex << ((Value >> 9) & 0x7)  << std::endl;
    std::cout << "PALPIDE3;" << "REG_DTU_CONF;" <<"Status;"	      << std::hex << ((Value >> 12)& 0x1)  << std::endl;
    
    fAlpide->ReadRegister(REG_DTU_DAC,&Value);
    std::cout << "PALPIDE3;" << "REG_DTU_DAC;" <<"PLL (default 8);"            << std::hex << (Value & 0xf)  << std::endl; 
    std::cout << "PALPIDE3;" << "REG_DTU_DAC;" <<"Driver (default 8);"	      << std::hex << ((Value >> 4) & 0xf)  << std::endl;
    std::cout << "PALPIDE3;" << "REG_DTU_DAC;" <<"Pre-Emphasis (default 0);"   << std::hex << ((Value >> 8) & 0xf)  << std::endl;
    
    fAlpide->ReadRegister(REG_DTU_PLL1,&Value);
    std::cout << "PALPIDE3;" << "REG_DTU_PLL1;" <<"Look Counter;"      << std::hex << (Value & 0xff)  << std::endl; 
    std::cout << "PALPIDE3;" << "REG_DTU_PLL1;" <<"Look Flag;"         << std::hex << ((Value >> 8) & 0x1)  << std::endl;
    std::cout << "PALPIDE3;" << "REG_DTU_PLL1;" <<"Look Status;"       << std::hex << ((Value >> 9) & 0x1)  << std::endl;
    
    fAlpide->ReadRegister(REG_DTU_PLL2,&Value);
    std::cout << "PALPIDE3;" << "REG_DTU_PLL2;" <<"Look Wait Cycles;"      << std::hex << (Value & 0xff)  << std::endl; 
    std::cout << "PALPIDE3;" << "REG_DTU_PLL2;" <<"Unlook Wait Cycles;"         << std::hex << ((Value >> 8) & 0xff)  << std::endl;
    
    fAlpide->ReadRegister(REG_DTU_CONF,&Value);
    std::cout << "PALPIDE3;" << "REG_DTU_CONF;" <<"PLL Configuration;" << std::hex << (Value & 0xf)  << std::endl; 
    std::cout << "PALPIDE3;" << "REG_DTU_CONF;" <<"Serializer Phase;"  << std::hex << ((Value >> 4) & 0xf)  << std::endl;
    std::cout << "PALPIDE3;" << "REG_DTU_CONF;" <<"PLL Reset;"	      << std::hex << ((Value >> 8) & 0x1)  << std::endl;
    std::cout << "PALPIDE3;" << "REG_DTU_CONF;" <<"Error Counter;"     << std::hex << ((Value >> 9) & 0x7)  << std::endl;
    std::cout << "PALPIDE3;" << "REG_DTU_CONF;" <<"Status;"	      << std::hex << ((Value >> 12)& 0x1)  << std::endl; 
    
    fAlpide->ReadRegister(REG_DTU_TEST1,&Value);      
    std::cout << "PALPIDE3;" << "REG_DTU_TEST1;" <<"Test [0] Disable [1]Enable;"			<< std::hex << (Value & 0x1)	<< std::endl;	  
    std::cout << "PALPIDE3;" << "REG_DTU_TEST1;" <<"Internal Path:[0] Disable [1]Enable;"  	<< std::hex << ((Value >> 1) & 0x1) << std::endl;
    std::cout << "PALPIDE3;" << "REG_DTU_TEST1;" <<"Prbs [0] Disable [1]Enable;"	    		<< std::hex << ((Value >> 2) & 0x1) << std::endl;
    std::cout << "PALPIDE3;" << "REG_DTU_TEST1;" <<"Bypass 8b10b: [0] Disable [1]Enable;"  	<< std::hex << ((Value >> 3) & 0x1) << std::endl;
    std::cout << "PALPIDE3;" << "REG_DTU_TEST1;" <<"BDIN8b10b0;"					<< std::hex << ((Value >> 4) & 0x3) << std::endl;
    std::cout << "PALPIDE3;" << "REG_DTU_TEST1;" <<"BDIN8b10b1;"					<< std::hex << ((Value >> 6) & 0x3) << std::endl;
    std::cout << "PALPIDE3;" << "REG_DTU_TEST1;" <<"BDIN8b10b2;"					<< std::hex << ((Value >> 8) & 0x3) << std::endl;
    std::cout << "PALPIDE3;" << "REG_DTU_TEST1;" <<"K0;"						<< std::hex << ((Value >> 10) & 0x1) << std::endl;
    std::cout << "PALPIDE3;" << "REG_DTU_TEST1;" <<"K1;"						<< std::hex << ((Value >> 11) & 0x1) << std::endl;
    std::cout << "PALPIDE3;" << "REG_DTU_TEST1;" <<"K2;"						<< std::hex << ((Value >> 12) & 0x1) << std::endl;
    std::cout << "PALPIDE3;" << "REG_DTU_TEST1;" <<"Force Set Load;"				<< std::hex << ((Value >> 13) & 0x1) << std::endl;
    std::cout << "PALPIDE3;" << "REG_DTU_TEST1;" <<"Force UnSet Load ;"				<< std::hex << ((Value >> 14) & 0x1) << std::endl;
    
    fAlpide->ReadRegister(REG_DTU_TEST2,&Value);      
    std::cout << "PALPIDE3;" << "REG_DTU_TEST2;" <<"DIN0;"	<< std::hex << (Value & 0xff)	<< std::endl;	  
    std::cout << "PALPIDE3;" << "REG_DTU_TEST2;" <<"DIN1;"  	<< std::hex << ((Value >> 8) & 0xff) << std::endl;
    
    fAlpide->ReadRegister(REG_DTU_TEST3,&Value);      
    std::cout << "PALPIDE3;" << "REG_DTU_TEST3;" <<"DIN2;"	<< std::hex << (Value & 0xff)	<< std::endl;	  
    
    fAlpide->ReadRegister(REG_BUSY_MIN_WIDTH,&Value);      
    std::cout << "PALPIDE3;" << "REG_BUSY_MIN_WIDTH;" <<"Length of BUSY signal (step 25ns);"	<< std::hex << (Value & 0x1f)	<< std::endl; 
    
    fAlpide->ReadRegister(REG_FUSES_W_LSB,&Value);      
    std::cout << "PALPIDE3;" << "REG_FUSES_W_LSB;" <<"Write Fuses LSB;"        	<< std::hex << (Value & 0xffffff)   << std::endl; 
    
    fAlpide->ReadRegister(REG_FUSES_W_MSB,&Value);      
    std::cout << "PALPIDE3;" << "REG_FUSES_W_MSB;" <<"Write Fuses MSB;"     	<< std::hex << (Value & 0xffffff)   << std::endl; 
    
    fAlpide->ReadRegister(REG_FUSES_R_LSB,&Value);      
    std::cout << "PALPIDE3;" << "REG_FUSES_R_LSB;" <<"Read Fuses LSB;"     	<< std::hex << (Value & 0xffffff)   << std::endl; 
    
    fAlpide->ReadRegister(REG_FUSES_R_MSB,&Value);     
    std::cout << "PALPIDE3;" << "REG_FUSES_R_MSB;" <<"Read Fuses MSB;"     	<< std::hex << (Value & 0xffffff)   << std::endl; 
    
    fAlpide->ReadRegister(REG_TEMPERATUR_SENSOR,&Value);   
    std::cout << "PALPIDE3;" << "REG_TEMPERATUR_SENSOR;" <<"Temperature Sensor;"   << std::hex << (Value & 0x1 )  << std::endl;
    
    fAlpide->ReadRegister(REG_COLDISABLEBASE,&Value);
    std::cout << "PALPIDE3;" << "COLUMN_DISABLE_BASE;"    << "Column Disable Register;" << std::hex << (Value & 0xffff) << std::endl;
    
    fAlpide->ReadRegister(REG_DMU_TRU_STATE,&Value);
    std::cout << "PALPIDE3;" << "REG_DMU_TRU_STATE;"    	<< "TRU SM State;"		<< std::hex << (Value & 0x7) << std::endl;
    std::cout << "PALPIDE3;" << "REG_DMU_TRU_STATE;" 	<< "DMU SM State;"		<< std::hex << ((Value >> 3) & 0x7) << std::endl;
    std::cout << "PALPIDE3;" << "REG_DMU_TRU_STATE;" 	<< "Busy Mismatch Error;"	<< std::hex << ((Value >> 6) & 0x1) << std::endl;
    std::cout << "PALPIDE3;" << "REG_DMU_TRU_STATE;" 	<< "Busy FIFO Full Error;"	<< std::hex << ((Value >> 7) & 0x1) << std::endl;  
    
    fAlpide->ReadRegister(REG_PIXELCONFIG1,&Value);
    std::cout << "PALPIDE3;" << "PIXEL_CFG_REG_1;"<< "Pixel row selector;"     				          	<< std::hex << (Value & 0x1ff)      << std::endl;
    std::cout << "PALPIDE3;" << "PIXEL_CFG_REG_1;"<< "Set all rows;"           		 				<< std::hex << ((Value >> 9)& 0x1)  << std::endl;
    std::cout << "PALPIDE3;" << "PIXEL_CFG_REG_1;"<< "Selection of the in-pixel register [0] Pulse [1] Mask;"        	<< std::hex << ((Value >> 10)& 0x1) << std::endl;
    std::cout << "PALPIDE3;" << "PIXEL_CFG_REG_1;"<< "Data to be written in the target register;" 			<< std::hex << ((Value >> 11)& 0x1) << std::endl; 
       
    fAlpide->ReadRegister(REG_PIXELCONFIG2,&Value);
    std::cout << "PALPIDE3;" << "PIXEL_CFG_REG_2;"<< "Pixel column selector;"      				        << std::hex << (Value & 0x3ff)      << std::endl;
    std::cout << "PALPIDE3;" << "PIXEL_CFG_REG_2;"<< "[1] Sets All Colums;"          				        << std::hex << ((Value >> 10)& 0x1) << std::endl;
      
    fAlpide->ReadRegister(REG_PIXELCONFIG3,&Value);
    std::cout << "PALPIDE3;" << "PIXEL_CFG_REG_3;"<< "Propagation of the row and the column signals matrix: [0] Disable [1] Enable;" << std::hex << (Value & 0x3ff)      << std::endl;
    
    fAlpide->ReadRegister(REG_MONITORING_3,&Value);
    std::cout << "PALPIDE3;" << "MONITORING_OVERRIDE_CTRL_REG;"<<"Voltage DAC Selection;"            	<< std::hex << (Value & 0xf)         << std::endl;
    std::cout << "PALPIDE3;" << "MONITORING_OVERRIDE_CTRL_REG;"<<"Current DAC Selection;"            	<< std::hex << ((Value >> 4) & 0x7)  << std::endl;
    std::cout << "PALPIDE3;" << "MONITORING_OVERRIDE_CTRL_REG;"<<"Configure the current DACMONI block;"  << std::hex << ((Value >> 7) & 0x1)  << std::endl;
    std::cout << "PALPIDE3;" << "MONITORING_OVERRIDE_CTRL_REG;"<<"Configure the voltage DACMONV block;"  << std::hex << ((Value >> 8) & 0x1)  << std::endl; 
    std::cout << "PALPIDE3;" << "MONITORING_OVERRIDE_CTRL_REG;"<<"Iref Buffer Current;"  		<< std::hex << ((Value >> 9) & 0x3)  << std::endl;
    
    fAlpide->ReadRegister(REG_VRESETP,&Value);
    std::cout << "PALPIDE3;" << "REG_VRESETP;"	<< "VRESETP;"        << std::hex << (Value & 0xff)        << std::endl;	
    		  
    fAlpide->ReadRegister(REG_VRESETD,&Value);
    std::cout << "PALPIDE3;" << "REG_VRESETD;"	<< "VRESETD;"        << std::hex << (Value & 0xff)	    << std::endl;   
    
    fAlpide->ReadRegister(REG_VCASP,&Value);
    std::cout << "PALPIDE3;" << "REG_VCASP;"	<< "VCASP;"     << std::hex << (Value & 0xff)	   << std::endl;
    
    fAlpide->ReadRegister(REG_VCASN,&Value);
    std::cout << "PALPIDE3;" << "REG_VCASN;"   	<< "VCASN;"	  << std::hex << (Value & 0xff)        << std::endl; 
    
    fAlpide->ReadRegister(REG_VPULSEH,&Value);
    std::cout << "PALPIDE3;" << "REG_VPULSEH;"	<< "VPULSEH;"	<< std::hex << (Value & 0xff)	     << std::endl;
    
    fAlpide->ReadRegister( REG_VPULSEL,&Value);
    std::cout << "PALPIDE3;" << " REG_VPULSEL;"  << "VPULSEL;"	<< std::hex << (Value & 0xff)	     << std::endl;
    		       
    fAlpide->ReadRegister(REG_VCASN2,&Value);
    std::cout << "PALPIDE3;" << "REG_VCASN2;"   	<< "VCASN2;"        << std::hex << (Value & 0xff)	   << std::endl; 
      
    fAlpide->ReadRegister( REG_VCLIP,&Value);
    std::cout << "PALPIDE3;" << " REG_VCLIP;"	<< "VCLIP;"     << std::hex << (Value & 0xff)	    << std::endl;
    
    fAlpide->ReadRegister(REG_VTEMP,&Value);
    std::cout << "PALPIDE3;" << "REG_VTEMP;"   	<< "VTEMP;"	  << std::hex << (Value & 0xff)        << std::endl; 
    
    fAlpide->ReadRegister(REG_IAUX2,&Value);
    std::cout << "PALPIDE3;" << "REG_IAUX2;"	<< "IAUX2;"       << std::hex << (Value & 0xff)	   << std::endl;
    
    fAlpide->ReadRegister(REG_IRESET,&Value);
    std::cout << "PALPIDE3;" << "REG_IRESET;"	<< "IRESET;"        << std::hex << (Value & 0xff)	   << std::endl;
    		     
    fAlpide->ReadRegister(REG_IDB,&Value);
    std::cout << "PALPIDE3;" << "REG_IDB;"   	<< "IDB;"        << std::hex << (Value & 0xff)	   << std::endl; 
      
    fAlpide->ReadRegister(REG_IBIAS,&Value);
    std::cout << "PALPIDE3;" << "REG_IBIAS;"	<< "IBIAS;"     << std::hex << (Value & 0xff)	   << std::endl;
    
    fAlpide->ReadRegister(REG_ITHR_3,&Value);
    std::cout << "PALPIDE3;" << "REG_ITHR;"   	<< "ITHR;"      << std::hex << (Value & 0xff)	  << std::endl;     
	    
    fAlpide->ReadRegister(REG_BUFF_BYPASS,&Value);
    std::cout << "PALPIDE3;" << "REG_BUFF_BYPASS;"	<< "[1] Bypass VCASN;"	<< std::hex << (Value & 0x1)        << std::endl;
    std::cout << "PALPIDE3;" << "REG_BUFF_BYPASS;" 	<< "[1] Bypass VCASN2;"	<< std::hex << ((Value >> 1) & 0x1) << std::endl;
    std::cout << "PALPIDE3;" << "REG_BUFF_BYPASS;" 	<< "[1] Bypass VCASP;"	<< std::hex << ((Value >> 2) & 0x1) << std::endl;
    std::cout << "PALPIDE3;" << "REG_BUFF_BYPASS;" 	<< "[1] Bypass VCLIP;"	<< std::hex << ((Value >> 3) & 0x1) << std::endl;
    std::cout << "PALPIDE3;" << "REG_BUFF_BYPASS;" 	<< "[1] Bypass IRESET;"	<< std::hex << ((Value >> 4) & 0x1) << std::endl;
    std::cout << "PALPIDE3;" << "REG_BUFF_BYPASS;" 	<< "[1] Bypass IBIAS;"	<< std::hex << ((Value >> 5) & 0x1) << std::endl;    
    std::cout << "PALPIDE3;" << "REG_BUFF_BYPASS;" 	<< "[1] Bypass ITHR;"	<< std::hex << ((Value >> 6) & 0x1) << std::endl;
    std::cout << "PALPIDE3;" << "REG_BUFF_BYPASS;" 	<< "[1] Bypass IDB;"	<< std::hex << ((Value >> 7) & 0x1) << std::endl;
    
    fAlpide->ReadRegister(REG_RGNSTATUSBASE,&Value);
    std::cout << "PALPIDE3;" << "REGION_READOUT_STATUS;"	<< "Pixel Satus [1] faulty pixel;"	 << std::hex << (Value & 0x1)	     << std::endl;
    std::cout << "PALPIDE3;" << "REGION_READOUT_STATUS;"	<< "Memory Satus [1] faulty memory;"	 << std::hex << ((Value >> 1) & 0x1) << std::endl;
    std::cout << "PALPIDE3;" << "REGION_READOUT_STATUS;"	<< "[1] All Columns Disabled;"  	 << std::hex << ((Value >> 2) & 0x1) << std::endl;
    std::cout << "PALPIDE3;" << "REGION_READOUT_STATUS;"	<< "Readout State Machine;"		 << std::hex << ((Value >> 3) & 0x3) << std::endl;
    std::cout << "PALPIDE3;" << "REGION_READOUT_STATUS;"	<< "FIFO Self Test State Machine;"	 << std::hex << ((Value >> 5) & 0x3) << std::endl;             
    }
    void ReadCHIPRegister4(){
    int Value;
    std::cout << "**********   PALPIDE 3 REGISTER   **********" <<std::endl;     
    fAlpide->ReadRegister(REG_CMD_REG_4	 ,&Value); 
    std::cout <<"PALPIDE4;" << "REG_CMD_REG;" << std::hex << (Value & 0xffff) <<std::endl;  
    fAlpide->ReadRegister(REG_CONTROL_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "CONTROL_REG;"<< "ChipMode [0] Config [1] Readout Triggered Mode [2] Readout Continuos Mode;" << std::hex << (Value & 0x3)         <<std::endl;
    std::cout <<"PALPIDE4;" << "CONTROL_REG;"<< "Clustering  [0] Disabled [1] Enabled;"                         << std::hex << ((Value >> 2) & 0x1)  << std::endl;	
    std::cout <<"PALPIDE4;" << "CONTROL_REG;"<< "Matrix Readout Speed: [0] 10 MHz [1] 20 MHz;"                  << std::hex << ((Value >> 3) & 0x1)  << std::endl;
    std::cout <<"PALPIDE4;" << "CONTROL_REG;"<< "IB Serial Link Speed: [0] 400 MB/s [1] 600 MB/s [2] 1200 MB/s;"<< std::hex << ((Value >> 4) & 0x3)  << std::endl;   	 
    std::cout <<"PALPIDE4;" << "CONTROL_REG;"<< "Enable Skewing Global [0] Disabled [1] Enabled;"               << std::hex << ((Value >> 6) & 0x1)  << std::endl;	     
    std::cout <<"PALPIDE4;" << "CONTROL_REG;"<< "Enable Skewing Start Readout [0] Disabled [1] Enabled;"        << std::hex << ((Value >> 7) & 0x1)  << std::endl;
    std::cout <<"PALPIDE4;" << "CONTROL_REG;"<< "Readout Clock Gating [0] Disabled [1] Enabled;"                << std::hex << ((Value >> 6) & 0x1)  << std::endl;	     
    std::cout <<"PALPIDE4;" << "CONTROL_REG;"<< "Readout From CMU [0] Disabled [1] Enabled;"                    << std::hex << ((Value >> 7) & 0x1)  << std::endl;  
    fAlpide->ReadRegister(REG_REGION_DISABLE1_4,&Value);
    std::cout <<"PALPIDE4;" << "REG_REGION_DISABLE1;" << std::hex << (Value & 0xffff) <<std::endl;
    fAlpide->ReadRegister(REG_REGION_DISABLE2_4,&Value);
    std::cout <<"PALPIDE4;" << "REG_REGION_DISABLE2;" << std::hex << (Value & 0xffff) <<std::endl;
    fAlpide->ReadRegister(REG_FROMUCONFIG1_4   ,&Value);
    std::cout <<"PALPIDE4;" << "REG_FROMU_CONFIG_1;"<< "Pixel MEB Mask: [bit0-1-2 0/1] En/Dis Slice 1-2-3;"                     << std::hex << (Value & 0x7)            << std::endl;
    std::cout <<"PALPIDE4;" << "REG_FROMU_CONFIG_1;"<< "Internal Strobe: [0] Disabled [1] strobe pulse in continuous mode;"     << std::hex << ((Value >> 3)& 0x1)      << std::endl;
    std::cout <<"PALPIDE4;" << "REG_FROMU_CONFIG_1;"<< "Busy Monitoring: [0] Disabled [1] monitoring external BUSY line;"       << std::hex << ((Value >> 4)& 0x1)      << std::endl; 
    std::cout <<"PALPIDE4;" << "REG_FROMU_CONFIG_1;"<< "Test Pulse Mode: [0] Digital(Default) [1] Analog;"                      << std::hex << ((Value >> 5)& 0x1)      << std::endl;
    std::cout <<"PALPIDE4;" << "REG_FROMU_CONFIG_1;"<< "Test Strobe:     [0] Disabled [1] Enabled;"                             << std::hex << ((Value >> 6)& 0x1)      << std::endl;
    std::cout <<"PALPIDE4;" << "REG_FROMU_CONFIG_1;"<< "Rotate Pulse Lines:     [0] Disabled [1] Enabled;"                      << std::hex << ((Value >> 7)& 0x1)      << std::endl;
    std::cout <<"PALPIDE4;" << "REG_FROMU_CONFIG_1;"<< "Trigger Delay: Latency between cmd and Strobe ;"                        << std::hex << ((Value >> 8)& 0x7)      << std::endl;
    fAlpide->ReadRegister(REG_FROMUCONFIG2_4   ,&Value);
    std::cout <<"PALPIDE4;" << "REG_FROMU_CONFIG_2;"<< "Strobe Timing 25ns-1638ns(step 25ns);"       << std::hex << (Value & 0xffff)      << std::endl;
    fAlpide->ReadRegister(REG_FROMUCONFIG3_4   ,&Value);
    std::cout <<"PALPIDE4;" << "REG_FROMU_CONFIG_3;"<< "Gap between subsequent STROBE;"              << std::hex << (Value & 0xffff)      << std::endl;
    fAlpide->ReadRegister(REG_FROMUPULSING1_4  ,&Value);
    std::cout <<"PALPIDE4;" << "REG_FROMU_PULSE_1;"<< "Delay from the test PULSE to the STROBE 0ns-1638ns (step 25ns);"       << std::hex << (Value & 0xffff)      << std::endl;
    fAlpide->ReadRegister(REG_FROMUPULSING2_4  ,&Value);
    std::cout <<"PALPIDE4;" << "REG_FROMU_PULSE_2;"<< "Test Pulse Duration  0ns-1638ns (step 25ns);;"       << std::hex << (Value & 0xffff)      << std::endl;
    fAlpide->ReadRegister(REG_FROMUSTATUS1_4   ,&Value);
    std::cout <<"PALPIDE4;" << "REG_FROMU_STATUS_1;"<< "Status1: TRIGGER Counter;"       << std::hex << (Value & 0xffff)      << std::endl;   
    fAlpide->ReadRegister(REG_FROMUSTATUS2_4   ,&Value);
    std::cout <<"PALPIDE4;" << "REG_FROMU_STATUS_2;"<< "Status2: STROBE Counter;"       << std::hex << (Value & 0xffff)      << std::endl;     
    fAlpide->ReadRegister(REG_FROMUSTATUS3_4   ,&Value);
    std::cout <<"PALPIDE4;" << "REG_FROMU_STATUS_3;"<< "Status3: MATRIX READOUT Counter;"       << std::hex << ((Value >> 12)& 0xffff)      << std::endl; 
    fAlpide->ReadRegister(REG_FROMUSTATUS4_4   ,&Value);
    std::cout <<"PALPIDE4;" << "REG_FROMU_STATUS_4;"<< "Status4: FRAME Counter;"       << std::hex << ((Value >> 12)& 0xffff)      << std::endl;
    fAlpide->ReadRegister(REG_FROMUSTATUS5_4   ,&Value);
    std::cout <<"PALPIDE4;" << "REG_FROMU_STATUS_5;"<< "Status5: BUNCH Counter;"       << std::hex << (Value & 0xfff)      << std::endl; 
    std::cout <<"PALPIDE4;" << "REG_FROMU_STATUS_5;"<< "Status5: EVENT Counter;"       << std::hex << ((Value >> 12)& 0x3)      << std::endl; 
    std::cout <<"PALPIDE4;" << "REG_FROMU_STATUS_5;"<< "Status5: FRAME Extended;"      << std::hex << ((Value >> 15)& 0x1)      << std::endl; 
    fAlpide->ReadRegister(REG_CLKCURRENTS_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "SET_DCLK_MCLK_CURRENT;" <<"DCLK  Receiver;"   << std::hex << (Value & 0xf)	   << std::endl;
    std::cout <<"PALPIDE4;" << "SET_DCLK_MCLK_CURRENT;" <<"DCLK Driver;"   << std::hex << ((Value >> 4) & 0xf) << std::endl;
    std::cout <<"PALPIDE4;" << "SET_DCLK_MCLK_CURRENT;" <<"MCLK  Receiver;"   << std::hex << ((Value >> 8) & 0xf) << std::endl;
    fAlpide->ReadRegister(REG_CMUCURRENTS_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "SET_DAC_CMU;" <<"DCTRL Receiver;"<< std::hex << (Value & 0xf)	     << std::endl;
    std::cout <<"PALPIDE4;" << "SET_DAC_CMU;" <<"DCTRL Driver;"  << std::hex << ((Value >> 4) & 0xf) << std::endl;
    fAlpide->ReadRegister(REG_CMUDMU_CONFIG_4  ,&Value);
    std::cout <<"PALPIDE4;" << "REG_CMU_DMU;"<<"Previous Chip Id;"				     << std::hex << (Value & 0xf)	  << std::endl;
    std::cout <<"PALPIDE4;" << "REG_CMU_DMU;"<<"Initial Token;"				             << std::hex << ((Value >> 5) & 0x1)  << std::endl;
    std::cout <<"PALPIDE4;" << "REG_CMU_DMU;"<<"Manchester Encoding [0] Enabled [1] Disabled;"       << std::hex << ((Value >> 6) & 0x1)  << std::endl;
    std::cout <<"PALPIDE4;" << "REG_CMU_DMU;"<<"Disable Double Data Rate [0] Enabled [1] Disabled;"  << std::hex << ((Value >> 7) & 0x1)  << std::endl;
    fAlpide->ReadRegister(REG_CMUERRORS_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_CMU_ERR;" <<"Deserializer Error Counter;"	<< std::hex << (Value & 0xf)  << std::endl; 
    std::cout <<"PALPIDE4;" << "REG_CMU_ERR;" <<"Time Out Error Counter;"	<< std::hex << ((Value >> 4) & 0xf)  << std::endl;
    std::cout <<"PALPIDE4;" << "REG_CMU_ERR;" <<"Unknown Error Counter;"	<< std::hex << ((Value >> 8) & 0xf)  << std::endl;
    fAlpide->ReadRegister(REG_DMUFIFO1_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_DMU_FIFO_LSB;" <<"DMU data or Busy Fifo (LSB);" << std::hex << (Value & 0xffff)  << std::endl; 
    fAlpide->ReadRegister(REG_DMUFIFO2_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_DMU_FIFO_MSB;" <<"DMU data or Busy Fifo (MSB);" << std::hex << (Value & 0xffff)  << std::endl;
    fAlpide->ReadRegister(REG_DTUCONFIG_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_DTU_CONF;" <<"PLL VCO Delay Stages Control;" << std::hex << (Value & 0x3)  << std::endl;
    std::cout <<"PALPIDE4;" << "REG_DTU_CONF;" <<"PLL Bandwith Control;" << std::hex << ((Value >> 2) & 0x1)  << std::endl;
    std::cout <<"PALPIDE4;" << "REG_DTU_CONF;" <<"PLL off signal;" << std::hex << ((Value >> 3) & 0x1)  << std::endl;
    std::cout <<"PALPIDE4;" << "REG_DTU_CONF;" <<"Serializer Phase;"  << std::hex << ((Value >> 4) & 0xf)  << std::endl;    
    std::cout <<"PALPIDE4;" << "REG_DTU_CONF;" <<"PLL Reset;"	      << std::hex << ((Value >> 8) & 0x1)  << std::endl;
    std::cout <<"PALPIDE4;" << "REG_DTU_CONF;" <<"Status;"	      << std::hex << ((Value >> 12)& 0x1)  << std::endl;
    fAlpide->ReadRegister(REG_DTUDACS_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_DTU_DAC;" <<"PLL (default 8);"                    << std::hex << (Value        & 0xf)  << std::endl; 
    std::cout <<"PALPIDE4;" << "REG_DTU_DAC;" <<"High Speed Line Driver (default 8);" << std::hex << ((Value >> 4) & 0xf)  << std::endl;
    std::cout <<"PALPIDE4;" << "REG_DTU_DAC;" <<"Pre-Emphasis Driver(default 0);"     << std::hex << ((Value >> 8) & 0xf)  << std::endl;
    fAlpide->ReadRegister(REG_DTU_PLLLOCKREG1_4,&Value);
    std::cout <<"PALPIDE4;" << "REG_DTU_LOCK_PLL1;" <<"Look Counter;"      << std::hex << (Value        & 0xff) << std::endl; 
    std::cout <<"PALPIDE4;" << "REG_DTU_LOCK_PLL1;" <<"Look Flag;"         << std::hex << ((Value >> 8) & 0x1)  << std::endl;
    std::cout <<"PALPIDE4;" << "REG_DTU_LOCK_PLL1;" <<"Look Status;"       << std::hex << ((Value >> 9) & 0x1)  << std::endl;
    fAlpide->ReadRegister(REG_DTU_PLLLOCKREG2_4,&Value);
    std::cout <<"PALPIDE4;" << "REG_DTU_PLL2;" <<"Look Wait Cycles;"       << std::hex << (Value & 0xff)  << std::endl; 
    std::cout <<"PALPIDE4;" << "REG_DTU_PLL2;" <<"Unlook Wait Cycles;"     << std::hex << ((Value >> 8) & 0xff)  << std::endl;
    fAlpide->ReadRegister(REG_DTU_TESTREG1_4   ,&Value);
    std::cout <<"PALPIDE4;" << "REG_DTU_TEST1;" <<"Test [0] Disable [1]Enable;"			<< std::hex << (Value         & 0x1)	<< std::endl;	  
    std::cout <<"PALPIDE4;" << "REG_DTU_TEST1;" <<"Internal Path:[0] Disable [1]Enable;"  	<< std::hex << ((Value >> 1)  & 0x1) << std::endl;
    std::cout <<"PALPIDE4;" << "REG_DTU_TEST1;" <<"Test Single Mode:[0] Disable [1]Enable;"  	<< std::hex << ((Value >> 2)  & 0x1) << std::endl;
    std::cout <<"PALPIDE4;" << "REG_DTU_TEST1;" <<"Prbs [0] Disable [1]Enable;"	    		<< std::hex << ((Value >> 3)  & 0x3) << std::endl;
    std::cout <<"PALPIDE4;" << "REG_DTU_TEST1;" <<"Bypass 8b10b: [0] Disable [1]Enable;"  	<< std::hex << ((Value >> 5)  & 0x1) << std::endl;
    std::cout <<"PALPIDE4;" << "REG_DTU_TEST1;" <<"BDIN8b10b0;"					<< std::hex << ((Value >> 6)  & 0x3) << std::endl;
    std::cout <<"PALPIDE4;" << "REG_DTU_TEST1;" <<"BDIN8b10b1;"					<< std::hex << ((Value >> 8)  & 0x3) << std::endl;
    std::cout <<"PALPIDE4;" << "REG_DTU_TEST1;" <<"BDIN8b10b2;"					<< std::hex << ((Value >> 10) & 0x3) << std::endl;
    std::cout <<"PALPIDE4;" << "REG_DTU_TEST1;" <<"K0;"						<< std::hex << ((Value >> 12) & 0x1) << std::endl;
    std::cout <<"PALPIDE4;" << "REG_DTU_TEST1;" <<"K1;"						<< std::hex << ((Value >> 13) & 0x1) << std::endl;
    std::cout <<"PALPIDE4;" << "REG_DTU_TEST1;" <<"K2;"						<< std::hex << ((Value >> 14) & 0x1) << std::endl;
    fAlpide->ReadRegister(REG_DTU_TESTREG2_4   ,&Value);
    std::cout <<"PALPIDE4;" << "REG_DTU_TEST2;" <<"DIN0;"	<< std::hex << (Value & 0xff)	<< std::endl;	  
    std::cout <<"PALPIDE4;" << "REG_DTU_TEST2;" <<"DIN1;"  	<< std::hex << ((Value >> 8) & 0xff) << std::endl;
    fAlpide->ReadRegister(REG_DTU_TESTREG3_4   ,&Value);
    std::cout <<"PALPIDE4;" << "REG_DTU_TEST3;" <<"DIN2;"	<< std::hex << (Value & 0xff)	<< std::endl;
    std::cout <<"PALPIDE4;" << "REG_DTU_TEST3;" <<"Force Load Enable High;" << std::hex << ((Value >> 8) & 0x1) << std::endl;
    std::cout <<"PALPIDE4;" << "REG_DTU_TEST3;" <<"Force Load Enable Low;"  << std::hex << ((Value >> 9) & 0x1) << std::endl;
    fAlpide->ReadRegister(REG_BUSYWIDTH_4	 ,&Value);         
    std::cout <<"PALPIDE4;" << "REG_BUSY_MIN_WIDTH;" <<"Length of BUSY signal (step 25ns);"	<< std::hex << (Value & 0x1f)	<< std::endl; 
    
    fAlpide->ReadRegister(REG_PIXELCONFIG1,&Value);
    std::cout <<"PALPIDE4;" << "PIXEL_CFG_REG;"<< "REGSEL [0] Mask Enable Register;[1] Pulse Register;" << std::hex << (Value       & 0x1) << std::endl;
    std::cout <<"PALPIDE4;" << "PIXEL_CFG_REG;"<< "Data to be written in the target register;" 		<< std::hex << ((Value >> 1)& 0x1) << std::endl;    
    
    fAlpide->ReadRegister(REG_MONITORING_4,&Value);
    std::cout <<"PALPIDE4;" << "MONITORING_OVERRIDE_CTRL_REG;"<<"Voltage DAC Selection;"            	<< std::hex << (Value & 0xf)         << std::endl;
    std::cout <<"PALPIDE4;" << "MONITORING_OVERRIDE_CTRL_REG;"<<"Current DAC Selection;"            	<< std::hex << ((Value >> 4) & 0x7)  << std::endl;
    std::cout <<"PALPIDE4;" << "MONITORING_OVERRIDE_CTRL_REG;"<<"Configure the current DACMONI block;"  << std::hex << ((Value >> 7) & 0x1)  << std::endl;
    std::cout <<"PALPIDE4;" << "MONITORING_OVERRIDE_CTRL_REG;"<<"Configure the voltage DACMONV block;"  << std::hex << ((Value >> 8) & 0x1)  << std::endl; 
    std::cout <<"PALPIDE4;" << "MONITORING_OVERRIDE_CTRL_REG;"<<"Iref Buffer Current;"  		<< std::hex << ((Value >> 9) & 0x3)  << std::endl;
    fAlpide->ReadRegister(REG_VRESETP_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_VRESETP;"	<< "VRESETP;"   << std::hex << (Value & 0xff)  << std::endl;
    fAlpide->ReadRegister(REG_VRESETD_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_VRESETD;"	<< "VRESETD;"   << std::hex << (Value & 0xff)  << std::endl;
    fAlpide->ReadRegister(REG_VCASN_4 	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_VCASN;"   	<< "VCASN;"	<< std::hex << (Value & 0xff)  << std::endl;
    fAlpide->ReadRegister(REG_VCASP_4 	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_VCASP;"	<< "VCASP;"     << std::hex << (Value & 0xff)  << std::endl;
    fAlpide->ReadRegister(REG_VPULSEH 	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_VPULSEH;"	<< "VPULSEH;"	<< std::hex << (Value & 0xff)  << std::endl;
    fAlpide->ReadRegister(REG_VPULSEL 	 ,&Value);
    std::cout <<"PALPIDE4;" << " REG_VPULSEL;"  << "VPULSEL;"	<< std::hex << (Value & 0xff)  << std::endl;
    fAlpide->ReadRegister(REG_VCASN2_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_VCASN2;"   	<< "VCASN2;"    << std::hex << (Value & 0xff)  << std::endl;
    fAlpide->ReadRegister(REG_VCLIP_4   	 ,&Value);
    std::cout <<"PALPIDE4;" << " REG_VCLIP;"	<< "VCLIP;"     << std::hex << (Value & 0xff)  << std::endl;
    fAlpide->ReadRegister(REG_VTEMP_4   	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_VTEMP;"   	<< "VTEMP;"	<< std::hex << (Value & 0xff)  << std::endl;
    fAlpide->ReadRegister(REG_IAUX2_4   	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_IAUX2;"	<< "IAUX2;"     << std::hex << (Value & 0xff)  << std::endl;
    fAlpide->ReadRegister(REG_IRESET_4  	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_IRESET;"	<< "IRESET;"    << std::hex << (Value & 0xff)  << std::endl;
    fAlpide->ReadRegister(REG_IDB_4	  	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_IDB;"   	<< "IDB;"       << std::hex << (Value & 0xff)  << std::endl;
    fAlpide->ReadRegister(REG_IBIAS_4   	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_IBIAS;"	<< "IBIAS;"     << std::hex << (Value & 0xff)  << std::endl;
    fAlpide->ReadRegister(REG_ITHR_4    	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_ITHR;"   	<< "ITHR;"      << std::hex << (Value & 0xff)  << std::endl;
            
    fAlpide->ReadRegister(REG_BUFF_BYPASS_4 	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_BUFF_BYPASS;"	<< "[1] Bypass VCASN;"	<< std::hex << (Value & 0x1)        << std::endl;
    std::cout <<"PALPIDE4;" << "REG_BUFF_BYPASS;" 	<< "[1] Bypass VCASN2;"	<< std::hex << ((Value >> 1) & 0x1) << std::endl;
    std::cout <<"PALPIDE4;" << "REG_BUFF_BYPASS;" 	<< "[1] Bypass VCASP;"	<< std::hex << ((Value >> 2) & 0x1) << std::endl;
    std::cout <<"PALPIDE4;" << "REG_BUFF_BYPASS;" 	<< "[1] Bypass VCLIP;"	<< std::hex << ((Value >> 3) & 0x1) << std::endl;
    std::cout <<"PALPIDE4;" << "REG_BUFF_BYPASS;" 	<< "[1] Bypass IRESET;"	<< std::hex << ((Value >> 4) & 0x1) << std::endl;
    std::cout <<"PALPIDE4;" << "REG_BUFF_BYPASS;" 	<< "[1] Bypass IBIAS;"	<< std::hex << ((Value >> 5) & 0x1) << std::endl;
    std::cout <<"PALPIDE4;" << "REG_BUFF_BYPASS;" 	<< "[1] Bypass ITHR;"	<< std::hex << ((Value >> 6) & 0x1) << std::endl;
    std::cout <<"PALPIDE4;" << "REG_BUFF_BYPASS;" 	<< "[1] Bypass IDB;"	<< std::hex << ((Value >> 7) & 0x1) << std::endl;
    fAlpide->ReadRegister(REG_ADCCONTROL_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_ADC_CONTROL_REG;"	<< "Mode [0] Manual [1] Calibration [2] Ramp Speed [3] Super-Manual;"  << std::hex << (Value &        0x3) << std::endl;
    std::cout <<"PALPIDE4;" << "REG_ADC_CONTROL_REG;"	<< "Select Input: Check table in the manual;"                          << std::hex << ((Value >> 2) & 0xf) << std::endl;
    std::cout <<"PALPIDE4;" << "REG_ADC_CONTROL_REG;"	<< "Comparator Current [0] 163 uA [1] 190 uA [2]296 uA [3] 410 uA;"    << std::hex << ((Value >> 6) & 0x3) << std::endl;
    std::cout <<"PALPIDE4;" << "REG_ADC_CONTROL_REG;"	<< "Descriminator Sign;"                                               << std::hex << ((Value >> 8) & 0x1) << std::endl;
    std::cout <<"PALPIDE4;" << "REG_ADC_CONTROL_REG;"	<< "Ramp Speed [0] 500 ns [1] 1 us [2] 2 us [3] 4 us;"                 << std::hex << ((Value >> 9) & 0x3) << std::endl;
    std::cout <<"PALPIDE4;" << "REG_ADC_CONTROL_REG;"	<< "Half LSB Trim;"                                                    << std::hex << ((Value >> 11)& 0x1) << std::endl;
    std::cout <<"PALPIDE4;" << "REG_ADC_CONTROL_REG;"	<< "Comparator Output;"                                                << std::hex << ((Value >> 15)& 0x1) << std::endl;
    fAlpide->ReadRegister(REG_ADCDAC_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_ADC_DAC;"   	<< "DAC Input(Super Manual Mode);"	 << std::hex << (Value & 0xffff)  << std::endl;
    fAlpide->ReadRegister(REG_ADCCALIB_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_ADC_CALIB;"   	<< "ADC Calibration (Calibration Mode);" << std::hex << (Value & 0xffff)  << std::endl;
    fAlpide->ReadRegister(REG_ADCAVSS_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_ADC_AVSS;"   	<< "ADC AVSS;"    << std::hex << (Value & 0x7ff)  << std::endl;
    fAlpide->ReadRegister(REG_ADCDVSS_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_ADC_DVSS;"   	<< "ADC DVSS;"    << std::hex << (Value & 0x7ff)  << std::endl;
    fAlpide->ReadRegister(REG_ADCAVDD_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_ADC_AVDD;"   	<< "ADC AVDD;"    << std::hex << (Value & 0x7ff)  << std::endl;
    fAlpide->ReadRegister(REG_ADCDVDD_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_ADC_DVDD;"   	<< "ADC DVDD;"    << std::hex << (Value & 0x7ff)  << std::endl;
    fAlpide->ReadRegister(REG_ADCVCASN_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_ADC_VCASN;"   	<< "ADC VCASN;"   << std::hex << (Value & 0x7ff)  << std::endl;
    fAlpide->ReadRegister(REG_ADCVCASP_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_ADC_VCASP;"   	<< "ADC VCASP;"   << std::hex << (Value & 0x7ff)  << std::endl;
    fAlpide->ReadRegister(REG_ADCVPULSEH_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_ADC_VPULSEH;"   	<< "ADC VPULSEH;" << std::hex << (Value & 0x7ff)  << std::endl;
    fAlpide->ReadRegister(REG_ADCVPULSEL_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_ADC_VPULSEL;"   	<< "ADC VPULSEL;" << std::hex << (Value & 0x7ff)  << std::endl;
    fAlpide->ReadRegister(REG_ADCVRESETP_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_ADC_VRESETP;"   	<< "ADC VRESETP;" << std::hex << (Value & 0x7ff)  << std::endl;
    fAlpide->ReadRegister(REG_ADCVRESETD_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_ADC_VRESETD;"   	<< "ADC VRESETD;" << std::hex << (Value & 0x7ff)  << std::endl;
    fAlpide->ReadRegister(REG_ADCVCASN2_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_ADC_VCASN2;"   	<< "ADC VCASN2;"  << std::hex << (Value & 0x7ff)  << std::endl;
    fAlpide->ReadRegister(REG_ADCVCLIP_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_ADC_VCLIP;"   	<< "ADC VCLIP;"   << std::hex << (Value & 0x7ff)  << std::endl;
    fAlpide->ReadRegister(REG_ADCVTEMP_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_ADC_VTEMP;"   	<< "ADC VTEMP;"   << std::hex << (Value & 0x7ff)  << std::endl;
    fAlpide->ReadRegister(REG_ADCITHR_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_ADC_ITHR;"   	<< "ADC VITHR;"   << std::hex << (Value & 0x7ff)  << std::endl;
    fAlpide->ReadRegister(REG_ADCIREF_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_ADC_IREF;"   	<< "ADC VIREF;"   << std::hex << (Value & 0x7ff)  << std::endl;
    fAlpide->ReadRegister(REG_ADCIDB_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_ADC_IDB;"   	<< "ADC IDB;"     << std::hex << (Value & 0x7ff)  << std::endl;
    fAlpide->ReadRegister(REG_ADCIBIAS_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_ADC_IBIAS;"   	<< "ADC IBIAS;"   << std::hex << (Value & 0x7ff)  << std::endl;
    fAlpide->ReadRegister(REG_ADCIAUX2_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_ADC_IAUX2;"   	<< "ADC VIAUX2;"  << std::hex << (Value & 0x7ff)  << std::endl;
    fAlpide->ReadRegister(REG_ADCIRESET_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_ADC_IRESET;"   	<< "ADC VIRESET;" << std::hex << (Value & 0x7ff)  << std::endl;
    fAlpide->ReadRegister(REG_ADCBG2V_4	 ,&Value);
    std::cout <<"PALPIDE4;" << "REG_ADC_BG2V;"   	<< "ADC VBG2V;"   << std::hex << (Value & 0x7ff)  << std::endl;
    fAlpide->ReadRegister(REG_ADCT2V_4	 ,&Value); 
    std::cout <<"PALPIDE4;" << "REG_ADC_T2V;"   	<< "ADC T2V;"     << std::hex << (Value & 0x7ff)  << std::endl;   
    }            
//
//--------------------------------------------------------------------------------------------- 
int main(int argc, const char * argv[])
{
    if (CreateTestSetup() > 0){	
        SetDescriptorRegister();
        if ( fChipType == 1){
	   std::cout << "READ REGISTER PALPIDE FS 1"<< std::endl;
	   ReadFPGARegister1();
	   ReadCHIPRegister1();
	   }
	if ( fChipType == 2){
	   std::cout << "READ REGISTER PALPIDE FS 2"<< std::endl;
	   ReadFPGARegister2();
	   ReadCHIPRegister2();
	   }
	 if ( fChipType == 3){
	   std::cout << "READ REGISTER PALPIDE FS 3"<< std::endl;
	   ReadFPGARegister2();
	   ReadCHIPRegister3();
	   }
	 if ( fChipType == 4){
	   std::cout << "READ REGISTER PALPIDE FS 4"<< std::endl;
	   ReadFPGARegister2();
	   ReadCHIPRegister4();
	   }       
        }
   cleanExit();
   return 0;
}
