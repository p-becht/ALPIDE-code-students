//_______________________________________________________________________________________________
// stuff by Miko

// Written by Miljenko Suljic, m.suljic@cern.ch
void     dblcol_adr_to_col_row(UShort_t double_col, UShort_t address, UShort_t &col, UShort_t &row);
UShort_t dblcol_adr_to_col(UShort_t double_col, UShort_t address);
UShort_t dblcol_adr_to_row(UShort_t double_col, UShort_t address);
void     zoom_th1(TH1* h, Int_t offset=5);
void     zoom_th2(TH2* h, Int_t offset=10);


//_______________________________________________________________________________________________
// config file processing by jacobus, 19.3.2015, similar to what can be found in TConfig.cpp
struct MeasConfig_t {
    // bias config
    Int_t VCASN;
    Int_t VCASP;
    Int_t ITHR;
    Int_t IDB;
    Int_t IBIAS;
    Int_t IRESET;
    Int_t VRESET;
    Int_t VAUX;
    Int_t VRESETP;
    Int_t VRESETD;
    Int_t VCASN2;
    Int_t VCLIP;
    // temp
    Float_t Temp;
    // chiller temperature
    Int_t TEMP_SET;
    // stuff for rate measurements
    Int_t RATE;
    Int_t BUSY;
    // vbb
    Float_t VBB;
};


void read_float_parameter (const char *line, float *address, bool expect_rest=false);
void read_int_parameter (const char *line, int *address, bool expect_rest=false);
void decode_line(const char *line, MeasConfig_t *conf); 
void print_meas_config(MeasConfig_t conf);
void reset_meas_config(MeasConfig_t *conf);
MeasConfig_t read_config_file (const char *fName);


void     set_my_style();
void     helpers();
