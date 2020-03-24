#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include <sstream>

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

#include "runLog_PulselengthScans.h"

using namespace std;


struct MaxPulseLengthBin_t {
    Float_t Time;
    Float_t Charge;
    Int_t BinX;
    Int_t BinY;
};

struct MinThresholdBin_t {
    Float_t Time;
    Float_t Charge;
    Int_t BinX;
    Int_t BinY;
};

struct MinThreshold_t {
    Float_t Time;
    Float_t Threshold;
    Float_t Noise;
};

struct FitRange_t {
    Int_t BinLow;
    Int_t BinHigh;
};

// get fast extimate for maximum pulse length location 
MaxPulseLengthBin_t getMaxPulseLengthFast(TH2F *h2) {
    MaxPulseLengthBin_t maxpl;
    maxpl.Time   = -1.;
    maxpl.Charge = -1.;
    maxpl.BinX   = -1;
    maxpl.BinY   = -1;
    Int_t n_bins_x = h2->GetXaxis()->GetNbins();
    Int_t n_bins_y = h2->GetYaxis()->GetNbins();
    // search for bin at highest delay with content==1 -> max pulse length
    for (Int_t i_col=n_bins_x-1; i_col>=0; i_col--) {
        for (Int_t i_row=0; i_row<n_bins_y-15; i_row++) {
            if (h2->GetBinContent(i_col+1, i_row+1)>0.999) {
                maxpl.Time   = h2->GetXaxis()->GetBinCenter(i_col+1); 
                maxpl.Charge = h2->GetYaxis()->GetBinCenter(i_row+1);
                maxpl.BinX   = i_col+1;
                maxpl.BinY   = i_row+1;
                //cout << i_col+1 << "\t" << i_row+1 << endl;
                return maxpl;
            }
        }
    }
    // no bin with content==1 found
    return maxpl; 
}

// get fast extimate for minimum threshold location 
MinThresholdBin_t getMinThresholdFast(TH2F *h2) {
    MinThresholdBin_t minth;
    minth.Time   = -1.;
    minth.Charge = -1.;
    minth.BinX   = -1;
    minth.BinY   = -1;
    Int_t n_bins_x = h2->GetXaxis()->GetNbins();
    Int_t n_bins_y = h2->GetYaxis()->GetNbins();

    // search for bin at highest delay with content==1 -> min threshold
    for (Int_t i_row=0; i_row<n_bins_y; i_row++) {
        for (Int_t i_col=0; i_col<n_bins_x; i_col++) {
            if (h2->GetBinContent(i_col+1, i_row+1)>0.999) {
                minth.Time   = h2->GetXaxis()->GetBinCenter(i_col+1); 
                minth.Charge = h2->GetYaxis()->GetBinCenter(i_row+1);
                minth.BinX   = i_col+1;
                minth.BinY   = i_row+1;
                //cout << i_col+1 << "\t" << i_row+1 << endl;
                return minth;
            }
        }
    }
    // no bin with content==1 found
    return minth; 
}

// get range for fitting hists for finding min threshold
FitRange_t getFitRange(TH2F *h2) {
    FitRange_t fr;
    fr.BinLow = -1;
    fr.BinHigh = -1;
    Int_t n_bins_x = h2->GetXaxis()->GetNbins();

    // fit range should be columns where row < [row(max_pulse_length) - row(min_threshold)]/2 + row(min_threshold)
    Int_t row_maxpl = getMaxPulseLengthFast(h2).BinY;
    Int_t row_minth = getMinThresholdFast(h2).BinY;
    // row in center of row_minth and row_maxpl
    Int_t row_center = row_minth + (row_maxpl - row_minth)/2;
    //cout << row_center << "\t" << h2->GetYaxis()->GetBinCenter(row_center) << endl;

    // low edge
    for (Int_t i_col=0; i_col<n_bins_x; i_col++) {
        if (h2->GetBinContent(i_col+1, row_center)>0.999) {
            fr.BinLow = i_col+1;
            //cout << i_col+1 << "\t" << h2->GetXaxis()->GetBinCenter(i_col+1) << endl;
            break;
        }
    }
    // high edge
    for (Int_t i_col=n_bins_x-1; i_col>=0; i_col--) {
        if (h2->GetBinContent(i_col+1, row_center)>0.999) {
            fr.BinHigh = i_col+1;
            //cout << i_col+1 << "\t" << h2->GetXaxis()->GetBinCenter(i_col+1) << endl;
            break;
        }
    }

    return fr;
}

// get minimum threshold
MinThreshold_t getMinThreshold(TH2F *h2) {
    MinThreshold_t minth;
    minth.Time   = -1.;
    minth.Threshold = -1.;
    minth.Noise = -1.;
    FitRange_t fr = getFitRange(h2);
    MaxPulseLengthBin_t maxpl = getMaxPulseLengthFast(h2);

    gStyle->SetOptStat(111111);
    TH1F *h_th = new TH1F(Form("%s_th", h2->GetName()), Form("%s_th", h2->GetTitle()), 100, 0, maxpl.Charge);
    TH1F *h_th_noise = new TH1F(Form("%s_th_noise", h2->GetName()), Form("%s_th_noise", h2->GetTitle()), 100, 0, 10);
    TH1F *h_th_chi2 = new TH1F(Form("%s_th_chi2", h2->GetName()), Form("%s_th_chi2", h2->GetTitle()), 100, 0, 0.10);
    // histograms for thresholds and noise
    TH1D  *h1_threshold = (TH1D*)h2->ProjectionX("h1_threshold");
    h1_threshold->SetTitle("");
    h1_threshold->SetStats(0);
    h1_threshold->SetLineColor(kBlue);
    h1_threshold->SetMarkerColor(kBlue); 
    h1_threshold->SetMarkerStyle(20); 
    h1_threshold->SetMarkerSize(0.8); 
    h1_threshold->GetYaxis()->SetTitle("Threshold [DAC]");
    TH1D  *h1_noise     = (TH1D*)h2->ProjectionX("h1_noise");
    h1_noise->SetTitle("");
    h1_noise->SetStats(0);
    h1_noise->SetLineColor(kBlue);
    h1_noise->SetMarkerColor(kBlue); 
    h1_noise->SetMarkerStyle(20); 
    h1_noise->SetMarkerSize(0.8); 
    h1_noise->GetYaxis()->SetTitle("Threshold Noise [DAC]");

    // create histograms to fit with erf
    const Int_t n_hists = fr.BinHigh - fr.BinLow + 1;
    TH1D  *h1[n_hists];
    TF1 *fit_erf[n_hists];
    for (Int_t i_bin=1; i_bin<=h1_threshold->GetNbinsX(); i_bin++) {
        h1_threshold->SetBinContent(i_bin, 0);
        h1_noise->SetBinContent(i_bin, 0);
    }

    for (Int_t i_hist=0; i_hist<n_hists; i_hist++) {
        h1[i_hist] = (TH1D*)h2->ProjectionY(Form("h1_yslice_at_xbin_%i", i_hist+fr.BinLow), i_hist+fr.BinLow, i_hist+fr.BinLow);
        h1[i_hist]->SetTitle(Form("yslice at xbin=%i, t=%f", i_hist+fr.BinLow, h2->GetXaxis()->GetBinCenter(i_hist+fr.BinLow)));
        h1[i_hist]->SetStats(0);
        fit_erf[i_hist] = new TF1(Form("fit_erf_%i", i_hist+fr.BinLow),
                "[3]+[0]*TMath::Erf((x-[1])/(sqrt(2)*[2]))", 
                h1[i_hist]->GetXaxis()->GetXmin(), maxpl.Charge);
        fit_erf[i_hist]->SetParNames("Constant", "Threshold", "Noise", "Offset"); // noise -> sigma of gaussian 
        Float_t thr_est = h1[i_hist]->GetXaxis()->GetBinCenter(h1[i_hist]->FindFirstBinAbove(0.3)); // estimate for threshold position
        fit_erf[i_hist]->SetParameters(0.5, thr_est, 2., 0.5);
        fit_erf[i_hist]->FixParameter(0, 0.5);
        fit_erf[i_hist]->FixParameter(3, 0.5);
        h1[i_hist]->Fit(Form("fit_erf_%i", i_hist+fr.BinLow), "RWWQ");
        if (h1[i_hist]->GetBinContent(1)>0.8 || fit_erf[i_hist]->GetParameter(1)<h1[i_hist]->GetXaxis()->GetBinLowEdge(1)) {
            // if already first bin not empty, or threshold below lowest bin of hist => fit not reasonable => just set values
             fit_erf[i_hist]->SetParameter(1, h1[i_hist]->GetXaxis()->GetBinLowEdge(1));
             fit_erf[i_hist]->SetParameter(2, 0);
        }
        h_th->Fill(fit_erf[i_hist]->GetParameter(1));
        h_th_noise->Fill(fit_erf[i_hist]->GetParameter(2));
        h_th_chi2->Fill(fit_erf[i_hist]->GetChisquare());

        h1_threshold->SetBinContent(i_hist+fr.BinLow, fit_erf[i_hist]->GetParameter(1));
        h1_noise->SetBinContent(i_hist+fr.BinLow, fit_erf[i_hist]->GetParameter(2));
    }

    h1_threshold->GetXaxis()->SetRange(fr.BinLow, fr.BinLow+n_hists-1);
    Int_t minbin=h1_threshold->GetMinimumBin();
    minth.Time      = h1_threshold->GetBinCenter(minbin);
    minth.Threshold = h1_threshold->GetBinContent(minbin);
    minth.Noise     = h1_noise->GetBinContent(minbin);
    h1_threshold->GetXaxis()->SetRange(0, h1_threshold->GetNbinsX());


    //TCanvas *c33 = new TCanvas("c33", "c33", 1200, 1200);
    //c33->Divide(2, 2);
    //c33->cd(1);
    //h1[0]->Draw();
    //c33->cd(2);
    //h_th->Draw();
    //c33->cd(3);
    //h_th_noise->Draw();
    //c33->cd(4);
    //h_th_chi2->Draw();

    //h2->SetStats(0);
    //TCanvas *c44 = new TCanvas("c44", "c44", 1200, 1200);
    //c44->Divide(2, 2);
    //c44->cd(1);
    //h2->Draw("colz");
    //c44->cd(2);
    //h2->Draw("colz");
    //c44->cd(3);
    //h1_threshold->Draw("P");
    //c44->cd(4);
    //h1_noise->Draw("P");

    return minth; 
}

void getTimeAboveThreshold(TH2F *h2, TGraph *g, Int_t n_bins_y) {
    Int_t n_bins_x = h2->GetXaxis()->GetNbins();
    //Int_t n_bins_y = h2->GetYaxis()->GetNbins();

    g->Set(n_bins_y);

    Float_t charge[n_bins_y];
    Float_t time_above_thr[n_bins_y];
    // search for bin at highest delay with content==1 -> min threshold
    for (Int_t i_row=0; i_row<n_bins_y; i_row++) {
        charge[i_row] = h2->GetYaxis()->GetBinCenter(i_row+1);
        time_above_thr[i_row] = 0;
        for (Int_t i_col=0; i_col<n_bins_x; i_col++) {
            if (h2->GetBinContent(i_col+1, i_row+1)>0.999) {
                time_above_thr[i_row]++;
            }
        }
        time_above_thr[i_row] *= h2->GetXaxis()->GetBinWidth(1);
        //g->SetPoint(i_row, time_above_thr[i_row], charge[i_row]);
        g->SetPoint(i_row, charge[i_row], time_above_thr[i_row]);
    }

    //TCanvas *c = new TCanvas("c", "c", 1200, 600);
    //c->cd(1);
    //g->SetMarkerColor(kBlue);
    //g->SetMarkerStyle(20);
    //g->SetMarkerSize(1.5);
    //g->Draw("ap");
}




// main macro
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Bool_t PulseshapeAnalysis(Int_t Run_num=0) {
//Bool_t PulseshapeAnalysis(Int_t Run_num=0, Bool_t Do_plot=kFALSE, Int_t Reg_sel=5) {
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
    Int_t DCol_sel = RUN_INFO[Run_num][2];
    Int_t Addr_sel = RUN_INFO[Run_num][3];
    Int_t VCasn = RUN_INFO[Run_num][4];
    Int_t Ithr = RUN_INFO[Run_num][5];
    Int_t Strobe_B_length = RUN_INFO[Run_num][6];
    Int_t Vbias = RUN_INFO[Run_num][7];

    // open input root file
    TFile *in_file = new TFile(Form("../pALPIDEfs-software/Data/Results/PulseShape_%s_pix_%i_%i_ITH%i_VCASN%i_VBIAS-%i.0V_StrBLen%ins.root", 
                fFileID.Data(), DCol_sel, Addr_sel, Ithr, VCasn, Vbias, Strobe_B_length), "READ");
    if (!in_file->IsOpen()) {
        cout << "iput file not found, please check!!!" << endl;
        return kFALSE;
    }

    // read the histograms
    TH2F *hPulse[32]; 
    for (Int_t i_reg=0; i_reg<32; i_reg++) {
        hPulse[i_reg] = (TH2F*)in_file->Get(Form("hPulse_%i", i_reg)); 
        if (hPulse[i_reg] == NULL) {
            cout << "hist of region " << i_reg << " not found, please check!!!" << endl;
        } 
    }

    // extract parameters like max pulse length, minimum threshold, time over threshold, etc.
    TFile *out_file = new TFile(Form("results/pulseShape/PulseShape_results__%s_pix_%i_%i_ITH%i_VCASN%i_VBIAS-%i.0V_StrBLen%ins.root", 
                fFileID.Data(), DCol_sel, Addr_sel, Ithr, VCasn, Vbias, Strobe_B_length), "RECREATE");
    out_file->cd();
    ofstream f_out(Form("results/pulseShape/PulseShape_results__%s_pix_%i_%i_ITH%i_VCASN%i_VBIAS-%i.0V_StrBLen%ins.dat",
                fFileID.Data(), DCol_sel, Addr_sel, Ithr, VCasn, Vbias, Strobe_B_length));
    f_out << "Reg" 
            << "\t DCol"
            << "\t Addr"
            << "\t MinThr"
            << "\t MinThrDelay"
            << "\t MinThrNoise"
            << "\t MaxPulseLength"
            << "\t MaxPulseLengthCharge"
            << endl;

    MinThreshold_t minThr;
    MaxPulseLengthBin_t maxPL;
    TGraph *gToT[32];
    TGraphErrors *gAvToT[4];
    const Int_t n_bins_y = hPulse[0]->GetYaxis()->GetNbins();
    Float_t charge[n_bins_y];
    Float_t av_tot[4][n_bins_y];
    Float_t av_tot_err[4][n_bins_y];
    Double_t x=0;
    Double_t y=0;
    TMultiGraph *mgToT          = new TMultiGraph();
    TMultiGraph *mgAvToT        = new TMultiGraph();
    //TMultiGraph *mgMinThr       = new TMultiGraph();
    //TMultiGraph *mgMinThrNoise  = new TMultiGraph();
    TH1F *hDCol             = new TH1F("hDCol", "hDCol; PixNum; DCol", 32, 0, 32); // hist for saving the DCols of the pixels 
    TH1F *hAddr             = new TH1F("hAddr", "hAddr; PixNum; Addr", 32, 0, 32); // hist for saving the Addr of the pixels 
    TH1F *hReg              = new TH1F("hReg", "hReg; PixNum; Reg", 32, 0, 32); // hist for saving the Reg of the pixels 
    TH1F *hMinThr           = new TH1F("hMinThr", "hMinThr; PixNum; Min Threshold [DAC]", 32, 0, 32); // hist for saving the min Thr of the pixels 
    TH1F *hMinThrDel        = new TH1F("hMinThrDel", "hMinThrDel; PixNum; Delay at Min Threshold [#mus]", 32, 0, 32); // hist for saving the min Thr delay value of the pixels 
    TH1F *hMinThrNoise      = new TH1F("hMinThrNoise", "hMinThrNoise; PixNum; Noise at Min Threshold [DAC]", 32, 0, 32); // hist for saving the min Thr noise value of the pixels 
    TH1F *hMaxPulseLen      = new TH1F("hMaxPulseLen", "hMaxPulseLen; PixNum; Max Pulse Length [#mus]", 32, 0, 32); // hist for saving the max pulse length value of the pixels 
    TH1F *hMaxPulseLenCharge= new TH1F("hMaxPulseLenCharge", "hMaxPulseLenCharge; PixNum; Charge at Max Pulse Length [DAC]", 32, 0, 32); // hist for saving the charge value for max pulse length of the pixels 
    TLegend *leg = new TLegend(0.4, 0.2, 0.7, 0.4);
    leg->SetBorderSize(0);
    leg->SetFillColor(kWhite);

    for (Int_t i_sec=0; i_sec<4; i_sec++) { 
        for (Int_t i_point=0; i_point<n_bins_y; i_point++) { 
            av_tot[i_sec][i_point] = 0;
            av_tot_err[i_sec][i_point] = 0;
        }
    }
    for (Int_t i_reg=0; i_reg<32; i_reg++) {
        hDCol->SetBinContent(i_reg+1, DCol_sel);
        hAddr->SetBinContent(i_reg+1, Addr_sel);
        hReg->SetBinContent(i_reg+1, i_reg);

        minThr = getMinThreshold(hPulse[i_reg]);
        cout << i_reg/8 << "\t" << minThr.Time << "\t" << minThr.Threshold << "\t" << minThr.Noise << endl;
       
        hMinThr->SetBinContent(i_reg+1, minThr.Threshold);
        hMinThrDel->SetBinContent(i_reg+1, minThr.Time);
        hMinThrNoise->SetBinContent(i_reg+1, minThr.Noise);

        maxPL = getMaxPulseLengthFast(hPulse[i_reg]);
        hMaxPulseLenCharge->SetBinContent(i_reg+1, maxPL.Charge);
        hMaxPulseLen->SetBinContent(i_reg+1, maxPL.Time);

        f_out << i_reg 
                << "\t" << DCol_sel
                << "\t" << Addr_sel
                << "\t" << minThr.Threshold
                << "\t" << minThr.Time
                << "\t" << minThr.Noise
                << "\t" << maxPL.Time
                << "\t" << maxPL.Charge
                << endl;

        gToT[i_reg] = new TGraph();
        getTimeAboveThreshold(hPulse[i_reg], gToT[i_reg], n_bins_y);
        for (Int_t i_point=0; i_point<n_bins_y; i_point++) { 
            gToT[i_reg]->GetPoint(i_point, x, y);
            av_tot[i_reg/8][i_point] += y;
            charge[i_point] = hPulse[i_reg]->GetYaxis()->GetBinCenter(i_point+1);
        }
        gToT[i_reg]->SetMarkerColor(1+i_reg/8);
        gToT[i_reg]->SetMarkerStyle(20+i_reg/8);
        gToT[i_reg]->SetMarkerSize(0.5);
        gToT[i_reg]->SetName(Form("gToT_%i", i_reg));
        gToT[i_reg]->SetTitle(Form("gToT_%i", i_reg));
        gToT[i_reg]->Draw("ap");
        gToT[i_reg]->Write();
        mgToT->Add(gToT[i_reg], "p");
    
    }
    
    f_out.close();
    hDCol->Write();
    hAddr->Write();
    hReg->Write();
    hMinThr->Write();
    hMinThrDel->Write();
    hMinThrNoise->Write();
    hMaxPulseLen->Write();
    hMaxPulseLenCharge->Write(); 
    for (Int_t i_sec=0; i_sec<4; i_sec++) { 
        for (Int_t i_point=0; i_point<n_bins_y; i_point++) { 
            av_tot[i_sec][i_point] /= (32/4);
        }
    }
    for (Int_t i_reg=0; i_reg<32; i_reg++) {
        for (Int_t i_point=0; i_point<n_bins_y; i_point++) { 
            gToT[i_reg]->GetPoint(i_point, x, y);
            av_tot_err[i_reg/8][i_point] += (y-av_tot[i_reg/8][i_point])*(y-av_tot[i_reg/8][i_point]);
        }
    }
    for (Int_t i_sec=0; i_sec<4; i_sec++) { 
        for (Int_t i_point=0; i_point<n_bins_y; i_point++) { 
            av_tot_err[i_sec][i_point] = sqrt(av_tot_err[i_sec][i_point]/(32/4-1));
        }
        gAvToT[i_sec] = new TGraphErrors(n_bins_y, charge, av_tot[i_sec], 0, av_tot_err[i_sec]); 
        gAvToT[i_sec]->SetLineColor(1+i_sec);
        gAvToT[i_sec]->SetMarkerColor(1+i_sec);
        gAvToT[i_sec]->SetMarkerStyle(20+i_sec);
        gAvToT[i_sec]->SetMarkerSize(0.8);
        gAvToT[i_sec]->SetName(Form("gAvToT_%i", i_sec));
        gAvToT[i_sec]->SetTitle(Form("gAvToT_%i", i_sec));
        gAvToT[i_sec]->Draw("ap");
        gAvToT[i_sec]->Write();
        mgAvToT->Add(gAvToT[i_sec], "p");
    
        leg->AddEntry(gAvToT[i_sec], Form("Sector %i", i_sec), "lp");
    }
    out_file->Close();

    // drawing 
    TCanvas *c = new TCanvas("c", "c", 1200, 800);
    c->cd();
    mgToT->SetTitle("");
    mgToT->Draw("ap");
    mgToT->SetTitle(Form("PulseShape_%s: Pix %i/%i, ITH%i, VCASN%i, VBIAS-%i.0V, StrBLen%ins", 
                fFileID.Data(), DCol_sel, Addr_sel, Ithr, VCasn, Vbias, Strobe_B_length));
    mgToT->GetXaxis()->SetTitle("Charge [DAC]");
    mgToT->GetYaxis()->SetTitle("Time over Threshold [#mus]");
    leg->SetTextSize(mgToT->GetXaxis()->GetTitleSize());
    leg->Draw("same");

    TCanvas *c1 = new TCanvas("c1", "c1", 1200, 800);
    c1->cd();
    mgAvToT->SetTitle("");
    mgAvToT->Draw("ap");
    mgAvToT->SetTitle(Form("PulseShape_%s: Pix %i/%i, ITH%i, VCASN%i, VBIAS-%i.0V, StrBLen%ins", 
                fFileID.Data(), DCol_sel, Addr_sel, Ithr, VCasn, Vbias, Strobe_B_length));
    mgAvToT->SetMinimum(0);
    //mgAvToT->SetMaximum(30);
    mgAvToT->GetXaxis()->SetTitle("Charge [DAC]");
    mgAvToT->GetYaxis()->SetTitle("Time over Threshold [#mus]");
    leg->Draw("same");

    c->Print(Form("results/pulseShape/PulseShape_%s_ToT__pix_%i_%i_ITH%i_VCASN%i_VBIAS-%i.0V_StrBLen%ins.png", 
                fFileID.Data(), DCol_sel, Addr_sel, Ithr, VCasn, Vbias, Strobe_B_length));
    c1->Print(Form("results/pulseShape/PulseShape_%s_AvToT__pix_%i_%i_ITH%i_VCASN%i_VBIAS-%i.0V_StrBLen%ins.png", 
                fFileID.Data(), DCol_sel, Addr_sel, Ithr, VCasn, Vbias, Strobe_B_length));

    
    //TCanvas *c2 = new TCanvas("c2", "c2", 1600, 1200);
    //c2->Divide(2, 3);
    //c2->cd(1); 
    //hReg->Draw("same");
    //hDCol->Draw("same");
    //hAddr->Draw("same");
    //c2->cd(2); 
    //hMinThr->Draw();
    //c2->cd(3); 
    //hMinThrDel->Draw();
    //c2->cd(4); 
    //hMinThrNoise->Draw();
    //c2->cd(5); 
    //hMaxPulseLen->Draw();
    //c2->cd(6); 
    //hMaxPulseLenCharge->Draw(); 
     

    return kTRUE;
}

