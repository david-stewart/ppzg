
float ls = 20;
float ts = 20;

void SetupHisto ( TH1* hFrame,
		  float xFactor, float yFactor,
		  float xloffset, float xtoffset,
		  float xtickbase, int ndivx,
		  float yloffset, float ytoffset,
		  float ytickbase, int ndivy
		  );

int FigResponse( ){
  // gROOT->LoadMacro("macros/FigPanel.cxx");

  TString infilename="Results/AEff0_PtSmear0_ATow0_SystGeant12_NoEff_NoBg_JP2_WithMisses_WithFakes_TrainedWith_McGeant12_NoEff_NoBg_all.root";
  TFile* fin = 0;
  if ( gSystem->FindFile(".",infilename) ){
    fin = new TFile( infilename, "READ");
  } 

  int rebinpt=10;
  int rebinzg=5;
  
  TString plotpath="./figs/";

  gStyle->SetHistLineWidth(2);
  gStyle->SetFrameLineWidth(2);
  gStyle->SetOptStat(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);

  gStyle->SetTitleX(0.1f);
  gStyle->SetTitleW(0.8f);
  gStyle->SetTitleBorderSize(0);	//Title box border thickness

  // gStyle->SetPalette(51); // too blue
  // gStyle->SetPalette(53); // kDarkBodyRadiator
  // gStyle->SetPalette(54); // kinda drab but not bad
  // gStyle->SetPalette(55); // standard
  if ( gROOT->GetVersionInt() < 60400 ){
    gStyle->SetPalette(56); // kInvertedDarkBodyRadiator
  } else {
    gStyle->SetPalette(57); // kBird, new default
  }

  // // Good for 400,750 divided in two
  // float lMargin = 0.17;
  // float rMargin = 0.17;
  // float bMargin = 0.17;
  // float tMargin = 0.05;
  // float xloffset=0.02;
  // float xtoffset=2.5;
  // int   ndivx= 505; 
  // float yloffset=0.02;
  // float ytoffset=2.6;
  // int   ndivy= 505; 
  // float xtickbase = 0.04;
  // float ytickbase = 0.04;

  // TCanvas* C =new TCanvas("C","", 400,750);
  // C->Divide(1,2);
  
  float lMargin = 0.17;
  float rMargin = 0.17;
  float bMargin = 0.17;
  float tMargin = 0.05;
  float xloffset=0.02;
  float xtoffset=1.3;
  int   ndivx= 505; 
  float yloffset=0.02;
  float ytoffset=1.3;
  int   ndivy= 505; 
  float xtickbase = 0.04;
  float ytickbase = 0.04;

  // pT response
  // -----------
  // C->cd(1);
  
  new TCanvas("c1","", 400,375);
  gPad->SetLogz();
  gPad->SetLeftMargin(lMargin);
  gPad->SetRightMargin(rMargin);
  gPad->SetTopMargin(tMargin);
  gPad->SetBottomMargin(bMargin);

  TH2D* ptresp;
  if ( fin ){
    RooUnfoldResponse* VisualIncPtResponse = (RooUnfoldResponse*) fin->Get("VisualIncPtResponse");
    ptresp = (TH2D*) VisualIncPtResponse->Hresponse()->Clone("ptresp");
    ptresp->SaveAs("ptresp.root");
  } else {
    TFile* f1 = new TFile ( "ptresp.root");
    ptresp = (TH2D*) f1->Get("ptresp");
  }

  TH2D* PtForProfile = (TH2D*) ptresp->Clone("PtForProfile");
  ptresp->Rebin2D(rebinpt,rebinpt); 
  ptresp->SetTitle(";p_{T}^{Det} [GeV/c];p_{T}^{Part} [GeV/c]");
  
  SetupHisto ( ptresp,
	       1, 1,
	       xloffset, xtoffset,
	       xtickbase, ndivx,
	       yloffset, ytoffset,
	       ytickbase, ndivy );
  
  // cout << ptresp->GetBinContent( ptresp->GetMinimumBin() ) << endl; return;
  // cout << ptresp->GetMinimum( 0 ) << endl;
  // cout << ptresp->GetMaximum(  ) << endl;
  ptresp->SetAxisRange( 1e-11, 5e-5, "z");
  ptresp->DrawCopy("9colz");


  PtForProfile->Rebin2D(30,1);
  for (int i=1; i<10; ++ i) cout << PtForProfile->GetXaxis()->GetBinLowEdge(i) << endl;
  TProfile* PtProfile = (TProfile*) PtForProfile->ProfileX("PtProfile",1,-1,"s");
  PtProfile->SetMarkerStyle( 33 );
  PtProfile->SetMarkerColor( kRed );
  PtProfile->SetMarkerSize( 1.8 );
  PtProfile->SetLineColor(kRed);
  PtProfile->SetLineWidth(2);

  PtProfile->SetAxisRange( 6+0.01, 60-0.01, "x");
  PtProfile->Draw("9E1X0same");

  TLine diag;
  diag.SetLineColor(kBlack);
  diag.SetLineStyle(2);
  diag.DrawLine(0,0,60,60);
  
  TLegend* leg = new TLegend( 10, 65, 40, 75,"", "br" );
  leg->SetBorderSize(0);
  leg->SetTextFont(43);
  leg->SetTextSize(ts);
  leg->SetFillStyle(0);
  leg->AddEntry( PtProfile, "#LTp_T^{part}#GT#pmRMS","lp");
  leg->Draw();

  gPad->SaveAs("figs/FigPtResponse.pdf");
  // gPad->SaveAs("figs/FigResponses.png");

  // zg Response
  // -----------
  // C->cd(2);
  new TCanvas("c2","", 400,375);
  gPad->SetLogz();
  gPad->SetLeftMargin(lMargin);
  gPad->SetRightMargin(rMargin);
  gPad->SetTopMargin(tMargin);
  gPad->SetBottomMargin(bMargin);

  TH2D* zgresp;
  if ( fin ){
    RooUnfoldResponse* VisualIncZgResponse = (RooUnfoldResponse*) fin->Get("VisualIncZgResponse");
    zgresp = (TH2D*) VisualIncZgResponse->Hresponse()->Clone("zgresp");
    zgresp->SaveAs("zgresp.root");
  } else {
    TFile* f2 = new TFile ( "zgresp.root");
    zgresp = (TH2D*) f2->Get("zgresp");
  }
  zgresp->Rebin2D(rebinzg,rebinzg); 
  zgresp->SetTitle(";z_{g}^{Det};z_{g}^{Part}");
  
  SetupHisto ( zgresp,
	       1, 1,
	       xloffset, xtoffset,
	       xtickbase, 006,
	       yloffset, ytoffset,
	       ytickbase, ndivy );
  
  // cout << zgresp->GetBinContent( zgresp->GetMinimumBin() ) << endl; return;
  cout << zgresp->GetMinimum( 0 ) << endl; 
  cout << zgresp->GetMaximum( ) << endl; 
  zgresp->SetMinimum(1e-11);

  TH2D* dummy = new TH2D( "dummy",";z_{g}^{Det};z_{g}^{Part}",100, -0.02, 0.502, 100, -0.02, 0.502 );
  SetupHisto ( dummy,
	       1, 1,
	       xloffset, xtoffset,
	       xtickbase, 6,
	       yloffset, ytoffset,
	       ytickbase, 6 );

  dummy->SetAxisRange( 1e-11, 2e-7, "z");
  dummy->DrawCopy("9colz");
  zgresp->SetAxisRange( 1e-11, 2e-7, "z");
  zgresp->DrawCopy("9colzsame");
  // cout << zgresp->GetMinimum( 0 ) << endl;
  // cout << zgresp->GetMaximum(  ) << endl;
  // zgresp->SetMinimum( 1e-11 );
  // zgresp->SetMaximum( 1e-7 );
  // zgresp->DrawCopy("9colz");

  diag.SetLineColor(kBlack);
  diag.SetLineStyle(2);
  diag.DrawLine(0,0,0.5,0.5);

  TLatex l;

  TLatex latex;
  latex.SetNDC( false );
  latex.SetTextSize(ts);
  latex.SetTextFont(43);
  latex.SetTextAlign(12);
  latex.SetTextColor( kBlack );
  latex.DrawLatex( 0.1,0.05, "20 < p_{T}^{Det}/(GeV/c) < 30");
  gPad->SaveAs("figs/FigZgResponse.pdf");
  
  // Delta zg
  // --------
  new TCanvas("c3","", 400,375);
  gPad->SetLogz();
  gPad->SetLeftMargin(lMargin);
  gPad->SetRightMargin(rMargin);
  gPad->SetTopMargin(tMargin);
  gPad->SetBottomMargin(bMargin);

  TH2D* DeltaZgvsPt;
  if ( fin ){
    DeltaZgvsPt = (TH2D*) fin->Get("DeltaZgvsPt");
    DeltaZgvsPt->SaveAs("DeltaZgvsPt.root");
  } else {
    TFile* f3 = new TFile ( "DeltaZgvsPt.root");
    DeltaZgvsPt = (TH2D*) f3->Get("DeltaZgvsPt");
  }

  TH2D* DeltaZgForColz = (TH2D*) DeltaZgvsPt->Clone("DeltaZgForColz");
  DeltaZgForColz->Rebin2D(2,3);

  TH2D* DeltaZgForProfile = (TH2D*) DeltaZgvsPt->Clone("DeltaZgForProfile");
  // DeltaZgForProfile->Rebin2D(5,3);
  DeltaZgForProfile->Rebin2D(2,3);
  TProfile* DeltaZgProfile = (TProfile*) DeltaZgForProfile->ProfileX("DeltaZgProfile",1,-1,"s");
  DeltaZgProfile->SetMarkerStyle( 33 );
  DeltaZgProfile->SetMarkerColor( kRed );
  DeltaZgProfile->SetMarkerSize( 1.8 );
  DeltaZgProfile->SetLineColor(kRed);
  DeltaZgProfile->SetLineWidth(2);

  TH2D* dummydel = new TH2D( "dummydel",";p_{T}^{Det};#Deltaz_{g} = z_{g}^{Det}-z_{g}^{Part}",100, 0, 63, 100, -0.51, 0.51 );
  SetupHisto ( dummydel,
	       1, 1,
	       xloffset, xtoffset,
	       xtickbase, ndivx,
	       yloffset, ytoffset,
	       ytickbase, ndivy );
  SetupHisto ( DeltaZgForColz,
	       1, 1,
	       xloffset, xtoffset,
	       xtickbase, ndivx,
	       yloffset, ytoffset,
	       ytickbase, ndivy );
  dummydel->SetAxisRange( 1e-11, 5e-5, "z");
  dummydel->Draw("9colz");
  // DeltaZgForColz->SetMinimum(1e-11);
  DeltaZgForColz->SetAxisRange( 1e-11, 5e-5, "z");
  DeltaZgForColz->SetAxisRange( 0, 60-0.01, "x");
  DeltaZgForColz->Draw("9colzsame");

  DeltaZgProfile->SetAxisRange( 5, 60-0.01, "x");
  DeltaZgProfile->Draw("9E1X0same");

  leg = new TLegend( 36, 0.38, 49, 0.49,"", "br" );
  leg->SetBorderSize(0);
  leg->SetTextFont(43);
  leg->SetTextSize(ts);
  leg->SetFillStyle(0);
  leg->AddEntry( DeltaZgProfile, "#LT#Deltaz_{g}#GT#pmRMS","lp");
  leg->Draw();
  
  gPad->SaveAs("figs/FigDeltaZgvsPt.pdf");

  // C->SaveAs("figs/FigResponses.pdf");
  // C->SaveAs("figs/FigResponses.png");

  TProfile* DeltaZgProfile2 = (TProfile*) DeltaZgForProfile->ProfileX("DeltaZgProfile2",1,-1,"");
  new TCanvas;
  DeltaZgProfile2->SetAxisRange( 5+0.01, 60-0.01, "x");
  DeltaZgProfile2->Fit("pol0");
  //  DeltaZgProfile2->Draw();

  TH1D* RMSes = (TH1D*) DeltaZgForProfile->ProjectionX("RMSes");
  RMSes->Reset();
  for (int i=1; i<=DeltaZgProfile->GetNbinsX(); ++i ){
    if ( DeltaZgProfile2->GetXaxis()->GetBinCenter(i)<=60 ){
      RMSes->SetBinContent(i,DeltaZgProfile->GetBinError(i));
      RMSes->SetBinError(i,0.0001);
      cout << DeltaZgProfile2->GetXaxis()->GetBinCenter(i)
	   << "  " << DeltaZgProfile->GetBinError(i) << endl;
    }
  }
  new TCanvas;
  // RMSes->Draw();
  RMSes->Fit("pol0","W","",5,60);
  // TF1* f = new TF1("f","[0]",5,60);
  // f->SetParameter(0,0.12);
  // RMSes->Fit("f","W","",5,60);

  return 0;
}
// -----------------------------------------------------------------------------
void SetupHisto ( TH1* hFrame,
		  float xFactor, float yFactor,
		  float xloffset, float xtoffset,
		  float xtickbase, int ndivx,
		  float yloffset, float ytoffset,
		  float ytickbase, int ndivy
		  )
{
  
  // Format for x axis
  hFrame->GetXaxis()->SetLabelFont(43);
  hFrame->GetXaxis()->SetLabelSize(ls);
  hFrame->GetXaxis()->SetLabelOffset(xloffset);
  hFrame->GetXaxis()->SetTitleFont(43);
  hFrame->GetXaxis()->SetTitleSize(ts);
  hFrame->GetXaxis()->SetTitleOffset(xtoffset);
  hFrame->GetXaxis()->CenterTitle( true );
  hFrame->GetXaxis()->SetNdivisions(ndivx);
  
  // TICKS X Axis
  hFrame->GetXaxis()->SetTickLength(yFactor*xtickbase/xFactor);
  
  // Format for y axis
  hFrame->GetYaxis()->SetLabelFont(43);
  hFrame->GetYaxis()->SetLabelSize(ls);
  hFrame->GetYaxis()->SetLabelOffset(yloffset);
  hFrame->GetYaxis()->SetTitleFont(43);
  hFrame->GetYaxis()->SetTitleSize(ls);
  hFrame->GetYaxis()->SetTitleOffset(ytoffset);
  
  hFrame->GetYaxis()->CenterTitle( true );
  hFrame->GetYaxis()->SetNdivisions(ndivy);
  
  // TICKS Y Axis
  hFrame->GetYaxis()->SetTickLength(xFactor*ytickbase/yFactor);      

  // Format for z axis
  hFrame->GetZaxis()->SetLabelFont(43);
  hFrame->GetZaxis()->SetLabelSize(ls);
  // hFrame->GetZaxis()->SetLabelOffset(zloffset);
  hFrame->GetZaxis()->SetTitleFont(43);
  hFrame->GetZaxis()->SetTitleSize(ls);
  // hFrame->GetZaxis()->SetTitleOffset(ztoffset);
  
  hFrame->GetZaxis()->CenterTitle( true );
  // hFrame->GetZaxis()->SetNdivisions(ndivz);
  
  
}

