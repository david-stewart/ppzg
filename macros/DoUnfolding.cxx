#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#include "RooUnfoldSvd.h"
#include "RooUnfoldTUnfold.h"

#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TLegend.h>

void SetErrors ( TH1D* const nom, const TH1D* const var, TH1D* const ratio=0 );

int DoUnfolding( 
		// TString trainname = "Results/Geant_NoEff_NoBg_HT54_WithMisses_WithFakes_TrainedWith_McGeant_NoEff_NoBg_MB.root",
		// TString ppname    = "Results/ForUnfolding_Pp_HT54_NoEff_NoBgSub.root",
		//TString trainname = "Results/Geant_NoEff_NoBg_HT54_WithMisses_WithFakes_TrainedWith_McGeant_NoEff_NoBg_MB.root",
		// TString ppname    = "Results/ForUnfolding_Pp_HT54_NoEff_NoBgSub.root",
		// TString trainname = "Results/ForClosure_Geant_NoEff_NoBg_HT54_WithMisses_WithFakes_TrainedWith_McGeant_NoEff_NoBg_MB.root",
		// TString ppname    = "Results/ForClosure_Geant_NoEff_NoBg_HT54_WithMisses_WithFakes_TrainedWith_McGeant_NoEff_NoBg_MB.root",
		// TString trainname = "Results/Geant_NoEff_NoBg_HT54_WithMisses_WithFakes_TrainedWith_McGeant_NoEff_NoBg_MB.root",
		// TString ppname    = "Results/Geant_NoEff_NoBg_HT54_WithMisses_WithFakes_TrainedWith_McGeant_NoEff_NoBg_MB.root",

		// TString trainname = "Results/Recut_Geant_NoEff_NoBg_HT54_WithMisses_WithFakes_TrainedWith_Recut_McGeant_NoEff_NoBg_MB.root",
		// TString ppname    = "Results/ForUnfolding_Recut_Pp_HT54_NoEff_NoBgSub.root",

		// TString trainname = "Results/ReCut_Geant12_NoEff_NoBg_HT54_JP_WithMisses_WithFakes_TrainedWith_ReCut_McGeant12_NoEff_NoBg_MB.root",
		// TString trainname = "Results/ReCut_Geant12_NoEff_NoBg_HT54_WithMisses_WithFakes_TrainedWith_ReCut_McGeant12_NoEff_NoBg_MB.root",
		// TString ppname    = "Results/ForUnfolding_ReCut_Pp12_HT54_NoEff_NoBgSub.root",
		// --- Latest Run 6 --- 
		// TString trainname = "Results/AEff0_PtSmear0_ATow0_SystGeant_NoEff_NoBg_HT54_WithMisses_WithFakes_TrainedWith_Recut_McGeant_NoEff_NoBg_MB.root",
		// TString ppname    = "Results/ForUnfolding_Recut_Pp_HT54_NoEff_NoBgSub.root",
		// --- Use one of these for Run 12: ---
		// TString trainname = "Results/AEff0_PtSmear0_ATow0_SystGeant12_NoEff_NoBg_HT54_JP2_WithMisses_WithFakes_TrainedWith_McGeant12_NoEff_NoBg_all.root",
		// TString ppname    = "Results/ForUnfolding_ForPaper_Pp12_HT54_JP2_NoEff_NoBgSub.root",
		// TString trainname = "Results/AEff0_PtSmear0_ATow0_SystGeant12_NoEff_NoBg_JP2_WithMisses_WithFakes_TrainedWith_McGeant12_NoEff_NoBg_all.root",
		// TString ppname    = "Results/ForUnfolding_ForPaper_Pp12_JP2_NoEff_NoBgSub.root",
		// // --- MIP or other hadronic correction cross check ---
		// TString trainname = "Results/AEff0_PtSmear0_ATow0_SystGeant12_MIP_NoEff_NoBg_JP2_WithMisses_WithFakes_TrainedWith_McGeant12_NoEff_NoBg_all.root",
		// TString ppname    = "Results/ForUnfolding_ForPaper_Pp12_JP2_MIP_NoEff_NoBgSub.root",
		// --- MIP or other hadronic correction cross check ---
		TString trainname = "Results/AEff0_PtSmear0_ATow0_SystGeant_MIP_NoEff_NoBg_HT54_WithMisses_WithFakes_TrainedWith_Recut_McGeant_NoEff_NoBg_MB.root",
		TString ppname    = "Results/ForUnfolding_Recut_Pp_HT54_MIP_NoEff_NoBgSub.root",
		const int MaxnBayes2D = 4
		) {
  
  int RebinZg=1;  // SHOULD BE DONE EARLIER

  bool ClosureTest = ( trainname.Contains( "ForClosure" ) || ppname.Contains( "ForClosure" ) );
  if ( ClosureTest && trainname != ppname ){
    cerr << "Inconsistent naming for closure test" << endl;
    return -1;
  }
  
  // if >0, take this N as the nominal value and provide
  // systematics histos using the full difference to N+1
  // (symmetrized for easiness. FIXME)
  int NDefault=2;
  
  gStyle->SetOptStat(0);
  gStyle->SetHistLineWidth(2);

  TFile* ftrain = new TFile( trainname, "READ");

  // Load pp data
  // ------------
  TFile* fmeas = new TFile( ppname, "READ");  
  TH2D* IncTestMeas2D  = (TH2D*) fmeas->Get("IncTestMeas2D");
  
  TH2D* IncTestTruth2D=0;
  if ( ClosureTest ){
    IncTestTruth2D  = (TH2D*) fmeas->Get("IncTestTruth2D");
  }

  // Load response
  // ------------
  TH2D* IncTrainMeas2D  = (TH2D*) ftrain->Get("IncMeas2D");
  TH2D* IncTrainTruth2D  = (TH2D*) ftrain->Get("IncTruth2D");
  // TH2D* RecoilTrainMeas2D  = (TH2D*) ftrain->Get("RecoilMeas2D");
  // TH2D* RecoilTrainTruth2D  = (TH2D*) ftrain->Get("RecoilTruth2D");
    
  // Load Pythia8 and Hard Probes comparison
  TFile* fP8 = new TFile( "~/BasicAj/AjResults/UnfoldedPpSystematics_Histos.root", "READ");  
  
  // // Unfold 1D
  // // ---------
  // int nBayes1D=3;
  // RooUnfoldResponse* IncPtResponse = (RooUnfoldResponse*) ftrain->Get("IncPtResponse");
  // TH1D* IncPtTestMeas  = (TH1D*) IncTestMeas2D->ProjectionX("IncPtTestMeas");
  // RooUnfoldBayes    IncPtBayesUnfold ( IncPtResponse, IncPtTestMeas, nBayes1D);
  // TH1D* IncPtBayesUnfolded= (TH1D*) IncPtBayesUnfold.Hreco();
  // IncPtBayesUnfolded->SetName("IncPtBayesUnfolded");

  
  
  // Output
  // ------
  TString OutFileName = "Results/Unfolded_";
  if ( RebinZg==1 ) OutFileName += "NoRebin_";
  // OutFileName += nBayes2D;   OutFileName += "_";
  OutFileName += gSystem->BaseName(ppname); OutFileName += "__With_";
  OutFileName += gSystem->BaseName(trainname);
  OutFileName.ReplaceAll(".root","");
  OutFileName.Append(".root");
  TFile* fout = new TFile( OutFileName, "RECREATE");

  TString PlotBase = "plots/Unfolded_";
  // PlotBase += nBayes2D;   PlotBase += "_";
  PlotBase += gSystem->BaseName(ppname); PlotBase += "__With_";
  PlotBase += gSystem->BaseName(trainname);
  PlotBase.ReplaceAll(".root","");

  IncTestMeas2D->SetDirectory( gDirectory );
  if ( ClosureTest ){
    IncTestTruth2D->SetDirectory( gDirectory );
  }
  IncTrainMeas2D->SetDirectory( gDirectory );
  IncTrainTruth2D->SetDirectory( gDirectory );

  // Unfold 2D
  // ---------
  RooUnfoldResponse* IncPtZgResponse2D = (RooUnfoldResponse*) ftrain->Get("IncPtZgResponse2D");
  IncPtZgResponse2D->Write();
  
  cout << endl << " Starting 2D Unfolding " <<  endl ;
  TH2D** IncBayesUnfolded = new TH2D*[MaxnBayes2D];
  for ( int nBayes2D=1; nBayes2D<=MaxnBayes2D; ++nBayes2D ){
    cout << endl << "=== Unfolding trigger distribution with nBayes2D=" << nBayes2D << " ===" << endl;
    RooUnfoldBayes    IncBayesUnfold ( IncPtZgResponse2D, IncTestMeas2D, nBayes2D);
    IncBayesUnfold.SetVerbose(1);
    IncBayesUnfolded[nBayes2D-1] = (TH2D*) IncBayesUnfold.Hreco( RooUnfold::kCovariance ); // RooUnfold::kCovariance seems to be the default
    // IncBayesUnfolded[nBayes2D-1] = (TH2D*) IncBayesUnfold.Hreco( RooUnfold::kCovToy ); // MUCH slower, bigger in 2nd sigdig
    // IncBayesUnfolded[nBayes2D-1] = (TH2D*) IncBayesUnfold.Hreco( RooUnfold::kErrors ); // identical to kCovariance?
     
    TString hname = "IncBayesUnfolded_"; hname+=nBayes2D;
    IncBayesUnfolded[nBayes2D-1] ->SetName(hname);
  }

  // =========================== Draw Spectra ===============================
  new TCanvas;
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetLogy();
  TLegend* leg = new TLegend( 0.55, 0.45, 0.89, 0.9, "Inclusive Jet p_{T}" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  gPad->SaveAs( PlotBase + "_Everything.pdf[");
    
  int rebinpt=2;
  TH1D* Incmeaspt = (TH1D*) IncTestMeas2D->ProjectionX("Incmeaspt");
  Incmeaspt->SetLineColor( kMagenta+2 );
  Incmeaspt->Rebin( rebinpt );
  Incmeaspt->SetTitle("");
  // Incmeaspt->SetDirectory( gDirectory );

  TH1D* IncTrainTruthpt = (TH1D*) IncTrainTruth2D->ProjectionX("IncTrainTruthpt");
  // IncTrainTruthpt->SetLineColor(kPink+7);
  IncTrainTruthpt->SetLineColor(kBlack);
  IncTrainTruthpt->Rebin( rebinpt );
  // IncTrainTruthpt->SetDirectory( gDirectory );

  TH1D* IncTestTruthpt=0;
  if ( ClosureTest ){
    IncTestTruthpt = (TH1D*) IncTestTruth2D->ProjectionX("IncTestTruthpt");
    // IncTestTruthpt->SetLineColor(kPink+7);
    IncTestTruthpt->SetLineColor(kBlack);
    IncTestTruthpt->Rebin( rebinpt );
    IncTestTruthpt->SetAxisRange( 2, 80, "x");
    IncTestTruthpt->SetAxisRange( 1e-1, 1e8, "y");
    IncTestTruthpt->SetTitle(";p_{T};arb. units" );
    // IncTestTruthpt->SetDirectory( gDirectory );
  }
      
  TH1D* IncTrainMeaspt = (TH1D*) IncTrainMeas2D->ProjectionX("IncTrainMeaspt");
  IncTrainMeaspt->SetLineColor(kCyan+2);
  IncTrainMeaspt->Rebin( rebinpt );

  // IncTrainMeaspt->SetDirectory( gDirectory );

  IncTrainTruthpt->SetAxisRange( 2, 80, "x");
  IncTrainTruthpt->SetAxisRange( 1e-1, 1e8, "y");
  IncTrainTruthpt->SetTitle(";p_{T};arb. units" );
  // tcs->cd();
  
  if ( trainname.Contains ("JP") ){
    if ( IncTestTruthpt ) IncTestTruthpt->Scale(1e8);
    if ( IncTrainTruthpt) IncTrainTruthpt->Scale(1e8);
  }
    
  TH1D* ptdummy = (TH1D*) IncTrainTruthpt->Clone("ptdummy");
  ptdummy->SetAxisRange( 2, 80, "x");
  ptdummy->SetAxisRange( 1e-1, 1e10, "y");
  ptdummy->Draw("axis");


  if ( ClosureTest ) IncTestTruthpt->Draw("9histsame");
  else IncTrainTruthpt->Draw("9histsame");

  Incmeaspt->Draw("9same");
  IncTrainMeaspt->Draw("9same");

  if ( ClosureTest ) leg->AddEntry("IncTestTruthpt", "Test Truth");
  else               leg->AddEntry("IncTrainTruthpt", "Training Truth");
  leg->AddEntry("IncTrainMeaspt", "Training Meas.");
  leg->AddEntry("Incmeaspt", "Measured");

  TH1D* SysIncunfoldpt=0;
  for ( int nBayes2D=1; nBayes2D<=MaxnBayes2D; ++nBayes2D){
    TString hname = "Incunfoldpt_"; hname+=nBayes2D;
    TH1D* Incunfoldpt = (TH1D*) IncBayesUnfolded[nBayes2D-1]->ProjectionX(hname);

    Incunfoldpt->SetLineColor(nBayes2D+1);
    Incunfoldpt->Rebin( rebinpt );
    
    Incunfoldpt->Draw("same");
    TString title = "Unfolded, N = ";
    title +=nBayes2D;
    leg->AddEntry(Incunfoldpt->GetName(), title);

    if ( NDefault>0 ) {
       // Initialize systematics histo
      if (nBayes2D==NDefault )	SysIncunfoldpt = (TH1D*) Incunfoldpt->Clone("SysIncunfoldpt");
      // Set Errors
      if (nBayes2D==NDefault+1 ) {
	if ( !SysIncunfoldpt ){
	  cerr << "Trying to calculate systematics but nominal histo doesn't exist!" << endl;
	  return -1;
	}
	SetErrors ( SysIncunfoldpt, Incunfoldpt );

	// And draw this and all before it on top
	SysIncunfoldpt->SetMarkerSize(0);
	SysIncunfoldpt->SetLineWidth( 0 );
	SysIncunfoldpt->SetFillStyle(1001);
	SysIncunfoldpt->SetFillColor( kGray );
	SysIncunfoldpt->SetMarkerColor( kGray );
	SysIncunfoldpt->Draw("9E2same");

	for ( int i=1; i<=nBayes2D; ++i){
	  hname = "Incunfoldpt_"; hname+=i;
	  TH1D* h = (TH1D*) gDirectory->Get( hname );
	  if ( h ) h->Draw("9same");
	}
      }      
    }
  }

  if ( ClosureTest ) IncTestTruthpt->Draw("9histsame");
  else IncTrainTruthpt->Draw("9histsame");
  Incmeaspt->Draw("9same");
  IncTrainMeaspt->Draw("9same");

  leg->Draw();
  gPad->SaveAs( PlotBase + "_IncUnfoldedSpectra.png");
  gPad->SaveAs( PlotBase + "_Everything.pdf");

  // fout->Write(); return 0;
   
  // =========================== Draw Zg ===============================
  TString name;
  TH1D* h;
  
  double Incbins[] = { 10, 15, 20, 25, 30, 40, 60};
  int nIncbins  = sizeof(Incbins) / sizeof(Incbins[0])-1;

  TH1D* dummy = new TH1D( "dummy","", 20, 0.05, 0.55);
  dummy->SetAxisRange(0, 9, "y");
  
  TH1D* rdummy = new TH1D( "rdummy","", 20, 0.05, 0.55);
  rdummy->SetAxisRange( -.01,0.20, "y");
	  
  for ( int i=0 ; i<nIncbins ; ++i ){
    float ptleft  = Incbins[i];
    float ptright = Incbins[i+1];

    new TCanvas;
    gPad->SetGridx(0);  gPad->SetGridy(0);
    TString title = "Inclusive Jet z_{g}, ";
    title += int(ptleft+0.01);   title += " < p_{T} < ";
    title += int(ptright+0.01);  title += " GeV/c";
    leg = new TLegend( 0.55, 0.45, 0.89, 0.9, title );
    leg->SetBorderSize(0);
    leg->SetLineWidth(10);
    leg->SetFillStyle(0);
    leg->SetMargin(0.1);
    dummy->Draw("axis");

    name = "Inc_truth_"; name += int(ptleft+0.01); name += int(ptright+0.01);
    TH1D* Inctruth=0;
    if ( ClosureTest )    Inctruth = (TH1D*) IncTestTruth2D->ProjectionY(title,IncTestTruth2D->GetXaxis()->FindBin( ptleft+0.01), IncTestTruth2D->GetXaxis()->FindBin( ptright-0.01));
    else                  Inctruth = (TH1D*) IncTrainTruth2D->ProjectionY(title,IncTrainTruth2D->GetXaxis()->FindBin( ptleft+0.01), IncTrainTruth2D->GetXaxis()->FindBin( ptright-0.01));
    // Inctruth->SetDirectory( gDirectory );

    Inctruth->SetLineColor(kBlack);
    Inctruth->SetTitle( ";z_{g};1/N dN/dz_{g}" );

    h=Inctruth; h->Scale ( 1./ h->Integral(h->FindBin(0.1+0.001),h->FindBin(0.5-0.001)) / h->GetXaxis()->GetBinWidth(1));
    h->Rebin( RebinZg );    h->Scale(1./RebinZg );
    h->SetAxisRange(0, 9, "y");

    Inctruth->SetLineStyle(2);
    Inctruth->Draw("9histsame");
    if ( ClosureTest )    leg->AddEntry(Inctruth, "Test Truth");
    else leg->AddEntry(Inctruth, "Training Truth");	  

    name = "Incmeas_"; name += int(ptleft+0.01); name += int(ptright+0.01);
    TH1D* Incmeas =
      (TH1D*) IncTestMeas2D->ProjectionY(name, IncTestMeas2D->GetXaxis()->FindBin( ptleft+0.01), IncTestMeas2D->GetXaxis()->FindBin( ptright-0.01));
    Incmeas->SetLineColor(kRed+1);
    Incmeas->SetLineStyle(2);
    h=Incmeas; h->Scale ( 1./ h->Integral(h->FindBin(0.1+0.001),h->FindBin(0.5-0.001)) / h->GetXaxis()->GetBinWidth(1));
    h->Rebin( RebinZg );    h->Scale(1./RebinZg );
    h->SetAxisRange(0, 9, "y");
    Incmeas->Draw("9histsame");
    leg->AddEntry(Incmeas->GetName(), "Measured, 10 < p_{T}^{Meas} < 20");
    
    name="UnfoldedNS_"; name += int(ptleft+0.01); name += "_"; name += int(ptright+0.01); name += "_minmax";
    TH1D* minmaxNS = (TH1D*) fP8->Get(name);
    // cout << " ---------------------------------> Adding " << name << endl;
    if ( minmaxNS && !(ppname.Contains("Geant")) ){
      minmaxNS->SetLineWidth( 0 );
      minmaxNS->SetFillStyle(1001);
      minmaxNS->SetFillColor( kRed-10 );
      minmaxNS->SetLineColor(kRed-10);
      minmaxNS->SetMarkerColor(kRed);
      minmaxNS->SetMarkerStyle(29);
      minmaxNS->SetMarkerSize(2);
      minmaxNS->Draw("9E2same");
      minmaxNS->Draw("9same");
      leg->AddEntry(minmaxNS->GetName(), "HP Result (Trigger)");
    } 

    TH1D* SysIncunfold=0;
    TH1D* SysIncRatio=0;
    TH1D* Default=0;
    for ( int nBayes2D=1; nBayes2D<=MaxnBayes2D; ++nBayes2D){
      name = "Incunfold_"; name += int(ptleft+0.01); name += int(ptright+0.01); name +="_" ; name+=nBayes2D;
      TH2D* h2 = IncBayesUnfolded[nBayes2D-1];
      TH1D* Incunfold = (TH1D*) h2->ProjectionY(name, h2->GetXaxis()->FindBin( ptleft+0.01),h2 ->GetXaxis()->FindBin( ptright-0.01));
      Incunfold->SetLineColor(nBayes2D+1);
      h=Incunfold; h->Scale ( 1./ h->Integral(h->FindBin(0.1+0.001),h->FindBin(0.5-0.001)) / h->GetXaxis()->GetBinWidth(1));
      h->SetTitle( ";z_{g};1/N dN/dz_{g}" );
      h->Rebin( RebinZg );    h->Scale(1./RebinZg );
      h->SetAxisRange(0, 9, "y");
      if (nBayes2D==NDefault && NDefault>0 )      h->SetLineColor(kBlack);      
      h->Draw("same");
      TString title = "Unfolded, N = ";
      title +=nBayes2D;
      if (nBayes2D==NDefault && NDefault>0 )      title += " (default)";
      leg->AddEntry(h->GetName(), title);

      if ( NDefault>0 ) {
	// Initialize systematics histo
	if (nBayes2D==NDefault ){
	  name = "SysIncunfold_"; name += int(ptleft+0.01); name += int(ptright+0.01);
	  SysIncunfold = (TH1D*) Incunfold->Clone(name);
	  name = "SysIncRatio_"; name += int(ptleft+0.01); name += int(ptright+0.01);
	  SysIncRatio = (TH1D*) Incunfold->Clone(name);
	  SysIncRatio->Reset();
	  title = "Rel. Unc. Inc pT=";
	  title += int(ptleft+0.01); title +="-";
	  title += int(ptright+0.01); title +=" GeV/c" ;
	  SysIncRatio->SetTitle( title );		
	  SysIncRatio->SetAxisRange( -.01,0.20, "y");
	  SysIncRatio->SetAxisRange( 0.1+0.01,0.5-0.01, "x");
	}
	
	// Set Errors
	if (nBayes2D==NDefault+1 ) {
	  if ( !SysIncunfold ){
	    cerr << "Trying to calculate systematics but nominal histo doesn't exist!" << endl;
	    return -1;
	  }
	  SetErrors ( SysIncunfold, Incunfold, SysIncRatio );
	  
	  // And draw this and all before it on top
	  SysIncunfold->SetMarkerSize(0);
	  SysIncunfold->SetLineWidth( 0 );
	  SysIncunfold->SetFillStyle(1001);
	  SysIncunfold->SetFillColor( kGray );
	  SysIncunfold->SetMarkerColor( kGray );
	  SysIncunfold->Draw("9E2same");
	  
	  for ( int j=1; j<=nBayes2D; ++j){
	    name = "Incunfold_"; name += int(ptleft+0.01); name += int(ptright+0.01); name +="_" ; name+=j;
	    TH1D* h = (TH1D*) gDirectory->Get( name );
	    // cout << "Drawing " << name << endl;
	    if ( h ) h->Draw("9same");
	  }
	}      
      }
    }

    Inctruth->Draw("9histsame");
    Incmeas->Draw("9histsame");
	
    name = "AS_P8_SoftJets_MB_"; name += int(ptleft+0.01); name+="_"; name += int(ptright+0.01);    
    h = (TH1D*) fP8->Get(name);
    if ( h ){
      // h->SetLineStyle(1);
      h->SetAxisRange(0.11, 0.49);
      h->SetAxisRange(0.11, 0.49);
      h->Draw("9lhist][same");
      leg->AddEntry(h->GetName(), "Pythia8 (Recoil)","l");
    }

    if ( SysIncunfold ){
      SysIncunfold->SetMarkerSize(2);
      SysIncunfold->SetLineWidth( 2 );
      SysIncunfold->SetLineColor( kBlack );
      SysIncunfold->SetFillStyle(1001);
      SysIncunfold->SetFillColor( kGray );
      SysIncunfold->SetMarkerColor( kBlack );
      SysIncunfold->SetMarkerStyle( 20 );
      SysIncunfold->Draw("9E2same");
      SysIncunfold->Draw("9same");
    }
   
    leg->Draw();
    name = PlotBase; name += "_Inc_";
    name += int(ptleft+0.01); name += int(ptright+0.01);	
    gPad->SaveAs( name + ".png");
    gPad->SaveAs( PlotBase + "_Everything.pdf");

    // cout << i << endl;
    if ( SysIncRatio ) {
      new TCanvas;
      rdummy->Draw("axis");
      SysIncRatio->Draw("9same");
      name = PlotBase; name += "_IncSysRatio_";
      name += int(ptleft+0.01); name += int(ptright+0.01);	
      gPad->SaveAs( name + ".png");
      gPad->SaveAs( PlotBase + "_Everything.pdf");
    } 
  }

  
  gPad->SaveAs( PlotBase + "_Everything.pdf]");
  
  fout->Write();
  cout << " -----------------------------------------" << endl;
  cout << " Wrote to" << endl << OutFileName << endl;
  return 0;
}

// ---------------------------------------------------------------------------
void SetErrors ( TH1D* const nom, const TH1D* const var, TH1D* const ratio ){
  for (int i=1; i< nom->GetNbinsX(); ++i ){
    double n = nom->GetBinContent(i);
    double v = var->GetBinContent(i);
    double e = fabs ( n - v );
    nom->SetBinError ( i, e );
    if ( ratio && fabs(n)>1e-6 )     ratio->SetBinContent ( i, e, e/n );
  }

}
