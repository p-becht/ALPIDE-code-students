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
#include <TBrowser.h>
#include <TObject.h>
#include <TH1.h>
#include <TH1D.h>
#include <TRandom.h>
#include <TRandom3.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TStyle.h>
#include <iostream> // used in both
#include <fstream>

using namespace std;

//#########################################################EXAMPLES

void read_Data_FromFile_example()
{

    //class to read an input file
    ifstream in;
    //open an existing file to read
    in.open("data.dat");

    Float_t x, y, z;
    while (1)
    {
        //insert the values of each line in the variables x y z
        in >> x >> y >> z;
        // when the file finishes, exit the loop
        if (!in.good())
            break;
        //print the values
        cout << x << " " << y << " " << z << endl;
    }
    //close the file
    in.close();
}

// writung data to "example.txt"
int write_data_to_file_example()
{
    ofstream myfile;
    myfile.open("example.txt");
    myfile << "Writing this to a file.\n";
    myfile.close();
    return 0;
}

//########################################################################################EXERCISES

// 1: I/O writung and reading

void gauss_hist_save_1()
{
    // histogram with gaussian distribution
    TH1F *gauss1 = new TH1F("gauss1", "Gaussian Distribution;slot;counts", 250, 0, 5);

    // gauss function
    TF1 gaussfunc1("gaussfunc", "gaus", 0, 5);
    // parameters (norm,mean,sigma)
    gaussfunc1.SetParameters(1, 2, 0.2);

    //fill random
    gauss1->FillRandom("gaussfunc", 10000);

    //create file
    TFile file1("save_gausshisto.root", "recreate");

    //write the histogram into the file
    gauss1->Write();
    //close the file(save)
    file1.Close();
}
void gauss_hist_save_2()
{
    // histogram with gaussian distribution
    TH1F *gauss2 = new TH1F("gauss2", "Gaussian Distribution;slot;counts", 250, 0, 5);

    // gauss function
    TF1 gaussfunc2("gaussfunc", "gaus", 0, 5);
    // parameters (norm,mean,sigma)
    gaussfunc2.SetParameters(1, 3, 0.2);

    //fill random
    gauss2->FillRandom("gaussfunc", 10000);

    //create file
    TFile file1("save_gausshisto.root", "update");

    //write the histogram into the file
    gauss2->Write();

    //close the file(save)
    file1.Close();
}

void load_gaussians() //does not work properly
{
    //create gaussians
    //gauss_hist_save_1();
    //gauss_hist_save_2();

    //load them from file
    TFile *file1 = new TFile("save_gausshisto.root", "read");
    TH1F *gauss1 = (TH1F *)file1->Get("gauss1");
    TH1F *gauss2 = (TH1F *)file1->Get("gauss2");

    // another way, more explicit: TH1F *gaussi = static_cast<TH1F*>(ile1.Get("gauss1"))

    //create canvas
    TCanvas *c = new TCanvas("c", "some histograms");

    //draw the histograms
    gauss1->Draw("hist");
    gauss2->SetLineColor(3); //green
    gauss2->Draw("same");

    gStyle->SetOptStat(false); //deact stat window

    //draw legend                (x-left,y-bottom,x-right,y-top)
    TLegend *legend = new TLegend(0.7, 0.8, 0.9, 0.9);
    legend->AddEntry(gauss1, "Gaussian 1");
    legend->AddEntry(gauss2, "Gaussian 2");
    legend->Draw();
}

void save_canvas()
{
    //loading the 3 files from vertex.root
    TFile *file = new TFile("vertex.root");
    TH2F *v2D = (TH2F *)file->Get("vertex2D");
    TH1D *Zv_1 = (TH1D *)file->Get("Zvert_1");
    TH1D *Zv_2 = (TH1D *)file->Get("Zvert_2");

    //create a canvas
    TCanvas *c_vertex = new TCanvas("c_vertex", "canvas vertex", 11, 80, 1634, 469);

    //creating 3 pads
    c_vertex->Divide(3);

    //1
    c_vertex->cd(1);
    v2D->Draw("colz");
    //2
    c_vertex->cd(2);
    Zv_1->Draw();
    //3
    c_vertex->cd(3);
    Zv_2->Draw();
    //save
    c_vertex->SaveAs("c_vertex.root");
    file->Close();
}

void load_canvas()
{
    //load the saved canvas
    TFile *file = new TFile("c_vertex.root");
    //load the canvas
    TCanvas *c_v = (TCanvas *)file->Get("c_vertex");

    //Draw vertex
    c_v->Draw();
}

void collections()
{
    //create histograms
    TH1F *gaussian1 = new TH1F("gaussian1", "Gaussian Distribution 1;slot;counts", 200, 0, 5);
    TH1F *gaussian2 = new TH1F("gaussian2", "Gaussian Distribution 2;slot;counts", 20, -5, 5);
    TH1F *uniform = new TH1F("uniform", "Uniform Distribution;slot;counts", 500, 0, 5);
    TH1F *landau = new TH1F("landau", "Landau Distribution;slot;counts", 200, 0, 5);
    TH1F *polynom = new TH1F("polynom", "Polynomial Distribution;slot;counts", 200, 0, 5);

    //uniform function
    TF1 unif("unif", "1", 0, 5);
    // gauss function
    TF1 gaussfunc("gaussfunc", "gaus", 0, 5);
    gaussfunc.SetParameters(1, 3, 0.5); // parameters (norm,mean,sigma)

    //fill histograms randomly
    gaussian1->FillRandom("gaussfunc", 10000);
    gaussian2->FillRandom("gaus", 1000);
    uniform->FillRandom("unif", 20000);
    landau->FillRandom("landau", 10000);
    polynom->FillRandom("pol3", 10000);

    //Filling histograms in Object array
    TObjArray array;
    array.Add(gaussian1);
    array.Add(gaussian2);
    array.Add(uniform);
    array.Add(landau);
    array.Add(polynom);

    //create graphs
    TGraphErrors *g1 = new TGraphErrors(10);
    TGraphErrors *g2 = new TGraphErrors(10);
    TGraphErrors *g3 = new TGraphErrors(10);
    TGraphErrors *g4 = new TGraphErrors(10);
    TGraphErrors *g5 = new TGraphErrors(10);

    //filling graphs
    for (Int_t i = 0; i < 10; ++i)
    {
        //(index,x,y),errors:(index, x_err, y_err)
        g1->SetPoint(i, i, i * i - i);
        g1->SetPointError(i, 0, 0.05 * i);
        g1->SetName("Graph1");
        g2->SetPoint(i, i, i / 2. + 3.);
        g2->SetPointError(i, 0, 0.05);
        g2->SetName("Graph2");
        g3->SetPoint(i, i, i + 1);
        g3->SetPointError(i, 0, 0.02 * i);
        g3->SetName("Graph3");
        g4->SetPoint(i, i, i * i * i);
        g4->SetPointError(i, 0, 0.02 * i * i);
        g4->SetName("Graph4");
        g5->SetPoint(i, i, -1. * i * i);
        g5->SetPointError(i, 0, 0.01 * i);
        g5->SetName("Graph5");
    }

    //creating the list collection and fill the graphs into it

    TList list;
    list.Add(g1);
    list.Add(g2);
    list.Add(g3);
    list.Add(g4);
    list.Add(g5);

    //create file to save the collections
    TFile file("collections.root", "recreate");
    // singlekey creates own folder for the collections
    array.Write("histograms", TObject::kSingleKey);
    list.Write("Graphs", TObject::kSingleKey);
    file.Close();
}

void tokenization()
{
    //define string
    TString string("/data/run2/histos/2020/06/03/");
    cout << "directory: " << string << endl;

    // split the string apart by the delimiter "/"
    TObjArray *token = string.Tokenize("/");
    // At(i): returns object at index i
    cout << "Date: " << token->At(5)->GetName() << "." << token->At(4)->GetName() << "." << token->At(3)->GetName() << endl;

    string.ReplaceAll("histos", "graphs");
    cout << "new directory: " << string << endl;
}

void gstyling()
{
    TH1F *gaussian1 = new TH1F("gaussian1", "Gaussian Distribution 1;slot;counts", 200, 0, 5);
    TF1 gaussfunc("gaussfunc", "gaus", 0, 5); //gaussfunction
    gaussfunc.SetParameters(1, 3, 0.5);       // parameters (norm,mean,sigma)
    gaussian1->FillRandom("gaussfunc", 10000);
    gStyle->SetOptStat("neMour"); //

    //define the fit function as sum of two functions:
    TF1 fFit("fFit", "gaus", 0.35, 0.65);

    // Set some reasonable start parameters :(norm,mean, sigma)
    Fit.SetParameters(1, 3, 0.5);

    // Perform the fit
    gaussian1->Fit(&fFit);

    //define stat box position and parameters
    gStyle->SetOptFit(1);  //all parameters, else(1111),1011...
    gStyle->SetStatX(0.8); //default=0
    // gStyle->SetStaty(0.8);

    gaussian1->Draw("HIST");
}