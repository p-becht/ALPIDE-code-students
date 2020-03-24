//
//  main.cpp
//  pALPIDEfs-software
//
//  Created by Carlo Puggioni and Markus Keil on 18/03/14.
//  Copyright (c) 2014 Carlo Puggioni, Markus Keil. All rights reserved.
//


// TODO: Write method for digital injection, analogue to PreparethresholdScan etc...

#include <iostream>
//#include <thread>
#include <unistd.h>
//#include "libusb.h"
#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <ctime>
#include <chrono>

#include "USB.h"
#include "TTestsetup.h"
#include "TModuleSetup.h"
#include "TDaqboard.h"
#include "TPalpidefs.h"
#include "chiptests.h"
#include "TConfig.h"

using namespace std;

int  myArgc;
char myArgv[11][50];
int  maxArgs = 10;
bool fWriteConfig;
bool fPowerOn;
bool fPowerOff;

// Creates the daq board and chip objects
// Chip and DAQ Board type (pALPIDE1/2) are determined in AddDUTs / FindDAQBoards from the corresponding value in the config

bool InitSetup(TTestSetup*& Ats, TpAlpidefs*& myAlpide, int board_id, TConfig *AConfig) {
    // if setup type module -> Ats = new TModuleSetup
    if (fConfig->GetType() == TYPE_MODULE) {
      Ats = new TModuleSetup ();
    }
    else {
      Ats = new TTestSetup ();
    }
    std::cout << "Searching for DAQ boards " << std::endl;
    int err = Ats->FindDAQBoards(AConfig);
    if (err) return false;
    int NBoards = Ats->GetNDAQBoards();

    if (NBoards == 0) return false;
    //std::cout << "Before AddDUTS" << std::endl;
    Ats->AddDUTs(AConfig);
    //std::cout << "After AddDUTS" << std::endl;

    return true;
}


// function to find correct USB_id from a config variable DUTID
// interpretation of DUTID depends on device type
void SetDUT (TTestSetup *Ats, TpAlpidefs*& myAlpide, int ADUTID) {
  if (fConfig->GetType() == TYPE_CHIP) {
    USB_id = 0;
  }
  else if (fConfig->GetType() == TYPE_MODULE) {
    std::cout << "Getting DUT id for chip " << ADUTID << std::endl;
    USB_id = ((TModuleSetup*)Ats)->GetChipIndexByID(ADUTID);
  }
  else if (fConfig->GetType() == TYPE_TELESCOPE) {
    USB_id = Ats->GetBoardIndexByAddress(ADUTID);
  }
  if (USB_id == -1) {
    std::cout << "Error, bad DUT ID " << ADUTID << std::endl;
    exit (EXIT_FAILURE);
  }
  myAlpide = (TpAlpidefs *) Ats->GetDUT(USB_id);
}


void IRpicture (TTestSetup *ts, TpAlpidefs *myAlpide) {
    for (int i = 0; i < 11; i++) {
        myAlpide->SetDAC( i, 0);
    }
    myAlpide->OverrideDACs(SWCNTL_VAUX, DACI_NONE);
    //ts->GetDAQBoard(0)->SetPowerOnSequence (10, 0, 12, 13);

    std::cout << "Overriding VAux DAC..." << std::endl;
    for (int i = 0; i < 600; i++) {
        std::cout << 600 -i << std::endl;
        sleep(1);
    }
}


void cleanExit(TTestSetup *Ats, int AExitValue) {
    struct libusb_context *context = Ats->GetContext();
    delete Ats;
    libusb_exit(context);
    exit (AExitValue);
}


void printUsage() {
    std::cout << "Start program with one of the following options, parameters in [] optional: " << std::endl << std::endl;

    std::cout << "POWERON                          - switches chip power on and initialises chip" << std::endl;
    std::cout << "POWEROFF                         - switches chip power off" << std::endl;
    std::cout << "READCURRENTS                     - reads currents and temp without switching chip on or off" << std::endl;
    std::cout << "FIFO                             - runs a fifo test" << std::endl;
    std::cout << "READDACS                         - reads all chip DACs once" << std::endl;
    std::cout << "SCANDACS [PAR1]                  - scans all chip DACs and writes the characteristics to file. PAR1 is the distance between the sampling points (default 1)" << std::endl;
    std::cout << "SCANDIGITAL PAR1 PAR2 [PAR3]     - Digital scan with PAR1 events per pix and PAR2 mask stages and PAR3 pixels per stage (default 1)" << std::endl;
    std::cout << "SCANDIGCLUSTER PAR1 PAR2 PAR3    - Inject cluster pattern, like digital scan, PAR3 file with pattern" << std::endl;
    std::cout << "SCANANALOGUE PAR1 PAR2 PAR3 [PAR4]- Analogue scan, PARs: Charge (in DAC units), events per pix, mask stages, pixels per stage (default 1)" << std::endl;
    std::cout << "THRESHOLD PAR1 .. PAR3 [PAR4/5/6] - Threshold scan, PARs: #Mask stages, start charge, stop charge, [VCASN, ITH, IDB, [VCASN2, VCLIP]]" << std::endl;
    std::cout << "PULSESHAPE PAR1 .. PAR6 [PAR7/8/9]- Pulseshape scan, PARs: #evts, dcol (0..15), address, delay start, stop, step [, VCASN, ITH, IDB]" << std::endl;
    std::cout << "NOISEOCC PAR1                    - Noise occupancy run, PAR1: Number of events" << std::endl;
    std::cout << "NOISEOCC PAR1 PAR2 PAR3 [PAR4]   - Noise occupancy run, PAR1: Number of events, PAR2: VCASN, PAR3: ITH, [Mask file]" << std::endl;
    std::cout << "PIXNOISEOCC PAR1 PAR2 ... PAR4   - Noise occupancy run, 1 pixel, PAR1: Number of events, PAR2/3/4: Region/DCol/Address" << std::endl;
    std::cout << "NOISEOCC_A PAR1 [PAR2 .. PAR4]   - Same as Noise occupancy but with Readout Mode A instead of B" << std::endl;
    std::cout << "PIXNOISEOCC_A PAR1 .. PAR4       - Same as PIXNOISEOCC but with Readout Mode A instead of B" << std::endl;
    std::cout << "NOISEOCCSCAN PAR1-5 [PAR6]       - Noise occupancy scan, PAR1: Number of events, PAR2-3: VCASN range, PAR4-5: ITHR range, PAR6: mask file" << std::endl;
    std::cout << "SOURCE PAR1 [PAR2]               - Source run, PAR1: Number of events, PAR2: mask file" << std::endl;
    std::cout << "SOURCE_A PAR1 [PAR2]             - Same as source run but with readout Mode A instead of B" << std::endl;
    std::cout << "NOISEMASK PAR1 PAR2 [PAR3..5]    - Prepares a noise mask, PARs: number of events, output file, [VCASN, ITH, Noise cut]" << std::endl;
    std::cout << "RATETEST PAR1 PAR2               - Take data with external trigger. PARs: number of events, print out every N events" << std::endl;
    std::cout << "RAWDUMP PAR1 PAR2                - Take data with external trigger and dump the data. PARs: number of events, print out every N events" << std::endl;
    std::cout << "SOFTRESET PAR1                   - Software Reset. PAR1: Duration of Reset. " << std::endl;
    std::cout << "SOFTRESET_FPGA PAR1              - Software FPGA Reset. PAR1: Duration of Reset. " << std::endl;
    std::cout << "SOFTRESET_FX3 PAR1               - Software FX3 Reset. PAR1: Duration of Reset. " << std::endl;
    std::cout << "FIRMWARE_VERSION                 - Firmware Version " << std::endl;
    std::cout << "SEU-PIXEL-DIGITIAL PAR1 PAR2 PAR3  - Measure SEUs inside pixel memories using digtial pulsing. PAR1: initial value of mask, PAR2: number of readouts, PAR3: time between readouts" << std::endl;
    std::cout << "SEU-PIXEL-ANALOGUE PAR1 PAR2 PAR3  - Measure SEUs inside pixel memories using digtial pulsing. PAR1: initial value of mask, PAR2: number of runs, PAR3: number of readouts/run" << std::endl;
    std::cout << "SEU-MEMORY PAR1 PAR2 PAR3        - Measure SEUs inside event memories. PAR1: inital memory content (-1: uniqe word per address), PAR2: number of readouts, PAR3: time between readouts" << std::endl;
    std::cout << std::endl;
}


void DoTheTest(TTestSetup *Ats, TpAlpidefs *AAlpide) {
    TAlpideMode ReadoutMode;
    if (strcmp (myArgv[0], "POWERON") == 0) {
        cleanExit(Ats, 0);
    }
    if (strcmp (myArgv[0], "POWEROFF") == 0) {
        Ats->PowerOffBoard(USB_id);
        cleanExit(Ats, 0);
    }
    else if (strcmp (myArgv[0], "FIFO") == 0) {
        FifoTest(AAlpide);
    }
    else if (strcmp (myArgv[0], "READDACS") == 0) {
        Ats->ReadAllChipDacs(0);
    }
    else if (strcmp (myArgv[0], "SCANDACS") == 0) {
        int SampleDistance = 1;
        if (myArgc == 2)
            SampleDistance = atoi(myArgv[1]);
        Ats->ScanAllChipDacs(0, SampleDistance);
    }
    else if (strcmp (myArgv[0], "SCANDIGITAL") == 0) {
        fWriteConfig = true;
        if (myArgc < 3) {
            printUsage();
            Ats->PowerOffBoard(USB_id);
            cleanExit(Ats, 0);
        }
        else {
            int NEvents     = atoi(myArgv[1]);
            int NMaskStages = atoi(myArgv[2]);
            int NPixels     = 1;
            if (myArgc == 4)
                NPixels = atoi(myArgv[3]);
            DigitalScan(Ats, AAlpide, NEvents, NMaskStages, NPixels);
        }
    }
    else if (strcmp (myArgv[0], "SCANDIGCLUSTER") == 0) {
        fWriteConfig = true;
        if (myArgc == 4) {
            int NEvents     = atoi(myArgv[1]);
            int NMaskStages = atoi(myArgv[2]);
            DigitalScanCluster(Ats, AAlpide, NEvents, NMaskStages, myArgv[3]);
        }
        else {
            printUsage();
            Ats->PowerOffBoard(USB_id);
            cleanExit(Ats, 0);
        }
    }
    else if (strcmp (myArgv[0], "SCANANALOGUE") == 0) {
        fWriteConfig = true;
        if (myArgc < 4) {
            printUsage();
            Ats->PowerOffBoard(USB_id);
            cleanExit(Ats, 0);
        }
        else {
            int Charge      = atoi(myArgv[1]);
            int NEvents     = atoi(myArgv[2]);
            int NMaskStages = atoi(myArgv[3]);
            int NPixels     = 1;
            if (myArgc == 5)
                NPixels = atoi(myArgv[4]);
            AnalogueScan(Ats, AAlpide, Charge, NEvents, NMaskStages, NPixels);
        }
    }
    else if (strcmp (myArgv[0], "THRESHOLD") == 0) {
        fWriteConfig = true;
        if (myArgc == 4) {
            int NMaskStages = atoi(myArgv[1]);
            int Start       = atoi(myArgv[2]);
            int Stop        = atoi(myArgv[3]);
            ThresholdScanFull(Ats, AAlpide, NMaskStages, Start, Stop);
        }
        else if (myArgc == 7) {
            int NMaskStages = atoi(myArgv[1]);
            int Start       = atoi(myArgv[2]);
            int Stop        = atoi(myArgv[3]);
            int VCASN       = atoi(myArgv[4]);
            int ITH         = atoi(myArgv[5]);
            int IDB         = atoi(myArgv[6]);
            ThresholdScanFull(Ats, AAlpide, NMaskStages, Start, Stop, VCASN, ITH, IDB);
        }
        else if (myArgc == 9) {
            int NMaskStages = atoi(myArgv[1]);
            int Start       = atoi(myArgv[2]);
            int Stop        = atoi(myArgv[3]);
            int VCASN       = atoi(myArgv[4]);
            int ITH         = atoi(myArgv[5]);
            int IDB         = atoi(myArgv[6]);
            int VCASN2      = atoi(myArgv[7]);
            int VCLIP       = atoi(myArgv[8]);
            ThresholdScanFull(Ats, AAlpide, NMaskStages, Start, Stop, VCASN, ITH, IDB, VCASN2, VCLIP);
        }
        else {
            printUsage();
            Ats->PowerOffBoard(USB_id);
            cleanExit(Ats, 0);
        }
    }
    else if (strcmp (myArgv[0], "THRESHOLDFAST") == 0) {
        fWriteConfig = true;
        if (myArgc == 4) {
            int NMaskStages = atoi(myArgv[1]);
            int Start       = atoi(myArgv[2]);
            int Stop        = atoi(myArgv[3]);
            ThresholdScanFast(Ats, AAlpide, NMaskStages, Start, Stop);
        }
        else if (myArgc == 6) {
            int NMaskStages = atoi(myArgv[1]);
            int Start       = atoi(myArgv[2]);
            int Stop        = atoi(myArgv[3]);
            int VCASN       = atoi(myArgv[4]);
            int ITH         = atoi(myArgv[5]);
            ThresholdScanFast(Ats, AAlpide, NMaskStages, Start, Stop, VCASN, ITH);
        }
        else {
            printUsage();
            Ats->PowerOffBoard(USB_id);
            cleanExit(Ats, 0);
        }
    }
    else if (strcmp (myArgv[0], "PULSESHAPE") == 0) {
        fWriteConfig = true;
        if (myArgc == 7) {
            int NEvts   = atoi(myArgv[1]);
            int DCol    = atoi(myArgv[2]);
            int Address = atoi(myArgv[3]);
            int Start   = atoi(myArgv[4]);
            int Stop    = atoi(myArgv[5]);
            int Step    = atoi(myArgv[6]);
            PulselengthScan (Ats, AAlpide, NEvts, DCol, Address, Start, Stop, Step);
        }
        else if (myArgc == 10) {
            int NEvts   = atoi(myArgv[1]);
            int DCol    = atoi(myArgv[2]);
            int Address = atoi(myArgv[3]);
            int Start   = atoi(myArgv[4]);
            int Stop    = atoi(myArgv[5]);
            int Step    = atoi(myArgv[6]);
            int VCASN   = atoi(myArgv[7]);
            int ITH     = atoi(myArgv[8]);
            int IDB     = atoi(myArgv[9]);
            PulselengthScan (Ats, AAlpide, NEvts, DCol, Address, Start, Stop, Step, VCASN, ITH, IDB);
        }
    }
    else if (strcmp (myArgv[0], "PARAMETER") == 0) {
        fWriteConfig = true;
        if (myArgc == 7) {
  	    char DAC[10];
            int Charge      = atoi(myArgv[2]);
            int NMaskStages = atoi(myArgv[3]);
            int Start       = atoi(myArgv[4]);
            int Stop        = atoi(myArgv[5]);
            int Step        = atoi(myArgv[6]);
            DACParameterScan (Ats, AAlpide, NMaskStages, Charge, myArgv[1], Start, Stop, Step);
	}
        else {
            printUsage();
            Ats->PowerOffBoard(USB_id);
            cleanExit(Ats, 0);
        }
    }
    else if ((strcmp (myArgv[0], "NOISEOCC") == 0) || (strcmp (myArgv[0], "NOISEOCC_A") == 0)) {
        fWriteConfig = true;
        if (strcmp (myArgv[0], "NOISEOCC") == 0) {
            ReadoutMode = MODE_ALPIDE_READOUT_B;
        }
        else {
            ReadoutMode = MODE_ALPIDE_READOUT_A;
        }
        if (myArgc < 2) {
            printUsage();
            Ats->PowerOffBoard(USB_id);
            cleanExit(Ats, 0);
        }
        else if (myArgc == 5) {   // standard with mask
            int NEvents = atoi(myArgv[1]);
            int VCASN   = atoi(myArgv[2]);
            int ITH     = atoi(myArgv[3]);
            int StrobeBLength = atoi(myArgv[4]);
            if (StrobeBLength > 0) { // integer number -> StrobeBLength
              fConfigGeneral->StrobeBlengthStandard = StrobeBLength;
              fConfig->GetChipConfig()->StrobeBLength = StrobeBLength;
              NoiseOccupancy(Ats, AAlpide, NEvents, ReadoutMode, true, VCASN, ITH);
	    }
            else {                   // no integer number -> Mask File
              NoiseOccupancy(Ats, AAlpide, NEvents, ReadoutMode, true, VCASN, ITH, myArgv[4]);
	    }
        }
        else if (myArgc == 4) {   // standard without mask
            int NEvents = atoi(myArgv[1]);
            int VCASN   = atoi(myArgv[2]);
            int ITH     = atoi(myArgv[3]);
            NoiseOccupancy(Ats, AAlpide, NEvents, ReadoutMode, true, VCASN, ITH);
        }
        else {
            int NEvents     = atoi(myArgv[1]);
            NoiseOccupancy(Ats, AAlpide, NEvents, ReadoutMode);
        }
    }
    else if ((strcmp (myArgv[0], "PIXNOISEOCC") == 0) || (strcmp (myArgv[0], "PIXNOISEOCC_A") == 0)) {
        fWriteConfig = true;
        if (strcmp (myArgv[0], "PIXNOISEOCC") == 0) {
            ReadoutMode = MODE_ALPIDE_READOUT_B;
        }
        else {
            ReadoutMode = MODE_ALPIDE_READOUT_A;
        }
        if (myArgc == 5) {
            int NEvents = atoi(myArgv[1]);
            int Region  = atoi(myArgv[2]);
            int DCol    = atoi(myArgv[3]);
            int Address = atoi(myArgv[4]);
            NoiseOccupancySingle (Ats, AAlpide, NEvents, ReadoutMode, true, Region, DCol, Address);
        }
        else {
            printUsage();
            Ats->PowerOffBoard(USB_id);
            cleanExit(Ats, 0);
        }
    }
    else if ((strcmp (myArgv[0], "SOURCE") == 0) || (strcmp (myArgv[0], "SOURCE_A") == 0)) {
        fWriteConfig = true;
        if (strcmp (myArgv[0], "SOURCE") == 0) {
            ReadoutMode = MODE_ALPIDE_READOUT_B;
        }
        else {
            ReadoutMode = MODE_ALPIDE_READOUT_A;
        }
        if (myArgc < 2) {
            printUsage();
            Ats->PowerOffBoard(USB_id);
            cleanExit(Ats, 0);
        }
        else {
            int NEvents     = atoi(myArgv[1]);
            if (myArgc == 3)  // mask filename given
                SourceScan(Ats, AAlpide, NEvents, ReadoutMode, true, myArgv[2]);
            else
                SourceScan(Ats, AAlpide, NEvents, ReadoutMode, true);
        }
    }
    else if (strcmp (myArgv[0], "NOISEMASK") == 0) {
        fWriteConfig = true;
        if (myArgc == 3) {
            int NEvents = atoi(myArgv[1]);
            PrepareNoiseMask (Ats, AAlpide, NEvents, myArgv[2]);
        }
        else if (myArgc == 5) {
            int NEvents = atoi(myArgv[1]);
            int VCASN   = atoi(myArgv[3]);
            int ITH     = atoi(myArgv[4]);
            PrepareNoiseMask (Ats, AAlpide, NEvents, myArgv[2], -1., VCASN, ITH);
        }
        else if (myArgc == 6) {
            int   NEvents = atoi(myArgv[1]);
            int   VCASN   = atoi(myArgv[3]);
            int   ITH     = atoi(myArgv[4]);
            float Cut     = atof(myArgv[5]);
            PrepareNoiseMask (Ats, AAlpide, NEvents, myArgv[2], Cut, VCASN, ITH);
        }
        else {
            printUsage();
            Ats->PowerOffBoard(USB_id);
            cleanExit(Ats, 0);
        }
    }
    else if (strcmp (myArgv[0], "RATETEST") == 0) {
        if (myArgc < 3) {
            printUsage();
            Ats->PowerOffBoard(USB_id);
            cleanExit(Ats, 0);
        }
        else {
            int NEvents     = atoi(myArgv[1]);
            int NPrintOut   = atoi(myArgv[2]);
            RateNoiseTest(Ats, AAlpide, NEvents, NPrintOut);
            //RateTest(Ats, AAlpide, NEvents, NPrintOut);
            //ReadoutRun(Ats, AAlpide, NEvents, NPrintOut);
            //RateTest2(Ats, AAlpide, NEvents, NPrintOut);
        }
    }
    else if (strcmp (myArgv[0], "RAWDUMP") == 0) {
        if (myArgc < 3) {
            printUsage();
            Ats->PowerOffBoard(USB_id);
            cleanExit(Ats, 0);
        }
        else {
            int NEvents     = atoi(myArgv[1]);
            int NPrintOut   = atoi(myArgv[2]);
            ExtTrigRawDump(Ats, AAlpide, NEvents, NPrintOut);
        }
    }
    else if (strcmp (myArgv[0], "SPECIAL") == 0) {
        if (myArgc > 1) fWriteConfig = true;
        std::cout << "n_args = " << myArgc << std::endl;
        if      (myArgc == 1) RunSpecialTest(Ats, AAlpide);
        else if (myArgc == 2) RunSpecialTest(Ats, AAlpide, atoi(myArgv[1]));
        else if (myArgc == 3) RunSpecialTest(Ats, AAlpide, atoi(myArgv[1]), atoi(myArgv[2]));
        else if (myArgc == 4) RunSpecialTest(Ats, AAlpide, atoi(myArgv[1]), atoi(myArgv[2]), atoi(myArgv[3]));
    }
    else if (strcmp (myArgv[0], "SOFTRESET") == 0) {
        if (myArgc != 2) {
            printUsage();
            exit(0);
        }
        int duration = atoi(myArgv[1]);
        SoftwareReset (Ats, duration);
    }
    else if (strcmp (myArgv[0], "SOFTRESET_FPGA") == 0) {
        if (myArgc != 2) {
            printUsage();
            exit(0);
        }
        int duration = atoi(myArgv[1]);
        SoftwareResetFPGA (Ats, duration);
    }
    else if (strcmp (myArgv[0], "SOFTRESET_FX3") == 0) {
        if (myArgc != 2) {
            printUsage();
            exit(0);
        }
        int duration = atoi(myArgv[1]);
        SoftwareResetFX3 (Ats, duration);
    }
    else if (strcmp (myArgv[0], "HIGHTHRESH") == 0) {
        if (myArgc < 4) {
            printUsage();
            Ats->PowerOffBoard(USB_id);
            cleanExit(Ats, 0);
        }
        else {
            int NMaskStages = atoi(myArgv[1]);
            int Start       = atoi(myArgv[2]);
            int Stop        = atoi(myArgv[3]);
            MyThresholdHigh(Ats, AAlpide, NMaskStages, Start, Stop);
        }

    }
    else if (strcmp (myArgv[0], "SOURCELOOP") == 0) {
        if (myArgc < 4) {
            printUsage();
            Ats->PowerOffBoard(USB_id);
            cleanExit(Ats, 0);
        }
        else {
            int NEvents     = atoi(myArgv[1]);
            int Start       = atoi(myArgv[2]);
            int Stop        = atoi(myArgv[3]);
            SourceLoop(Ats, AAlpide, NEvents, Start, Stop);
        }

    }
    else if (strcmp (myArgv[0], "NOISEOCCSCAN") == 0) {
        fWriteConfig = true;
        if (myArgc < 2) {
            printUsage();
            Ats->PowerOffBoard(USB_id);
            cleanExit(Ats, 0);
        }
        else if (myArgc == 7) {
            int NEvents     = atoi(myArgv[1]);
            unsigned short limits[4] = {30, 70, 30, 70};
            for(int i=2; i<6; ++i) limits[i-2] = atoi(myArgv[i]);
            NoiseOccupancyScan(Ats, AAlpide, NEvents, MODE_ALPIDE_READOUT_B, limits[0], limits[1], limits[2], limits[3], myArgv[6]);
        }
        else {
            int NEvents     = atoi(myArgv[1]);
            unsigned short limits[4] = {30, 70, 30, 70};
            for(int i=2; i<myArgc; ++i) limits[i-2] = atoi(myArgv[i]);
            NoiseOccupancyScan(Ats, AAlpide, NEvents, MODE_ALPIDE_READOUT_B, limits[0], limits[1], limits[2], limits[3]);
        }
    }
    else if (strcmp (myArgv[0], "FIRMWARE_VERSION") == 0) {

        FirmwareVersion(Ats);
    }
    else if (strcmp (myArgv[0], "READCURRENTS") == 0) {
        Ats->GetDAQBoard(USB_id)->ReadAllADCs();
    }
    else if (strcmp (myArgv[0], "FUSES") == 0) {
      if (AAlpide->GetChipType() == DUT_PALPIDEFS2) {
        TpAlpidefs2 *myAlpide2 = (TpAlpidefs2*)AAlpide;
        int Wafer = atoi(myArgv[1]);
        int Chip  = atoi(myArgv[2]);
        myAlpide2->WriteFuseValue(Wafer, Chip);
      }
      else {
	std::cout << "Fuse writing not implemented for this chip type" << std::endl;
      }
    }
    else if (strcmp (myArgv[0], "SEU-PIXEL-DIGITAL") == 0) {
        if (myArgc != 4) {
            printUsage();
            Ats->PowerOffBoard(USB_id);
            cleanExit(Ats, 0);
        }
        int   mask  = atoi(myArgv[1]);
        int   n     = atoi(myArgv[2]);
        float dt    = atof(myArgv[3]);
        SEUPixelDigital(Ats, AAlpide, mask, n, dt);
    }
    else if (strcmp (myArgv[0], "SEU-PIXEL-ANALOGUE") == 0) {
        if (myArgc != 4) {
            printUsage();
            Ats->PowerOffBoard(USB_id);
            cleanExit(Ats, 0);
        }
        int   mask  = atoi(myArgv[1]);
        int   n     = atoi(myArgv[2]);
        float m     = atof(myArgv[3]);
        SEUPixelAnalogue(Ats, AAlpide, mask, n, m);
    }
    else if (strcmp (myArgv[0], "SEU-MEMORY") == 0) {
        if (myArgc != 4) {
            printUsage();
            Ats->PowerOffBoard(USB_id);
            cleanExit(Ats, 0);
        }
        int   pattern = atoi(myArgv[1]);
        int   n       = atoi(myArgv[2]);
        float dt      = atof(myArgv[3]);
        SEUMemory(Ats, AAlpide, pattern, n, dt);
    }
    else {
        std::cout << "Unknown option " << myArgv[0] << std::endl;
        printUsage();
    }

}


void InitVariables (int argc, const char *argv[])
{
#ifdef XCODE
    sprintf(OutputPath, "/Users/mkeil/Work/FSTestsystem/Data");
    sprintf(myArgv[0], "THRESHOLD");
    sprintf(myArgv[1], "10");
    sprintf(myArgv[2], "10");
    sprintf(myArgv[3], "50");
    myArgc = 4;

#else
    sprintf(OutputPath, "./Data");
    for (int i = 0; (i < argc-1) && (i < maxArgs); i ++) {
        sprintf(myArgv[i], "%s", argv[i+1]);
        std::cout << myArgv[i] << std::endl;
    }
    myArgc = argc - 1;
    if (maxArgs < myArgc) {
        std::cout  << "WARNING: value maxArgs smaller than actual value of number of arguments!!! please check!!!" << std::endl;
    }
#endif

    if ((strcmp (myArgv[0], "READCURRENTS") == 0) || (strcmp (myArgv[0], "SOFTRESET") == 0)) {
      fPowerOn  = false;
      fPowerOff = false;
    }
    else if (strcmp (myArgv[0], "POWEROFF") == 0) {
      fPowerOn  = false;
      fPowerOff = true;
    }
    else {
      fPowerOn  = true;
      fPowerOff = true;
    }

   // date_time suffix for output file names
    time_t       t = time(0);   // get time now
    struct tm *now = localtime( & t );
    sprintf(Suffix, "%02d%02d%02d_%02d%02d%02d", now->tm_year - 100, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);

}


void StartMessage()
{
    std::cout << std::endl << "============================" << std::endl;
    std::cout  << "pALPIDE Test program" << std::endl;
#ifndef XCODE
    std::cout << "Git commit: " << VERSION << std::endl;
#endif
    std::cout << "============================" << std::endl << std::endl;

    if (myArgc == 0) {
        printUsage ();
        exit (0);
    }
}


void FillConditionsData(TTestSetup *Ats, TConfig *AConfig) {
  snprintf(AConfig->GetCondData()->SWVersion, sizeof(AConfig->GetCondData()->SWVersion)/sizeof(char), "%s", VERSION);
  AConfig->GetCondData()->FWVersion   = Ats->GetDAQBoard(USB_id)->GetFirmwareVersion();
  AConfig->GetCondData()->TempInit    = Ats->ReadInitialTemp(USB_id);
  AConfig->GetCondData()->IDDD        = Ats->GetDAQBoard(USB_id)->ReadDigitalI();
  AConfig->GetCondData()->IDDA        = Ats->GetDAQBoard(USB_id)->ReadAnalogI();
  AConfig->GetCondData()->TempVoltage = (float) Ats->ReadTempOnChip(USB_id);
}


void PowerBoardInitChip(TTestSetup *Ats, TpAlpidefs *AAlpide)
{
    int Overflow;
    if ( !Ats->PowerOnBoard(USB_id, Overflow) ) {
      std::cout << "Power on board failed, LDOs are off (overcurrent?)" << std::endl;
      Ats->GetDAQBoard(USB_id)->DecodeOverflow(Overflow);
      std::cout << std::endl << "Exiting ..." << std::endl;
      exit(0);
    }
    fConfig->GetCondData()->IDDDPowerOn = Ats->GetDAQBoard(USB_id)->ReadDigitalI();
    fConfig->GetCondData()->IDDAPowerOn = Ats->GetDAQBoard(USB_id)->ReadAnalogI ();

    if (fConfig->GetType() == TYPE_MODULE) {
    TModuleSetup* Ms = (TModuleSetup *) Ats;
      if ( !Ms->InitialiseAllChips(Overflow)) {
        std::cout << "Initialise chip failed, LDOs are off (overcurrent?)" << std::endl;
        Ms->GetDAQBoard(USB_id)->DecodeOverflow(Overflow);
        std::cout << std::endl << "Exiting ..." << std::endl;
        exit(0);
       }
     }
    else {
      if ( !Ats->InitialiseChip(USB_id, Overflow)) {
        std::cout << "Initialise chip failed, LDOs are off (overcurrent?)" << std::endl;
        Ats->GetDAQBoard(USB_id)->DecodeOverflow(Overflow);
        std::cout << std::endl << "Exiting ..." << std::endl;
        exit(0);
      }
    }

    std::cout << std::endl;
    if (AAlpide->GetChipType() == DUT_PALPIDEFS2) {
      int Wafer, Chip;
      ((TpAlpidefs2*)AAlpide)->ReadFuseValue(Wafer, Chip);
      std::cout << "Fuse value: Wafer " << Wafer << ", Chip " << Chip << std::endl;
      std::cout << "raw value: " << std::hex << ((TpAlpidefs2*)AAlpide)->ReadFuseValue() << std::dec << std::endl;
    }
    std::cout << std::endl;
}


int main(int argc, const char *argv[])
{
    TTestSetup *ts;
    TpAlpidefs *myAlpide;
    char        fName[100];  // Config file name

    fWriteConfig = false;                   // Default: no config written, changed in DoTheTest according to Scan type.
    fConfig      = new TConfig("Config.cfg");

    InitVariables (argc, argv);             // reads arguments, initialises output filename and path
    StartMessage  ();                       // prints startup message with git commit and (if necessary) usage

    // initialise board and chip object
    if ( !InitSetup(ts, myAlpide, 0, fConfig)) exit(0);
    SetDUT (ts, myAlpide, fConfig->GetDUTID());              // 0 to be changed in DUT Id

    // (try to) power up board and initialise chip
    if (fPowerOn) {
      PowerBoardInitChip(ts, myAlpide);
      FillConditionsData(ts, fConfig);
    }

    TpAlpidefs2 *myAlpide2 = (TpAlpidefs2*) myAlpide;
    if ((myAlpide2->GetChipId() == 17) && (myAlpide2->GetPreviousChipId() == 16)) {
        TDAQBoard2* myDAQBoard2 = (TDAQBoard2*)ts->GetDAQBoard(USB_id);
        myDAQBoard2->SetEmulationMode(0x1);
    	std::cout << "Chip in slave mode" << std::endl;
    }

    //TDAQBoard2* myDAQBoard2 = (TDAQBoard2*)ts->GetDAQBoard(USB_id);
    //myDAQBoard2->SelectDataOutI2C(0);

    // the actual test

    //TpAlpidefs3 *myAlpide3 = (TpAlpidefs3*) myAlpide;
    //int BunchCounter, EventCounter, Value1, Value2;
    //myAlpide3->ReadFROMUCounters (BunchCounter, EventCounter);
    //std::cout << "Strobe counter: " << myAlpide3->ReadStrobeCounter() << ", Event counter: " << EventCounter << std::endl;


    //  myAlpide3->SendCommand  (0xe4);
    //myAlpide3->WriteRegister(0x1, 0x21);
    //myAlpide3->WriteRegister(0xc, 0xe0);
    //myAlpide3->WriteRegister(0x5, 0x20);
    //myAlpide3->WriteRegister(0x4, 0x40);
    //myAlpide3->WriteRegister(0x6, 0x20);
    //myAlpide3->WriteRegister(0x7, 100);


    //myAlpide3->WriteRegister(0x500, 0x200);
    //myAlpide3->WriteRegister(0x501, 0x400);
    //myAlpide3->WriteRegister(0x502, 0x1);
    //myAlpide3->WriteRegister(0x502, 0x0);

    //myAlpide3->WriteRegister(0x500, 0x600);
    //myAlpide3->WriteRegister(0x501, 0x400);
    //myAlpide3->WriteRegister(0x502, 0x1);
    //myAlpide3->WriteRegister(0x502, 0x0);


    //myAlpide3->WriteRegister(0x500, 0x803);
    //myAlpide3->WriteRegister(0x501, 0x001);
    //myAlpide3->WriteRegister(0x502, 0x1);
    //myAlpide3->WriteRegister(0x502, 0x0);

    //myAlpide3->SendCommand  (0x63);
    //myAlpide3->SendCommand  (0x78);

    //ts->GetDAQBoard(USB_id)->ConfigureReadout (1, false, false);       // buffer depth = 1, sampling on rising edge
    //ts->GetDAQBoard(USB_id)->ConfigureTrigger (0, 10, 1, 1);

    //myAlpide3->SendCommand  (0x55);


    //unsigned char  data_buf[1024];
    //int            Length;

    //ts->GetDAQBoard(USB_id)->ReadChipEvent(data_buf, &Length, 1024);

    //for (int i = 0; i < Length; i ++) {
    //  std::cout << std::hex << (int)data_buf[i] << " " << std::dec << std::endl;
    //}


    DoTheTest(ts, myAlpide);

    if (fWriteConfig) {
        if (fPowerOn) fConfig->GetCondData()->TempFinal = ts->ReadFinalTemp(USB_id);   // save the final temperature
        sprintf   (fName, "%s/ScanConfig_%s.cfg", OutputPath, Suffix);
        ts->WriteConfigToFile(fName, fConfig);
    }

    //power off, release USB interface and exit
    if (fPowerOff) ts->PowerOffBoard(USB_id); // Caterina: comment to do not switch OFF the chip at the end of the test
    cleanExit(ts, 0);
    return 0;
}
