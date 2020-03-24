/* Written by Miljenko Suljic, m.suljic@cern.ch */

#include <iostream>
#include <fstream>

#include <TStyle.h>
#include <TColor.h>
#include <TH1.h>
#include <TH2.h>

#include "helpers.h"

//_______________________________________________________________________________________________
void dblcol_adr_to_col_row(UShort_t doublecol, UShort_t address, UShort_t &col, UShort_t &row) {
    col = doublecol*2 + (address%4 < 2 ? 1 : 0);
    row = 2*(address/4) + 1-(address%2);
}

//_______________________________________________________________________________________________
UShort_t dblcol_adr_to_col(UShort_t doublecol, UShort_t address) {
    UShort_t col, row;
    dblcol_adr_to_col_row(doublecol, address, col, row);
    return col;
}

//_______________________________________________________________________________________________
UShort_t dblcol_adr_to_row(UShort_t doublecol, UShort_t address) {
    UShort_t col, row;
    dblcol_adr_to_col_row(doublecol, address, col, row);
    return row;
}

//_______________________________________________________________________________________________
void zoom_th1(TH1* h, Int_t offset) {
    Int_t x1 = h->FindFirstBinAbove(0, 1);
    Int_t x2 = h->FindLastBinAbove(0, 1);
    if( x1 != -1 && x2 != -1 ) {
        h->GetXaxis()->SetRange(x1-offset, x2+offset);
    }

}

//_______________________________________________________________________________________________
void zoom_th2(TH2* h, Int_t offset) {
    Int_t x1 = h->FindFirstBinAbove(0, 1);
    Int_t x2 = h->FindLastBinAbove(0, 1);
    Int_t y1 = h->FindFirstBinAbove(0, 2);
    Int_t y2 = h->FindLastBinAbove(0, 2);
    if( x1 != -1 && x2 != -1 && y1 != -1 && y2 != -1 ) {
        h->GetXaxis()->SetRange(x1-offset, x2+offset);
        h->GetYaxis()->SetRange(y1-offset, y2+offset);
    }

}

//_______________________________________________________________________________________________
void reset_meas_config(MeasConfig_t *conf) {
    conf->VCASN     = -1;
    conf->VCASP     = -1;
    conf->ITHR      = -1;
    conf->IDB       = -1;
    conf->IBIAS     = -1;
    conf->IRESET    = -1;
    conf->VRESET    = -1;
    conf->VAUX      = -1;
    conf->VRESETP   = -1;
    conf->VRESETD   = -1;
    conf->VCASN2    = -1;
    conf->VCLIP     = -1;
    // temp
    conf->Temp      = -1.;
    // stuff for rate measurements
    conf->RATE      = -1;
    conf->BUSY      = -1;
    // chiller temperature
    conf->TEMP_SET  = -1;
    // VBB
    conf->VBB       = -1;
}


//_______________________________________________________________________________________________
void read_int_parameter (const char *line, int *address, bool expect_rest) {
    char param[128];
    char rest[896];
    if (!expect_rest) {
        sscanf (line,"%s\t%d", param, address);
        //std::cout << "found parameter " << param << ", value " << *address << std::endl;
    }
    else {
        sscanf (line,"%s\t%d\t%s", param, address, rest);
        //std::cout << "found parameter " << param << ", value " << *address << std::endl;
    }
}


//_______________________________________________________________________________________________
void read_float_parameter (const char *line, float *address, bool expect_rest) {
    char param[128];
    char rest[896];
    if (!expect_rest) {
        sscanf (line,"%s\t%f", param, address);
        //std::cout << "found parameter " << param << ", value " << *address << std::endl;
    }
    else {
        sscanf (line,"%s\t%f%s", param, address, rest);
        //std::cout << "found parameter " << param << ", value " << *address << std::endl;
    }
}


//_______________________________________________________________________________________________
void decode_line(const char *line, MeasConfig_t *conf) {
    char param[128], rest[896];
    if ((line[0] == '\n') || (line[0] == '#')) {   // empty Line or comment
        return;
    }
    sscanf (line,"%s\t%s", param, rest);

    if (!strcmp(param,"VAUX")) {
        read_int_parameter(line, &(conf->VAUX));
    }
    if (!strcmp(param,"VRESET")) {
        read_int_parameter(line, &(conf->VRESET));
    }
    if (!strcmp(param,"VCASN")) {
        read_int_parameter(line, &(conf->VCASN));
    }
    if (!strcmp(param,"VCASP")) {
        read_int_parameter(line, &(conf->VCASP));
    }
    if (!strcmp(param,"IRESET")) {
        read_int_parameter(line, &(conf->IRESET));
    }
    if (!strcmp(param,"IBIAS")) {
        read_int_parameter(line, &(conf->IBIAS));
    }
    if (!strcmp(param,"IDB")) {
        read_int_parameter(line, &(conf->IDB));
    }
    if (!strcmp(param,"ITHR")) {
        read_int_parameter(line, &(conf->ITHR));
    }
    if (!strcmp(param,"VRESETP")) {
        read_int_parameter(line, &(conf->VRESETP));
    }
    if (!strcmp(param,"VRESETD")) {
        read_int_parameter(line, &(conf->VRESETD));
    }
    if (!strcmp(param,"VCASN2")) {
        read_int_parameter(line, &(conf->VCASN2));
    }
    if (!strcmp(param,"VCLIP")) {
        read_int_parameter(line, &(conf->VCLIP));
    }


    if (!strcmp(param,"RATE:")) {
        read_int_parameter(line, &(conf->RATE), true);
    }
    if (!strcmp(param,"BUSY:")) {
        read_int_parameter(line, &(conf->BUSY), true);
    }

    if (!strcmp(param,"TEMP:")) {
        read_int_parameter(line, &(conf->TEMP_SET));
    }
    if (!strcmp(param,"VBB")) {
        read_float_parameter(line, &(conf->VBB));
    }

    if (!strcmp(param,"TempI")) {
        read_float_parameter(line, &(conf->Temp));
    }
}


//_______________________________________________________________________________________________
void print_meas_config(MeasConfig_t conf) {
    std::cout << "VCASN:    " << conf.VCASN     << std::endl; 
    std::cout << "VCASP:    " << conf.VCASP     << std::endl; 
    std::cout << "ITHR:     " << conf.ITHR      << std::endl; 
    std::cout << "IDB:      " << conf.IDB       << std::endl; 
    std::cout << "IBIAS:    " << conf.IBIAS     << std::endl; 
    std::cout << "IRESET:   " << conf.IRESET    << std::endl; 
    std::cout << "VRESET:   " << conf.VRESET    << std::endl; 
    std::cout << "VAUX:     " << conf.VAUX      << std::endl; 
    std::cout << "VCASN2:   " << conf.VCASN2    << std::endl; 
    std::cout << "VCLIP:    " << conf.VCLIP     << std::endl; 
    std::cout << "VRESETP:  " << conf.VRESETP   << std::endl; 
    std::cout << "VRESETD:  " << conf.VRESETD   << std::endl; 
    std::cout << std::endl;
    std::cout << "Temp:     " << conf.Temp      << std::endl; 
    std::cout << std::endl;
    std::cout << "RATE:     " << conf.RATE      << std::endl; 
    std::cout << "BUSY:     " << conf.BUSY      << std::endl; 
    std::cout << std::endl;
    std::cout << "TEMP SET: " << conf.TEMP_SET  << std::endl; 
    std::cout << "VBB:      " << conf.VBB       << std::endl; 
    std::cout << std::endl;
}


//_______________________________________________________________________________________________
MeasConfig_t read_config_file (const char *fName) {
    char line[1024];
    FILE *fp = fopen (fName, "r");
    MeasConfig_t conf;
    reset_meas_config(&conf);

    if (!fp) {
        std::cout << "WARNING: Config file " << fName << " not found" << std::endl;
        return conf;
    }

    // process the file
    while (fgets(line, 1023, fp) != NULL) {
        decode_line(line, &conf);
    }
    
    //print_meas_config(conf);
    return conf;
}


//_______________________________________________________________________________________________
void set_my_style() {
    const Int_t NRGBs = 5;
    const Int_t NCont = 255;
    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);
    gStyle->SetOptFit(1111);
}


void helpers() {
    printf("Helpers loaded.\n");
}
