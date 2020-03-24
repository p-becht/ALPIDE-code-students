#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TStyle.h"
#include "TColor.h"
#include "TString.h"
#include "TFile.h"
#include "TF1.h"
#include "TColor.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TLegend.h"

using namespace std;

// data handling tools
vector<Int_t> read_int_vals(ifstream &f);
vector<Float_t> read_float_vals(ifstream &f);

// check if setting available
Bool_t is_setting_available(TString path_results, TString chip_name, Int_t temperature, Float_t irrad_level, Float_t vbb, Int_t vcasn, Int_t ithr, Int_t rate, Int_t busy);


// functions to compare chip behaviour dependiing on settings/conditions; chip by chip
Bool_t compare_settings(TString path_data, TString run_list_name, TString chip_name, Int_t temperature, Float_t irrad_level, TString param_to_compare="ITHR", Int_t param1=0, Int_t param2=57, Int_t rate=10000, Int_t busy=50, Int_t sec=2);

Bool_t compare_vbb(TString path_data, TString run_list_name, TString chipname, Int_t temperature, Float_t irrad_level, Int_t ithr=51, Int_t rate=10000, Int_t busy=50, Int_t sec=2);

Bool_t compare_temperature(TString path_data, TString run_list_name, TString chip_name, Float_t irrad_level, Float_t vbb=0, Int_t vcasn=57, Int_t ithr=51, Int_t rate=10000, Int_t busy=50, Int_t sec=2);

// functions to compare different chips or irradiation levels
Bool_t compare_chips(TString path_data, TString run_list_name, Int_t temperature, Float_t irrad_level, Float_t vbb=0, Int_t vcasn=57, Int_t ithr=51, Int_t rate=10000, Int_t busy=50, Int_t sec=2);

Bool_t compare_irradiation(TString path_data, TString run_list_name, Int_t temperature, Float_t vbb=0, Int_t vcasn=57, Int_t ithr=51, Int_t rate=10000, Int_t busy=50, Int_t sec=2);


// plotting function
Bool_t plot_hists(vector<TH1F*> v_hists, Float_t y_min, Float_t y_max,  TString y_title, TString x_title, TString legend_title, TString legend_subtitle, vector<TString> v_legend_str, TString chip_name, TString plot_name, Bool_t set_color=kTRUE);



