// macro for generating random trigger waveforms for noise studies 

#include <iostream>
#include <fstream>
#include <vector>

#include "TH1.h"
#include "TH1F.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TMath.h"

using namespace std;


// TRandom::Exp(tau) returns:  an exponential deviate exp( -t/tau ) ==>>> tau = 1/lambda!!! 

Bool_t GenerateWaveform(Float_t trg_rate=2.5e4, Float_t busy_time = 50, Float_t f_clk=80e6) {
    busy_time*=1e-6;
    
    // average particle rate during spill [Hz]
    Float_t lambda = trg_rate;
    //cout << lambda << endl;
    Float_t tau = 1./lambda;

    // exponential random numbers for time between events 
    TRandom *R = new TRandom(65539);
    
    // generate waveform sequence
    const Int_t n_samples = 100000;
    Int_t seq_trg[n_samples];
    Int_t seq_trg_acpt[n_samples];
    cout << "total sequence duration: " << n_samples*1./f_clk << " s" << endl;

    //Float_t sum_times = 0;
    //Float_t sum_times_busy = sum_times+busy_time; 
    Float_t next_trg = R->Exp(tau); 
    Float_t cur_trg = 0; 
    Float_t busy_end = cur_trg+busy_time; 

    Float_t rndexp = 0; 
    Float_t last_trg = 0; 
    Float_t last_trg_acpt = 0; 

    // quality controll
    TH1F *h_R = new TH1F("h_R", "h_R; #DeltaT trigger; counts", 200, 0, 625e-6);
    TH1F *h_R_check = new TH1F("h_R_check", "h_R_check; #DeltaT trigger; counts", 200, 0, 625e-6);
    TH1F *h_R_busy = new TH1F("h_R_busy", "h_R_busy; #DeltaT trigger; counts", 200, 0, 625e-6);
    h_R_busy->SetLineColor(kRed);
    TH1F *h_seq = new TH1F("h_seq", "h_seq", n_samples+1, 0, n_samples);
    TH1F *h_seq_acpt = new TH1F("h_seq_acpt", "h_seq_acpt", n_samples+1, 0, n_samples);
    h_seq_acpt->SetLineColor(kRed);

    Int_t n_hits = 0; 
    Int_t n_hits_acpt = 0; 
    Float_t t_sample=0;

    Float_t t_clk=1./f_clk;
    cout << "clock duration: " << t_clk << " s"<< endl; 

    for (Int_t i_sample=0; i_sample<n_samples; i_sample++) {
        seq_trg[i_sample] = 0;
        seq_trg_acpt[i_sample] = 0;
    
        t_sample = i_sample*t_clk;
        //cout<< t_sample << endl;

        if (t_sample >= next_trg) {

            seq_trg[i_sample] = 1;

            rndexp = R->Exp(tau);
            h_R->Fill(rndexp);

            // find next trg time
            cur_trg = next_trg;
            next_trg += rndexp;

            h_R_check->Fill(t_sample-last_trg);
            last_trg = t_sample;

            n_hits++;

            // accept trigger or not 
            // set busy if trigger accepted
            if (t_sample>=busy_end) {
                
                seq_trg_acpt[i_sample] = 1;
                busy_end = cur_trg+busy_time; 

                n_hits_acpt++;     
                h_R_busy->Fill(t_sample-last_trg_acpt);
                last_trg_acpt = t_sample;
            }
        } 

        // sequence histograms
        h_seq->SetBinContent(i_sample+1, seq_trg[i_sample]);
        h_seq_acpt->SetBinContent(i_sample+1, seq_trg_acpt[i_sample]*0.5);
    }

    // number of hits
    cout << "number of hits found: " << endl;
    cout << "-) expected: " << n_samples*trg_rate*t_clk << endl;
    cout << "-) found: " << n_hits << endl;
    cout << "_) of which accepted: " << n_hits_acpt << endl;

    // drawing
    TCanvas *c_R = new TCanvas("c_R", "c_R", 1200, 1200);
    c_R->Divide(1,2);
    TCanvas *c_seq = new TCanvas("c_seq", "c_seq", 1600, 1200);
    c_seq->Divide(1,2);

    c_R->cd(1);
    h_R->Draw();
    c_R->cd(2);
    h_R_check->Draw();
    h_R_busy->Draw("same");

    c_seq->cd(1);
    h_seq->Draw();
    h_seq_acpt->Draw("same");
    c_seq->cd(2);
    h_seq_acpt->Draw("");

    // write sequence to file
    ofstream f_out;   
    // input parameters: Float_t trg_rate=2.5e4, Float_t busy_time = 50e-6, Float_t f_clk=80e6
    f_out.open(Form("./waveforms/waveform_rate%.0fHz_busy%.0fus_fclk%.0fHz.txt", trg_rate, busy_time*1e6, f_clk));
    for (Int_t i_sample=0; i_sample<n_samples; i_sample++) {
        f_out << seq_trg_acpt[i_sample] << endl;
    }
    f_out.close();

    return kTRUE;
}




