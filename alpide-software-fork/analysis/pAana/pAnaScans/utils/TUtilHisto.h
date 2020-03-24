TH1D* MakeRebinTH1D(TH1D const *hRaw, TH1D const *hRef)
{
  if ((!hRaw) || (!hRef)) return 0x0;

  const Int_t nRef = hRef->GetNbinsX();

  const Double_t dLower = hRef->GetXaxis()->GetBinLowEdge(1);
  const Double_t dUpper = hRef->GetXaxis()->GetBinUpEdge(nRef);

  TH1D *hNew = hRef->Clone(Form("h_%s_%s", hRaw->GetName(),
                                           hRef->GetName()));
  hNew->Reset();
//=============================================================================

  for (Int_t k=1; k<=hRaw->GetNbinsX(); k++) {
    Double_t dXvar = hRaw->GetBinCenter(k);
    if ((dXvar<dLower) || (dXvar>=dUpper)) continue;

    Int_t iBinX = hNew->FindBin(dXvar);
    Double_t dYvar = hRaw->GetBinContent(k);
    Double_t dYerr = hRaw->GetBinError(k);
    Double_t dYsw2 = hNew->GetBinError(iBinX);

    hNew->SetBinContent(iBinX, hNew->GetBinContent(iBinX) + dYvar);
    hNew->SetBinError(iBinX, TMath::Sqrt(dYsw2*dYsw2 + dYerr*dYerr));
  }

  return hNew;
}

//_____________________________________________________________________________
void NormBinningHistogram(TH1D *h, Double_t dw=1.)
{
  if (!h) return;
//=============================================================================

  for (Int_t k=1; k<=h->GetNbinsX(); k++) {
    Double_t dBW = h->GetBinWidth(k) * dw;
    h->SetBinContent(k, h->GetBinContent(k)/dBW);
    h->SetBinError(k, h->GetBinError(k)/dBW);
  }

  return;
}

//_____________________________________________________________________________
void DeNormBinningHistogram(TH1D* h)
{
  if (!h) return;
//=============================================================================

  for (Int_t k=1; k<=h->GetNbinsX(); k++) {
    Double_t dBW = h->GetBinWidth(k);
    h->SetBinContent(k, h->GetBinContent(k) * dBW);
    h->SetBinError(k, h->GetBinError(k) * dBW);
  }

  return;
}

//_____________________________________________________________________________
void CalcRelDeviation(TH1D *hi, TH1D *hr)
{
  if ((!hi) || (!hr)) return;
//=============================================================================

  for (Int_t k=1; k<=hr->GetNbinsX(); k++) {
    Double_t dVarI = hi->GetBinContent(k); if (TMath::Abs(dVarI)<1e-12) dVarI = 1e-12;
    Double_t dErrI = hi->GetBinError(k) / dVarI;

    Double_t dVarR = hr->GetBinContent(k); if (TMath::Abs(dVarR)<1e-12) dVarR = 1e-12;
    Double_t dErrR = hr->GetBinError(k) / dVarR;

    Double_t dVarD = TMath::Abs(dVarI/dVarR - 1.);
    Double_t dErrD = dVarD * TMath::Sqrt(dErrI*dErrI + dErrR*dErrR);
    hi->SetBinContent(k, dVarD); hi->SetBinError(k, dErrD);
  }

  return;
}

//_____________________________________________________________________________
TH1D* MaxHistograms(const Int_t n, TH1D **h)
{
  if ((n<=0) || (!h[0])) return 0x0;
//=============================================================================

  Double_t *d = new Double_t[n];
  TH1D *hMax = (TH1D*)h[0]->Clone("hMax");
  for (Int_t k=1; k<=hMax->GetNbinsX(); k++) {
    for (Int_t j=0; j<n; j++) d[j] = h[j]->GetBinContent(k);
    hMax->SetBinContent(k, TMath::MaxElement(n,d));
    hMax->SetBinError(k, TMath::RMS(n,d));
  }

  return hMax;
}

//_____________________________________________________________________________
TH1D* QuadraticSum(const Int_t n, TH1D **h)
{

  if ((n<=0) || (!h)) return 0x0;
//=============================================================================

  TH1D *hSum = (TH1D*)h[0]->Clone("hSum");
  for (Int_t k=1; k<=hSum->GetNbinsX(); k++) {
    Double_t dSum = 0.;
    for (Int_t j=0; j<n; j++) {
      Double_t d = h[j]->GetBinContent(k);
      dSum += (d*d);
    }

    hSum->SetBinContent(k, TMath::Sqrt(dSum));
  }

  return hSum;
}

//_____________________________________________________________________________
TH1D *ConvRelError(TH1D *hi, TH1D *hr)
{
  TH1D *hc = (TH1D*)hi->Clone(Form("h_%s_%s",hi->GetName(),hr->GetName()));
  for (Int_t k=1; k<=hc->GetNbinsX(); k++) hc->SetBinError(k, hi->GetBinContent(k) * hr->GetBinContent(k));

  return hc;
}

//_____________________________________________________________________________
TGraphErrors* ConvHistogramToGraphErrors(TH1D *hVar, TH1D *hErr, const Int_t n)
{
  Double_t *dtm = new Double_t[n];

  Double_t dvx[n], dvy[n];
  Double_t dex[n], dey[n];
  for (Int_t i=0, k=1; i<n; i++, k++) {
    dvx[i] = hVar->GetBinCenter(k);
    dex[i] = hVar->GetBinWidth(k) / 2.;

    if (dex[i]>0.2) dex[i] = 0.1;

    dvy[i] = hVar->GetBinContent(k);
    if (hErr) {
      dey[i] = hErr->GetBinContent(k) * dvy[i];
    } else {
      dey[i] = hVar->GetBinError(k);
    }
  }

  TGraphErrors *g = new TGraphErrors(n, dvx, dvy, dex, dey);

  return g;
}

//_____________________________________________________________________________
TGraphErrors* ConvHistogramToGraphErrorL(TH1D *hVar, TH1D *hErr, const Int_t n)
{
  Double_t *dtm = new Double_t[n];

  Double_t dvx[n], dvy[n];
  Double_t dex[n], dey[n];
  for (Int_t i=0, k=1; i<n; i++, k++) {
    dvx[i] = hVar->GetBinCenter(k);
    dex[i] = hVar->GetBinWidth(k) / 2.;

    if (dex[i]>0.3) dvx[i] -= 0.2;
    if (dex[i]>0.2) dex[i]  = 0.1;

    dvy[i] = hVar->GetBinContent(k);

    if (hErr) {
      dey[i] = hErr->GetBinContent(k) * dvy[i];
    } else {
      dey[i] = hVar->GetBinError(k);
    }
  }

  TGraphErrors *g = new TGraphErrors(n, dvx, dvy, dex, dey);

  return g;
}

//_____________________________________________________________________________
TGraphErrors* ConvHistogramToGraphErrorR(TH1D *hVar, TH1D *hErr, const Int_t n)
{
  Double_t *dtm = new Double_t[n];

  Double_t dvx[n], dvy[n];
  Double_t dex[n], dey[n];
  for (Int_t i=0, k=1; i<n; i++, k++) {
    dvx[i] = hVar->GetBinCenter(k);
    dex[i] = hVar->GetBinWidth(k) / 2.;

    if (dex[i]>0.3) dvx[i] += 0.2;
    if (dex[i]>0.2) dex[i]  = 0.1;

    dvy[i] = hVar->GetBinContent(k);

    if (hErr) {
      dey[i] = hErr->GetBinContent(k) * dvy[i];
    } else {
      dey[i] = hVar->GetBinError(k);
    }
  }

  TGraphErrors *g = new TGraphErrors(n, dvx, dvy, dex, dey);

  return g;
}

//_____________________________________________________________________________
TGraphAsymmErrors* ConvHistogramToGraphErrors(TH1D *hVar, TH1D *hErl, TH1D *hErh)
{
  Int_t n = hVar->GetNbinsX();
  const Int_t m = n;

  Double_t *dtm = new Double_t[m];
  Double_t dvx[m], dlx[m], dhx[m];
  Double_t dvy[m], dly[m], dhy[m];


  for (Int_t i=0, k=1; i<n; i++, k++) {
    dvx[i] = hVar->GetBinCenter(k);
    dlx[i] = 0.05; // hVar->GetBinWidth(k) / 2.;
    dhx[i] = 0.05; // hVar->GetBinWidth(k) / 2.;

    dvy[i] = hVar->GetBinContent(k);
    dly[i] = hErl->GetBinContent(k) * dvy[i];
    dhy[i] = hErh->GetBinContent(k) * dvy[i];
  }

  TGraphAsymmErrors *g = new TGraphAsymmErrors(n, dvx, dvy, dlx, dhx, dly, dhy);

  return g;
}

//_____________________________________________________________________________
TGraphErrors* ConvHistogramToGraphErrors(TH1D *hVar, const Int_t n)
{
  Double_t *dtm = new Double_t[n];
  Double_t dvx[n], dex[n];
  Double_t dvy[n], dey[n];

  for (Int_t i=0, k=1; i<n; i++, k++) {
    dvx[i] = hVar->GetBinCenter(k);
    dex[i] = 0.05;

    dvy[i] = hVar->GetBinContent(k);
    dey[i] = hVar->GetBinError(k);
  }

  TGraphErrors *g = new TGraphErrors(n, dvx, dvy, dex, dey);

  return g;
}
