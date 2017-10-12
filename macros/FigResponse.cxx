
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
  TFile* fin = new TFile( infilename, "READ");

  int rebinpt=10;
  int rebinzg=4;
  
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
  // gStyle->SetPalette(56); // kInvertedDarkBodyRadiator
  gStyle->SetPalette(57); // kBird, new default,  doesn't work at all


  // Good for 400,750 divided in two
  float lMargin = 0.17;
  float rMargin = 0.17;
  float bMargin = 0.17;
  float tMargin = 0.05;
  float xloffset=0.02;
  float xtoffset=2.5;
  int   ndivx= 505; 
  float yloffset=0.02;
  float ytoffset=2.6;
  int   ndivy= 505; 
  float xtickbase = 0.04;
  float ytickbase = 0.04;

  TCanvas* C =new TCanvas("C","", 400,750);
  C->Divide(1,2);
  
  // pT response
  // -----------
  C->cd(1);
  
  // TPad* pad = new TPad("ptpad","",hposl,vposd,hposr,vposu);
  // TPad* pad = new TPad();
  gPad->SetLogz();
  gPad->SetLeftMargin(lMargin);
  gPad->SetRightMargin(rMargin);
  gPad->SetTopMargin(tMargin);
  gPad->SetBottomMargin(bMargin);

  // RooUnfoldResponse* VisualIncPtResponse = (RooUnfoldResponse*) fin->Get("VisualIncPtResponse");
  // TH2D* ptresp = VisualIncPtResponse->Hresponse()->Clone("ptresp");
  // ptresp->SaveAs("ptresp.root");
  TFile* f1 = new TFile ( "ptresp.root");
  TH2D* ptresp = (TH2D*) f1->Get("ptresp");
  ptresp->Rebin2D(rebinpt,rebinpt); 
  ptresp->SetTitle(";p_{T}^{Det} [GeV/c];p_{T}^{Part} [GeV/c]");
  
  SetupHisto ( ptresp,
	       1, 1,
	       xloffset, xtoffset,
	       xtickbase, ndivx,
	       yloffset, ytoffset,
	       ytickbase, ndivy );
  
  // cout << ptresp->GetBinContent( ptresp->GetMinimumBin() ) << endl; return;
  cout << ptresp->GetMinimum( 0 ) << endl; 
  ptresp->SetMinimum(1e-11);
  ptresp->DrawCopy("9colz");
  TLine diag;
  diag.SetLineColor(kBlack);
  diag.SetLineStyle(2);
  diag.DrawLine(0,0,60,60);
  
  // zg Response
  // -----------
  C->cd(2);
  gPad->SetLogz();
  gPad->SetLeftMargin(lMargin);
  gPad->SetRightMargin(rMargin);
  gPad->SetTopMargin(tMargin);
  gPad->SetBottomMargin(bMargin);

  // RooUnfoldResponse* VisualIncZgResponse = (RooUnfoldResponse*) fin->Get("VisualIncZgResponse");
  // TH2D* zgresp = VisualIncZgResponse->Hresponse()->Clone("zgresp");
  // zgresp->SaveAs("zgresp.root");
  TFile* f2 = new TFile ( "zgresp.root");
  TH2D* zgresp = (TH2D*) f2->Get("zgresp");
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

  // dummy->SetAxisRange( 1e-11, 1e7,"z");
  dummy->DrawCopy("9colz");
  zgresp->DrawCopy("9colzsame");
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
  
  C->SaveAs("figs/FigResponses.pdf");
  C->SaveAs("figs/FigResponses.png");

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

