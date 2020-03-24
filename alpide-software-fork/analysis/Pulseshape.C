#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include <sstream>

#include "TCanvas.h"
#include "TH2F.h"
#include "TStyle.h"
#include "TColor.h"
#include "TString.h"
#include "TFile.h"

#include "../analysis/runLog_PulselengthScans.h"

using namespace std;

// function for finding last line of file
string findLastLine(ifstream &infile) {
        infile.seekg(-2, ios_base::end);                // move safely before end of file, not to catch newline
        bool keepLooping = true;
        while(keepLooping) {
            char ch;
            infile.get(ch);                            // Get current byte's data

            if((int)infile.tellg() <= 1) {             // If the data was at or before the 0th byte
                infile.seekg(0);                       // The first line is the last line
                keepLooping = false;                // So stop there
            }
            else if(ch == '\n') {                   // If the data was a newline
                keepLooping = false;                // Stop at the current position.
            }
            else {                                  // If the data was neither a newline nor at the 0 byte
                infile.seekg(-2,ios_base::cur);        // Move to the front of that data, then to the front of the data before it
            }
        }

        string lastLine;            
        getline(infile,lastLine);                      // Read the current line
        //cout << "Result: " << lastLine << '\n';     // Display it

        infile.seekg(0, ios_base::beg);
        return lastLine; 
}

// function for finding first line of file
string findFirstLine(ifstream &infile) {
        infile.seekg(0, ios_base::beg);

        string firstLine;            
        getline(infile,firstLine);                      // Read the current line
        //cout << "Result: " << firstLine << '\n';     // Display it

        infile.seekg(0, ios_base::beg);
        return firstLine; 
}

// function for finding start value for charge
Int_t getStartCharge(ifstream &infile) {
    Int_t posCharge = 0;
    Int_t startCharge = 0;
    istringstream iss(findFirstLine(infile));

    for (Int_t i_pos=0; i_pos<posCharge+1; i_pos++) {  
        iss >> startCharge; 
    }
    return startCharge;
}

// function for finding end value for charge
Int_t getEndCharge(ifstream &infile) {
    Int_t posCharge = 0;
    Int_t endCharge = 0;
    istringstream iss(findLastLine(infile));

    for (Int_t i_pos=0; i_pos<posCharge+1; i_pos++) {  
        iss >> endCharge; 
    }
    return endCharge;
}

// function for finding begin value for delay
Int_t getStartDelay(ifstream &infile) {
    Int_t posDelay = 1;
    Int_t startDelay = 0;
    istringstream iss(findFirstLine(infile));

    for (Int_t i_pos=0; i_pos<posDelay+1; i_pos++) {  
        iss >> startDelay;
    }
    return startDelay;
}

// function for finding end value for delay
Int_t getEndDelay(ifstream &infile) {
    Int_t posDelay = 1;
    Int_t endDelay = 0;
    istringstream iss(findLastLine(infile));

    for (Int_t i_pos=0; i_pos<posDelay+1; i_pos++) {  
        iss >> endDelay; 
    }
    return endDelay;
}

// function for determining step value for charge
Int_t getStepCharge(ifstream &infile) {
    Int_t posCharge = 0;
    Int_t startCharge = getStartCharge(infile);
    Int_t charge = startCharge; 
    string line;

    while (charge == startCharge) {
        getline(infile, line);
        istringstream iss(line);
        for (Int_t i_pos=0; i_pos<posCharge+1; i_pos++) {  
            iss >> charge; 
        }
    }
    Int_t chargeStep = charge - startCharge; 
    infile.seekg(0, ios_base::beg);
   
    return chargeStep;
}

// function for determining step value for delay
Int_t getStepDelay(ifstream &infile) {
    Int_t posDelay = 1;
    Int_t startDelay = getStartDelay(infile);
    Int_t delay = startDelay; 
    string line;

    while (delay == startDelay) {
        getline(infile, line);
        istringstream iss(line);
        for (Int_t i_pos=0; i_pos<posDelay+1; i_pos++) {  
            iss >> delay; 
        }
    }
    Int_t delayStep = delay - startDelay; 
    infile.seekg(0, ios_base::beg);
   
    return delayStep;
}


// main macro
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Bool_t Pulseshape(Int_t Run_num=0, Bool_t Do_plot=kFALSE, Int_t Reg_sel=5) {
    if (Run_num > NUM_RUNS-1) {
        cout << "run number too large, please check!!!" << endl;
        return kFALSE;
    }
    // read info from runLog file  
    TString fFileID = "";
    if (RUN_INFO[Run_num][1]<1000) {
        fFileID = Form("%i_0%i", RUN_INFO[Run_num][0], RUN_INFO[Run_num][1]);
    }
    else {
        fFileID = Form("%i_%i", RUN_INFO[Run_num][0], RUN_INFO[Run_num][1]);
    }
    TString fName = Form("../pALPIDEfs-software/Data/PulselengthScan_%s.dat", fFileID.Data());
    cout << fName << endl;
    Int_t DCol_sel = RUN_INFO[Run_num][2];
    Int_t Addr_sel = RUN_INFO[Run_num][3];
    Int_t VCasn = RUN_INFO[Run_num][4];
    Int_t Ithr = RUN_INFO[Run_num][5];
    Int_t Strobe_B_length = RUN_INFO[Run_num][6];
    Int_t Vbias = RUN_INFO[Run_num][7];
    //Int_t IDB = RUN_INFO[Run_num][8];
    Int_t n_inj = 50;
    Int_t Charge, Delay, Reg, DCol, Addr;
    //Float_t clk_dur = 1;
    Float_t clk_dur = 0.0125; // 80MHz clock

    // read measurement parameters from input file
    Int_t start_charge = 0;
    Int_t end_charge = 0;
    Int_t step_charge = 0;
    Int_t range_charge = 0;
    Int_t n_bins_charge = 0;
    Int_t start_delay = 0;
    Int_t end_delay = 0;
    Int_t step_delay = 0;
    Int_t range_delay = 0; // number of values scanned
    Int_t n_bins_delay = 0;
    ifstream infile(fName.Data());
    if (infile) {
        TString firstline = findFirstLine(infile);
        TString lastline = findLastLine(infile);

        start_charge = getStartCharge(infile);
        cout << "start charge value: " << start_charge << endl;
        end_charge = getEndCharge(infile);
        cout << "end charge value: " << end_charge << endl;
        step_charge = getStepCharge(infile);
        cout << "step charge: " << step_charge << endl;
        range_charge = end_charge-start_charge+1;
        n_bins_charge = (range_charge-1)/step_charge+1;

        start_delay = getStartDelay(infile);
        //cout << "start delay value: " << start_delay << endl;
        end_delay = getEndDelay(infile);
        //cout << "end delay value: " << end_delay << endl;
        step_delay = getStepDelay(infile);
        //cout << "step delay: " << step_delay << endl;
        range_delay = end_delay-start_delay+1; // number of values scanned
        n_bins_delay = (range_delay-1)/step_delay+1;
    }
    else { 
        cout << "cannot open file!!!" << endl;
        return kFALSE; 
    }

    // histograms for all 32 regions
    TH2F *hPulse[32];
    Float_t TimeDelay = 0;
    for (Int_t i_reg=0; i_reg<32; i_reg++) {
        hPulse[i_reg] = new TH2F(Form("hPulse_%i", i_reg), 
                Form("Pulse Shape, pixel %i/%i/%i, IThr %i, VCasn %i, Vbias -%i.0V, StrBlen %ins", i_reg, DCol_sel, Addr_sel, Ithr, VCasn, Vbias, Strobe_B_length),
                n_bins_delay, (start_delay+4)*clk_dur-clk_dur*step_delay*0.5, (end_delay+4)*clk_dur+clk_dur*step_delay*0.5, 
                n_bins_charge, start_charge-step_charge*0.5, end_charge+step_charge*0.5);
    }
    //TAxis *ax = (TAxis*)hPulse[Reg_sel]->GetXaxis();
    //cout << ax->GetBinLowEdge(1) << endl;
    //cout << ax->GetBinLowEdge(10) << endl;
    //TAxis *ax = (TAxis*)hPulse[Reg_sel]->GetYaxis();
    //cout << ax->GetBinLowEdge(1) << endl;
    //cout << ax->GetBinLowEdge(10) << endl;
        
    // read file and fill histogram
    while (infile >> Charge >> Delay >> Reg >> DCol >> Addr) {
        if ((DCol == DCol_sel) && (Addr == Addr_sel)) {
            TimeDelay = Delay + 4; // add 50ns offset
            TimeDelay *= clk_dur;
            hPulse[Reg]->Fill(TimeDelay, Charge);
        }
    }
    infile.close();
    // normalizing bin contents for efficiency
    for (Int_t i_reg=0; i_reg<32; i_reg++) {
        for (Int_t i_col=1; i_col<=n_bins_delay; i_col++) {
            for (Int_t i_row=1; i_row<=n_bins_charge; i_row++) {
                 hPulse[i_reg]->SetBinContent(i_col, i_row, hPulse[i_reg]->GetBinContent(i_col, i_row)/n_inj);
            }
        }
    }
    
    // drawing
    TCanvas *c = new TCanvas("c", "c", 1200, 700);
    c->cd();
    c->cd()->SetGrid();
    gStyle->SetOptStat(0);
    //define new nice color palette:
    const Int_t NRGBs = 5;
    const Int_t NCont = 75;
    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]     = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]    = { 0.51, 1.00, 0.12, 0.00, 0.00 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);

    // write result histograms to file
    TFile *out_file = new TFile(Form("../pALPIDEfs-software/Data/Results/PulseShape_%s_pix_%i_%i_ITH%i_VCASN%i_VBIAS-%i.0V_StrBLen%ins.root", 
            fFileID.Data(), DCol_sel, Addr_sel, Ithr, VCasn, Vbias, Strobe_B_length), "RECREATE");
    out_file->cd();

    for (Int_t i_reg=0; i_reg<32; i_reg++) {
        c->Clear();
        hPulse[i_reg]->Draw("COLZ");
        hPulse[i_reg]->GetXaxis()->SetTitle("Delay [#mus]");
        hPulse[i_reg]->GetYaxis()->SetTitle("Charge [DAC]");
        if (Reg_sel==i_reg && Do_plot) {
            c->Print(Form("../pALPIDEfs-software/Data/Plots/PulseShape_%s_pix_%i_%i_%i_ITH%i_VCASN%i_VBIAS-%i.0V_StrBLen%ins.pdf", 
                    fFileID.Data(), Reg_sel, DCol_sel, Addr_sel, Ithr, VCasn, Vbias, Strobe_B_length));
            c->Print(Form("../pALPIDEfs-software/Data/Plots/PulseShape_%s_pix_%i_%i_%i_ITH%i_VCASN%i_VBIAS-%i.0V_StrBLen%ins.png", 
                    fFileID.Data(), Reg_sel, DCol_sel, Addr_sel, Ithr, VCasn, Vbias, Strobe_B_length));
        }
        hPulse[i_reg]->Write();
    }
    out_file->Close();

    return kTRUE;
}


