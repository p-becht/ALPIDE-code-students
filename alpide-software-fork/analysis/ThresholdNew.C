#include "TH1F.h"
#include "TF1.h"
#include "TMath.h"
#include "TGraph.h"
#include "TString.h"
#include "TCanvas.h"

#include <stdio.h>
#include <fstream>
#include <iostream>

using namespace std;

int nInj       = 50;

float ChisqCut = 20;

int data[256];
int x   [256];
int NPoints;

char fNameOut [50];

int ELECTRONS_PER_DAC = 7; 

int NPixels;

int NNostart;
int NChisq;

TH1F *hThresh;

TH1F *hThresh1;
TH1F *hThresh2;
TH1F *hThresh3;
TH1F *hThresh4;

TH1F *hNoise;

TH1F *hNoise1;
TH1F *hNoise2;
TH1F *hNoise3;
TH1F *hNoise4;

TH1F *hChisq;


void PrepareHistos() {
  hThresh = new TH1F ("hThresh", "Threshold", 250, 0., 1000.);
  hThresh1 = new TH1F ("hThresh1", "Threshold, sector 1", 400, 0., 1600.);
  hThresh2 = new TH1F ("hThresh2", "Threshold, sector 2", 400, 0., 1600.);
  hThresh3 = new TH1F ("hThresh3", "Threshold, sector 3", 400, 0., 1600.);
  hThresh4 = new TH1F ("hThresh4", "Threshold, sector 4", 400, 0., 1600.);

  hNoise = new TH1F ("hNoise", "Noise", 30, 0., 20.);
  hNoise1 = new TH1F ("hNoise1", "Noise, sector 1", 90, 0., 60.);
  hNoise2 = new TH1F ("hNoise2", "Noise, sector 2", 90, 0., 60.);
  hNoise3 = new TH1F ("hNoise3", "Noise, sector 3", 90, 0., 60.);
  hNoise4 = new TH1F ("hNoise4", "Noise, sector 4", 90, 0., 60.);

  hChisq = new TH1F ("hChisq", "Chi square distribution", 1000, 0., 100.);
}


void ResetData() {
  for (int i=0; i <= 256; i++) {
    data[i] = 0;
  }
}


Double_t erf( Double_t *xx, Double_t *par){
  return (nInj / 2) *TMath::Erf((xx[0] - par[0]) / (sqrt(2) *par[1])) +(nInj / 2);
}


float FindStart (int NPoints, int *xx, int *data) {
  float Upper = -1;
  float Lower = -1;
 
  for (int i = 0; i < NPoints; i ++) {
    if (data[i] == nInj) Upper = (float) xx[i];
  }
  if (Upper == -1) return -1;
  for (int i = NPoints; i > 0; i--) {
    if (data[i] == 0) Lower = (float)xx[i];
  }
  if ((Lower == -1) || (Upper < Lower)) return -1;
  return (Upper + Lower)/2;
}

bool GetThreshold(double *AThreshold, double *ANoise, double *AChisq) {
   TGraph *g      = new TGraph(NPoints, x, data);
   TF1    *fitfcn = new TF1("fitfcn", erf,0,1500,2);
   float   Start  = FindStart(NPoints, x, data);
  
   if (Start < 0) {
     NNostart ++;
     return false;
   }

   fitfcn->SetParameter(0,Start);

   fitfcn->SetParameter(1,8);

   fitfcn->SetParName(0, "Threshold");
   fitfcn->SetParName(1, "Noise");
 
   //g->SetMarkerStyle(20);
   //g->Draw("AP");
   g->Fit("fitfcn","Q");

   *ANoise     = fitfcn->GetParameter(1);
   *AThreshold = fitfcn->GetParameter(0);
   *AChisq     = fitfcn->GetChisquare()/fitfcn->GetNDF();

   hChisq->Fill(*AChisq);

   g->Delete();
   fitfcn->Delete();
   return true;
}


void ProcessPixel (int col, int row) {
  double Threshold, Noise, Chisq;
  int    Sector = col / 128 +1;

  if ((col + row) % 100 == 0)
    std::cout << "processing pixel " << col << "/" << row << std::endl;
  if (!GetThreshold (&Threshold, &Noise, &Chisq)){
    //std::cout << "GetThreshold returned false" << std::endl;
     return;
  }

  FILE *fp = fopen(fNameOut, "a");
  fprintf(fp, "%d %d %.1f %.1f %.2f\n", col, row, (float)Threshold, (float)Noise, (float)Chisq);
  fclose(fp);

  hThresh->Fill(Threshold);

  switch (Sector) {
  case 1:
    hThresh1->Fill(Threshold);
    hNoise1->Fill(Noise);
    break;
  case 2:
    hThresh2->Fill(Threshold);
    hNoise2->Fill(Noise);
    break;
  case 3: 
    hThresh3->Fill(Threshold);
    hNoise3->Fill(Noise);
    break;
  case 4: 
    hThresh4->Fill(Threshold);
    hNoise4->Fill(Noise);
    break;
  }

  hNoise->Fill(Noise);
}



void ProcessFile (const char *fName) {
  FILE *fp = fopen (fName, "r");
  int col, address, ampl, hits;
  int lastcol = -1, lastaddress = -1;
  NPoints  = 0;
  NPixels  = 0;
  NNostart = 0;
  NChisq   = 0;

  //printf("strstr result: %s\n", strstr(
  sprintf(fNameOut, "FitValues%s", strstr(fName,"_"));
  printf("Output file: %s\n", fNameOut);
  ResetData();
  while ((fscanf (fp, "%d %d %d %d", &col, &address, &ampl, &hits) == 4)) {

    //if ((col < 255) || ((col == 255) && (address < 280))) continue;

    if (((lastcol != col) || (address != lastaddress)) && (NPoints!= 0)) {
      ProcessPixel(lastcol, lastaddress);
      NPixels ++;
      ResetData   ();
      NPoints  = 0;
    }

    lastcol = col;
    lastaddress = address;
    data [NPoints] = hits;
    x    [NPoints] = ampl * ELECTRONS_PER_DAC;
    NPoints ++;
  }
  fclose(fp);
}


int ThresholdNew(const char *fName, bool WriteToFile =false, int ITH = 0, int VCASN = 0, bool saveCanvas = false) {
  float NPix   [25];
  float Thresh [25];
  float threshold, noise;

  PrepareHistos();
  ProcessFile(fName);

  std::cout << "Found " << NPixels << " pixel." << std::endl;
  std::cout << "No start point found: " << NNostart << std::endl;
  std::cout << "Chisq cut failed:     " << NChisq << std::endl;

  //  hThresh->Draw();
  hThresh2->SetLineColor(kBlue);
  hThresh3->SetLineColor(kRed);
  hThresh4->SetLineColor(kGreen);

  hNoise2->SetLineColor(kBlue);
  hNoise3->SetLineColor(kRed);
  hNoise4->SetLineColor(kGreen);

  hThresh1->SetMaximum(150);
  hNoise1->SetMaximum(500);

  TCanvas* c = new TCanvas;

  hThresh1->Draw();
  hThresh2->Draw("SAME");
  hThresh3->Draw("SAME");
  hThresh4->Draw("SAME");

  if (saveCanvas)
    c->SaveAs("threshold_scan.png");

  std::cout << "Threshold sector 1: " << hThresh1->GetMean() << " +- " << hThresh1->GetRMS() << std::endl;
  std::cout << "Threshold sector 2: " << hThresh2->GetMean() << " +- " << hThresh2->GetRMS() << std::endl;
  std::cout << "Threshold sector 3: " << hThresh3->GetMean() << " +- " << hThresh3->GetRMS() << std::endl;
  std::cout << "Threshold sector 4: " << hThresh4->GetMean() << " +- " << hThresh4->GetRMS() << std::endl;

  std::cout << "Noise sector 1: " << hNoise1->GetMean() << " +- " << hNoise1->GetRMS() << std::endl;
  std::cout << "Noise sector 2: " << hNoise2->GetMean() << " +- " << hNoise2->GetRMS() << std::endl;
  std::cout << "Noise sector 3: " << hNoise3->GetMean() << " +- " << hNoise3->GetRMS() << std::endl;
  std::cout << "Noise sector 4: " << hNoise4->GetMean() << " +- " << hNoise4->GetRMS() << std::endl;

  if (WriteToFile) {
      FILE *fp = fopen("ThresholdSummary.dat", "a");

      fprintf(fp, "1 %d %d %.1f %.1f %.1f %.1f\n", ITH, VCASN, hThresh1->GetMean(), hThresh1->GetRMS(), 
	  hNoise1->GetMean(), hNoise1->GetRMS());
      fprintf(fp, "2 %d %d %.1f %.1f %.1f %.1f\n", ITH, VCASN, hThresh2->GetMean(), hThresh2->GetRMS(), 
	  hNoise2->GetMean(), hNoise2->GetRMS());
      fprintf(fp, "3 %d %d %.1f %.1f %.1f %.1f\n", ITH, VCASN, hThresh3->GetMean(), hThresh3->GetRMS(), 
	  hNoise3->GetMean(), hNoise3->GetRMS());
      fprintf(fp, "4 %d %d %.1f %.1f %.1f %.1f\n", ITH, VCASN, hThresh4->GetMean(), hThresh4->GetRMS(), 
    	  hNoise4->GetMean(), hNoise4->GetRMS());

      fclose(fp);
  }


  //hChisq->Draw();
  return 0;
}
