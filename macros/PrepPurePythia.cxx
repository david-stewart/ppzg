int PrepPurePythia(){

  TString InName = "Results/Prepped_ForPaper_Pythia8_NoEff_NoBgSub.root";
  TString OutName = InName;
  OutName.ReplaceAll("Prepped","CutUp");

  // Load pp data
  // ------------
  TFile* fmeas = new TFile( InName, "READ");  
  TH2D* IncTestMeas2D  = (TH2D*) fmeas->Get("IncTestMeas2D");
  
  // Output
  // ------
  TFile* fout = new TFile( OutName, "RECREATE");
    
  // pT
  // --
  int rebinpt=2;
  TH1D* Incmeaspt = (TH1D*) IncTestMeas2D->ProjectionX("p8_Incmeaspt");
  Incmeaspt->SetLineColor( kMagenta+2 );
  Incmeaspt->Rebin( rebinpt );
  Incmeaspt->SetTitle("");
  
  // zg
  // --
  double Incbins[] = { 10, 15, 20, 25, 30, 40, 60};
  int nIncbins  = sizeof(Incbins) / sizeof(Incbins[0])-1;

  for ( int i=0 ; i<nIncbins ; ++i ){
    float ptleft  = Incbins[i];
    float ptright = Incbins[i+1];

    TString name = "p8_Incmeas_"; name += int(ptleft+0.01); name += int(ptright+0.01);
    TH1D* Incmeas =
      (TH1D*) IncTestMeas2D->ProjectionY(name, IncTestMeas2D->GetXaxis()->FindBin( ptleft+0.01), IncTestMeas2D->GetXaxis()->FindBin( ptright-0.01));

    TH1D* h=Incmeas;
    h->Rebin(2);
    h->Scale ( 1./ h->Integral(h->FindBin(0.1+0.001),h->FindBin(0.5-0.001)) / h->GetXaxis()->GetBinWidth(1));
  }

  fout->Write();
  return 0;
}
