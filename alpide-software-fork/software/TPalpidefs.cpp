//
//  TPalpidefs.cpp
//  pALPIDEfs-software
//
//  Created by Carlo Puggioni and Markus Keil on 14/04/14.
//  Copyright (c) 2014 Carlo Puggioni, Markus Keil. All rights reserved.
//

#include "TPalpidefs.h"
#include "TDaqboard.h"
#include "TTestsetup.h"

#include <unistd.h>
#include <cstdio>
#include <iostream>
#include "TConfig.h"

//---------------------------------------------------------------------------------------------


// DAC definitions for the different prototypes
// Note: DAC names have to be defined also in TConfig.cpp (TChipConfig::DACNames)
const int  TpAlpidefs::DACDefaults [11]     = {117, 117, 57, 86, 50, 255, 50, 50, 64, 64, 51};
const char TpAlpidefs::DACNames    [11][10] = {"VAUX", "VRESET", "VCASN", "VCASP", "VPULSEL", "VPULSEH", "IRESET", "IAUX2", "IBIAS", "IDB", "ITHR"};
const int  TpAlpidefs1::DACDefaults[11]     = {117, 117, 57, 86, 50, 255, 50, 50, 64, 64, 51};
const char TpAlpidefs1::DACNames   [11][10] = {"VAUX", "VRESET", "VCASN", "VCASP", "VPULSEL", "VPULSEH", "IRESET", "IAUX2", "IBIAS", "IDB", "ITHR"};
const int  TpAlpidefs2::DACDefaults[11]     = {117, 117, 57, 86, 50, 255, 50, 50, 64, 64, 51};
const char TpAlpidefs2::DACNames   [11][10] = {"VAUX", "VRESET", "VCASN", "VCASP", "VPULSEL", "VPULSEH", "IRESET", "IAUX2", "IBIAS", "IDB", "ITHR"};
const int  TpAlpidefs3::DACDefaults[14]     = {117, 147, 86, 50, 255, 0, 57, 0, 0, 0, 50, 64, 64, 51};
//const int  TpAlpidefs3::DACDefaults[14]     = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const char TpAlpidefs3::DACNames   [14][10] = {"VRESETP", "VRESETD", "VCASP", "VCASN", "VPULSEH", "VPULSEL", "VCASN2", "VCLIP", "VTEMP", "IAUX2", "IRESET", "IDB", "IBIAS", "ITHR"};
const int  TpAlpidefs4::DACDefaults[14]     = {117, 147, 86, 50, 170, 100, 62, 0, 0x00, 0x00, 50, 29, 64, 51};
const char TpAlpidefs4::DACNames   [14][10] = {"VRESETP", "VRESETD", "VCASP", "VCASN", "VPULSEH", "VPULSEL", "VCASN2", "VCLIP", "VTEMP", "IAUX2", "IRESET", "IDB", "IBIAS", "ITHR"};
////////////////////////////////////////////////////////////////////////
//                                                                    //
//                    class TpAlpidefs                                //
//                                                                    //
////////////////////////////////////////////////////////////////////////

//const int TpAlpidefs::DAC_DEFAULTS[11] = {117, 117, 57, 86, 50, 255, 50, 50, 64, 64, 51};


TpAlpidefs::TpAlpidefs (TTestSetup *ASetup, int AIndex, TChipConfig *AConfig, bool Parent) : TDUT (ASetup, AIndex, AConfig)
{
  for (int icol = 0; icol < NCOL; icol ++) {
    for (int irow = 0; irow < NROW; irow ++) {
      fMasked[icol][irow] = false;
    }
  }
  fNoisyPixels.clear();
  if (!Parent) {      // initialise array only if *not* called as superclass constructor
    fDACs = new int[GetNDacs()];
  }
}


//======================================================
//     General peripheral control register methods
//======================================================


int TpAlpidefs::SetChipMode (TAlpideMode AMode)
{
  fChipMode = AMode;
  return WriteControlRegister(false, false);
}


// initiate self-test and evaluate results
// i.e. check for "true"s for the pixel self-test and the memory self-test of the 32 regions

int TpAlpidefs::DoSelfTest ()
{
  int err = WriteControlRegister(true, false);
  if (err == 1) GetRegionStatusAll();

  if (err != 1) return err;

  for (int ireg = 0; ireg < 32; ireg ++) {
    if (fStatusReg[ireg].PixelSelfTest || fStatusReg[ireg].MemSelfTest) {
      std::cout << "Error in self test, region " << ireg << " pixel self-test = " << (int) fStatusReg[ireg].PixelSelfTest << ", mem self-test = " << (int) fStatusReg[ireg].MemSelfTest << std::endl;
    }
  }
  return err;
}


int TpAlpidefs::StrobePixelConfig ()
{
  return WriteControlRegister(false, true);
}


int TpAlpidefs::SetEnableClustering (bool enabled)
{
  fClusteringEnabled = enabled;
  return WriteControlRegister(false, false);
}


int TpAlpidefs::SetReadoutDelay(int8_t ADelay)
{
  fReadoutDelay = ADelay;
  return WriteControlRegister(false, false);
}


int TpAlpidefs::SetStrobeTiming(int AStrobeTiming) {
  fStrobeTiming = AStrobeTiming;
  return WriteRegister(REG_STROBE_TIMING, AStrobeTiming);
}


// Simple readback test of chip register,
// Write value AValue to register Aadress, read it back and compare
// private: call TestRegister instead
bool TpAlpidefs::RegReadbackTest (int Aaddress, int AValue)
{
  int ReadValue;

  WriteRegister(Aaddress, AValue);
  //sleep(1);
  //GlobalReset(10);
  ReadRegister(Aaddress, &ReadValue);
  if (ReadValue != AValue) {
    std::cout << std::hex << "Register readback of reg 0x" << Aaddress << " failed, wrote 0x" << AValue << ", read 0x " << ReadValue << std::dec << std::endl;
    //sleep(10);
    return false;
  }
  else {
    // std::cout << std::hex << "Register readback of reg 0x" << Aaddress << " worked, wrote 0x" << AValue << ", read 0x " << ReadValue << std::dec << std::endl;
  }
  return true;
}



// Readback test of chip register located at Aaddress
// if AValue is not -1, AValue will be written, read back and compared with readback value
// otherwise (default) the same is done with the bit patterns 0000..., 1111..., and 0101...
bool TpAlpidefs::TestRegister (int Aaddress, int AValue)
{
  bool Result1, Result2, Result3;
  if (AValue == -1) {
    Result1 = RegReadbackTest(Aaddress, 0);
    Result2 = RegReadbackTest(Aaddress, 0xffff);
    Result3 = RegReadbackTest(Aaddress, 0x5555);
    return ( Result1 && Result2 && Result3 );
  }
  else {
    return RegReadbackTest(Aaddress, AValue);
  }
}


// Fills the fifos with fake hit data
// the pixel address is ascending, the double column address is region / 2.
void TpAlpidefs::FillFifos() {
  int Address, Value;
  for (int ireg = 0; ireg < 32; ireg ++) {
    for (int iword = 0; iword < SIZEDATAFIFO; iword ++) {
      Address  = REG_DATAFIFOBASE | iword | (ireg << 11);
      int dcol = ireg / 2;
      Value    = iword | (dcol << 10);
    }
  }
}

//======================================================
//     pixel configuration methods
//======================================================

// Helper method: sets the pixel config registers to configure a single pixel
// No strobing of the configuration (call StrobePixelConfig afterwards)
// Will in most cases not be called directly
//
// ACol, ARow: pixel to configure
// APixReg: Mask register or inject enable register
// AValue: Value to write into the register (true or false)
// APulseType: pulse type (analogue or digital)

int TpAlpidefs::LoadSinglePixelConfig (int              ACol,
                                       int              ARow,
                                       TAlpidePixreg    APixReg,
                                       bool             AValue,
                                       TAlpidePulseType APulseType)
{
  fPulseType    = APulseType;    // probably redundant (if method is not called directly)
  fPixelConfig1 = 0;
  fPixelConfig2 = 0;

  fPixelConfig1 |= (ARow & 0x1ff);             // config 1, Bit 0-8: row address
  fPixelConfig1 |= (((int) APixReg) << 10);    // config 1, Bit 10: PIXCNFG_REGSEL
  fPixelConfig1 |= ((AValue?1:0) << 11);       // config 1, Bit 11: PIXCNFG_DATA

  fPixelConfig2 |= (ACol & 0x3ff);             // config 2, Bit 0-9: col address
  fPixelConfig2 |= (((int) APulseType) << 11); // config 2, Bit 11: PULSE_TYPE

//    std::cout << "Writing 0x" << std::hex << fPixelConfig1 << std::dec << " to pixelconfig1" << std::endl;
//    std::cout << "Writing 0x" << std::hex << fPixelConfig2 << std::dec << " to pixelconfig2" << std::endl;

  int err = WriteRegister(REG_PIXELCONFIG1, fPixelConfig1);
  if (err == 1) {
    return WriteRegister(REG_PIXELCONFIG2, fPixelConfig2);
  }
  else return err;
}


// Helper method: sets the pixel config registers to configure ALL pixels
// No strobing of the configuration (call StrobePixelConfig afterwards)
// Will in most cases not be called directly
//
// APixReg: Mask register or inject enable register
// AValue: Value to write into the register (true or false)
// APulseType: pulse type (analogue or digital)

int TpAlpidefs::LoadPixelConfigAll (TAlpidePixreg    APixReg,
                                    bool             AValue,
                                    TAlpidePulseType APulseType)
{
  fPulseType    = APulseType;   // probably redundant (if method is not called directly)
  fPixelConfig1 = 0;
  fPixelConfig2 = 0;

  fPixelConfig1 |= (1 << 9);                   // config 1, Bit 9:  all rows
  fPixelConfig1 |= (((int) APixReg) << 10);    // config 1, Bit 10: PIXCNFG_REGSEL
  fPixelConfig1 |= ((AValue?1:0) << 11);       // config 1, Bit 11: PIXCNFG_DATA

  fPixelConfig2 |= (1 << 10);                  // config 2, Bit 10: all columns
  fPixelConfig2 |= (((int) fPulseType) << 11); // config 2, Bit 11: PULSE_TYPE

  int err = WriteRegister(REG_PIXELCONFIG1, fPixelConfig1);
  if (err == 1) {
    return WriteRegister(REG_PIXELCONFIG2, fPixelConfig2);
  }
  else return err;
}


// Sets the mask of all pixels
// (PR_MASK is the default value of APixReg, method can be used also for strobe (de-)selection of all pixels)
// loads pixel config register AND strobes the configuration
// AMasked: true to mask, false to unmask

int TpAlpidefs::SetMaskAllPixels (bool AMasked, TAlpidePixreg APixReg)
{
    std::cout << " wrong mask all" << std::endl;
  int err = LoadPixelConfigAll(APixReg, AMasked, fPulseType);
  if (err == 1)
    return StrobePixelConfig();
  else return err;
}


// Sets the mask of single pixel (ACol, ARow)
// loads pixel config register AND strobes the configuration
// AMasked: true to mask, false to unmask

int  TpAlpidefs::SetMaskSinglePixel (int ACol, int ARow, bool AMasked)
{
  int err = LoadSinglePixelConfig(ACol, ARow, PR_MASK, AMasked, fPulseType);
  if (err == 1) {
    fMasked [ACol][ARow] = AMasked;
    return StrobePixelConfig();
  }
  else return err;
}


// Same but in coordinates region, double column, pixel address

int TpAlpidefs::SetMaskSinglePixel     (int AReg, int ADCol, int APixel, bool AMasked) {
  return SetMaskSinglePixel(AddressToColumn(AReg, ADCol, APixel), AddressToRow(AReg, ADCol, APixel), AMasked);
}


// Sets the inject enable of single pixel (ACol, ARow)
// loads pixel config register AND strobes the configuration
// AValue: true to enable injection, false to disable injection
// APulseType: pulse type (analogue or digital)
// ADisableOthers:  if this is set, all other pixels will be disabled from injection first
//                  if not set (ACol, ARow) will be enabled in addition to the already enabled pixels

int TpAlpidefs::SetInjectSinglePixel (int              ACol,
                                      int              ARow,
                                      bool             AValue,
                                      TAlpidePulseType APulseType,
                                      bool             ADisableOthers)
{
  fPulseType = APulseType;
  int err = 1;
  if (ADisableOthers) { // first disable all pixel from injection
    err = LoadPixelConfigAll (PR_PULSEENABLE, false, fPulseType);
    if (err == 1) {
      err = StrobePixelConfig();
    }
  }
  if (err == 1) err = LoadSinglePixelConfig(ACol, ARow, PR_PULSEENABLE, AValue, fPulseType);
  if (err == 1) err = StrobePixelConfig();
  return err;
}


// Same with coordinates Region, Double column, pixel address

int TpAlpidefs::SetInjectSinglePixel (int              AReg,
                                      int              ADCol,
                                      int              APixel,
                                      bool             AValue,
                                      TAlpidePulseType APulseType,
                                      bool             ADisableOthers)
{
  return SetInjectSinglePixel(AddressToColumn(AReg, ADCol, APixel),
                              AddressToRow(AReg, ADCol, APixel),
                              AValue,
                              APulseType,
                              ADisableOthers);
}


//======================================================
//     Region enable / disable methods
//======================================================

int TpAlpidefs::EnableAllRegions()
{
  for (int i = 0; i < 32; i++) {
    fRegionDisabled[i] = false;
  }
  WriteRegister(REG_REGION_DISABLE1, 0);
  WriteRegister(REG_REGION_DISABLE2, 0);
  return 1;
}

int TpAlpidefs::DisableAllRegions()
{
  for (int i = 0; i < 32; i++) {
    fRegionDisabled[i] = true;
  }
  WriteRegister(REG_REGION_DISABLE1, 0);
  WriteRegister(REG_REGION_DISABLE2, 0);
  return 1;
}

int TpAlpidefs::WriteRegionDisable(bool LowHalf)
{
  int Value = 0;
  if (LowHalf) {
    for (int i = 0; i < 16; i++) {
      Value |= ((fRegionDisabled[i]?1:0) << i);
    }
    WriteRegister(REG_REGION_DISABLE1, Value);
  }
  else {
    for (int i = 0; i < 16; i++) {
      Value |= ((fRegionDisabled[i+16]?1:0) << i);
    }
    WriteRegister(REG_REGION_DISABLE2, Value);

  }
  return 1;
}


int TpAlpidefs::SetRegionDisable (int ARegion, bool Disabled)
{
  if ((ARegion < 0) || (ARegion >31)) {
    std::cout << "Bad region number " << ARegion << ", doing nothing." << std::endl;
    return -1;
  }
  fRegionDisabled[ARegion] = Disabled;
  return WriteRegionDisable(ARegion < 16);
}


//======================================================
//     Column enable / disable methods
//======================================================

int TpAlpidefs::WriteColumnDisable (int ARegion)
{
  if ((ARegion < 0) || (ARegion > 31)) {
    std::cout << "Error, bad region number " << ARegion << std::endl;
    return -1;
  }
  int Address = REG_COLDISABLEBASE + (ARegion << 11);
  int Value   = 0;
  for (int icol = 0; icol < 16; icol ++) {
    Value |= ((fColumnDisabled[ARegion][icol]?1:0) << icol);
    //Value =0;
  }
#ifdef MYDEBUG
  std::cout << "WriteColumnDisable: Writing 0x" << std::hex << Value << " to register 0x" << Address << std::dec << std::endl;
#endif
  return WriteRegister(Address, Value);
}


int TpAlpidefs::SetDisableAllColumns (int ARegion, bool Disabled)
{
  if ((ARegion < 0) || (ARegion > 31)) {
    std::cout << "Error, bad region number " << ARegion << std::endl;
    return -1;
  }

  for (int icol = 0; icol < NDCOL; icol ++) {
    fColumnDisabled[ARegion][icol] = Disabled;
  }
  //sleep(1);
  return WriteColumnDisable(ARegion);
}


int TpAlpidefs::SetDisableColumn (int  ARegion,
                                  int  AColumn,
                                  bool Disabled)
{
  if ((ARegion < 0) || (ARegion >= NREG)) {
    std::cout << "Error, bad region number " << ARegion << std::endl;
    return -1;
  }
  if ((AColumn < 0) || (AColumn >= NDCOL)) {
    std::cout << "Error, bad colpair number " << AColumn << std::endl;
    return -1;
  }
  fColumnDisabled[ARegion][AColumn] = Disabled;
  return WriteColumnDisable(ARegion);
}


//======================================================
//     Noisy pixel methods
//======================================================

void TpAlpidefs::ReadNoisyPixelFile     (const char *fName, bool incremental)
{
  int NNoisy = 0;
  int reg, dcol, address;
  FILE *fp = fopen(fName, "r");
  if (!fp) {
    std::cout << "Error, could not open input file: " << fName << std::endl;
    return;
  }
  if (!incremental) fNoisyPixels.clear();
  while (! (fscanf(fp, "%d %d %d", &reg, &dcol, &address) < 3))  {
    TPixHit hit;
    hit.region = reg;
    hit.doublecol = dcol;
    hit.address = address;
    fNoisyPixels.push_back(hit);
    NNoisy ++;
  }
  fclose(fp);
  std::cout << "Read " << NNoisy << " noisy pixels from file." << std::endl;
}


void TpAlpidefs::WriteNoisyPixelFile    (const char *fName)
{
  int NNoisy = 0;
  FILE *fp = fopen (fName, "w");
  for (int ipix= 0; ipix < fNoisyPixels.size(); ipix++) {
    fprintf(fp, "%d %d %d\n",fNoisyPixels.at(ipix).region,
            fNoisyPixels.at(ipix).doublecol,
            fNoisyPixels.at(ipix).address);
    NNoisy ++;
  }
  fclose(fp);
  std::cout << "Wrote " << NNoisy << " noisy pixels to file." << std::endl;
}


void TpAlpidefs::MaskNoisyPixels        ()
{
  for (int ipix = 0; ipix < fNoisyPixels.size(); ipix ++) {
    SetMaskSinglePixel(fNoisyPixels.at(ipix).region,
                       fNoisyPixels.at(ipix).doublecol,
                       fNoisyPixels.at(ipix).address,
                       true);
  }
}

void TpAlpidefs::AddNoisyPixel          (int AReg, int ADCol, int AAddress) {
  TPixHit Pixel;
  Pixel.region    = AReg;
  Pixel.doublecol = ADCol;
  Pixel.address   = AAddress;
  AddNoisyPixel (Pixel);
}


void TpAlpidefs::AddNoisyPixel          (TPixHit APixel) {
  for (int ipix = 0; ipix < fNoisyPixels.size(); ipix ++) {
    if ((fNoisyPixels.at(ipix).region    == APixel.region) &&
        (fNoisyPixels.at(ipix).doublecol == APixel.doublecol) &&
        (fNoisyPixels.at(ipix).address   == APixel.address))
      return;
  }
  fNoisyPixels.push_back(APixel);
}


void TpAlpidefs::AddStuckPixel (TPixHit APixel) {
  for (int ipix = 0; ipix < fConfig->fStuckPixels.size(); ipix ++) {
    if ((fConfig->fStuckPixels.at(ipix).region    == APixel.region) &&
        (fConfig->fStuckPixels.at(ipix).doublecol == APixel.doublecol) &&
        (fConfig->fStuckPixels.at(ipix).address   == APixel.address))
      return;
  }
  fConfig->fStuckPixels.push_back(APixel);
}


//======================================================
//     On-chip DAC methods
//======================================================


int TpAlpidefs::SetDACValue (int ADAC, int AValue)
{
  if ((AValue < 0) || (AValue > 255)) {
    std::cout << "Invalid DAC value " << AValue << std::endl;
    return -1;
  }
  if (ADAC<0 || ADAC > GetNDacs()) {
    return -2;
  }
  fDACs [ADAC] = AValue;
  return 1;
}


void TpAlpidefs::SetDefaultDACValues ()
{
  for (int i = 0; i < GetNDacs(); i++) {
    int Value = fConfig->GetDACValue (GetDACName(i));
    // DAC value has been given in config -> Take that value
    if (Value > 0) {
      fDACs[i] = Value;
    }
    // No value for this DAC given in config
    // -> use default and write default to config (only for cfg-file writing)
    else {
      fDACs[i] = GetDACDefault (i);
      fConfig->SetDACValue (GetDACName(i), GetDACDefault(i));
    }
  }
}


int TpAlpidefs::WriteAllDACRegisters()
{
  int err = 1;
  for (int i = 0; i < 12; i += 2) {
    err = WriteDACRegister (i);
    if (err != 1) break;
  }
  return err;
}


int TpAlpidefs::SetDAC (int ADAC, int AValue)
{
  int err = SetDACValue(ADAC, AValue);
  if (err == 1) {
    err = WriteDACRegister(ADAC);
  }
  return err;
}


int TpAlpidefs::SetTestPulse(int AHigh, int ALow)
{
  SetDAC(FindDAC("VPULSEH"), AHigh);
  return SetDAC(FindDAC("VPULSEL"), ALow);
}


// Monitor the DACs ADacV and ADacI

int TpAlpidefs::MonitorDACs (TAlpideDacmonV ADacV, TAlpideDacmonI ADacI)
{
  return SetMonitoringRegister(ADacV, ADacI, false, false);
}


// Override two DACs: voltage DAC ADacV and current DAC ADacI

int TpAlpidefs::OverrideDACs (TAlpideDacmonV ADacV, TAlpideDacmonI ADacI)
{
  return SetMonitoringRegister(ADacV, ADacI, true, true);
}


// Override only a current DAC

int TpAlpidefs::OverrideCurrentDAC (TAlpideDacmonI ADacI)
{
  return SetMonitoringRegister(DACV_NONE, ADacI, false, true);
}


// Override only a voltage DAC

int TpAlpidefs::OverrideVoltageDAC (TAlpideDacmonV ADacV)
{
  return SetMonitoringRegister(ADacV, DACI_NONE, true, false);
}


//======================================================
//     Region status register methods
//======================================================

// Read the status register of region ARegion, result is saved into AValue

int TpAlpidefs::ReadStatusRegister (int ARegion, int *AValue)
{
  int Address = REG_RGNSTATUSBASE;
  Address    |= (ARegion << 11);
  return ReadRegister(Address, AValue);
}


// Read the status register of a region and extract the status information
// Results are filled into fStatusReg[ARegion]

int TpAlpidefs::GetRegionStatus (int ARegion)
{
  int Value;
  int err = ReadStatusRegister(ARegion, &Value);
#ifdef MYDEBUG
  std::cout << "GetRegionStatus, region " << ARegion << ", status reg 0x" << std::hex << Value << std::dec << std::endl;
#endif
  if (err == 1) {
    fStatusReg[ARegion].PixelSelfTest   = ((Value & 0x1) == 0x1);  // Bit 0
    fStatusReg[ARegion].MemSelfTest     = ((Value & 0x2) == 0x2);  // Bit 1
    fStatusReg[ARegion].ColsDisabled    = ((Value & 0x4) == 0x4);  // Bit 2
    fStatusReg[ARegion].RegionROSMState = ((Value >> 3) & 0x7);    // Bits 5:3
    fStatusReg[ARegion].RegionSTSMState = ((Value >> 6) & 0x3);    // Bits 7:6
    fStatusReg[ARegion].PeriROSMState   = ((Value >> 8) & 0xf);    // Bits 11:8
  }
  return err;
}


int TpAlpidefs::GetRegionStatusAll ()
{
  int err = 1;
  for (int ireg = 0; ireg < 32; ireg++) {
    err = GetRegionStatus(ireg);
    if (err != 1) return err;
  }
  return err;
}

//======================================================
//     Event decoding methods
//======================================================

int16_t TpAlpidefs::GetDataWord (unsigned char *Data)
{
  int16_t       Data16 = 0;
  unsigned char LSByte, MSByte;

  LSByte = Data[0];
  MSByte = Data[1];

  Data16 += ((int16_t)MSByte) << 8;
  Data16 += LSByte;

  return Data16;
}


void TpAlpidefs::DumpHitData(std::vector<TPixHit> Hits)
{
  std::cout << "Found " << Hits.size() << " Hits." << std::endl;
  for (int ihit = 0; ihit < Hits.size(); ihit ++) {
    std::cout << "Hit " << ihit << ": " << Hits.at(ihit).region <<
      "/" << Hits.at(ihit).doublecol <<
      "/" << Hits.at(ihit).address << std::endl;
  }
}



// Only starts readout, e.g. after going into configuration mode during a scan
void TpAlpidefs::StartReadout (TAlpideMode AReadoutMode)
{
  SetChipMode(AReadoutMode);
}


////////////////////////


int TpAlpidefs::AddressToColumn      (int ARegion, int ADoubleCol, int AAddress)
{
  int Column    = ARegion * 32 + ADoubleCol * 2;    // Double columns before ADoubleCol
  int LeftRight = ((AAddress % 4) < 2 ? 1:0);       // Left or right column within the double column

  Column += LeftRight;

  return Column;
}


int TpAlpidefs::AddressToRow         (int ARegion, int ADoubleCol, int AAddress)
{
  // Ok, this will get ugly
  int Row = AAddress / 2;                // This is OK for the top-right and the bottom-left pixel within a group of 4
  if ((AAddress % 4) == 3) Row -= 1;      // adjust the top-left pixel
  if ((AAddress % 4) == 0) Row += 1;      // adjust the bottom-right pixel
  return Row;
}


int TpAlpidefs::ColRowToAddress      (int AColumn, int ARow) {
  int LeftRight = AColumn % 2;     // 0 for left column, 1 for right
  int OddEven   = ARow    % 2;     // 0 for even rows, 1 for odd
  int Address   = ARow * 2;        // this is OK only for the bottom left-pixel within a group of 4

  if ((LeftRight == 0) && (OddEven == 0)) Address += 3;     // adjust top-left pixel
  if ((LeftRight == 1) && (OddEven == 0)) Address += 1;     // adjust top-right pixel
  if ((LeftRight == 1) && (OddEven == 1)) Address -= 2;     // adjust bottom-right pixel
  return Address;
}


int TpAlpidefs::ColToRegion          (int AColumn) {
  int Region = AColumn / 32;

  return Region;
}


int TpAlpidefs::ColToDoubleCol       (int AColumn) {
  int ColInRegion = AColumn % 32;
  int DoubleCol   = ColInRegion / 2;

  return DoubleCol;
}


////////////////////////////////////////////////////////////////////////
//                                                                    //
//                    class TpAlpidefs1                               //
//                                                                    //
////////////////////////////////////////////////////////////////////////


TpAlpidefs1::TpAlpidefs1 (TTestSetup *ASetup, int AIndex, TChipConfig *AConfig, bool Parent) : TpAlpidefs (ASetup, AIndex, AConfig, true)
{
  fEvtDataPortOen1 = false;
  fEvtDataPortOen2 = false;
  if (!Parent) {      // initialise array only if *not* called as superclass constructor
    fDACs = new int[GetNDacs()];
  }
}


// Init chip according to recipe in pALPIDEfs manual

int TpAlpidefs1::Init (bool Reset)
{
  int err = 1;
  // global chip reset
  if (Reset) err = GlobalReset ();
  if (err != 1) return err;

  SetChipMode(MODE_ALPIDE_CONFIG);

  // write default DAC values
  SetDefaultDACValues();
  err = WriteAllDACRegisters();
  if (err != 1) return err;

  // set in-pixel MASK_EN registers -> mask all pixels
  //err = SetMaskAllPixels (true);
  //if (err != 1) return err;

  // set in-pixel PULSE_EN registers -> select no pixel
  SetMaskAllPixels (false, PR_PULSEENABLE);
  if (err != 1) return err;

  // reset pixel state registers
  err = PixelReset();

  return err;
}


// Helper function to write monitoring / override register
// allows all combinations of which current / voltage dac to monitor / override
// ADacV, ADacI: which voltage current DAC
// OverrideV, OverrideI: true to override the voltage / current DAC, false to monitor

int TpAlpidefs1::SetMonitoringRegister (TAlpideDacmonV ADacV,
                                        TAlpideDacmonI ADacI,
                                        bool           OverrideV,
                                        bool           OverrideI)
{
  int Value = 0;
  Value |= ((int)ADacV & 0x7 );
  Value |= ((int)ADacI & 0x7 ) << 3;
  Value |= (OverrideI ? 1:0) << 6;
  Value |= (OverrideV ? 1:0) << 7;      // Is this correct? according to the manual the order (I/V) for the overrides is opposite of the order of the values ...

  return WriteRegister(REG_MONITORING, Value);
}


int TpAlpidefs1::WriteControlRegister(bool SelfTest, bool ConfigStrobe)
{
  int16_t Value = 0;
  Value |= ((int) fChipMode);
  Value |= ((int) fClusteringEnabled << 2);
  Value |= ((int) SelfTest           << 3);
  Value |= ((int) ConfigStrobe       << 4);
  Value |= ((int) fEvtDataPortOen1   << 5);
  Value |= ((int) fEvtDataPortOen2   << 6);
  Value |= (      fReadoutDelay      << 8);
#ifdef MYDEBUG
  std::cout<< "Writing 0x" << std::hex << Value << std::dec << " to control register" << std::endl;
#endif
  return WriteRegister(REG_CONTROL, Value);
}


int TpAlpidefs1::WriteDACRegister (int ADAC)
{
  int DACPair = ADAC / 2;
  int DACHigh = ADAC % 2;
  int Value;

  int Register = REG_DACBASE + DACPair;
  if (DACHigh) {
    Value = (fDACs [ADAC] << 8) | fDACs[ADAC-1];
    return WriteRegister(Register, Value);
  }
  else if (ADAC < 10) {
    Value = fDACs[ADAC] | (fDACs[ADAC + 1] << 8);
    return WriteRegister(Register, Value);
  }
  else {
    Value = fDACs[ADAC];
    return WriteRegister(Register, Value);
  }
}


// Send RESET signal to chip
// default value for length is 1 clock cycles

int TpAlpidefs1::GlobalReset (int ALength)
{
  return SendSignal(SIG_RESET, ALength);
}


// Send PRST signal to chip
// default value for length is 16 clock cycles

int TpAlpidefs1::PixelReset (int ALength)
{
  return SendSignal(SIG_PRST, ALength);
}


int TpAlpidefs1::Pulse (int ALength)
{
  return SendSignal(SIG_PULSE, ALength);
}


int TpAlpidefs1::SetEnableOutputDrivers (bool OEN1, bool OEN2)
{
  fEvtDataPortOen1 = OEN1;
  fEvtDataPortOen2 = OEN2;
  return WriteControlRegister(false, false);
}

////////////////////////

// Prepares and starts readout
void TpAlpidefs1::PrepareReadout (int AStrobeBLength, int AReadoutDelay, TAlpideMode AReadoutMode)
{
  SetChipMode(MODE_ALPIDE_CONFIG);

  SetReadoutDelay        (AReadoutDelay);
  SetEnableClustering    (false);
  SetStrobeTiming        (AStrobeBLength);
  SetEnableOutputDrivers (true, true);
  SetChipMode            (AReadoutMode);

}



//======================================================
//     Event decoding methods
//======================================================


void TpAlpidefs1::DecodeRegionHeader   (int16_t Data, int *Region, int *EvtLength)
{
  *Region    = (Data & 0xf800) >> 11;
  *EvtLength = (Data & 0x00ff);
#ifdef MYDEBUG
  std::cout << "Header: Region " << *Region << ", " << *EvtLength << " Hit(s)" << std::endl;
#endif
}


bool TpAlpidefs1::DecodeDataWord (int16_t Data, int Region,  std::vector<TPixHit> *Hits, int NOldHits)
{
  TPixHit Hit;
  int     ClusterSize;
  int     DoubleCol;
  int     Pixel;

  ClusterSize = (Data & 0xc000) >> 14;
  DoubleCol   = (Data & 0x3c00) >> 10;
  Pixel       = (Data & 0x03ff);



  for (int i = 0; i <= ClusterSize; i ++) {
    Hit.doublecol = DoubleCol;
    Hit.region    = Region;
    Hit.address   = Pixel + i;
#ifdef MYDEBUG
    std::cout << "  Hit Data: Region " << Region << ", Dataword 0x" << std::hex << Data << std::dec << ", DoubleCol " << DoubleCol << ", Pixel " << Hit.address << std::endl;
#endif
    if ((Hits->size() > NOldHits) && (Hit == Hits->at(Hits->size()-1))) {
      std::cout << "Warning, pixel " << Hit.region << "/" <<Hit.doublecol << "/" << Hit.address  << " found twice" << std::endl;
      AddStuckPixel(Hit);
    }
    if ((Hits->size() > NOldHits) && (Hit < Hits->at(Hits->size()-1))) std::cout << "Warning, pixel " << Hit.region << "/" <<Hit.doublecol << "/" << Hit.address  << " wrong ordering. previous: " << Hits->at(Hits->size()-1).region << "/" <<Hits->at(Hits->size()-1).doublecol << "/" << Hits->at(Hits->size()-1).address << std::endl;
    Hits->push_back(Hit);
  }
  return true;
}



// DecodeEvent decodes a pAlpide event contained in evtData
//  - length of the data array (number of characters) has to be given as parameter
//  - Method works with and without clustering (cf. DecodeDataWord())
//  - Method returns when event finished or maxLength reached

bool TpAlpidefs1::DecodeEvent (unsigned char        *evtData,
                               int                   maxLength,
                               std::vector<TPixHit> *Hits,
                               unsigned int* StrobeCounter /*=0x0*/,
                               unsigned int* BunchCounter  /*=0x0*/,
                               unsigned int* StatusBits /*=0x0*/)
{
  //std::vector<TPixHit> Hits;
  int                  iword     = 0;
  int                  region    = 0;
  int                  expRegion = -1;
  int                  evtLength = 0;
  bool                 Result    = true;
  int16_t              word;
  int                  NOldHits  = Hits->size();
  //return true;
  while ((iword+1)*2 <= maxLength) {
    word = GetDataWord(evtData, iword * 2);
    if (!evtLength) {              // next word expected to be header
      expRegion ++;              // expect that header belongs to next region, first region should be 0
      if (expRegion > 31) {      // all region data read, but maxLength not yet reached
        if (maxLength - iword*2 > 2) {
          std::cout << "Warning: data seems to be longer than one event. "<< maxLength - iword*2 << " bytes left unread." << std::endl;
          return false;
        }
        else return Result;    // 2 bytes more are OK, since data always filled up to 4-byte words
      }
      DecodeRegionHeader(word, &region, &evtLength);
      if (region != expRegion) {
        std::cout << "Warning: wrong region header, expected " << expRegion << ", found "
                  << region << " (data word: 0x" << std::hex << word <<std::dec << ")" << std::endl;
        Result = false;
      }
    }
    else {                         // next word expected to be data
	    DecodeDataWord(word, region, Hits, NOldHits);
      evtLength --;
    }
    iword ++;
  }
  if ((expRegion < 31) || (evtLength > 0)) {    // maxLength has been reached but not all regions are completely read
    int LastComplete;
    if (evtLength) {
      LastComplete = expRegion - 1;
    }
    else {
      LastComplete = expRegion;
    }
    std::cout << "Warning: event not complete. Last complete region: " << LastComplete << std::endl;
    Result = false;
  }
  return Result;
}

// DecodeEvent decodes a pAlpide event contained in evtData
//  - length of the data array (number of characters) has to be given as parameter
//  - Method works with and without clustering (cf. DecodeDataWord())
//  - Method returns when event finished or maxLength reached

bool TpAlpidefs1::DecodeEventNew (int16_t *evtData,int maxLength, std::vector<TPixHit> *Hits)
{
  //std::vector<TPixHit> Hits;
  int                  iword     = 0;
  int                  region    = 0;
  int                  expRegion = -1;
  int                  evtLength = 0;
  bool                 Result    = true;
  int16_t              word;
  int                  LastComplete;
  int                  NOldHits  = Hits->size();

  while ((iword+1) <= maxLength) {
    word = evtData[iword];
    if (!evtLength) {              // next word expected to be header
      expRegion ++;              // expect that header belongs to next region, first region should be 0
      if (expRegion > 31) {      // all region data read, but maxLength not yet reached
        if (maxLength - iword> 1) {
          std::cout << "Warning: data seems to be longer than one event. "<< maxLength - iword*2 << " bytes left unread." << std::endl;
          return false;
        }
        else return Result;    // 2 bytes more are OK, since data always filled up to 4-byte words
      }
      DecodeRegionHeader(word, &region, &evtLength);
      if (region != expRegion) {
        std::cout << "Warning: wrong region header, expected " << expRegion << ", found "
                  << region << " (data word: 0x" << std::hex << word <<std::dec << ")" << std::endl;
        Result = false;
      }
    }
    else {                         // next word expected to be data
      DecodeDataWord(word, region, Hits, NOldHits);
      evtLength --;
    }
    iword ++;
  }
  if ((expRegion < 31) || (evtLength > 0)) {    // maxLength has been reached but not all regions are completely read
    if (evtLength) {
      LastComplete = expRegion - 1;
    }
    else {
      LastComplete = expRegion;
    }
    std::cout << "Warning: event not complete. Last complete region: " << LastComplete << std::endl;
    Result = false;
  }
  return Result;
}


////////////////////////////////////////////////////////////////////////
//                                                                    //
//                    class TpAlpidefs2                               //
//                                                                    //
////////////////////////////////////////////////////////////////////////


TpAlpidefs2::TpAlpidefs2 (TTestSetup *ASetup, int AIndex, TChipConfig *AConfig, bool Parent) : TpAlpidefs (ASetup, AIndex, AConfig, true)
{
  fChipId       = AConfig->ChipId;

  fPreviousChip = AConfig->PreviousChip;
  fPulseLength  = AConfig->PulseLength;
  fPrstLength   = AConfig->PrstDuration;

  fDCTRLRcvrCurrent      = AConfig->DCTRLRcvrCurrent;
  fDCTRLDrvrCurrent      = AConfig->DCTRLDrvrCurrent;
  fDCLKRcvrCurrent       = AConfig->DCLKRcvrCurrent;
  fDPORTRcvrCurrent      = AConfig->DPORTRcvrCurrent;
  fDPORTDrvrCurrent      = AConfig->DPORTDrvrCurrent;
  fDisableDoubleDataRate = AConfig->DisableDDR;
  fDisableManchester     = true;
  fChipMasterFPGASlave   = false;
  fSimData               = false;
  if (!Parent) {      // initialise array only if *not* called as superclass constructor
    fDACs = new int[GetNDacs()];
  }
}


int TpAlpidefs2::Init (bool Reset)
{
  int err = 1;
  TDAQBoard2 *myBoard = (TDAQBoard2*) fSetup->GetDAQBoard(fIndex);
  myBoard->SetChipIdI2C (fChipId);


  // global chip reset
  if (Reset) err = GlobalReset ();
  if (err != 1) return err;

  SetChipMode(MODE_ALPIDE_CONFIG);

  // write default DAC values
  SetDefaultDACValues();
  err = WriteAllDACRegisters();
  if (err != 1) return err;

  // set in-pixel MASK_EN registers -> mask all pixels
  err = SetMaskAllPixels (true);
  if (err != 1) return err;

  // set in-pixel PULSE_EN registers -> select no pixel
  SetMaskAllPixels (false, PR_PULSEENABLE);
  if (err != 1) return err;

  // reset pixel state registers
  err = PixelReset();
  if (err != 1) return err;

  err =  WriteCMUCurrents();
  if (err != 1) return err;

  //err = WriteDPUCurrents();
  //if (err != 1) return err;

  err = WriteCMUDMUConfig ();
  if (err != 1) return err;

  //SetMEBMemEnabled(true);
  PrintCMUErrors();
  return err;

}


int TpAlpidefs2::SendCommand (int OpCode)
{
  TDAQBoard2 *myBoard = (TDAQBoard2*) fSetup->GetDAQBoard(fIndex);
  //std::cout << "Sending opcode 0x" << std::hex << OpCode << std::dec << std::endl;
  return myBoard->WriteChipCommand(OpCode);
}


int TpAlpidefs2::WriteRegister (int Address, int Value) {
  TDAQBoard2 *myBoard = (TDAQBoard2*) fSetup->GetDAQBoard(fIndex);
  //std::cout << "TpAlpidefs2::WriteRegister Writing value 0x" << std::hex << Value <<  " to register 0x" << Address << std::dec << "  " << fChipId << std::endl;
  return myBoard->WriteChipRegister(Address, Value, fChipId);
}


int TpAlpidefs2::ReadRegister  (int Address, int *Value) {
  //std::cout << std::endl << "Reading chip " << fChipId << ", register 0x" << std::hex << Address << std::dec << std::endl;
  TDAQBoard2 *myBoard    = (TDAQBoard2*) fSetup->GetDAQBoard(fIndex);
  int         err        = myBoard->ReadChipRegister(Address, Value, fChipId);
  int         RcvdChipId = *Value & 0xff;

  //std::cout << "TpAlpidefs2::ReadRegister Value read = 0x" << std::hex << (*Value>>8)   << " from register 0x" << Address << std::dec << std::endl;
  if (RcvdChipId != fChipId) {
    std::cout << "Warning, received chip ID " << RcvdChipId << " instead of " << fChipId << std::endl;
    //sleep(5);
  }
  *Value >>= 8;
  return err;
}

int TpAlpidefs2::WriteControlRegister(bool SelfTest, bool ConfigStrobe)
{
  int16_t Value = 0;
  Value |= ((int) fChipMode);
  Value |= ((int) fClusteringEnabled     << 2);
  Value |= ((int) SelfTest               << 3);
  Value |= ((int) ConfigStrobe           << 4);
  Value |= ((int) fMEBMemEnabled         << 7);
  Value |= (      (fReadoutDelay & 0xff) << 8);
#ifdef MYDEBUG
  std::cout<< "Writing 0x" << std::hex << Value << std::dec << " to control register" << std::endl;
#endif
  return WriteRegister(REG_CONTROL, Value);
}


int TpAlpidefs2::SetMEBMemEnabled (bool AEnabled)
{
  fMEBMemEnabled = AEnabled;
  return WriteControlRegister (false, false);
}


int TpAlpidefs2::WriteDACRegister (int ADAC)                     // Writes the register of a given DAC with the value stored in fDACs
{
  int DACPair = ADAC / 2;
  int DACHigh;
  if ((ADAC == DAC_ALPIDE_VAUX) || (ADAC == DAC_ALPIDE_VRESET)) {     // in pALPIDE2 VAUX and VRESET are switched w.r.t. pALPIDE1
    DACHigh = (ADAC + 1) % 2;
  }
  else {                                                              // all other DAC pairs are as before
    DACHigh = ADAC % 2;
  }
  int Value;

  int Register = REG_DACBASE + DACPair;
  if (DACHigh) {
    if (ADAC == DAC_ALPIDE_VAUX)  Value = (fDACs [ADAC] << 8) | fDACs[ADAC+1];
    else Value = (fDACs [ADAC] << 8) | fDACs[ADAC-1];
    return WriteRegister(Register, Value);
  }
  else if (ADAC < 10) {
    if (ADAC == DAC_ALPIDE_VRESET) Value = fDACs[ADAC] | (fDACs[ADAC - 1] << 8);
    else Value = fDACs[ADAC] | (fDACs[ADAC + 1] << 8);
    return WriteRegister(Register, Value);
  }
  else {
    Value = fDACs[ADAC];
    return WriteRegister(Register, Value);
  }
}


int TpAlpidefs2::SetMonitoringRegister (TAlpideDacmonV ADacV,
                                        TAlpideDacmonI ADacI,
                                        bool           OverrideV,
                                        bool           OverrideI)
{
  int            Value = 0;
  TAlpideDacmonV DacV;
  if (ADacV == SWCNTL_VAUX) {
    DacV = SWCNTL_VRESET;
  }
  else if (ADacV == SWCNTL_VRESET) {
    DacV = SWCNTL_VAUX;
  }
  else {
    DacV = ADacV;
  }

  Value |= ((int)DacV & 0x7 );
  Value |= ((int)ADacI & 0x7 ) << 3;
  Value |= (OverrideI ? 1:0) << 6;
  Value |= (OverrideV ? 1:0) << 7;

  return WriteRegister(REG_MONITORING, Value);
}


int TpAlpidefs2::GlobalReset (int ALength)
{
  TDAQBoard2 *myBoard = (TDAQBoard2*) fSetup->GetDAQBoard(fIndex);
  TDataPort   OldPort = myBoard->GetDataPort ();

  myBoard->SetDataPort      (PORT_NONE);
  myBoard->ConfigureReadout (1, false, false);
  myBoard->SetDataPort      (OldPort);

  return SendCommand (OPCODE_GRST);
}


int TpAlpidefs2::PixelReset (int ALength)
{
  if (ALength != fPrstLength) {
    SetPrstLength    (ALength);
    WritePRLengthReg ();
  }
  return SendCommand (OPCODE_PRST);
}


int TpAlpidefs2::Pulse (int ALength)
{
  if (ALength != fPulseLength) {
    //std::cout << "Changing Pulse length from " << fPulseLength << " to "<< ALength << std::endl;
    ConfigurePulse (ALength);
  }
  return SendSignal (SIG_PULSE, ALength);
  //return SendCommand(OPCODE_PULSE);
}


void TpAlpidefs2::SetPrstLength  (int ALength)
{
  if (ALength < 1) {
    std::cout << "Invalid reset length " << ALength << ", not changing anything." << std::endl;
  }
  else if (ALength > 255) {
    std::cout << "Invalid reset length " << ALength << ", setting to 255." << std::endl;
    fPrstLength = 255;
  }
  else {
    fPrstLength = ALength;
  }
}


void TpAlpidefs2::SetPulseLength  (int ALength)
{
  if (ALength < 1) {
    std::cout << "Invalid pulse length " << ALength << ", not changing anything." << std::endl;
  }
  else if (ALength > 255) {
    std::cout << "Invalid pulse length " << ALength << ", setting to 255." << std::endl;
    fPulseLength = 255;
  }
  else {
    fPulseLength = ALength;
  }
}


int TpAlpidefs2::WritePRLengthReg ()
{
  int Value = fPrstLength | (fPulseLength << 8);
  return WriteRegister (REG_PRLENGTH, Value);
}


int TpAlpidefs2::ConfigurePulse (int ALength)
{
  SetPulseLength (ALength);
  return WritePRLengthReg();
}


int TpAlpidefs2::WritePRLengthReg (int APrstLength, int APulseLength)
{
  if (APrstLength  != fPrstLength)  SetPrstLength  (APrstLength);
  if (APulseLength != fPulseLength) SetPulseLength (APulseLength);
  return WritePRLengthReg();
}


////////////////////////

// Prepares and starts readout, to be checked
void TpAlpidefs2::PrepareReadout (int AStrobeBLength, int AReadoutDelay, TAlpideMode AReadoutMode)
{
  std::cout << "TpAlpidefs2: Preparing readout with StrobeBLength " << AStrobeBLength << ", ReadoutDelay " << AReadoutDelay << ", Readout Mode " << AReadoutMode << std::endl;

  SetChipMode(MODE_ALPIDE_CONFIG);
  SetReadoutDelay        (AReadoutDelay);
  SetEnableClustering    (false);
  SetStrobeTiming        (AStrobeBLength);
  SetChipMode            (AReadoutMode);

}




int TpAlpidefs2::WriteCMUCurrents ()
{
  int Value = ((fDCTRLRcvrCurrent & 0xf) << 8) |
    ((fDCTRLDrvrCurrent & 0xf) << 4) |
    ((fDCLKRcvrCurrent  & 0xf));
  return WriteRegister(REG_CMUCURRENTS, Value);
}


int TpAlpidefs2::WriteCMUCurrents (int ADCTRLRcvrCurrent, int ADCTRLDrvrCurrent, int ADCLKRcvrCurrent)
{
  fDCTRLRcvrCurrent = ADCTRLRcvrCurrent;
  fDCTRLDrvrCurrent = ADCTRLDrvrCurrent;
  fDCLKRcvrCurrent  = ADCLKRcvrCurrent;
  return WriteCMUCurrents();
}


int TpAlpidefs2::WriteDPUCurrents ()
{
  int Value = ((fDPORTRcvrCurrent & 0xf) << 4) | (fDPORTDrvrCurrent & 0xf);
  return WriteRegister (REG_DPUCURRENTS, Value);
}


int TpAlpidefs2::WriteDPUCurrents (int ADPORTRcvrCurrent, int ADPORTDrvrCurrent)
{
  fDPORTRcvrCurrent = ADPORTRcvrCurrent;
  fDPORTDrvrCurrent = ADPORTDrvrCurrent;
  return WriteDPUCurrents();
}


int TpAlpidefs2::WriteCMUDMUConfig ()
{
  int Value = ((fDisableDoubleDataRate ? 1:0) << 7) |
    ((fDisableManchester ? 1:0) << 6) |
    ((fPreviousChip & 0xf));
  return WriteRegister (REG_CMUDMU_CONFIG, Value);
}


int TpAlpidefs2::GetCMUErrorCounts (int &CodeUnknown, int &Timeout, int &Deserializer)
{
  int Value;
  int err = ReadRegister (REG_CMUERRORS, &Value);
  CodeUnknown  = (Value >> 8) & 0xf;
  Timeout      = (Value >> 4) & 0xf;
  Deserializer = Value & 0xf;
  return err;
}


void TpAlpidefs2::PrintCMUErrors ()
{
  int Unknown, Timeout, Deserializer;
  int err = GetCMUErrorCounts (Unknown, Timeout, Deserializer);
  if (err != 1) {
    std::cout << "Unable to read CMU error counts, err = " << err << std::endl;
  }
  else if (Unknown + Timeout + Deserializer == 0) {
    std::cout << "No CMU errors found." << std::endl;
  }
  else {
    std::cout << "CMU error counts: " << std::endl;
    std::cout << "  Unknown OPCODE:      " << Unknown << std::endl;
    std::cout << "  Time-outs:           " << Timeout << std::endl;
    std::cout << "  Deserializer errors: " << Deserializer << std::endl;
  }
}


// Warning: the data type is not unambiguous if called with byte of a dataword,
// which is not the most significant byte (in case of DATA SHORT or DATA LONG)
TPalpideDataType TpAlpidefs2::CheckDataType (unsigned char DataWord)
{
  if      (DataWord == 0xf0)          return DT_IDLE;
  else if (DataWord == 0xff)          return DT_NOP;
  else if (DataWord == 0xf1)          return DT_BUSYON;
  else if (DataWord == 0xf2)          return DT_BUSYOFF;
  else if ((DataWord & 0xf0) == 0xa0) return DT_CHIPHEADER;
  else if ((DataWord & 0xf0) == 0xb0) return DT_CHIPTRAILER;
  else if ((DataWord & 0xe0) == 0xc0) return DT_REGHEADER;
  else if ((DataWord & 0xc0) == 0)    return DT_DATASHORT;
  else if ((DataWord & 0xc0) == 0x40) return DT_DATALONG;
  else return DT_UNKNOWN;
}


bool TpAlpidefs2::DecodeRegionHeader (const char Data, int &Region)
{
  int NewRegion = Data & 0x1f;
  if (NewRegion != Region + 1) {
    std::cout << "Corrupt region header, expected region " << Region +1 << ", found " << NewRegion << std::endl;
    return false;
  }
  else {
    Region = NewRegion;
    return true;
  }
}


bool TpAlpidefs2::DecodeChipHeader (const char Data, int &ChipId)
{
  if (CheckDataType(Data) != DT_CHIPHEADER) {
    std::cout << "Error, data word 0x" << std::hex << (int) Data << std::dec << " is no chip header" << std::endl;
    return false;
  }
  ChipId = Data & 0xf;
  //std::cout << "found chip id" << ChipId << std::endl;
  if ((ChipId != (fChipId & 0xf)) && (!fChipMasterFPGASlave)) {
    std::cout << "pALPIDE-2: Error, found wrong chip ID " << ChipId << " in header." << std::endl;
    return false;
  }
  else if (ChipId != (fChipId & 0xf)) {     // Data comes from FPGA MasterSlave emulator
    fSimData = true;
  }
  else {
    fSimData = false;
  }
  return true;
}


bool TpAlpidefs2::DecodeChipTrailer (const char Data, int &ChipId)
{
  if (CheckDataType(Data) != DT_CHIPTRAILER) {
    std::cout << "Error, data word 0x" << std::hex << (int)Data << std::dec << " is no chip trailer" << std::endl;
    return false;
  }
  ChipId = Data & 0xf;
  if ((ChipId != fChipId) && (!fChipMasterFPGASlave)) {
    std::cout << "Error, found wrong chip ID " << ChipId << " in header." << std::endl;
    return false;
  }
  fSimData = false;
  return true;
}


// should be moved somewhere else;
void TpAlpidefs2::WriteHitToFile (const char *fName, TPixHit Hit) {
  FILE *fp    = fopen (fName, "a");
  int  Column = AddressToColumn (Hit.region, Hit.doublecol, Hit.address);
  int  Row   = AddressToRow    (Hit.region, Hit.doublecol, Hit.address);
  if (fp) fprintf(fp, "%d %d\n", Column, Row);
  fclose (fp);
}


bool TpAlpidefs2::DecodeDataWord (unsigned char *Data, int Region, std::vector<TPixHit> *Hits, int NOldHits, bool Long)
{
  TPixHit Hit;
  int     ClusterSize;
  int     DoubleCol;
  int     Pixel;

  int16_t data_field = (((int16_t) Data[0]) << 8) + Data[1];


  DoubleCol   = (data_field & 0x3c00) >> 10;
  Pixel       = (data_field & 0x03ff);

  if (Long) {
    ClusterSize = (Data[2] & 0xc0) >> 6;
  }
  else {
    ClusterSize = 0;
  }


  for (int i = 0; i <= ClusterSize; i ++) {
    Hit.doublecol = DoubleCol;
    Hit.region    = Region;
    Hit.address   = Pixel + i;
#ifdef MYDEBUG
    std::cout << "  Hit Data: Region " << Region << ", Dataword 0x" << std::hex << Data << std::dec << ", DoubleCol " << DoubleCol << ", Pixel " << Hit.address << std::endl;
#endif
    if ((Hits->size() > NOldHits) && (Hit == Hits->at(Hits->size()-1))) {
      std::cout << "Warning, pixel " << Hit.region << "/" <<Hit.doublecol << "/" << Hit.address  << " found twice" << std::endl;
      AddStuckPixel(Hit);
    }
    if ((Hits->size() > NOldHits) && (Hit < Hits->at(Hits->size()-1))) std::cout << "Warning, pixel " << Hit.region << "/" <<Hit.doublecol << "/" << Hit.address  << " wrong ordering. previous: " << Hits->at(Hits->size()-1).region << "/" <<Hits->at(Hits->size()-1).doublecol << "/" << Hits->at(Hits->size()-1).address << std::endl;
    if (fSimData) {
      std::cout << "found simulated data" << std::endl;
      WriteHitToFile(gEmulatedDataFile, Hit);
    }
    else {
      Hits->push_back(Hit);
    }

  }
  return true;
}


bool TpAlpidefs2::DecodeEvent (unsigned char        *evtData,
                               int                   maxLength,
                               std::vector<TPixHit> *Hits,
                               unsigned int* StrobeCounter /*=0x0*/,
                               unsigned int* BunchCounter  /*=0x0*/,
                               unsigned int* StatusBits /*=0x0*/)
{
  int       byte    = 0;
  int       region  = -1;
  int       chip    = 0;
  bool      started = false; // event has started, i.e. chip header has been found
  TPalpideDataType type;
  int       NOldHits = Hits->size();

  // Have to revert byte order first?

  while (byte < maxLength) {
    type = CheckDataType (evtData[byte]);
    switch (type) {
    case DT_IDLE:
      byte +=1;
      break;
    case DT_NOP:
      byte += 1;
      break;
    case DT_BUSYON:
      byte += 1;
      break;
    case DT_BUSYOFF:
      byte += 1;
      break;
    case DT_CHIPHEADER:
      started = true;
      if (!DecodeChipHeader (evtData[byte], chip)) return false;
      byte += 1;
      break;
    case DT_CHIPTRAILER:
      if (!started) {
        std::cout << "Error, chip trailer found before chip header" << std::endl;
        return false;
      }
      if (region < 31) {
        std::cout << "Error, chip trailer found before last region, current region = " << region << std::endl;
        return false;
      }
      started = false;
      byte += 1;
      break;
    case DT_REGHEADER:
      if (!started) {
        std::cout << "Error, region header found before chip header or after chip trailer" << std::endl;
        return false;
      }
      if (!DecodeRegionHeader (evtData[byte], region)) return false;
      byte +=1;
      break;
    case DT_DATASHORT:
      if (!started) {
        std::cout << "Error, hit data found before chip header or after chip trailer" << std::endl;
        return false;
      }
      if (!DecodeDataWord (evtData + byte, region, Hits, NOldHits, false)) return false;
      byte += 2;
      break;
    case DT_DATALONG:
      if (!started) {
        std::cout << "Error, hit data found before chip header or after chip trailer" << std::endl;
        return false;
      }
      if (!DecodeDataWord (evtData + byte, region, Hits, NOldHits, true)) return false;
      byte += 3;
      break;
    case DT_UNKNOWN:
      std::cout << "Error, data of unknown type 0x" << std::hex << (int) evtData[byte] << std::dec << ", byte " << byte << std::endl;
      return false;
    }
  }
  if (started) {
    std::cout << "Warning, event not finished at end of data" << std::endl;
  }

  //std::cout << "Found " << Hits->size() - NOldHits << " hits" << std::endl;
  return true;
}


void TpAlpidefs2::GetTemperature (int &Value1, int &Value2) {
  int Value, OldValue = 1;
  float Result = 0;
  for (int i = 0; i < 256; i++) {
    SetDAC (FindDAC("VAUX"), i);
    ReadRegister(REG_TEMP, &Value);
    if (OldValue != Value) {
      //std::cout << "Temp: Switch from " << OldValue << " to " << Value << " at VAUX = " << i << std::endl;
      Value1 = i;
      OldValue = Value;
      break;
    }
    OldValue = Value;
  }
  for (int i = 255; i >= 0; i--) {
    SetDAC (FindDAC("VAUX"), i);
    ReadRegister(REG_TEMP, &Value);
    if (OldValue != Value) {
      //std::cout << "Temp: Switch from " << OldValue << " to " << Value << " at VAUX = " << i << std::endl;
      Value2 = i;
      break;
    }
    OldValue = Value;
  }
}


int TpAlpidefs2::ReadFuseValue () {
  int Low, High, Result;

  //WriteFuseBit(0);
  //sleep(1);

  ReadRegister(REG_FUSEREAD_LOW,  &Low);
  ReadRegister(REG_FUSEREAD_HIGH, &High);
  Result = Low | (High << 16);
  return Result;
}


// Bits to write fuses are active low, but inverted between register and fuse circuitry.
// Supposed to be written "one-hot", i.e. only one bit should be set to 1 at a time
void TpAlpidefs2::WriteFuseBit (int NBit) {
  int High = 0;
  int Low  = 0;
  int Value;

  std::cout << "writing fuse bit " << NBit << std::endl;
  if (NBit < 16) {
    Low |= (1 << NBit);
    std::cout << "Writing 0x" << std::hex << Low << " to register 0x" << REG_FUSEWRITE_LOW << std::endl;
    WriteRegister (REG_FUSEWRITE_LOW, Low);
    ReadRegister  (REG_FUSEREAD_LOW,  &Value);
    std::cout << "Readback value from reg 0x" << REG_FUSEREAD_LOW << ": 0x"  << Value << std::dec << std::endl;
    sleep(5);
    WriteRegister (REG_FUSEWRITE_LOW, 0x0);
  }
  else if (NBit < 24) {
    High |= (1 << (NBit - 16));
    //std::cout << "Writing 0x" << std::hex << High << " to register 0x" << REG_FUSEWRITE_HIGH << std::endl;
    WriteRegister (REG_FUSEWRITE_HIGH, High);
    sleep(5);
    WriteRegister (REG_FUSEWRITE_HIGH, 0x0);
  }
  else {
    std::cout << "Illegal bit number " << NBit << std::endl;
    return;
  }

}


// Write fuse value bit by bit
void TpAlpidefs2::WriteFuseValue(int Value) {

  int OldValue = ReadFuseValue();
  std::cout << "Writing fuses to value 0x" << std::hex << Value << ", current Value 0x" << OldValue << std::dec << std::endl;
  for (int i = 0; i < 24; i++) {
    if ((Value >> i) & 0x1) {
      WriteFuseBit (i);
    }
  }

  int NewValue = ReadFuseValue();

  std::cout << "WriteFuseValue: new value = 0x" << std::hex << NewValue << " (old value was 0x" << OldValue << std::dec << ")" << std::endl;

}


void TpAlpidefs2::WriteFuseValue(int AWafer, int AChip) {
  int Value = (AChip & 0xff);
  Value    |= ((AWafer & 0xffff) << 8);
  std::cout << "Writing fuse value 0x" << std::hex << Value << std::endl;
  WriteFuseValue (Value);
}


void TpAlpidefs2::ReadFuseValue (int &AWafer, int &AChip) {
  int Value = ReadFuseValue();
  AChip     = Value & 0xff;
  AWafer    = (Value >> 8) & 0xffff;
}


////////////////////////////////////////////////////////////////////////
//                                                                    //
//                    class TpAlpidefs3                               //
//                                                                    //
////////////////////////////////////////////////////////////////////////


TpAlpidefs3::TpAlpidefs3(TTestSetup *ASetup, int AIndex, TChipConfig *AConfig, bool Parent) : TpAlpidefs2(ASetup, AIndex, AConfig, true)
{
  fDCLKDrvrCurrent = AConfig->DCLKDrvrCurrent;
  fMCLKRcvrCurrent = AConfig->MCLKRcvrCurrent;

  fIRefCurrent     = 0x2;

  fInternalStrobeCont = false;
  fBusyMonitoring     = false;
  fPulseType          = PT_DIGITAL;
  fInternalStrobe     = false;

  fSlowMatrix = false;

  fDisableDoubleDataRate = AConfig->DisableDDR;
  if (!Parent) {      // initialise array only if *not* called as superclass constructor
    fDACs = new int[GetNDacs()];
  }
}


int TpAlpidefs3::Init (bool Reset)
{
  int err = 1;
  TDAQBoard2 *myBoard = (TDAQBoard2*) fSetup->GetDAQBoard(fIndex);
  myBoard->SetChipIdI2C (fChipId);

  // global chip reset
  if (Reset) err = GlobalReset ();
  if (err != 1) return err;

  //return err;

  SetChipMode(MODE_ALPIDE_CONFIG);

  // SendCommand(OPCODE_CLEARCMUFLAGS);

  // write default DAC values
  SetDefaultDACValues();
  err = WriteAllDACRegisters();
  if (err != 1) return err;

  // set in-pixel MASK_EN registers -> mask all pixels
  err = SetMaskAllPixels (true);
  if (err != 1) return err;

  // set in-pixel PULSE_EN registers -> select no pixel
  SetMaskAllPixels (false, PR_PULSEENABLE);
  if (err != 1) return err;

  // reset pixel state registers
  err = PixelReset();
  if (err != 1) return err;

  //err =  WriteCMUCurrents();
  if (err != 1) return err;

  //err = WriteDTUCurrents();
  if (err != 1) return err;

  err = WriteCMUDMUConfig ();
  if (err != 1) return err;

  err = WriteFROMUConfig();
  if (err != 1) return err;

  //PrintCMUErrors();

  return err;
}


int TpAlpidefs3::WriteAllDACRegisters()
{
  int err = 1;
  for (int i = 0; i < GetNDacs(); i ++) {
    err = WriteDACRegister (i);
    if (err != 1) break;
  }
  return err;
}


int TpAlpidefs3::WriteDACRegister (int ADAC)
{
  //std::cout<<"Writing dac "<<ADAC<<" to "<<fDACs[ADAC]<<std::endl;
  int Address = REG_DACBASE + ADAC + 1;   // pALPIDE-3: one register per DAC, starting at 0x601
  return WriteRegister (Address, fDACs[ADAC]);
}


int TpAlpidefs3::SetMonitoringRegister (TAlpideDacmonV ADacV, TAlpideDacmonI ADacI, bool OverrideV, bool OverrideI)
{
  int Value = 0;
  int Voltage, Current;
  switch (ADacV) {        // not nice, fix later...
  case SWCNTL_VCASN:      Voltage = 0; break;
  case SWCNTL_VCASP:      Voltage = 1; break;
  case SWCNTL_VPLSE_HIGH: Voltage = 2; break;
  case SWCNTL_VPLSE_LOW:  Voltage = 3; break;
  case SWCNTL_VRESETP:    Voltage = 4; break;
  case SWCNTL_VRESETD:    Voltage = 5; break;
  case SWCNTL_VCASN2:     Voltage = 6; break;
  case SWCNTL_VCLIP:      Voltage = 7; break;
  case SWCNTL_VTEMP:      Voltage = 8; break;
  default:                Voltage = 15; break;
  }
  switch (ADacI) {
  case SWCNTL_IRESET:  Current = 0; break;
  case SWCNTL_IAUX2:   Current = 1; break;
  case SWCNTL_IBIAS:   Current = 2; break;
  case SWCNTL_IDB:     Current = 3; break;
  case SWCNTL_IREF:    Current = 4; break;
  case SWCNTL_ITHR:    Current = 5; break;
  case SWCNTL_IREFBUF: Current = 6; break;
  default:             Current = 7; break;
  }

  Value |= (Voltage & 0xf );
  Value |= (Current & 0x7 )     << 4;
  Value |= (OverrideI ? 1:0)    << 7;
  Value |= (OverrideV ? 1:0)    << 8;
  Value |= (fIRefCurrent & 0x3) << 9;

  return WriteRegister(REG_MONITORING, Value);

}


int TpAlpidefs3::WriteClockCurrents ()
{
  int Value = (fDCLKRcvrCurrent & 0xf) |
    ((fDCLKDrvrCurrent & 0xf) << 4) |
    ((fMCLKRcvrCurrent & 0xf) << 8);
  return WriteRegister(REG_CLKCURRENTS, Value);
}


int TpAlpidefs3::WriteClockCurrents (int ADCLKRcvrCurrent, int ADCLKDrvrCurrent, int AMCLKRcvrCurrent)
{
  fDCLKRcvrCurrent = ADCLKRcvrCurrent;
  fDCLKDrvrCurrent = ADCLKDrvrCurrent;
  fMCLKRcvrCurrent = ADCLKDrvrCurrent;
  return WriteClockCurrents();
}


int TpAlpidefs3::WriteCMUCurrents ()
{
  int Value = (fDCTRLRcvrCurrent & 0xf) |
    ((fDCTRLDrvrCurrent & 0xf) << 4);
  return WriteRegister(REG_CMUCURRENTS, Value);
}


int TpAlpidefs3::WriteCMUCurrents (int ADCTRLRcvrCurrent, int ADCTRLDrvrCurrent)
{
  fDCTRLRcvrCurrent = ADCTRLRcvrCurrent;
  fDCTRLDrvrCurrent = ADCTRLDrvrCurrent;
  return WriteCMUCurrents();
}


int TpAlpidefs3::WriteDTUCurrents ()
{
  int Value = ( fPLLDAC           & 0xf) |
    ((fDPORTDrvrCurrent & 0xf) << 4) |
    ((fPreempCurrent    & 0xf) << 8);
  return WriteRegister (REG_DTUDACS, Value);
}


int TpAlpidefs3::WriteDTUCurrents (int APLLDAC, int ADPORTDrvrCurrent, int APreempCurrent)
{
  fPLLDAC           = APLLDAC;
  fDPORTDrvrCurrent = ADPORTDrvrCurrent;
  fPreempCurrent    = APreempCurrent;
  return WriteDTUCurrents();
}


int TpAlpidefs3::WriteDTUCfgReg (int pllCfg, int serPhase, int PLLRst)
{
  int value = ( (pllCfg   & 0xf) |
                ((serPhase & 0xf) << 4) |
                ((PLLRst   & 0x1) << 8) );

  return WriteRegister (REG_DTUCONFIG, value);
}


int TpAlpidefs3::Write16BitReg (int Register, int &fValue, int AValue)
{
  if ((AValue < 0) || (AValue > 0xffff)) {
    std::cout << "Invalid Value " << AValue << "for reg 0x" << std::hex << Register << std::dec << ", setting to maximum (65535)" << std::endl;
    fValue = 0xffff;
  }
  else {
    fValue = AValue;
  }
  return WriteRegister(Register, fValue);
}


int TpAlpidefs3::WriteCMUDMUConfig     ()
{
  int InitialToken = 1;
  int Value = ((fDisableDoubleDataRate ? 0:1) << 7) |
    ((fDisableManchester ? 1:0) << 6) |
    ((InitialToken ? 1:0) << 5) |
    ((fPreviousChip & 0xf));
  //std::cout << "Writing 0x" << std::hex << Value << std::dec << " to CMUDMU config reg " <<std::endl;
  int err = WriteRegister (REG_CMUDMU_CONFIG, Value);
  if (err == 1) return SendCommand(OPCODE_RORST);
  else return err;
}


int TpAlpidefs3::GetCMUErrorCounts (int &CodeUnknown, int &Timeout, int &Deserializer)
{
  int Value;
  int err = ReadRegister (REG_CMUERRORS, &Value);
  CodeUnknown  = (Value >> 8) & 0xf;
  Timeout      = (Value >> 4) & 0xf;
  Deserializer = Value & 0xf;
  return err;
}


int TpAlpidefs3::WriteControlRegister(bool SelfTest, bool Busy)
{
  int16_t Value = 0;
  if (fChipMode == MODE_ALPIDE_PATTERNGEN) {
    std::cout << "Mode not supported by pALPIDE-3" << std::endl;
  }
  else if (fChipMode == MODE_ALPIDE_READOUT_A) {
    Value = 2;
  }
  else if (fChipMode == MODE_ALPIDE_READOUT_B) {
    Value = 1;
  }
  else Value = 0;
  Value |= ((int) fClusteringEnabled     << 2);
  Value |= ((int) SelfTest               << 3);
  Value |= ((int) fXOff                  << 4);
  Value |= ((fSlowMatrix?0:1)            << 5);
  Value |= ((int) fForceBusy             << 6);
  Value |= ((int) Busy                   << 7);
  //#ifdef MYDEBUG
  //std::cout<< "Writing 0x" << std::hex << Value << std::dec << " to control register" << std::endl;
  //#endif
  return WriteRegister(REG_CONTROL, Value);
}


int TpAlpidefs3::WriteFROMUConfig ()
{
  int16_t Value = fMEBMask & 0x7;
  Value |= ((int) fInternalStrobeCont << 3);
  Value |= ((int) fBusyMonitoring     << 4);
  Value |= ((int) fPulseType          << 5);
  Value |= ((int) fInternalStrobe     << 6);

  return WriteRegister(REG_FROMUCONFIG1, Value);
}


int TpAlpidefs3::MaskMEB(int ABuffer)
{
  if ((ABuffer < 0) || (ABuffer > 2)) {
    std::cout << "Bad Buffer number " << ABuffer << std::endl;
    return -1;
  }
  fMEBMask |= (1 << ABuffer);
  return WriteFROMUConfig();
}


int TpAlpidefs3::UnmaskMEB(int ABuffer)
{
  if ((ABuffer < 0) || (ABuffer > 2)) {
    std::cout << "Bad Buffer number " << ABuffer << std::endl;
    return -1;
  }
  fMEBMask &= 0x7 - (1 << ABuffer);
  return WriteFROMUConfig();
}


int TpAlpidefs3::SetPulseType (TAlpidePulseType APulseType)
{
  fPulseType = APulseType;
  return WriteFROMUConfig();
}


int TpAlpidefs3::ReadFROMUCounters (int &BunchCounter, int &EventCounter )
{
  int Value;
  int err = ReadRegister (REG_FROMUSTATUS2, &Value);
  if (err == 1) {
    BunchCounter = Value & 0xfff;
    EventCounter = (Value >> 12) & 0x3;
  }
  return err;
}


int TpAlpidefs3::ReadStrobeCounter ()
{
  int Value;
  int err = ReadRegister (REG_FROMUSTATUS1, &Value);
  if (err == 1) {
    return Value;
  }
  else return -1;
}


int TpAlpidefs3::StrobePixelConfig()
{
  int err = WriteRegister(REG_PIXELCONFIG3, 0x1);
  if (err == 1)
    return WriteRegister (REG_PIXELCONFIG3, 0x0);
  else return err;
}


// Warning: the data type is not unambiguous if called with byte of a dataword,
// which is not the most significant byte (in case of DATA SHORT or DATA LONG)
TPalpideDataType TpAlpidefs3::CheckDataType (unsigned char DataWord)
{
  if      (DataWord == 0xff)          return DT_IDLE;
  //  else if (DataWord == 0xff)          return DT_NOP;
  else if (DataWord == 0xf1)          return DT_BUSYON;
  else if (DataWord == 0xf0)          return DT_BUSYOFF;
  else if (DataWord == 0xbc)          return DT_COMMA;
  else if ((DataWord & 0xf0) == 0xa0) return DT_CHIPHEADER;
  else if ((DataWord & 0xf0) == 0xb0) return DT_CHIPTRAILER;
  else if ((DataWord & 0xf0) == 0xe0) return DT_EMPTYFRAME;
  else if ((DataWord & 0xe0) == 0xc0) return DT_REGHEADER;
  else if ((DataWord & 0xc0) == 0x40) return DT_DATASHORT;
  else if ((DataWord & 0xc0) == 0x0)  return DT_DATALONG;
  else return DT_UNKNOWN;
}


bool TpAlpidefs3::DecodeRegionHeader (const char Data, int &Region)
{
  int NewRegion = Data & 0x1f;
  if (NewRegion <= Region) {
    std::cout << "Corrupt region header, expected region, found " << NewRegion << " after " << Region << std::endl;
    return false;
  }
  else {
    Region = NewRegion;
    return true;
  }
}


bool TpAlpidefs3::DecodeChipHeader (unsigned char *Data, int &ChipId, unsigned int& StrobeCounter, unsigned int& BunchCounter)
{
  int16_t header = (((int16_t) Data[0]) << 8) + Data[1];
  if ((CheckDataType(Data[0]) != DT_CHIPHEADER) && (CheckDataType(Data[0]) != DT_EMPTYFRAME)) {
    std::cout << "Error, data word 0x" << std::hex << header << std::dec << " is neither chip header nor empty frame" << std::endl;
    return false;
  }
  ChipId = header & 0xf00;
  //std::cout << "found chip id" << ChipId << std::endl;
  if ((ChipId != (fChipId & 0xf)) && (!fChipMasterFPGASlave)) {
    std::cout << "pALPIDE-3: Error, found wrong chip ID " << ChipId << " in header." << std::endl;
    return false;
  }
  else if (ChipId != (fChipId & 0xf)) {     // Data comes from FPGA MasterSlave emulator
    fSimData = true;
  }
  else {
    fSimData = false;
  }
  StrobeCounter = header & 0x3;
  BunchCounter  = (header >> 2) & 0x3f;
  return true;
}


bool TpAlpidefs3::DecodeChipTrailer  (unsigned char *Data, unsigned int& statusBits)
{
  int16_t trailer =  Data[0]; //(((int16_t) Data[0]) << 8) + Data[1];
  if (CheckDataType(Data[0]) != DT_CHIPTRAILER) {
    std::cout << "Error, data word 0x" << std::hex << trailer << std::dec << " is no chip trailer" << std::endl;
    return false;
  }

  if (trailer & 0xf) {
    std::cout << "Warning, readout flags not 0. Found " << (trailer & 0x8 ? "BUSY_VIOLATION ":"")
              << (trailer & 0x4 ? "FLUSHED_INCOMPLETE ":"")
              << (trailer & 0x2 ? "FATAL ":"")
              << (trailer & 0x1 ? "BUSY_TRANSITION":"")
              << std::endl;
  }
  statusBits = trailer & 0xf;

  return true;
}


bool TpAlpidefs3::DecodeDataWord     (unsigned char *Data, int Region, std::vector<TPixHit> *Hits, int NOldHits, bool Long)
{
  TPixHit Hit;
  int     ClusterSize;
  int     DoubleCol;
  int     Pixel;

  int16_t data_field = (((int16_t) Data[0]) << 8) + Data[1];


  DoubleCol   = (data_field & 0x3c00) >> 10;
  Pixel       = (data_field & 0x03ff);

  if (Long) {
    ClusterSize = 7;
  }
  else {
    ClusterSize = 0;
  }

  //      ClusterSize = (Data[2] & 0xc0) >> 6;

  for (int i = 0; i <= ClusterSize; i ++) {
    if ((i > 0) && ((Data[2] >> (i-1)) & 0x1)) continue;   // always add the first hit, any further hit only if corresp. hit pattern bit = 1
    Hit.doublecol = DoubleCol;
    Hit.region    = Region;
    Hit.address   = Pixel + i;
#ifdef MYDEBUG
    std::cout << "  Hit Data: Region " << Region << ", Dataword 0x" << std::hex << Data << std::dec << ", DoubleCol " << DoubleCol << ", Pixel " << Hit.address << std::endl;
#endif
    if ((Hits->size() > NOldHits) && (Hit == Hits->at(Hits->size()-1))) {
      std::cout << "Warning, pixel " << Hit.region << "/" <<Hit.doublecol << "/" << Hit.address  << " found twice" << std::endl;
      AddStuckPixel(Hit);
    }
    if ((Hits->size() > NOldHits) && (Hit < Hits->at(Hits->size()-1))) std::cout << "Warning, pixel " << Hit.region << "/" <<Hit.doublecol << "/" << Hit.address  << " wrong ordering. previous: " << Hits->at(Hits->size()-1).region << "/" <<Hits->at(Hits->size()-1).doublecol << "/" << Hits->at(Hits->size()-1).address << std::endl;
    if (fSimData) {
      std::cout << "found simulated data" << std::endl;
      WriteHitToFile(gEmulatedDataFile, Hit);
    }
    else {
      Hits->push_back(Hit);
    }
  }
  return true;

}


bool TpAlpidefs3::DecodeEvent (unsigned char        *evtData,
                               int                   maxLength,
                               std::vector<TPixHit>  *Hits,
                               unsigned int* StrobeCounter /*=0x0*/,
                               unsigned int* BunchCounter  /*=0x0*/,
                               unsigned int* StatusBits /*=0x0*/)
{
  int       byte    = 0;
  int       region  = -1;
  int       chip    = 0;
  bool      started = false; // event has started, i.e. chip header has been found
  bool      finished = false; // event trailer found
  TPalpideDataType type;
  int       NOldHits = (Hits) ? Hits->size() : -1;

  unsigned int StrobeCounterTmp;
  unsigned int BunchCounterTmp;
  unsigned int StatusBitsTmp;

  // Have to revert byte order first?

  while (byte < maxLength) {
    type = CheckDataType (evtData[byte]);
    switch (type) {
    case DT_IDLE:
      byte +=1;
      break;
    case DT_BUSYON:
      byte += 1;
      break;
    case DT_BUSYOFF:
      byte += 1;
      break;
    case DT_COMMA:
      byte += 1;
      break;
    case DT_EMPTYFRAME:
      started = true;
      if (!DecodeChipHeader (evtData + byte, chip, StrobeCounterTmp, BunchCounterTmp)) return false;
      byte += 2;
      if (StrobeCounter) *StrobeCounter = StrobeCounterTmp;
      if (BunchCounter)  *BunchCounter  = BunchCounterTmp;
      byte += 3;
      finished = true;
      break;
    case DT_CHIPHEADER:
      started = true;
      if (!DecodeChipHeader (evtData + byte, chip, StrobeCounterTmp, BunchCounterTmp)) return false;
      byte += 2;
      if (StrobeCounter) *StrobeCounter = StrobeCounterTmp;
      if (BunchCounter)  *BunchCounter  = BunchCounterTmp;
      break;
    case DT_CHIPTRAILER:
      if (!started) {
        std::cout << "Error, chip trailer found before chip header" << std::endl;
        return false;
      }
      if (finished) {
        std::cout << "Error, chip trailer found after event was finished" << std::endl;
        return false;
      }
      if (!DecodeChipTrailer (evtData + byte, StatusBitsTmp)) return false;
      if (StatusBits) *StatusBits = StatusBitsTmp;
      finished = true;
      byte += 2;
      break;
    case DT_REGHEADER:
      if (!started) {
        std::cout << "Error, region header found before chip header or after chip trailer" << std::endl;
        return false;
      }
      if (!DecodeRegionHeader (evtData[byte], region)) return false;
      byte +=1;
      break;
    case DT_DATASHORT:
      if (!started) {
        std::cout << "Error, hit data found before chip header or after chip trailer" << std::endl;
        return false;
      }
      if (Hits) {
        if (!DecodeDataWord (evtData + byte, region, Hits, NOldHits, false)) return false;
      }
      byte += 2;
      break;
    case DT_DATALONG:
      if (!started) {
        std::cout << "Error, hit data found before chip header or after chip trailer" << std::endl;
        return false;
      }
      if (Hits) {
        if (!DecodeDataWord (evtData + byte, region, Hits, NOldHits, true)) return false;
      }
      byte += 3;
      break;
    case DT_NOP:
    break;  
    case DT_UNKNOWN:
      std::cout << "Error, data of unknown type 0x" << std::hex << evtData[byte] << std::dec << std::endl;
      return false;
    }
  }
  //std::cout << "Found " << Hits->size() - NOldHits << " hits" << std::endl;
  if (started && finished) return true;
  else {
    if (started && !finished) {
      std::cout << "Warning, event not finished at end of data" << std::endl;
    }
    if (!started) {
      std::cout << "Warning, event not started at end of data" << std::endl;
    }
    return false;
  }
}


int TpAlpidefs3::ConfigureDTU_PropPLLclock()
{
  // static const int  REG_DTU_TESTREG1   = 0x12;
  std::cout << "TpAlpidefs3::ConfigureDTU_TESTREG1. Propagates PLL clock" << std::endl;

  // DTU TEST REG 1
  // Forces unset load_ena, hence propagates pll clock to hsdat
  const int value_dtutestreg1 = 0x4000;
  int err = WriteRegister (REG_DTU_TESTREG1, value_dtutestreg1 );
  int read_value = 0;
  err = ReadRegister (REG_DTU_TESTREG1, &read_value );

  return err;
}

int TpAlpidefs3::ConfigureDTU_setCurrents (int pll_dac = 8, int driver_dac  = 8, int preemph_dac = 8 )
{
  // static const int  REG_DTU_TESTREG1   = 0x12;
  std::cout << "TpAlpidefs3::ConfigureDTU_setCurrets. setting PLL, driver and preemph currents" << std::endl;

  // DTU DACs Register
  // set pll, driver and preemph currents
  return WriteDTUCurrents( pll_dac, driver_dac, preemph_dac );
}

int TpAlpidefs3::ConfigureTestDTU()
{
  // static const int  REG_DTU_PLLLOCKREG1= 0x10;
  // static const int  REG_DTU_PLLLOCKREG2= 0x11;
  // static const int  REG_DTU_TESTREG1   = 0x12;
  // static const int  REG_DTU_TESTREG2   = 0x13;
  // static const int  REG_DTU_TESTREG3   = 0x14;


  //std::cout << "TpAlpidefs3::ConfigureTestDTU. Starting DTU_test" << std::endl;

  // DTU TEST REG 1
  // Forces unset load_ena, hence propagates pll clock to hsdat
  const int value_dtutestreg1 = 0x4000;
  int err = WriteRegister (REG_DTU_TESTREG1, value_dtutestreg1 );
  int read_value = 0;
  err = ReadRegister (REG_DTU_TESTREG1, &read_value );

  // DTU DACs Register
  // set pll, driver and preemph currents
  const int pll_dac_default     = 2;
  const int driver_dac_default  = 8;
  const int preemph_dac_default = 0;
  WriteDTUCurrents( pll_dac_default, driver_dac_default, preemph_dac_default );

  // check pll lock counter
  err = ReadRegister (REG_DTU_PLLLOCKREG1, &read_value );

  // PLL config register
  int LockWaitCycles = 10;        // 8 bit [0;7]
  int UnlockWaitCycles = 10;      // 8 bit [8:15]
  const int plllockreg2value    = ( (LockWaitCycles   & 0x8) |
                                    ((UnlockWaitCycles & 0x8) << 8) );;
  err = WriteRegister (REG_DTU_PLLLOCKREG2, plllockreg2value );
  err = ReadRegister  (REG_DTU_PLLLOCKREG2, &read_value );

  // DTU Configuration Register
  enum stages {STAGES3 = 0, STAGES4 = 1, STAGES5 = 3};
  stages pllStages    = STAGES4;
  int pllFilter       = 0;
  int pllOutDisable   = 0;
  const int pllCfg_default      = ( (pllStages      & 0x3) |
                                    ((pllFilter     & 0x1) << 2) |
                                    ((pllOutDisable & 0x1) << 3) );;
  const int serPhase_default    = 8;
  int PLLRst              = 1;

  // set values and assert reset and read back
  WriteDTUCfgReg ( pllCfg_default , serPhase_default, PLLRst);
  err = ReadRegister (REG_DTUCONFIG, &read_value );

  // deassert reset and read back
  PLLRst              = 0;
  WriteDTUCfgReg ( pllCfg_default , serPhase_default, PLLRst);
  err = ReadRegister (REG_DTUCONFIG, &read_value );

  // check pll lock counter
  err = ReadRegister (REG_DTU_PLLLOCKREG1, &read_value );
  usleep(10000);
  err = ReadRegister (REG_DTU_PLLLOCKREG1, &read_value );
  usleep(10000);
  err = ReadRegister (REG_DTU_PLLLOCKREG1, &read_value );
  usleep(10000);
  err = ReadRegister (REG_DTU_PLLLOCKREG1, &read_value );
  usleep(10000);
  err = ReadRegister (REG_DTU_PLLLOCKREG1, &read_value );

  sleep(3);

  return err;
}

int TpAlpidefs3::ConfigureTestDTUprbs()
{
  // static const int  REG_DTU_PLLLOCKREG1= 0x10;
  // static const int  REG_DTU_PLLLOCKREG2= 0x11;
  // static const int  REG_DTU_TESTREG1   = 0x12;
  // static const int  REG_DTU_TESTREG2   = 0x13;
  // static const int  REG_DTU_TESTREG3   = 0x14;


  //std::cout << "TpAlpidefs3::ConfigureTestDTUprbs. Starting DTU_test" << std::endl;

  // DTU TEST REG 1
  // Forces prbs clock propagates pll clock to hsdata
  const int value_dtutestreg1 = 0x0004;
  int err = WriteRegister (REG_DTU_TESTREG1, value_dtutestreg1 );
  int read_value = 0;
  err = ReadRegister (REG_DTU_TESTREG1, &read_value );

  // DTU DACs Register
  // set pll, driver and preemph currents
  const int pll_dac_default     = 8;
  const int driver_dac_default  = 8;
  const int preemph_dac_default = 8;
  WriteDTUCurrents( pll_dac_default, driver_dac_default, preemph_dac_default );

  // check pll lock counter
  err = ReadRegister (REG_DTU_PLLLOCKREG1, &read_value );

  // PLL config register
  int LockWaitCycles = 10;        // 8 bit [0;7]
  int UnlockWaitCycles = 10;      // 8 bit [8:15]
  const int plllockreg2value    = ( (LockWaitCycles   & 0x8) |
                                    ((UnlockWaitCycles & 0x8) << 8) );;
  err = WriteRegister (REG_DTU_PLLLOCKREG2, plllockreg2value );
  err = ReadRegister  (REG_DTU_PLLLOCKREG2, &read_value );

  // DTU Configuration Register
  enum stages {STAGES3 = 0, STAGES4 = 1, STAGES5 = 3};
  int pllOutDisable   = 0;
  int pllFilter       = 1;
  stages pllStages    = STAGES5;
  const int pllCfg_default      = ( (pllStages      & 0x3) |
                                    ((pllFilter     & 0x1) << 2) |
                                    ((pllOutDisable & 0x1) << 3) );;
  const int serPhase_default    = 8;
  int PLLRst              = 1;

  // set values and assert reset and read back
  WriteDTUCfgReg ( pllCfg_default , serPhase_default, PLLRst);
  err = ReadRegister (REG_DTUCONFIG, &read_value );

  // deassert reset and read back
  PLLRst              = 0;
  WriteDTUCfgReg ( pllCfg_default , serPhase_default, PLLRst);
  err = ReadRegister (REG_DTUCONFIG, &read_value );

  // check pll lock counter
  err = ReadRegister (REG_DTU_PLLLOCKREG1, &read_value );
  usleep(10000);
  err = ReadRegister (REG_DTU_PLLLOCKREG1, &read_value );
  usleep(10000);
  err = ReadRegister (REG_DTU_PLLLOCKREG1, &read_value );
  usleep(10000);
  err = ReadRegister (REG_DTU_PLLLOCKREG1, &read_value );
  usleep(10000);
  err = ReadRegister (REG_DTU_PLLLOCKREG1, &read_value );

  sleep(3);
  return err;
}


int TpAlpidefs3::ConfigurePulse        (int ALength)
{
  fPulseLength=ALength;
  return WriteRegister (REG_FROMUPULSING2, ALength);
}


int TpAlpidefs3::SetStrobeTiming     (int AStrobeTiming)
{
  fStrobeTiming = AStrobeTiming;
  return WriteRegister (REG_FROMUCONFIG2, fStrobeTiming);
}


bool TpAlpidefs3::FifoReadbackTest (int AReg, int AWord, int AValue)
{
  int AddressLow  = REG_DATALSBBASE | (AReg << 11) | (AWord);
  int AddressHigh = REG_DATAMSBBASE | (AReg << 11) | (AWord);
  int ReadValue, ReadLow, ReadHigh;

  WriteRegister(AddressLow,   AValue        & 0xffff);
  WriteRegister(AddressHigh, (AValue >> 16) & 0xff);

  ReadRegister(AddressLow,  &ReadLow);
  ReadRegister(AddressHigh, &ReadHigh);
  ReadValue = ReadLow | (ReadHigh << 16);
  if (ReadValue != AValue) {
    std::cout << std::hex << "Register readback of reg 0x" << AddressLow << " failed, wrote 0x" << AValue << ", read 0x " << ReadValue << std::dec << std::endl;
    //sleep(10);
    return false;
  }
  else {
    // std::cout << std::hex << "Register readback of reg 0x" << Aaddress << " worked, wrote 0x" << AValue << ", read 0x " << ReadValue << std::dec << std::endl;
  }
  return true;
}


bool TpAlpidefs3::TestFifo (int AReg, int AWord, int AValue)
{
  bool Result1, Result2, Result3;
  if (AValue == -1) {
    Result1 = FifoReadbackTest(AReg, AWord, 0);
    Result2 = FifoReadbackTest(AReg, AWord, 0xffffff);
    Result3 = FifoReadbackTest(AReg, AWord, 0x555555);
    return ( Result1 && Result2 && Result3 );
  }
  else {
    return FifoReadbackTest(AReg, AWord, AValue);
  }
}


int TpAlpidefs3::AddressToColumn      (int ARegion, int ADoubleCol, int AAddress)
{
  int Column    = ARegion * 32 + ADoubleCol * 2;    // Double columns before ADoubleCol
  int LeftRight = ((((AAddress % 4) == 1) || ((AAddress % 4) == 2))? 1:0);       // Left or right column within the double column

  Column += LeftRight;

  return Column;
}


int TpAlpidefs3::AddressToRow         (int ARegion, int ADoubleCol, int AAddress)
{
  int Row = AAddress / 2;
  return Row;
}


int TpAlpidefs3::ColRowToAddress      (int AColumn, int ARow)
{
  int Address = ARow * 2;
  if (ARow % 2) {
    Address += 1 - (AColumn % 2);
  }
  else {
    Address += (AColumn %2);
  }
  return Address;
}

void TpAlpidefs3::GetTemperature (int &Value1, int &Value2) {
  int Value, OldValue = 1;
  float Result = 0;
  for (int i = 0; i < 256; i++) {
    SetDAC (FindDAC("VTEMP"), i);
    ReadRegister(REG_TEMP, &Value);
    if (OldValue != Value) {
      //std::cout << "Temp: Switch from " << OldValue << " to " << Value << " at VTEMP = " << i << std::endl;
      Value1 = i;
      OldValue = Value;
      break;
    }
    OldValue = Value;
  }
  for (int i = 255; i >= 0; i--) {
    SetDAC (FindDAC("VTEMP"), i);
    ReadRegister(REG_TEMP, &Value);
    if (OldValue != Value) {
      //std::cout << "Temp: Switch from " << OldValue << " to " << Value << " at VTEMP = " << i << std::endl;
      Value2 = i;
      break;
    }
    OldValue = Value;
  }
}

void TpAlpidefs3::ReadAllRegisters() {
  int regs[] = { REG_CONTROL, REG_FROMUCONFIG1, REG_FROMUCONFIG2, REG_FROMUPULSING1, REG_FROMUPULSING2, REG_FROMUSTATUS1, REG_FROMUSTATUS2, REG_CLKCURRENTS, REG_CMUCURRENTS, REG_CMUDMU_CONFIG, REG_CMUERRORS, REG_DTUCONFIG, REG_DTUDACS, REG_DTU_PLLLOCKREG1, REG_DTU_PLLLOCKREG2, REG_DTU_TESTREG1, REG_DTU_TESTREG2, REG_DTU_TESTREG3, REG_BUSYWIDTH, REG_FUSEWRITE_LOW, REG_FUSEWRITE_HIGH, REG_FUSEREAD_LOW, REG_FUSEREAD_HIGH, REG_TEMP, REG_PIXELCONFIG3, REG_DATALSBBASE, REG_DATAMSBBASE, REG_MONITORING, REG_BYPASSBUFFER };
  const char* reg_names[] = {"REG_CONTROL", "REG_FROMUCONFIG1", "REG_FROMUCONFIG2", "REG_FROMUPULSING1", "REG_FROMUPULSING2", "REG_FROMUSTATUS1", "REG_FROMUSTATUS2", "REG_CLKCURRENTS", "REG_CMUCURRENTS", "REG_CMUDMU_CONFIG", "REG_CMUERRORS", "REG_DTUCONFIG", "REG_DTUDACS", "REG_DTU_PLLLOCKREG1", "REG_DTU_PLLLOCKREG2", "REG_DTU_TESTREG1", "REG_DTU_TESTREG2", "REG_DTU_TESTREG3", "REG_BUSYWIDTH", "REG_FUSEWRITE_LOW", "REG_FUSEWRITE_HIGH", "REG_FUSEREAD_LOW", "REG_FUSEREAD_HIGH", "REG_TEMP", "REG_PIXELCONFIG3", "REG_DATALSBBASE", "REG_DATAMSBBASE", "REG_MONITORING", "REG_BYPASSBUFFER" };
  const int n_regs = sizeof(regs)/sizeof(int);

  int value = -1;
  for (int ireg=0; ireg<n_regs; ++ireg) {
    ReadRegister(regs[ireg], &value);
    std::cout << ireg << "\t" << reg_names[ireg] << " (0x" << std::hex << regs[ireg] << "):\t0x" << value << std::dec << std::endl;
  }
}


////////////////////////////////////////////////////////////////////////
//                                                                    //
//                    class TpAlpidefs4                               //
//                                                                    //
////////////////////////////////////////////////////////////////////////


TpAlpidefs4::TpAlpidefs4(TTestSetup *ASetup, int AIndex, TChipConfig *AConfig, bool Parent) : TpAlpidefs3(ASetup, AIndex, AConfig, true)
{
    // 0x0001 Mode Control Register
    fSlowMatrix = false;  
    fChipMode = 0;
    fClusteringEnable = 1;
    fMatrixReadoutSpeed = 1;
    fIBSerialLinkSpeed = 0;
    fSkewingGlobalSignal = 0;
    fSkewingStartReadout = 0;
    fReadoutClockGating = 0; 
    fReadoutCMU = 0;
    //0x0004  FROMU Configuration Register 1   
    fMEBMask = 0;              // 3 bit mask for MEBuffers. 0 = all enabled, 7 = all disabled
    fBusyMonitoring = 0;
    fInternalStrobeCont = 0;   // Repetetive strobe for continuous mode    
    fPulseType = 0;	   
    fInternalStrobe = 0;    
    fRotatePulseLine = 0;   
    fTriggerDelay = 0;
    //0x0010 CMU DMU
    fPreviousChipID = 0;
    fInitialToken = 1;        
    fDisableManchester = 1;
    fEnableDoubleDataRate = 0;
    // TO CHECK
    fDCLKDrvrCurrent = AConfig->DCLKDrvrCurrent;
    fMCLKRcvrCurrent = AConfig->MCLKRcvrCurrent;
    fIRefCurrent     = 0x2;

    if (!Parent) {      // initialise array only if *not* called as superclass constructor
        fDACs = new int[GetNDacs()];
    }
}

//REGISTERS

int TpAlpidefs4::WriteControlRegister()
{
    int16_t Value = 0;
    Value |= (fChipMode              << 0);
    Value |= (fClusteringEnable      << 2);
    Value |= (fMatrixReadoutSpeed    << 3);  
    Value |= (fIBSerialLinkSpeed     << 4); // 0 -> 400mbit 1-> 600 mbit 2 -> 1200 mbit
    Value |= (fSkewingGlobalSignal   << 6);
    Value |= (fSkewingStartReadout   << 7);
    Value |= (fReadoutClockGating    << 8);
    Value |= (fReadoutCMU 	       << 9);
    return WriteRegister(0x1, Value);
}

int  TpAlpidefs4::WriteControlRegister(TAlpideMode AMode, int AClustering, int ASlowMatrix, int AIBSerialLinkSpeed,int ASkewingGlobalSignal, int ASkewingStartReadout,int AReadoutClockGating,int AReadoutCMU)
{
    fChipMode              = AMode;
    fClusteringEnabled     = AClustering;
    fSlowMatrix            = ASlowMatrix;
    fIBSerialLinkSpeed     = AIBSerialLinkSpeed;
    fSkewingGlobalSignal   = ASkewingGlobalSignal;
    fSkewingStartReadout   = ASkewingStartReadout;
    fReadoutClockGating    = AReadoutClockGating;
    fReadoutCMU	           = AReadoutCMU;
    return WriteControlRegister();    
}

void TpAlpidefs4::PrepareReadout (int AStrobeBLength, int AReadoutDelay, TAlpideMode AReadoutMode)
{
    SetChipMode(0); WriteControlRegister(); 
    WriteStrobeLength(AStrobeBLength);
    if (AReadoutMode == MODE_ALPIDE_READOUT_B)
        SetChipMode(1);
    else if (AReadoutMode == MODE_ALPIDE_READOUT_A)
        SetChipMode(2);
    WriteControlRegister();
}
  
int TpAlpidefs4::WriteFROMUConfig ()
{
    int16_t Value = fMEBMask & 0x7;
    Value |= ((int) fInternalStrobeCont  << 3);
    Value |= ((int) fBusyMonitoring      << 4);
    Value |= ((int) fPulseType           << 5);
    Value |= ((int) fInternalStrobe      << 6);
    Value |= ((int) fRotatePulseLine     << 7);
    Value |= (((    fTriggerDelay & 0x3)     << 8));
    return WriteRegister(0x4, Value);
}
   

void TpAlpidefs4::DecodeStatusFROMU5 (int AValue, int *ABunchCounter, int *AEventMEB, int *AFrameExt)
{
    *ABunchCounter =  AValue & 0xfff;
    *AEventMEB     = (AValue >> 12) & 0x7; 
    *AFrameExt     = (AValue >> 15) & 0x1;
}

int TpAlpidefs4::WriteDTUCfgReg (int PLL_VCO, int PLL_Bandwidth, int PLL_Off, int SerializerPhase, int PLL_Reset)
{
    int Value = ( ( PLL_VCO       & 0x3)         |
                  ((PLL_Bandwidth & 0x1)   << 2) |
                  ((PLL_Off       & 0x1)   << 3) |
                  ((SerializerPhase & 0xf) << 4) |
                  ((PLL_Reset     & 0x1)   << 8));
    return WriteRegister(0x14, Value);
}

int TpAlpidefs4::WriteDTUDACReg (int PLL_ChargePump, int HighSpeedLineDriver, int PreEmphasis )
{
    int Value = ( (PLL_ChargePump      & 0xf)       |
                 ((HighSpeedLineDriver & 0xf) << 4) |
                 ((PreEmphasis         & 0xf) << 8));
    return WriteRegister(0x15, Value);
}

int TpAlpidefs4::WriteADCControlReg(int Mode, int Input, int ComparatorCurrent, int Discriminator, int RampSpeed, int HalfLSB, int ComparatorOutput)
{
    int value = ( (Mode               & 0x3) |
                  ((Input             & 0xf) << 2) |
                  ((ComparatorCurrent & 0x3) << 6) |
                  ((Discriminator     & 0x1) << 8) |
                  ((RampSpeed         & 0x3) << 9) |
                  ((HalfLSB           & 0x1) << 11)|
                  ((ComparatorOutput  & 0x1) << 15));		
    return WriteRegister (0x610, value);
}

int TpAlpidefs4::WriteCMUDMUConfig()
{
    int Value = ( (fPreviousChip & 0xf)        |
                  (fInitialToken         << 4) |
                  (fDisableManchester    << 5) |
                  (fEnableDoubleDataRate << 6) );
    return WriteRegister (0x10, Value);
}

int TpAlpidefs4::GetCMUErrorCounts (int &CodeUnknown, int &Timeout, int &Deserializer)
{
    int Value;
    int err = ReadRegister (0x11, &Value);
    CodeUnknown  = (Value >> 8) & 0xf;
    Timeout      = (Value >> 4) & 0xf;
    Deserializer = Value & 0xf;
    return err;
}

//MASK AND PULSE PIXELS
int TpAlpidefs4::SetMaskAllPixels(bool AMasked,TAlpidePixreg APixReg)
{
    WriteRegister(0x500, (AMasked << 1) | (!APixReg) );
    WriteRegister(0b100 << 8 | 1<<7 | 0x7, 0xFFFF);
    return WriteRegister(0b100 << 8 | 1<<7 | 0x7, 0x0);
}

int TpAlpidefs4::SetMaskSinglePixel(int ACol, int ARow, bool AMasked)
{
    WriteRegister(0x500, AMasked << 1);
    WriteRegister( ((ACol>>5) & (0x1F)) << 11 | 4<<8 | (0x1+((ACol>>4) & 0x1)), 1<<(ACol & 0xF) );
    WriteRegister( (ARow >> 4) << 11 | 4<<8 | 0x4, 1<<(ARow & 0xF) );
    return WriteRegister(0b100 << 8 | 1<<7 | 0x7, 0x0);
}

int TpAlpidefs4::SetInjectSinglePixel(int ACol, int ARow, bool AValue,
                                      TAlpidePulseType APulseType,
                                      bool             ADisableOthers) {
    WriteRegister(0x500, AValue << 1 | 1);
    WriteRegister( ((ACol>>5) & (0x1F)) << 11 | 4<<8 | (0x1+((ACol>>4) & 0x1)), 1<<(ACol & 0xF) );
    WriteRegister( (ARow >> 4) << 11 | 4<<8 | 0x4, 1<<(ARow & 0xF) );
    return WriteRegister(0b100 << 8 | 1<<7 | 0x7, 0x0);
}

// INIT CHIP

int TpAlpidefs4::Init (bool Reset)
{
    int err = 1;
    if (Reset){
        err = SendCommand(0x00D2); // GRST
        if (err != 1) return err;
        err = SendCommand(0x00E4); // PRST
        if (err != 1) return err;
    }
    err = WriteControlRegister();
    if (err != 1) return err;

    err = WriteFROMUConfig();
    if (err != 1) return err;

    SetDefaultDACValues();
    err = WriteAllDACRegisters();
    if (err != 1) return err;
  
    err = SetMaskAllPixels(0, PR_MASK);
    if (err != 1) return err;
    err = SetMaskAllPixels(1, PR_PULSEENABLE);
    if (err != 1) return err;

    err = WriteCMUDMUConfig ();
    if (err != 1) return err; 
    PrintCMUErrors();

    err = SendCommand(OPCODE_RORST);
    if (err != 1) return err;
      
    return err;
}

void TpAlpidefs4::ReadAllRegisters() {
    int value = -1;
    int regs[100];
    int nregs = 0;
    for (int ireg=0; ireg<=0x1b; ++ireg) regs[nregs++] = ireg;
    regs[nregs++] = 0x500;
    for (int ireg=0x600; ireg<=0x612; ++ireg) regs[nregs++] = ireg;
    for (int ireg=0x700; ireg<=0x707; ++ireg) regs[nregs++] = ireg;
    for (int ireg=0; ireg<nregs; ++ireg) {
        ReadRegister(regs[ireg], &value);
        std::cout << "\t0x"  << std::hex << regs[ireg]
                  << ":\t0x" << value << std::dec
                  << "\t0d"  << value << std::endl;
    }
}

int TpAlpidefs4::AddressToColumn(int ARegion, int ADoubleCol, int AAddress)
{
    return ARegion*32 + ADoubleCol*2 + (AAddress % 4 == 1 || AAddress % 4 == 2);
}

int TpAlpidefs4::AddressToRow(int ARegion, int ADoubleCol, int AAddress)
{
    return 2*(AAddress>>2)+((AAddress&0x2)>>1);
}



//DECODE DATA ALPIDE 4

// Warning: the data type is not unambiguous if called with byte of a dataword,
// which is not the most significant byte (in case of DATA SHORT or DATA LONG)
TPalpideDataType TpAlpidefs4::CheckDataType (unsigned char DataWord)
{
  if      (DataWord == 0xff)          return DT_IDLE;
  else if (DataWord == 0xf1)          return DT_BUSYON;
  else if (DataWord == 0xf0)          return DT_BUSYOFF;
  else if ((DataWord & 0xf0) == 0xa0) return DT_CHIPHEADER;
  else if ((DataWord & 0xf0) == 0xb0) return DT_CHIPTRAILER;
  else if ((DataWord & 0xf0) == 0xe0) return DT_EMPTYFRAME;
  else if ((DataWord & 0xe0) == 0xc0) return DT_REGHEADER;
  else if ((DataWord & 0xc0) == 0x40) return DT_DATASHORT;
  else if ((DataWord & 0xc0) == 0x0)  return DT_DATALONG;
  else return DT_UNKNOWN;
}


bool TpAlpidefs4::DecodeRegionHeader (const char Data, int &Region)
{
  int NewRegion = Data & 0x1f;
  if (NewRegion <= Region) {
    std::cout << "Corrupt region header, expected region, found " << NewRegion << " after " << Region << std::endl;
    return false;
  }
  else {
    Region = NewRegion;
    return true;
  }
}


bool TpAlpidefs4::DecodeChipHeader (unsigned char *Data, int &ChipId, unsigned int& BunchCounter)
{
  int16_t header = (((int16_t) Data[0]) << 8) + Data[1];
  if ((CheckDataType(Data[0]) != DT_CHIPHEADER) && (CheckDataType(Data[0]) != DT_EMPTYFRAME)) {
    std::cout << "Error, data word 0x" << std::hex << header << std::dec << " is neither chip header nor empty frame" << std::endl;
    return false;
  }
  ChipId = header & 0xf00;
  //std::cout << "found chip id" << ChipId << std::endl;
  if ((ChipId != (fChipId & 0xf)) && (!fChipMasterFPGASlave)) {
    std::cout << "pALPIDE-4: Error, found wrong chip ID " << ChipId << " in header." << std::endl;
    return false;
  }
  else if (ChipId != (fChipId & 0xf)) {     // Data comes from FPGA MasterSlave emulator
    fSimData = true;
  }
  else {
    fSimData = false;
  }
  BunchCounter  = (header & 0xff);
  return true;
}


bool TpAlpidefs4::DecodeChipTrailer  (unsigned char *Data, unsigned int& StatusBits)
{
  int16_t trailer =  Data[0]; //(((int16_t) Data[0]) << 8) + Data[1];
  if (CheckDataType(Data[0]) != DT_CHIPTRAILER) {
    std::cout << "Error, data word 0x" << std::hex << trailer << std::dec << " is no chip trailer" << std::endl;
    return false;
  }

  if (trailer & 0xf) {
    std::cout << "Warning, readout flags not 0. Found " << (trailer & 0x8 ? "BUSY_VIOLATION ":"")
              << (trailer & 0x4 ? "FLUSHED_INCOMPLETE ":"")
              << (trailer & 0x2 ? "STROBE EXTENDED ":"")
              << (trailer & 0x1 ? "BUSY_TRANSITION":"")
              << std::endl;
  }
  StatusBits = trailer & 0xf;

  return true;
}


bool TpAlpidefs4::DecodeDataWord     (unsigned char *Data, int Region, std::vector<TPixHit> *Hits, int NOldHits, bool Long)
{
  TPixHit Hit;
  int     ClusterSize;
  int     DoubleCol;
  int     Pixel;

  int16_t data_field = (((int16_t) Data[0]) << 8) + Data[1];


  DoubleCol   = (data_field & 0x3c00) >> 10;
  Pixel       = (data_field & 0x03ff);

  if (Long) {
    ClusterSize = 7;
  }
  else {
    ClusterSize = 0;
  }

  //      ClusterSize = (Data[2] & 0xc0) >> 6;

  for (int i = 0; i <= ClusterSize; i ++) {
    if ((i > 0) && ((Data[2] >> (i-1)) & 0x1)) continue;   // always add the first hit, any further hit only if corresp. hit pattern bit = 1
    Hit.doublecol = DoubleCol;
    Hit.region    = Region;
    Hit.address   = Pixel + i;
#ifdef MYDEBUG
    std::cout << "  Hit Data: Region " << Region << ", Dataword 0x" << std::hex << Data << std::dec << ", DoubleCol " << DoubleCol << ", Pixel " << Hit.address << std::endl;
#endif
    if ((Hits->size() > NOldHits) && (Hit == Hits->at(Hits->size()-1))) {
      std::cout << "Warning, pixel " << Hit.region << "/" <<Hit.doublecol << "/" << Hit.address  << " found twice" << std::endl;
      AddStuckPixel(Hit);
    }
    if ((Hits->size() > NOldHits) && (Hit < Hits->at(Hits->size()-1))) std::cout << "Warning, pixel " << Hit.region << "/" <<Hit.doublecol << "/" << Hit.address  << " wrong ordering. previous: " << Hits->at(Hits->size()-1).region << "/" <<Hits->at(Hits->size()-1).doublecol << "/" << Hits->at(Hits->size()-1).address << std::endl;
    if (fSimData) {
      std::cout << "found simulated data" << std::endl;
      WriteHitToFile(gEmulatedDataFile, Hit);
    }
    else {
      Hits->push_back(Hit);
    }
  }
  return true;

}

bool TpAlpidefs4::DecodeEmptyFrame (unsigned char *data, int &chipId, unsigned int &bunchCounter) {
  int16_t data_field = (((int16_t) data[0]) << 8) + data[1];

  bunchCounter = data_field & 0xff;
  chipId       = (data_field >> 8) & 0xf;
  return true;
}

bool TpAlpidefs4::DecodeEvent (unsigned char        *evtData,
                               int                   maxLength,
                               std::vector<TPixHit>  *Hits,
                               unsigned int* StrobeCounter /*=0x0*/,
                               unsigned int* BunchCounter /*=0x0*/,
                               unsigned int* StatusBits /*=0x0*/)
{
  int       byte    = 0;
  int       region  = -1;
  int       chip    = 0;
  bool      started = false; // event has started, i.e. chip header has been found
  bool      finished = false; // event trailer found
  TPalpideDataType type;
  int       NOldHits = (Hits) ? Hits->size() : -1;

  unsigned int BunchCounterTmp;
  unsigned int StatusBitsTmp;
  // Have to revert byte order first?

  while (byte < maxLength) {
    type = CheckDataType (evtData[byte]);
    switch (type) {
    case DT_IDLE:
      byte +=1;
      break;
    case DT_BUSYON:
      byte += 1;
      break;
    case DT_BUSYOFF:
      byte += 1;
      break;
    case DT_COMMA:
      byte += 1;
      break;
    case DT_EMPTYFRAME:
      started = true;
      if (!DecodeEmptyFrame (evtData + byte,chip, BunchCounterTmp)) return false;
      byte += 2;
      if (BunchCounter)  *BunchCounter  = BunchCounterTmp;
      //byte += 2;
      finished = true;
      break;
    case DT_CHIPHEADER:
      started = true;
      if (!DecodeChipHeader (evtData + byte, chip, BunchCounterTmp)) return false;
      byte += 2;
      if (BunchCounter)  *BunchCounter  = BunchCounterTmp;
      break;
    case DT_CHIPTRAILER:
      if (!started) {
        std::cout << "Error, chip trailer found before chip header" << std::endl;
        return false;
      }
      if (finished) {
        std::cout << "Error, chip trailer found after event was finished" << std::endl;
        return false;
      }
      if (!DecodeChipTrailer (evtData + byte, StatusBitsTmp)) return false;
      if (StatusBits) *StatusBits = StatusBitsTmp;
      finished = true;
      byte += 1;
      break;
    case DT_REGHEADER:
      if (!started) {
        std::cout << "Error, region header found before chip header or after chip trailer" << std::endl;
        return false;
      }
      if (!DecodeRegionHeader (evtData[byte], region)) return false;
      byte +=1;
      break;
    case DT_DATASHORT:
      if (!started) {
        std::cout << "Error, hit data found before chip header or after chip trailer" << std::endl;
        return false;
      }
      if (Hits) {
        if (!DecodeDataWord (evtData + byte, region, Hits, NOldHits, false)) return false;
      }
      byte += 2;
      break;
    case DT_DATALONG:
      if (!started) {
        std::cout << "Error, hit data found before chip header or after chip trailer" << std::endl;
        return false;
      }
      if (Hits) {
        if (!DecodeDataWord (evtData + byte, region, Hits, NOldHits, true)) return false;
      }
      byte += 3;
      break;
    case DT_UNKNOWN:
      std::cout << "Error, data of unknown type 0x" << std::hex << evtData[byte] << std::dec << std::endl;
      return false;
    }
  }
  //std::cout << "Found " << Hits->size() - NOldHits << " hits" << std::endl;
  if (started && finished) return true;
  else {
    if (started && !finished) {
      std::cout << "Warning, event not finished at end of data" << std::endl;
    }
    if (!started) {
      std::cout << "Warning, event not started at end of data" << std::endl;
    }
    return false;
  }
}

