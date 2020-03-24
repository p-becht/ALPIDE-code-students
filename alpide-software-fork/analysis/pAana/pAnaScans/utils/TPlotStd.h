TCanvas* MakeCanvas(TString sName)
{
  TCanvas *c = new TCanvas(Form("c%s",sName.Data()), sName.Data(), 700, 500);
  c->Range(0., 0., 1., 1.);
  c->SetFillColor(0);
  c->SetBorderMode(0);
  c->SetBorderSize(0);
  c->SetRightMargin(0.03);
  c->SetLeftMargin(0.13);
  c->SetTopMargin(0.02);
  c->SetBottomMargin(0.15);
  c->SetFrameFillStyle(0);
  c->SetFrameBorderMode(0);

  return c;
}

//_____________________________________________________________________________
void CanvasEnd(TCanvas *c)
{
  if (!c) return;

  c->Modified();
  c->cd();
  c->SetSelected(c);

  return;
}
