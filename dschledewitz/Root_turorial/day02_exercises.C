
#include<iostream>
#include<cmath>
#include<TCanvas.h>
#include<TF1.h>
#include<TMath.h>
#include<TH1F.h>
#include<TColor.h>
#include<TH2F.h>
#include<TF2.h>
#include<TGraph.h>

using namespace std;





///////////////// first exercise
void Function(Int_t a=1, Int_t b=1) //definition, one can call through root
{
    TF1 *func =new TF1("func","[0]*pow(x,[1])*sin(x)",-3,3);
    func->SetParameters(a,b);
    func->Draw();

    cout<<"f(2): "                            << func->Eval(2)        <<endl;
    cout<<"f'(2): "                           << func->Derivative(2)  <<endl;
    cout<<"Integral of f(x) from 0 to 1: "    << func->Integral(0,1)  <<endl;
}



////////////////second exercise
void Histograms ()
{
    //building the canvas
    TCanvas *c= new TCanvas("c","some histograms",1000,1000);
    c->Divide(2,2);

    ////first pad:
    c->cd(1);

    TH1F *random = new TH1F("random", "Uniform Distribution;slot;counts",50,-5,5);

    //uniform function
    TF1 unif("unif","1",-5,5);

    //fill Random with uniform distribution
    random->FillRandom("unif",10000);

    random->Draw("hist");



    ////second pad:
    c->cd(2);

    // Now: second histogram with gaussian distribution
    TH1F *gaussian = new TH1F("gaussian", "Gaussian Distribution;slot;counts",50,-5,5);

    // gauss function
    TF1 gaussfunc("gaussfunc","gaus",-5,5);
    // parameters (norm,mean,sigma)
    gaussfunc.SetParameters(1,0,1);

    //fill random
    gaussian->FillRandom("gaussfunc", 1000);

    gaussian->Draw("hist");


    ////third pad:
    c->cd(3);
    
    // Now: combine histo 1 and 2
    TH1F *combine = new TH1F("combine", "Combination of Histogramm 1 and 2;slot;counts",50,-5,5);

    // add the histos together
    combine->Add(random,gaussian);

    combine->Draw();

    
    
    ////fourth pad:
    c->cd(4);
    
    // Now: unity and errors
    TH1F *unity = new TH1F("unity", "Unification of the combined histogram with error;slot;counts",50,-5,5);

    //error
    gaussian->Sumw2();
    random->Sumw2();
    
    // create and unify
    unity->Add(gaussian,random);
    unity->Scale(1./unity->GetEntries());

    unity->Draw("E");
}


////////////////third exercise
void Fits ()
{
   //creating the histogramm
   TH1F *K0mass= new TH1F("K0mass","K0 mass",100,0.35,0.65);
   K0mass->SetBinContent(1,307);
   K0mass->SetBinContent(2,267);
   K0mass->SetBinContent(3,249);
   K0mass->SetBinContent(4,231);
   K0mass->SetBinContent(5,245);
   K0mass->SetBinContent(6,237);
   K0mass->SetBinContent(7,209);
   K0mass->SetBinContent(8,212);
   K0mass->SetBinContent(9,188);
   K0mass->SetBinContent(10,209);
   K0mass->SetBinContent(11,197);
   K0mass->SetBinContent(12,196);
   K0mass->SetBinContent(13,162);
   K0mass->SetBinContent(14,202);
   K0mass->SetBinContent(15,186);
   K0mass->SetBinContent(16,159);
   K0mass->SetBinContent(17,155);
   K0mass->SetBinContent(18,158);
   K0mass->SetBinContent(19,142);
   K0mass->SetBinContent(20,148);
   K0mass->SetBinContent(21,150);
   K0mass->SetBinContent(22,135);
   K0mass->SetBinContent(23,134);
   K0mass->SetBinContent(24,111);
   K0mass->SetBinContent(25,125);
   K0mass->SetBinContent(26,117);
   K0mass->SetBinContent(27,123);
   K0mass->SetBinContent(28,115);
   K0mass->SetBinContent(29,130);
   K0mass->SetBinContent(30,106);
   K0mass->SetBinContent(31,101);
   K0mass->SetBinContent(32,112);
   K0mass->SetBinContent(33,95);
   K0mass->SetBinContent(34,105);
   K0mass->SetBinContent(35,102);
   K0mass->SetBinContent(36,104);
   K0mass->SetBinContent(37,97);
   K0mass->SetBinContent(38,84);
   K0mass->SetBinContent(39,82);
   K0mass->SetBinContent(40,82);
   K0mass->SetBinContent(41,99);
   K0mass->SetBinContent(42,88);
   K0mass->SetBinContent(43,100);
   K0mass->SetBinContent(44,105);
   K0mass->SetBinContent(45,118);
   K0mass->SetBinContent(46,225);
   K0mass->SetBinContent(47,464);
   K0mass->SetBinContent(48,857);
   K0mass->SetBinContent(49,1205);
   K0mass->SetBinContent(50,1177);
   K0mass->SetBinContent(51,878);
   K0mass->SetBinContent(52,477);
   K0mass->SetBinContent(53,198);
   K0mass->SetBinContent(54,118);
   K0mass->SetBinContent(55,62);
   K0mass->SetBinContent(56,69);
   K0mass->SetBinContent(57,59);
   K0mass->SetBinContent(58,73);
   K0mass->SetBinContent(59,68);
   K0mass->SetBinContent(60,69);
   K0mass->SetBinContent(61,53);
   K0mass->SetBinContent(62,66);
   K0mass->SetBinContent(63,62);
   K0mass->SetBinContent(64,61);
   K0mass->SetBinContent(65,60);
   K0mass->SetBinContent(66,60);
   K0mass->SetBinContent(67,56);
   K0mass->SetBinContent(68,55);
   K0mass->SetBinContent(69,53);
   K0mass->SetBinContent(70,72);
   K0mass->SetBinContent(71,48);
   K0mass->SetBinContent(72,57);
   K0mass->SetBinContent(73,48);
   K0mass->SetBinContent(74,43);
   K0mass->SetBinContent(75,57);
   K0mass->SetBinContent(76,42);
   K0mass->SetBinContent(77,76);
   K0mass->SetBinContent(78,51);
   K0mass->SetBinContent(79,62);
   K0mass->SetBinContent(80,46);
   K0mass->SetBinContent(81,68);
   K0mass->SetBinContent(82,63);
   K0mass->SetBinContent(83,42);
   K0mass->SetBinContent(84,53);
   K0mass->SetBinContent(85,46);
   K0mass->SetBinContent(86,53);
   K0mass->SetBinContent(87,62);
   K0mass->SetBinContent(88,56);
   K0mass->SetBinContent(89,53);
   K0mass->SetBinContent(90,47);
   K0mass->SetBinContent(91,46);
   K0mass->SetBinContent(92,70);
   K0mass->SetBinContent(93,59);
   K0mass->SetBinContent(94,59);
   K0mass->SetBinContent(95,58);
   K0mass->SetBinContent(96,57);
   K0mass->SetBinContent(97,57);
   K0mass->SetBinContent(98,48);
   K0mass->SetBinContent(99,49);
   K0mass->SetBinContent(100,48);
   K0mass->SetEntries(15000);
   K0mass->GetXaxis()->SetTitle("m [GeV/c]");
   
   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   K0mass->SetLineColor(ci);
   K0mass->GetXaxis()->SetLabelFont(42);
   K0mass->GetXaxis()->SetLabelSize(0.035);
   K0mass->GetXaxis()->SetTitleSize(0.035);
   K0mass->GetXaxis()->SetTitleFont(42);
   K0mass->GetYaxis()->SetLabelFont(42);
   K0mass->GetYaxis()->SetLabelSize(0.035);
   K0mass->GetYaxis()->SetTitleSize(0.035);
   K0mass->GetYaxis()->SetTitleFont(42);
   K0mass->GetZaxis()->SetLabelFont(42);
   K0mass->GetZaxis()->SetLabelSize(0.035);
   K0mass->GetZaxis()->SetTitleSize(0.035);
   K0mass->GetZaxis()->SetTitleFont(42);
   K0mass->Draw("");
   
   
   
   //define the fit function as sum of two functions:
   TF1 fFit("fFit", "[0]*pow(x,[1])+gaus(2)",0.35,0.65);
   
   // Set some reasonable start parameters :(exp^0,exp-order, gauss:norm,mean, sigma)
   fFit.SetParameters(1.,-1,1000.,0.5,0.01);


  // Hint: in case the fit does not work, try to search the correct parameters for the two functions individually:
  // Fit of the peak region with a gaussian
  // Fit the whole region with the second function
   
   // Perform the fit
    K0mass->Fit(&fFit);
}




////////////////fourth exercise
void histogram2D()
{
    //create histogram
    TH2F *histo2D = new TH2F("histo2D", "2D-Histogram;x;y;z",100,-5,5,100,-5,5);

    // 2d function for the distribution of the histogram
    // AfuncD(i): A= axis, D = order (for some functions), i= first par index
    TF2 func2d("func2d", "xgaus(0)*ypol2(3)");

    func2d.SetParameters(100.,-1.,1.,1.,-2.,2.);

    histo2D->FillRandom("func2d",5000);

    //histo2D->Draw("");
    //histo2D->Draw("colz");
    histo2D->Draw("surf2");
}

TH1F *GetHistoGaus(const char* name)
{
  // create a simple histogram filled with random gaus numbers
    TH1F *h1 = new TH1F(name,"gauss;random gaus number; entries",100,-5,5);
    h1->FillRandom("gaus",100000);
    return h1;
}

    //__________________________________________________________________________
TGraph* GetGraph()
{
    // fill a nice graph
    TGraph *gr=new TGraph;
    gr->SetTitle("My first graph;x-values;y-values");
    gr->SetPoint(0,1.,2.);
    gr->SetPoint(1,2.,2.);
    gr->SetPoint(2,3.,5.);
    gr->SetPoint(3,5.,7.);
    gr->SetPoint(4,6.,8.);
    return gr;
}


void canvas6pads ()
{

    //building the canvas
    TCanvas *c6= new TCanvas("c6","6 histograms",1000,1000);
    c6->Divide(3,2);

    ////first pad:
    c6->cd(1);

    TH1F *hist1=GetHistoGaus("hist1");
    hist1->SetLineWidth(2);
    hist1->SetLineStyle(2);
    hist1->SetLineColor(kRed);

    hist1->Draw("");

    c6->cd(2);

    TH1F *hist2=GetHistoGaus("hist2");
    hist2->SetLineWidth(10);
    hist2->SetLineStyle(4);
    hist2->SetLineColor(kGreen);

    hist2->Draw("");

    c6->cd(3);

    TH1F *hist3=GetHistoGaus("hist3");
    hist3->SetLineWidth(6);
    hist3->SetLineStyle(6);
    hist3->SetLineColor(kBlue);
    hist3->SetFillColor(kViolet);

    hist3->Draw("");


    c6->cd(4);
    //create graphs. Use different draw options below
    TGraph *graph1=GetGraph();
    graph1->SetLineWidth(3);

    graph1->Draw("alp");


    c6->cd(5);

    TGraph *graph2=GetGraph();
    graph2->SetLineWidth(3);
    graph2->SetLineColor(kBlue-4);
    graph2->Draw("acp");


    c6->cd(6);

    TGraph *graph3=GetGraph();
    graph3->SetLineWidth(3);
    graph3->SetLineColor(kBlue+2);
    graph3->Draw("alf");

    c6->SaveAs("Documents/root_tut/exercise/canvas_6pads.png");
}