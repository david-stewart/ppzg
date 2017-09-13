{
  gStyle->SetOptStat(0);
  gStyle->SetHistLineWidth(2);

  TFile * ppfile = new TFile("Results/ForUnfolding_ForPaper_Pp12_JP2_NoEff_NoBgSub.root");
  TFile * geantfile = new TFile("Results/AEff0_PtSmear0_ATow0_SystGeant12_NoEff_NoBg_JP2_WithMisses_WithFakes_TrainedWith_McGeant12_NoEff_NoBg_all.root");

  // TFile * ppfile = new TFile("Results/ForUnfolding_Recut_Pp_HT54_NoEff_NoBgSub.root");
  // TFile * geantfile = new TFile("Results/AEff0_PtSmear0_ATow0_SystGeant_NoEff_NoBg_HT54_WithMisses_WithFakes_TrainedWith_Recut_McGeant_NoEff_NoBg_MB.root");

  TH2D* pp2d = (TH2D*) ppfile->Get("IncMeas2D");
  pp2d->SetName("pp2d");
  
  TH2D* geant2d = (TH2D*) geantfile->Get("IncMeas2D");
  geant2d->SetName("geant2d");

  double Incbins[] = { 10, 15, 20, 25, 30, 40, 60};
  int nIncbins  = sizeof(Incbins) / sizeof(Incbins[0])-1;

  // TH1D* dummy = new TH1D( "dummy","", 20, 0.05, 0.55);
  TH1D* dummy = new TH1D( "dummy","", 22, -0.05, 0.55);
  dummy->SetAxisRange(0, 9, "y");

  for ( int i=0 ; i<nIncbins ; ++i ){
    float ptleft  = Incbins[i];
    float ptright = Incbins[i+1];

    TString name = "ppmeas_"; name += int(ptleft+0.01); name += int(ptright+0.01);
    TH1D* ppmeas =
      (TH1D*) pp2d->ProjectionY(name, pp2d->GetXaxis()->FindBin( ptleft+0.01), pp2d->GetXaxis()->FindBin( ptright-0.01));

    TH1D* h=ppmeas;
    h->Rebin(2);
    h->Scale ( 1./ h->Integral(h->FindBin(0.1+0.001),h->FindBin(0.5-0.001)) / h->GetXaxis()->GetBinWidth(1));

    TString name = "geantmeas_"; name += int(ptleft+0.01); name += int(ptright+0.01);
    TH1D* geantmeas =
      (TH1D*) geant2d->ProjectionY(name, geant2d->GetXaxis()->FindBin( ptleft+0.01), geant2d->GetXaxis()->FindBin( ptright-0.01));

    TH1D* h=geantmeas;
    h->Rebin(2);
    h->Scale ( 1./ h->Integral(h->FindBin(0.1+0.001),h->FindBin(0.5-0.001)) / h->GetXaxis()->GetBinWidth(1));

    new TCanvas;
    dummy->DrawCopy();
    // ppmeas->SetLineColor(kRed); ppmeas->Draw("9same");
    ppmeas->SetLineColor(kRed); ppmeas->Draw("9");
    geantmeas->SetLineColor(kBlack); geantmeas->Draw("9same");
  }

  
}

