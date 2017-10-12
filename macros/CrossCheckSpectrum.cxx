int CrossCheckSpectrum( TString n="2" )
{

  gStyle->SetHistLineWidth(2);
  gStyle->SetFrameLineWidth(2);
  gStyle->SetOptStat(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);

  gStyle->SetTitleX(0.1f);
  gStyle->SetTitleW(0.8f);
  gStyle->SetTitleBorderSize(0);	//Title box border thickness

  
  TString Defr12data = "  Results/FinePtBinning_Default.root";
  TString Larger12data = "  Results/Unfolded_ForUnfolding_R0.6_ForPaper_Pp12_JP2_NoEff_NoBgSub__With_AEff0_PtSmear0_ATow0_R0.6_SystGeant12_NoEff_NoBg_JP2_WithMisses_WithFakes_TrainedWith_R0.6_McGeant12_NoEff_NoBg_all.root";

  // TString nP8 = "Results/CutUp_ForPaper_Pythia8_NoEff_NoBgSub.root";
  TString nP8 = "Results/Prepped_ForPaper_Pythia8_NoEff_NoBgSub.root";

  TFile* Deff12 = new TFile( Defr12data, "READ");
  TH1D* Defdata = (TH1D*) Deff12->Get("Incunfoldpt_" + n);
  TH1D* p6 = (TH1D*) Deff12->Get("IncTrainTruthpt");
  
  TFile* Largerf12 = new TFile( Larger12data, "READ");
  TH1D* Largerdata = (TH1D*) Largerf12->Get("Incunfoldpt_" + n);

  TFile* fP8 = new TFile( nP8, "READ");
  TH2D* pdata2d = (TH2D*) fP8->Get("PythiaTruth2D");
  TH1D* pdata = (TH1D*) pdata2d->ProjectionX("ppdata");
  // pdata->Rebin(2);


  float normleft = 20;
  float normright = 35;

  Defdata->Scale ( 1./Defdata->Integral ( Defdata->FindBin(normleft+0.001), Defdata->FindBin(normright-0.001) ) );
  Largerdata->Scale ( 1./Largerdata->Integral ( Largerdata->FindBin(normleft+0.001), Largerdata->FindBin(normright-0.001) ) );
  pdata->Scale ( 1./pdata->Integral ( pdata->FindBin(normleft+0.001), pdata->FindBin(normright-0.001) ) );
  p6->Scale ( 1./p6->Integral ( p6->FindBin(normleft+0.001), p6->FindBin(normright-0.001) ) );
  
  new TCanvas;
  gPad->SetLogy();

  Defdata->SetLineColor(kRed+1);
  Largerdata->SetLineColor(kBlack);
  pdata->SetLineColor(kGreen+1);
  p6->SetLineColor(kBlue);

  Defdata->DrawCopy("9");
  pdata->DrawCopy("9same");
  p6->DrawCopy("9same");
  gPad->SaveAs(TString ("plots/") + Defdata->GetName() + "_andPythia8.png");
  
  TH1D* ratio = Defdata->Clone("ratio");
  ratio->Divide(pdata);

  TH1D* r6 = Defdata->Clone("r6");
  r6->Divide(p6);

  new TCanvas;
  ratio->SetLineColor( pdata->GetLineColor() );
  r6->SetLineColor( p6->GetLineColor() );
    
  ratio->SetAxisRange( 0,60,"x");
  ratio->SetAxisRange( 0.4,1.6,"y");
  ratio->DrawCopy("9");
  r6->DrawCopy("9same");
  
  gPad->SaveAs(TString ("plots/") + Defdata->GetName() + "_RatioToPythia8.png");  


  // Now cut up into Kevin's bins
  // orig: 6.1, 7.0, 8.4, 9.9, 11.7, 13.8, 16.3, 19.2, 22.7, 26.8, 31.6
  // approximate to
  const double bins[] = {6, 7, 8, 10, 12, 14, 16, 19, 23, 27, 32};
  int nbins = sizeof(bins)/sizeof(bins[0]) -1 ;

  TH1D* rebinned = Defdata->Rebin( nbins, "rebinned", bins );

  for (int i=1; i<=nbins; ++i ){
    // cout << i << " " << rebinned->GetBinLowEdge(i) << " -- " << rebinned->GetBinLowEdge(i+1) << endl;
    float dpt = rebinned->GetBinWidth(i);
    rebinned->SetBinContent(i, rebinned->GetBinContent(i)/dpt );
    rebinned->SetBinError(i, rebinned->GetBinError(i)/dpt );
  }

  
  new TCanvas;
  gPad->SetLogy();
  rebinned->SetLineColor(kMagenta+1);
  rebinned->Scale ( 0.28 / rebinned->GetBinContent(1) );
  //rebinned->DrawCopy("9");

  
  TH1D* dummy = new TH1D("dummy","",100, 5, 37.2);
  dummy->SetAxisRange( 1e-6, 1e1,"y");
  dummy->DrawCopy("9");
  rebinned->DrawCopy("9same");
  // Defdata->DrawCopy("9same");
  gPad->SaveAs("kevin.png");
  
  
  
  // Published spectrum
  // ------------------
  // from https://drupal.star.bnl.gov/STAR/publications/longitudinal-double-spin-asymmetry-and-cross-section-inclusive-jet-production-polarized-proton-collisions-sqrt-s-20

  double mbpt[5] = { 5.5,     6.8,     8.3,     10.3,    12.6};
  double mbv[5]  = { 9.30E+06,2.70E+06,6.70E+05,1.84E+05,5.10E+04};
  double mbe[5]  = { 3.00E+05,1.00E+05,5.00E+04,2.40E+04,1.10E+04};

  double htpt[9] = {   8.3,     10.3,    12.6,    15.5,    19,       23.4,  28.7,      35.3,  43.3 };
  double htv[9]  = { 6.40E+05,2.40E+05,5.90E+04,1.19E+04,3.50E+03,7.80E+02,1.41E+02,2.20E+01,2.60E+00};
  double hte[9]  = { 9.00E+04,3.00E+04,6.00E+03,1.00E+03,2.00E+02,4.00E+01,9.00E+00,3.00E+00,6.00E-01};

  TGraphErrors *mb = new TGraphErrors (5, mbpt, mbv, 0, mbe);
  TGraphErrors *ht = new TGraphErrors (9, htpt, htv, 0, hte);

  new TCanvas;
  gPad->SetLogy();
  TH1D* h = new TH1D( "h","",100, 0, 50);
  h->SetAxisRange( 1E-1,1E8,"y");
  h->DrawCopy("axis");
  
  mb->SetMarkerColor(kGray+1);
  mb->SetMarkerStyle(21);
  mb->Draw("LP");
  
  ht->SetMarkerColor(kGray+1);
  ht->SetMarkerStyle(20);
  ht->Draw("LPsame");

  int scalebin = 5;
  TH1D* DefR6comp=(TH1D*) Defdata->Clone("DefR6comp");
  DefR6comp->Scale ( htv[scalebin] / DefR6comp->GetBinContent( DefR6comp->FindBin( htpt[scalebin] ) ) );
  DefR6comp->SetLineColor( kGreen+1 );
  DefR6comp->DrawCopy("9same");

  TH1D* LargerR6comp=(TH1D*) Largerdata->Clone("LargerR6comp");
  LargerR6comp->Scale ( htv[scalebin] / LargerR6comp->GetBinContent( LargerR6comp->FindBin( htpt[scalebin] ) ) );
  LargerR6comp->SetLineColor( kRed );
  LargerR6comp->DrawCopy("9same");

  TLegend* leg;
  leg = new TLegend( 0.3, 0.6, 0.88, 0.88, "Comparison to Run6" );
  leg->SetBorderSize(0);
  leg->SetTextFont(43);
  leg->SetFillStyle(0);

  leg->AddEntry ( mb,"Run6 MB, Cone, R=0.5","lp");
  leg->AddEntry ( ht,"Run6 HT, Cone, R=0.5","lp");
  leg->AddEntry ( DefR6comp->GetName(),"Run12 JP2, Cone, R=0.4","lp");
  leg->AddEntry ( LargerR6comp->GetName(),"Run12 JP2, Cone, R=0.6","lp");
  leg->Draw();
  gPad->SaveAs("plots/Run6Comparison.png");

  
  // -------
  // 2009
  // -------
  // |eta|<1
  // double r9pt[10]   =  { 10.7032,     12.6309,    14.9538,     17.532,    20.5296,      24.2735,     28.6826,     33.7055,  39.6978,  46.6486 };
  // double r9v[10]    =  { 1.78648,    0.511837,   0.186324,  0.0547838,  0.0169785,   0.00482871,  0.00123751, 0.000270104, 5.22E-05, 7.28E-06 };
  // double r9stat[10] =  { 0.116464,  0.0203868, 0.00445789, 0.00183527, 0.000269511,    8.09E-05,    1.73E-05,    4.36E-06, 1.28E-06, 3.22E-07 };
  // double r9sys[10]  =  { 0.0864877, 0.0396818,  0.0181181, 0.00606079,  0.00214534, 0.000580962, 0.000189372,    4.57E-05, 1.01E-05, 1.56E-06 };
    
  // |eta|<0.5
  double r9pt[10]   =  { 10.7032,     12.6309,    14.9538,     17.532,     20.5296,      24.2735,     28.6826,     33.7055,  39.6978,  46.6486 };
  double r9v[10]    =  { 1.92465,    0.541807,   0.190522,  0.0607975,   0.0186717,    0.0052993,  0.00143537, 0.000337934, 7.06E-05, 1.09E-05 };
  double r9stat[10] =  { 0.101685,  0.0209879, 0.00447876, 0.00182701, 0.000351952,     7.75E-05,    2.29E-05,    5.85E-06, 1.84E-06, 5.34E-07 };
  double r9sys[10]  =  { 0.0931769, 0.0420052,  0.0185264,  0.0067261,  0.00235928,   0.00063758, 0.000219649,    5.72E-05, 1.36E-05, 2.33E-06 };
  
  TGraphErrors *r9 = new TGraphErrors (10, r9pt, r9v, 0, r9stat);
  
  new TCanvas;
  gPad->SetLogy();

  // TH1D* h = new TH1D( "h","",100, 0, 50);
  h->SetAxisRange( 1E-6,1E3,"y");
  h->DrawCopy("axis");
  
  r9->SetMarkerColor(kBlue);
  r9->SetMarkerStyle(21);
  r9->Draw("LP");
  
  int scalebin = 5;
  TH1D* DefR9comp=(TH1D*) Defdata->Clone("DefR9comp");
  TH1D* LargerR9comp=(TH1D*) Largerdata->Clone("LargerR9comp");
  
  DefR9comp->Scale ( r9v[scalebin] / DefR9comp->GetBinContent( DefR9comp->FindBin( r9pt[scalebin] ) ) );
  LargerR9comp->Scale ( r9v[scalebin] / LargerR9comp->GetBinContent( LargerR9comp->FindBin( r9pt[scalebin] ) ) );

  DefR9comp->SetLineColor( kGreen+1 );
  DefR9comp->DrawCopy("9same");

  LargerR9comp->SetLineColor( kRed );
  LargerR9comp->DrawCopy("9same");

  TLegend* leg;
  leg = new TLegend( 0.3, 0.7, 0.88, 0.88, "Comparison to Run9" );
  leg->SetBorderSize(0);
  leg->SetTextFont(43);
  leg->SetFillStyle(0);

  leg->AddEntry ( r9,"Run9 JP2, anti-k_{T}, R=0.6, |#eta|<1", "lp");
  leg->AddEntry ( DefR9comp->GetName(),"Run12 JP2, Cone, R=0.4, |#eta|<1-R","lp");
  leg->AddEntry ( LargerR9comp->GetName(),"Run12 JP2, Cone, R=0.6, |#eta|<1-R","lp");
  leg->Draw();
  gPad->SaveAs("plots/Run9Comparison.png");
}
