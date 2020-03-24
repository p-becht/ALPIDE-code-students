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
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"
#include "TLegend.h"

using namespace std;

// data handling tools
vector<Int_t> read_int_vals(ifstream &f);
vector<Float_t> read_float_vals(ifstream &f);

//// create root files with graphs for different settings
Bool_t create_threshold_graphs_vsITHR(TString path_data, TString run_list_name);
Bool_t create_threshold_graphs_vsTemp(TString path_data, TString run_list_name);


//// functions to compare chip behaviour dependiing on settings/conditions; chip by chip
// vsITHR graph comparisons
//Bool_t compare_vsITHR_temperature(TString path_data, TString chip_name, Float_t irrad_level, Float_t vbb=0, Int_t vcasn=57, Int_t sec=2);

// vsTemp graph comparisons
//Bool_t compare_vsTemp_ITHR(TString path_data, TString chip_name, Float_t irrad_level, Float_t vbb=0, Int_t vcasn=57t, sec=2);
//Bool_t compare_vsTemp_VBB(TString path_data, TString chip_name, Float_t irrad_level, Int_t ITHR=51, Int_t sec=2);
//Bool_t compare_vsTemp_Chips(TString path_data, Float_t irrad_level, Float_t vbb=0, Int_t vcasn=57, Int_t ithr=51, Int_t sec=2);
Bool_t compare_vsTemp_Irrad(TString path_data, Float_t vbb=0, Int_t vcasn=57, Int_t ithr=51, Int_t sec=2, TString compare_select="thres");


// plotting function
Bool_t plot_graphs(vector<TGraph*> v_graps, Float_t y_min, Float_t y_max,  TString y_title, TString x_title, TString legend_title, TString legend_subtitle, vector<TString> v_legend_str, TString chip_name, TString plot_name, Bool_t set_color=kTRUE);

