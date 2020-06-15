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
#include <TRandom.h> // needed for gRandom
#include <TRandom3.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TStyle.h>

// day04
#include <TChain.h>
#include <TLorentzVector.h>
#include <TMath.h>
#include <TString.h>
#include <TTree.h>

#include <iostream> // used in both
#include <fstream>

using namespace std;

//#########################################################EXAMPLES
/*
// load with
.L chain.C+

// first create the trees to be chained:
createData();

//Then make a chain out of the trees and draw something
TChain *tree=chain();
TCanvas c;
c.Divide(2,2);
c.cd(1);
gPad->SetLogy();
tree->Draw("particle.Pt()");
c.cd(2);
TH1F hEta("hEta","Eta distribution;#eta;entries",30,-1.5,1.5);
tree->Draw("particle.Eta()>>hEta");
c.cd(3);
tree->Draw("particle.Pt():particle.Eta()>>hPtEta(25,-1.25,1.25,30,0,6)","","colz");
c.cd(4);
//draw using directly the data members
tree->Draw("particle.fP.fY:particle.fP.fX");

*/

TChain *chain(Int_t nFiles = 10)
{
    //
    // it is possible to 'chain' root file with trees which contain the same structure
    // The class use is a TChain which interits from TTree and thus has the same
    // functionality. A chain can be used like a tree
    //

    // create a chain and add all files which contain the particleTrees
    // NOTE: then name of the chain must be the name of the tree!
    //       otherwise the tree is not found inside the file
    TChain *c = new TChain("particleTreeee");
    // add all files to the chain
    for (Int_t ifile = 0; ifile < nFiles; ++ifile)
    {
        TString fileName("particleTree");
        fileName += ifile;
        fileName += ".root";
        c->AddFile(fileName.Data());
    }

    return c;
}

//_____________________________________________________________
void writeTree(Int_t fileNumber, Int_t nparticles)
{
    // write a tree into a file with fileNumber
    // create nparticles in this tree

    // First open the file to associate the tree with the file
    TString fileName("particleTree");
    fileName += fileNumber;
    fileName += ".root";

    TFile f(fileName.Data(), "recreate");

    TTree t("particleTreeee", "A tree with particles");
    TLorentzVector *v = new TLorentzVector;
    // create a branch to hold the TLorentzVector
    t.Branch("particle", &v);
    // crate a function that parametrises the transverse momentum distributions of pions
    TF1 fpt("fpt", Form("x*(1+(sqrt(x*x+%f^2)-%f)/([0]*[1]))^(-[0])*[2]", 0.14, 0.14), 0.3, 100);
    fpt.SetParameters(7.24, 0.120, 3);
    fpt.SetNpx(200);

    // create the particles and fill them in the tree
    for (Int_t i = 0; i < nparticles; ++i)
    {
        Double_t phi = gRandom->Uniform(0.0, 2 * TMath::Pi());
        Double_t eta = gRandom->Uniform(-1, 1);
        Double_t pt = fpt.GetRandom();
        v->SetPtEtaPhiE(pt, eta, phi, .14);
        t.Fill();
    }

    f.Write();
    f.Close();

    delete v;
}

//_____________________________________________________________
void createData(Int_t nFiles = 10)
{
    //
    // create 'nFiles' root files with particles trees
    //

    for (Int_t ifile = 0; ifile < nFiles; ++ifile)
    {
        writeTree(ifile, gRandom->Uniform(10000, 100000));
    }
}

// #########################################################EXERCISES

// 1-read and write
void write_data_tree()
{
    //open file
    TFile file("vertex_data.root", "recreate");

    //creating TTree and store vertex.dat in it
    TTree tree("extree", "extree");
    tree.ReadFile("../../example/day04/vertex.dat", "x:y:z");

    //write TTree into file
    tree.Write();

    //close file
    file.Close();
}

void read_data_tree()
{

    //only works, if vertex_data.root already created

    //read file
    TFile *file = new TFile("vertex_data.root");
    if (!file->IsOpen())
    {
        cout << "problems with file, exiting" << endl;
        return;
    }
    //retrieve the TTree inside using a casting
    TTree *tree = (TTree *)file->Get("extree");
    if (!tree)
    {
        cout << "no TTree found" << endl;
        return;
    }
    //print TTree
    tree->Print();

    //Drawing data in differnet ways
    TCanvas *c = new TCanvas();
    c->Divide(2, 2);
    c->cd(1);
    tree->Draw("x:y", "x>0", "colz");
    c->cd(2);
    tree->Draw("x:z:y", "", "colz");
    c->cd(3);
    tree->Draw("z:x", "z>0&&x>0", "surf2");
    c->cd(4);
    tree->Draw("log(x):y", "");
    c->SaveAs("data.png");
    //close the ROOT file
    file->Close();
}

void create_2_branches()
{

    // Create a tree with two branches

    // open file
    TFile file("two_branches.root", "recreate");
    // create a tree
    TTree tree("two_branch_tree", "two_branch_tree");

    // Branch 1: number
    Float_t randomgaus = 0;
    tree.Branch("randomGaus", &randomgaus);

    // Branch 2: TGraphs
    TGraph *Graph = new TGraph();
    tree.Branch("randomGraph", &Graph);

    Int_t nEntries = 5;
    //fill 50 branches
    for (Int_t i = 0; i < 50; ++i)
    {
        // Branch 1
        randomgaus = gRandom->Gaus();

        // generate the number of entries for one graph:
        //numEntries = (int)1 + 10 * gRandom->Uniform();

        //second branch
        for (Int_t j = 0; j < nEntries; ++j)
        {
            Graph->SetPoint(j, gRandom->Uniform(), gRandom->Uniform());
        }
        tree.Fill();
    }

    tree.Print();
    tree.Write();
    file.Close();
}

void read_branch_tree()
{

    //open file, get TTree
    TFile f("two_branches.root");
    if (!f.IsOpen())
    {
        cout << "problems with file, exiting" << endl;
        return;
    }
    TTree *t = (TTree *)f.Get("two_branch_tree");
    if (!t)
    {
        cout << "no TTree found" << endl;
        return;
    }

    //create the variables for the branches and set their address
    Float_t randomGaus = 0;
    t->SetBranchAddress("randomGaus", &randomGaus);
    TGraph *myGraph = new TGraph();
    t->SetBranchAddress("randomGraph", &myGraph);

    // loop over all the entries of the TTree
    for (Int_t i = 0; i < t->GetEntries(); ++i)
    {
        //load the entry
        t->GetEntry(i);
        //print the value
        cout << randomGaus << " ";
        // draw the graph -
        // note that in this example only the last one
        // will be displayed in the canvas
        myGraph->Draw("*ap");
    }

    cout << " " << endl;
    f.Close();
}

void tree_cahin(Int_t numtrees = 10)
{
    createData(numtrees);
    TChain *CHA=chain(numtrees);
    CHA->Draw("particle.Pt()");


}