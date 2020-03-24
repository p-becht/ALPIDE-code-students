const Color_t wcl[] = { kBlack,  kRed+1,  kBlue+1, kGreen+3, kMagenta+1, kOrange-1, kCyan+2, kYellow+2 };
const Color_t wcf[] = { kGray+1, kRed-10, kBlue-9, kGreen-8, kMagenta-9, kOrange-9, kCyan-8, kYellow-7 };
const Style_t wmk[] = { kFullCircle, kFullSquare, kOpenCircle, kOpenSquare, kOpenDiamond, kOpenCross, kFullCross, kFullDiamond, kFullStar, kOpenStar};
//=============================================================================

void DrawAliLogo (Double_t dXmin, Double_t dYmin, Int_t ip=24, Bool_t bPre=kTRUE)
{
  TLatex *t = new TLatex(dXmin, dYmin, (bPre ? "ALICE Preliminary" : "ALICE"));
  t->SetNDC();
  t->SetTextFont(42);
  t->SetTextSizePixels(ip);
  t->Draw();

  return;
}

//_____________________________________________________________________________
void DrawGraph(TGraph *g, Color_t wc, Option_t *opt)
{
  g->SetLineWidth(2);
  g->SetLineColor(wc);
  g->SetFillStyle(0);
  g->SetFillColor(g->GetLineColor());
  g->Draw(opt);

  return;
}

//_____________________________________________________________________________
void DrawGraph(TGraph *g, Color_t wc, Style_t ws, Option_t *opt)
{
  g->SetLineWidth(2);
  g->SetLineColor(wc);
  g->SetMarkerStyle(ws);
  g->SetMarkerColor(wc);
  g->SetFillStyle(0);
  g->Draw(opt);

  return;
}

//_____________________________________________________________________________
void DrawGraph(TGraphErrors *g, Color_t wc, Option_t *opt)
{
  g->SetLineWidth(2);
  g->SetLineColor(wc);
  g->SetFillStyle(0);
  g->SetFillColor(g->GetLineColor());
  g->Draw(opt);

  return;
}

//_____________________________________________________________________________
void DrawGraph(TGraphErrors *g, Color_t wc, Style_t ws, Option_t *opt)
{
  g->SetLineWidth(2);
  g->SetLineColor(wc);
  g->SetMarkerStyle(ws);
  g->SetMarkerColor(wc);
  g->SetFillStyle(0);
  g->Draw(opt);

  return;
}

//_____________________________________________________________________________
void DrawGraph(TGraphAsymmErrors *g, Color_t wc, Option_t *opt)
{
  g->SetLineWidth(2);
  g->SetLineColor(wc);
  g->SetFillStyle(0);
  g->SetFillColor(g->GetLineColor());
  g->Draw(opt);

  return;
}

//_____________________________________________________________________________
void DrawGraph(TGraphAsymmErrors*g, Color_t wc, Style_t ws, Option_t *opt)
{
  g->SetLineWidth(2);
  g->SetLineColor(wc);
  g->SetMarkerStyle(ws);
  g->SetMarkerColor(wc);
  g->SetFillStyle(0);
  g->Draw(opt);

  return;
}

//_____________________________________________________________________________
void DrawHisto(TH1D *h, Color_t wc, Option_t *opt)
{
  h->SetLineWidth(2);
  h->SetLineColor(wc);
  h->SetMarkerSize(0);
  h->SetFillStyle(0);
  h->DrawCopy(opt);

  return;
}

//_____________________________________________________________________________
void DrawHisto(TH1D *h, Color_t wc, Style_t ws, Option_t *opt)
{
  h->SetLineWidth(2);
  h->SetLineColor(wc);
  h->SetMarkerStyle(ws);
  h->SetMarkerColor(wc);
  h->SetFillStyle(0);
  h->DrawCopy(opt);

  return;
}

//_____________________________________________________________________________
void SetupFrame(TH1F *f, const Float_t dlsx,    const Float_t dlsy,
                         const Float_t dtsx,    const Float_t dtsy,
                         const Float_t dtox,    const Float_t dtoy,
                         const TString stnx="", const TString stny="")
{
  f->GetXaxis()->SetLabelSize(dlsx);
  f->GetYaxis()->SetLabelSize(dlsy);

  f->GetXaxis()->SetTitleSize(dtsx);
  f->GetYaxis()->SetTitleSize(dtsy);

  f->GetXaxis()->SetTitleOffset(dtox);
  f->GetYaxis()->SetTitleOffset(dtoy);

  f->SetXTitle(stnx.Data());
  f->SetYTitle(stny.Data());

  f->GetYaxis()->SetNdivisions(505);

  return;
}

//_____________________________________________________________________________
void SetupLegend(TLegend *l)
{
  l->SetFillColor(0);
  l->SetFillStyle(0);
  l->SetBorderSize(0);
  l->SetTextSize(0.8*gStyle->GetTextSize());

  return;
}

//_____________________________________________________________________________
void SetUpPad(TPad *p, Float_t dlm, Float_t drm, Float_t dtm, Float_t dbm)
{
  p->SetLeftMargin(dlm);
  p->SetRightMargin(drm);
  p->SetTopMargin(dtm);
  p->SetBottomMargin(dbm);

  return;
}

//_____________________________________________________________________________
void SetStyle(Bool_t bGraph=kFALSE)
{
  gStyle->Reset("Plain");
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);

  gStyle->SetPalette(1);
  if(bGraph) gStyle->SetPalette(8,0);

  gStyle->SetCanvasColor(10);
  gStyle->SetCanvasBorderMode(0);

  gStyle->SetFrameLineWidth(1);
  gStyle->SetFrameFillColor(kWhite);

  gStyle->SetPadColor(10);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadLeftMargin(0.15);

  gStyle->SetHistLineWidth(1);
  gStyle->SetHistLineColor(kRed);

  gStyle->SetFuncWidth(2);
  gStyle->SetFuncColor(kGreen);

  gStyle->SetLineWidth(2);

  gStyle->SetLabelSize(0.045,"XYZ");
  gStyle->SetLabelOffset(0.01,"X");
  gStyle->SetLabelOffset(0.01,"Y");
  gStyle->SetLabelColor(kBlack,"XYZ");

  gStyle->SetTitleSize(0.05,"XYZ");
  gStyle->SetTitleOffset(1.2,"X");
  gStyle->SetTitleOffset(1.25,"Y");
  gStyle->SetTitleFillColor(kWhite);

  gStyle->SetTextFont(42);
  gStyle->SetTextSizePixels(24);

//gStyle->SetTickLength(0.04,"X");
//gStyle->SetTickLength(0.04,"Y");

  gStyle->SetLegendBorderSize(0);
  gStyle->SetLegendFillColor(kWhite);
  gStyle->SetLegendFont(42);

//gStyle->SetFillColor(kWhite);

  return;
}
