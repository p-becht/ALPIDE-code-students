//from ex day02
#include <cmath>
#include <TCanvas.h>
#include <TF1.h>
#include <TMath.h>
#include <TH1F.h>
#include <TColor.h>
#include <TH2F.h>
#include <TF2.h>
#include <TGraph.h>

//explicit used in day03 so far
#include <TFile.h>
#include <TObject.h>
#include <TH1.h>
#include <TH1D.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TStyle.h>
#include <iostream> // used in both
#include <fstream>

//modified for epp
#include <vector>
#include <TAxis.h>
using namespace std;

//firstly load the data
/*void load_data()
{
    //create array
    Int_t size = 0;
    Int_t dummy[8];
    char string;

    //class to read an input file
    ifstream in;
    //open an existing file to read
    in.open("test_data.txt");

    //determine the length of the file
    while (1)
    {
        getline(in,string,"\n" );
        // when the file finishes, exit the loop
        if (!in.good())
            cout << size << endl;
            break;
        
        size++;
        cout << dummy[0] << endl;
    }
    in.close();
    //container
    Int_t data[size][7];
    Int_t i = 0;

    in.open("test_data.txt");
    if (in.is_open())
    {
        in >> dummy[0] >> dummy[1] >> dummy[2] >> dummy[3] >> dummy[4] >> dummy[5] >> dummy[6] >> dummy[7];
        while (in.good())
        {
            //insert the data
            in >> dummy[0] >> data[i][0] >> data[i][1] >> data[i][2] >> data[i][3] >> data[i][4] >> data[i][5] >> data[i][6];
            i++;
        }
    }
    cout << data << endl;
}*/

//creating a histogram to compare results of the measurement with the theory in terms of muon rate
void event_plane_histogram()
{
    //theoreticall muon rate
    Double_t Accept[]{0.06644468, 0.05528426, 0.03052087, 0.01706546, 0.01051952, 0.00704046, 0.00501284};
    //rate over the runtime
    /*Rate pulser : 95.8 us, trigger length (from pulser) : 200 ns, TRIGGER IN (sent to telescope) ~120 ns*/
    //for 9 million events the run duration is about 864s
    Int_t accept[7];
    for (unsigned int i = 0; i < 7; i++)
    {
        // include multiplicity: m=8-n
        accept[i] = (Accept[i]) * (864) * (7 - i);
    }
    //test data
    Double_t test[]{0, 50, 50, 20, 10, 5, 2};

    TH1D *ppe = new TH1D("ppe", "Expacted muon-rate per planes per run (864 s);at least traversed planes;counts", 7, 1, 8);
    TH1D *ppe_log = new TH1D("ppe_log", "Planes per event per run, logarithmic;traversed planes;counts", 7, 1, 8);
    TH1D *ppe2 = new TH1D("ppe2", "Expacted rate of multi-plane-events per run (864 s);traversed planes;counts", 7, 1, 7);
    for (unsigned int i = 1; i < 8; i++)
    {
        ppe->SetBinContent(i, accept[i - 1]);
        if (i < 7)
        {
            ppe2->SetBinContent(i, (accept[i - 1] - accept[i]));
        }
        else
        {
            ppe2->SetBinContent(i, (accept[i - 1] - 0));
        }
    }
    //create histograms
    TCanvas *c = new TCanvas("c", "some histograms", 1000, 1000);
    c->Divide(2);

    c->cd(1);

    ppe->SetLineColor(kRed);
    ppe->GetXaxis()->SetLimits(0.5, 7.5);
    gStyle->SetOptStat(false); //deact stat window
    gPad->SetGridy();
    gPad->SetLogy(1);
    ppe->Draw("");

    c->cd(2);

    ppe2->SetLineColor(kRed);
    ppe2->GetXaxis()->SetLimits(0.5, 7.5);
    gStyle->SetOptStat(false); //deact stat window
    gPad->SetGridy();
    gPad->SetLogy();
    ppe2->Draw("");

    // ppe2->SetLineColor(3); //green
    // ppe2->Draw("SAME");
}

void compare_event_plane_histogram()
{
    //theoreticall muon rate
    Double_t Accept[]{0.06644468, 0.05528426, 0.03052087, 0.01706546, 0.01051952, 0.00704046, 0.00501284};
    // experimental results fo 9 million
    Double_t results[]{661.5174433624243, 95.83518325077783, 20.947658680684256, 6.4398838606777415, 2.268854668598578, 1.0059608450988982, 0.8504914018712583};
    Double_t results_d[]{3.232804797706118, 0.6641264030543372, 0.26309829032259513, 0.14477458767243254, 0.07841333005752525, 0.056219850291740585, 0.049177022304970355};
    //rate over the runtime
    /*Rate pulser : 95.8 us, trigger length (from pulser) : 200 ns, TRIGGER IN (sent to telescope) ~120 ns*/
    //for 9 million events the run duration is about 864s
    Double_t accept[7];
    Double_t planes[7];
    Double_t xerr[7];
    for (unsigned int i = 0; i < 7; i++)
    {
        // include multiplicity: m=8-n
        planes[i] = i + 1;
        xerr[i] = 0.5;
        accept[i] = (Accept[i]) * (7 - i);
        results[i] = (results[i]) / (864);
        results_d[i] = (results_d[i]) / (864);
    }

    TH1D *theo = new TH1D("theo", "Expacted and measured muon-rate per planes;traversed planes;counts", 7, 1, 8);
    TGraphErrors *res = new TGraphErrors(7, planes, results, xerr, results_d);

    for (unsigned int i = 1; i < 8; i++)
    {
        if (i < 7)
        {
            theo->SetBinContent(i, (accept[i - 1] - accept[i]));
        }
        else
        {
            theo->SetBinContent(i, (accept[i - 1] - 0));
        }
    }
    // create histograms
    TCanvas *c = new TCanvas("c", "some histograms", 1000, 1000);

    c->cd(1);

    res->SetLineColor(kRed);
    gStyle->SetOptStat(false); //deact stat window
    gPad->SetGridy();
    gPad->SetLogy(1);
    res->GetXaxis()->SetLimits(0.5, 7.5);
    res->SetTitle("Expacted and measured muon-rate per planes;traversed planes;rate [1/s]");
    // res->SetMarkerStyle(21);
    res->SetLineWidth(2);
    res->Draw("ap");

    theo->GetXaxis()->SetLimits(0.5, 7.5);
    // theo->GetYaxis()->SetLimits(0.0008, 1);
    theo->SetLineColor(kBlack);
    theo->Draw("same");


    //draw legend                (x-left,y-bottom,x-right,y-top)
    TLegend *legend = new TLegend(0.7, 0.8, 0.9, 0.9);
    legend->AddEntry(theo, "Expected rate");
    legend->AddEntry(res, "Measured rate");
    legend->Draw();
}