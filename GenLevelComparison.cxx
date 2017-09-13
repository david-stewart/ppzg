{
  gStyle->SetOptStat(0);
  gStyle->SetHistLineWidth(2);


  TFile * p6file = new TFile("Results/AEff0_PtSmear0_ATow0_SystGeant_NoEff_NoBg_HT54_WithMisses_WithFakes_TrainedWith_Recut_McGeant_NoEff_NoBg_MB.root");
  TH2D* p62d = (TH2D*) p6file->Get("IncTruth2D");
  p62d->SetName("p62d");

  TFile * p8file = new TFile("Results/Prepped_ForPaper_Pythia8_NoEff_NoBgSub.root");
  TH2D* p82d = (TH2D*) p8file->Get("IncMeas2D");
  p82d->SetName("p82d");

  double Incbins[] = { 10, 15, 20, 25, 30, 40, 60};
  int nIncbins  = sizeof(Incbins) / sizeof(Incbins[0])-1;

  TH1D* dummy = new TH1D( "dummy","", 20, 0.05, 0.55);
  dummy->SetAxisRange(0, 9, "y");

  for ( int i=0 ; i<nIncbins ; ++i ){
    float ptleft  = Incbins[i];
    float ptright = Incbins[i+1];

    TString name = "p6meas_"; name += int(ptleft+0.01); name += int(ptright+0.01);
    TH1D* p6meas =
      (TH1D*) p62d->ProjectionY(name, p62d->GetXaxis()->FindBin( ptleft+0.01), p62d->GetXaxis()->FindBin( ptright-0.01));

    TH1D* h=p6meas;
    h->Rebin(2);
    h->Scale ( 1./ h->Integral(h->FindBin(0.1+0.001),h->FindBin(0.5-0.001)) / h->GetXaxis()->GetBinWidth(1));

    TString name = "p8meas_"; name += int(ptleft+0.01); name += int(ptright+0.01);
    TH1D* p8meas =
      (TH1D*) p82d->ProjectionY(name, p82d->GetXaxis()->FindBin( ptleft+0.01), p82d->GetXaxis()->FindBin( ptright-0.01));

    TH1D* h=p8meas;
    h->Rebin(2);
    h->Scale ( 1./ h->Integral(h->FindBin(0.1+0.001),h->FindBin(0.5-0.001)) / h->GetXaxis()->GetBinWidth(1));

    new TCanvas;
    dummy->DrawCopy();
    p6meas->SetLineColor(kRed); p6meas->Draw("9same");
    p8meas->SetLineColor(kBlack); p8meas->Draw("9same");
  }

  
}

