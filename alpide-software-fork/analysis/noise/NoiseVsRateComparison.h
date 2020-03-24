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
vector<Int_t> read_settings(ifstream &f);
vector<Float_t> read_vbb_vals(ifstream &f);
Bool_t create_graphs(TString path_results="", Int_t n_excl_pix=20);
Bool_t compare_graphs(TString path_results="", TString type="", Float_t vbb=0.0, Int_t vcasn=57, Int_t ithr=51, Int_t sec=2, Int_t n_excl_pix=20, Bool_t save_plot=kFALSE);
Bool_t plot_graphs(vector<TGraph*> v_graphs, Float_t y_min, Float_t y_max, TString y_title, TString x_title, TString legent_tile, vector<TString> v_legend_str, Int_t n_excl_pix=20, TString chip_name="pALPIDE-1", TString plot_name="");



