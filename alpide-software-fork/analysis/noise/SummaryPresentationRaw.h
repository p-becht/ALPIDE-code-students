#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <unistd.h>

#include "TString.h"

// presentation creation tools
#include "SummaryPresentationsHelpers.h"


// comparsion summary creation macros
Bool_t create_summary_RateBusy(TString path_results, Int_t sec, Int_t n_excl_pix);

Bool_t create_summary_Chips(TString path_results, Int_t sec, Int_t comparison_select=0);
Bool_t create_summary_VBB(TString path_results, Int_t sec, Int_t comparison_select=0);
Bool_t create_summary_ITHR(TString path_results, Int_t sec, Int_t comparison_select=0);
Bool_t create_summary_Temp(TString path_results, Int_t sec, Int_t comparison_select=0);
Bool_t create_summary_Irrad(TString path_results, Int_t sec, Int_t comparison_select=0);


