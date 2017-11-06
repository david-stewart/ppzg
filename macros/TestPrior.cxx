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

int TestPrior( 
		// --- Latest Run 6 --- 
		// TString trainname = "Results/AEff0_PtSmear0_ATow0_SystGeant_NoEff_NoBg_HT54_WithMisses_WithFakes_TrainedWith_Recut_McGeant_NoEff_NoBg_MB.root",
		// TString ppname    = "Results/ForUnfolding_Recut_Pp_HT54_NoEff_NoBgSub.root",
		// --- Use these for Run 12: ---
		TString trainname = "Results/AEff0_PtSmear0_ATow0_SystGeant12_NoEff_NoBg_JP2_WithMisses_WithFakes_TrainedWith_McGeant12_NoEff_NoBg_all.root",
		TString ppname    = "Results/ForUnfolding_ForPaper_Pp12_JP2_NoEff_NoBgSub.root",	
		// // --- MIP or other hadronic correction cross check ---
		// TString trainname = "Results/AEff0_PtSmear0_ATow0_SystGeant12_MIP_NoEff_NoBg_JP2_WithMisses_WithFakes_TrainedWith_McGeant12_NoEff_NoBg_all.root",
		// TString ppname    = "Results/ForUnfolding_ForPaper_Pp12_JP2_MIP_NoEff_NoBgSub.root",
		// // --- Different R ---
		// TString trainname = "Results/AEff0_PtSmear0_ATow0_R0.6_SystGeant12_NoEff_NoBg_JP2_WithMisses_WithFakes_TrainedWith_R0.6_McGeant12_NoEff_NoBg_all.root",
		// TString ppname    = "Results/ForUnfolding_R0.6_ForPaper_Pp12_JP2_NoEff_NoBgSub.root",


		const int MaxnBayes2D = 4
		) {

  bool ClosureTest = ( trainname.Contains( "ForClosure" ) || ppname.Contains( "ForClosure" ) );
  if ( ClosureTest && trainname != ppname ){
    cerr << "Inconsistent naming for closure test" << endl;
    return -1;
  }
  
  // if >0, take this N as the nominal value and provide
  // systematics histos using the full difference to N+1
  // (symmetrized for easiness. FIXME)
  int NDefault=2;
  
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

  // Unfold 1D
  // ---------
  int MaxnBayes1D=4;
  RooUnfoldResponse* IncPtResponse = (RooUnfoldResponse*) ftrain->Get("IncPtResponse");
  TH1D* IncPtTestMeas  = (TH1D*) IncTestMeas2D->ProjectionX("IncPtTestMeas");

  TH1D** DefaultPtBayesUnfolded = new TH1D*[MaxnBayes1D];
  new TCanvas; gPad->SetLogy();
  for ( int nBayes1D=1; nBayes1D<=MaxnBayes1D; ++nBayes1D ){
    cout << endl << "=== Unfolding distribution with nBayes1D=" << nBayes1D << " ===" << endl;
    RooUnfoldBayes DefaultPtBayesUnfold ( IncPtResponse, IncPtTestMeas, nBayes1D);

    DefaultPtBayesUnfold.SetVerbose(1);
    DefaultPtBayesUnfolded[nBayes1D-1] = (TH1D*) DefaultPtBayesUnfold.Hreco( RooUnfold::kCovariance ); // RooUnfold::kCovariance seems to be the default
     
    TString hname = "DefaultBayesUnfolded_"; hname+=nBayes1D;
    DefaultPtBayesUnfolded[nBayes1D-1] ->SetName(hname);

    DefaultPtBayesUnfolded[nBayes1D-1]->SetLineColor( nBayes1D+1 );
    DefaultPtBayesUnfolded[nBayes1D-1]->Draw("same");
  }


  // ---------------------
  // Now mess up the prior
  // ---------------------
  
  // // Flat -- doesn't work
  // RooUnfoldResponse* FlatPriorResponse = IncPtResponse->Clone("FlatPriorResponse");
  // TH1D* prior = (TH1D*) FlatPriorResponse->Htruth();
  // float integral=prior->Integral();
  // for (int i=1; i<=prior->GetNbinsX(); ++i ){
  //   prior->SetBinContent( i, integral/prior->GetNbinsX() );
  //   prior->SetBinError( i, 0);
  // }
  
  // TH1D** FlatPriorPtBayesUnfolded = new TH1D*[MaxnBayes1D];
  // new TCanvas; gPad->SetLogy();
  // for ( int nBayes1D=1; nBayes1D<=MaxnBayes1D; ++nBayes1D ){
  //   cout << endl << "=== Unfolding distribution with nBayes1D=" << nBayes1D << " ===" << endl;
  //   RooUnfoldBayes FlatPriorPtBayesUnfold ( FlatPriorResponse, IncPtTestMeas, nBayes1D);

  //   FlatPriorPtBayesUnfold.SetVerbose(1);
  //   FlatPriorPtBayesUnfolded[nBayes1D-1] = (TH1D*) FlatPriorPtBayesUnfold.Hreco( RooUnfold::kCovariance ); // RooUnfold::kCovariance seems to be the default
     
  //   TString hname = "FlatPriorBayesUnfolded_"; hname+=nBayes1D;
  //   FlatPriorPtBayesUnfolded[nBayes1D-1] ->SetName(hname);

  //   FlatPriorPtBayesUnfolded[nBayes1D-1]->SetLineColor( nBayes1D+1 );
  //   FlatPriorPtBayesUnfolded[nBayes1D-1]->Draw("same");
  // }
  // return 0;

  // // -------------
  // // dumb exponent
  // // -------------
  // RooUnfoldResponse* ExpoPriorResponse = IncPtResponse->Clone("ExpoPriorResponse");
  // TH1D* prior = (TH1D*) ExpoPriorResponse->Htruth(); // The actual object, we're directly changing it

  // new TCanvas; gPad->SetLogy();
  // DefaultPtBayesUnfolded[0]->DrawCopy("");
      
  // // prior->DrawCopy();
  // // prior->Fit( "expo", "","same",20, 40);
  // prior->Fit( "expo", "0","",20, 40);  

  // TF1* myexpo =   (TF1*) prior->GetFunction("expo") ->Clone ( "myexpo");
  // prior->Reset();
  // prior->Eval( myexpo );

  // // Remove errors and small bins
  // for (int i=1; i<=prior->GetNbinsX(); ++i ){
  //   if ( prior->GetBinCenter( i )<5 ) prior->SetBinContent( i, 0 );
  //   prior->SetBinError( i, 0);
  // }
  
  // TH1D** ExpoPriorPtBayesUnfolded = new TH1D*[MaxnBayes1D];
  // // new TCanvas; gPad->SetLogy();
  // for ( int nBayes1D=1; nBayes1D<=MaxnBayes1D; ++nBayes1D ){
  //   cout << endl << "=== Unfolding distribution with nBayes1D=" << nBayes1D << " ===" << endl;
  //   RooUnfoldBayes ExpoPriorPtBayesUnfold ( ExpoPriorResponse, IncPtTestMeas, nBayes1D );
  //   // RooUnfoldSvd ExpoPriorPtBayesUnfold ( ExpoPriorResponse, IncPtTestMeas, nBayes1D);

  //   ExpoPriorPtBayesUnfold.SetVerbose(1);
  //   ExpoPriorPtBayesUnfolded[nBayes1D-1] = (TH1D*) ExpoPriorPtBayesUnfold.Hreco( RooUnfold::kCovariance ); // RooUnfold::kCovariance seems to be the default
     
  //   TString hname = "ExpoPriorBayesUnfolded_"; hname+=nBayes1D;
  //   ExpoPriorPtBayesUnfolded[nBayes1D-1] ->SetName(hname);

  //   ExpoPriorPtBayesUnfolded[nBayes1D-1]->SetLineColor( nBayes1D+1 );
  //   ExpoPriorPtBayesUnfolded[nBayes1D-1]->DrawCopy("same");
  // }
  // return 0;
  
  // // Rescaled  RESPONSE
  // // ------------------
  // // Now reweight the response
  // TH2D* scaledresponse = IncPtResponse->Hresponse()->Clone("scaledresponse");

  // // Also scale the truth the same way?
  // // That seems to do the wrong thing.
  // TH1D* scaledtruth = IncPtResponse->Htruth()->Clone("scaledtruth");
  
  // for ( int j=1; j<scaledresponse->GetNbinsY(); ++j ){
  //   float scaler = 1./j;
  //   // double scaler = 1e-1;
  //   scaledtruth->SetBinContent(j, scaler*scaledtruth->GetBinContent(j) );
  //   for ( int i=1; i<scaledresponse->GetNbinsX(); ++i ){
  //     scaledresponse->SetBinContent(i,j, scaler*scaledresponse->GetBinContent(i,j ) );
  //   }
  // }
  // // new TCanvas;  gPad->SetLogy(); scaledresponse->ProjectionY()->Draw();
  // // new TCanvas;  gPad->SetLogy(); ScaledResponse->Hresponse()->ProjectionY()->Draw();
  // // return 0;

  // RooUnfoldResponse* ScaledResponse = new RooUnfoldResponse;
  // ScaledResponse->Setup ( IncPtResponse->Hmeasured(), IncPtResponse->Htruth(), scaledresponse );
  // // ScaledResponse->Setup ( IncPtResponse->Hmeasured(), scaledtruth, scaledresponse );
  
  // // TVectorD& Vprior = ScaledResponse->Vtruth();
  // // Vprior = RooUnfoldResponse::H2V( prior, prior->GetNbinsX() );
  
  // TH1D** ScaledPtBayesUnfolded = new TH1D*[MaxnBayes1D];
  // new TCanvas; gPad->SetLogy();
  // DefaultPtBayesUnfolded[0]->DrawCopy("");
  
  // for ( int nBayes1D=1; nBayes1D<=MaxnBayes1D; ++nBayes1D ){
  //   cout << endl << "=== Unfolding distribution with nBayes1D=" << nBayes1D << " ===" << endl;
  //   RooUnfoldBayes ScaledPtBayesUnfold ( ScaledResponse, IncPtTestMeas, nBayes1D );

  //   ScaledPtBayesUnfold.SetVerbose(1);
  //   ScaledPtBayesUnfolded[nBayes1D-1] = (TH1D*) ScaledPtBayesUnfold.Hreco( RooUnfold::kCovariance ); // RooUnfold::kCovariance seems to be the default
     
  //   TString hname = "ScaledBayesUnfolded_"; hname+=nBayes1D;
  //   ScaledPtBayesUnfolded[nBayes1D-1] ->SetName(hname);

  //   ScaledPtBayesUnfolded[nBayes1D-1]->SetLineColor( nBayes1D+1 );
  //   ScaledPtBayesUnfolded[nBayes1D-1]->DrawCopy("same");
  // }
  // // prior->DrawCopy("same");

  // Finally, scale more cleverly
  // ----------------------------
  TF1* LevyFit_pT            = new TF1("LevyFit_pT",LevyFitFunc_pT,0.0,60.0,4);

  float T_prior = 0.2; // 0.6? 1?
  float n_prior = 7; // 5.8?

  LevyFit_pT->SetParNames("B","T_prior","n_prior","m0"); // B, changes the amplitude, 0.1

  LevyFit_pT->SetParameter(0,1e12); // B, changes the amplitude, 0.1
  LevyFit_pT->SetParameter(1,T_prior); // T, changes slope, 0.4
  //   LevyFit_pT->SetParLimits(1,0.5*T_prior,1.5*T_prior); // T, changes slope, 0.4
  
  LevyFit_pT->SetParameter(2,n_prior); // n, changes how fast spectrum drops, 5.8
  //  LevyFit_pT->SetParLimits(2,0.5*n_prior,1.5*n_prior); // T, changes slope, 0.4

  // LevyFit_pT->FixParameter(3,0.14); // m0, changes the width, 0.0001 --> that parameter seems to have no influence on the function
  // LevyFit_pT->FixParameter(3,0.02); // m0, changes the width, 0.0001 --> that parameter seems to have no influence on the function
  LevyFit_pT->SetParameter(3,0.02); // m0, changes the width, 0.0001 --> that parameter seems to have no influence on the function

  LevyFit_pT->SetRange(5,60);
  LevyFit_pT->SetLineWidth(2);
  LevyFit_pT->SetLineStyle(1);
  LevyFit_pT->SetLineColor(2);
  // LevyFit_pT->SetNpx(1000);

  gROOT->ForceStyle();
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1);
  DefaultPtBayesUnfolded[0]->UseCurrentStyle(); // for some reason necessary.
	

  // new TCanvas; gPad->SetLogy();
  // DefaultPtBayesUnfolded[0]->Fit( LevyFit_pT,"IM","", 5,55 );
  // return 0;

  
  // I hate Levy. Never converges right
  // Different tack, from https://arxiv.org/pdf/1411.4969.pdf
  TF1* Tsallis = new TF1("Tsallis","[0] * pow(1+x/[1],-[2])",0.0,60.0);
  Tsallis->SetParNames("a","b","c");

  TF1* Tsallis1 = new TF1("Tsallis1","[0] * pow(1+x/[1],-[2])",0.0,60.0);
  Tsallis1->SetParNames("a","b","c");
  TF1* Tsallis2 = new TF1("Tsallis2","[0] * pow(1+x/[1],-[2])",0.0,60.0);
  Tsallis2->SetParNames("a","b","c");
  TF1* Tsallis3 = new TF1("Tsallis3","[0] * pow(1+x/[1],-[2])",0.0,60.0);
  Tsallis3->SetParNames("a","b","c");

  
  TH1D* origtruth = (TH1D*) IncPtResponse->Htruth()->Clone("origtruth");
  //  origtruth->Fit( Tsallis,"IM0","", 5,60 );
  new TCanvas; gPad->SetLogy();
  Tsallis->SetParameter(0,2);
  Tsallis->SetParameter(1,8.5);
  Tsallis->SetParameter(2,10.8);
  origtruth->Fit( Tsallis,"IM","", 5,60 );

  Tsallis1->SetLineColor(kGreen+1);
  Tsallis1->SetParameter(0,2);
  Tsallis1->FixParameter(1,8.5);
  Tsallis1->FixParameter(2,10.8);
  origtruth->Fit( Tsallis1,"IM+","same", 5,60 );

  Tsallis2->SetLineColor(kRed);
  Tsallis2->SetParameter(0,2);
  Tsallis2->FixParameter(1,9);
  Tsallis2->FixParameter(2,11);
  origtruth->Fit( Tsallis2,"IM+","same", 5,60 );

  Tsallis3->SetLineColor(kBlue);
  Tsallis3->SetParameter(0,2);
  Tsallis3->FixParameter(1,8);
  Tsallis3->FixParameter(2,9.5);
  origtruth->Fit( Tsallis3,"IM+","same", 5,60 );

  // Alright, this is an atrocious fit, but let's see what it does
  TH2D* tsallisscaledresponse = IncPtResponse->Hresponse()->Clone("tsallisscaledresponse");

  // And save the scaler for future use
  TH1D* TsallisScaler = origtruth->Clone("TsallisScaler");
  TsallisScaler->Reset();
  
  for ( int j=1; j<tsallisscaledresponse->GetNbinsY(); ++j ){
    float pt = tsallisscaledresponse->GetYaxis()->GetBinCenter(j);
    if (pt<5) continue;
    if (origtruth->GetBinContent(j)<1e-12) continue;
    // double scaler = Tsallis->Eval(pt) / origtruth->GetBinContent(j);
    double scaler = Tsallis3->Eval(pt) / origtruth->GetBinContent(j);

    TsallisScaler->SetBinContent(j, scaler );
    TsallisScaler->SetBinError(j, 0 );
    for ( int i=1; i<tsallisscaledresponse->GetNbinsX(); ++i ){
      tsallisscaledresponse->SetBinContent(i,j, scaler*tsallisscaledresponse->GetBinContent(i,j ) );
    }
  }
  TFile * fScaler = new TFile ( "TsallisScaler.root","RECREATE");
  TsallisScaler->Write();
  Tsallis3->Write();

  RooUnfoldResponse* TsallisScaledResponse = new RooUnfoldResponse;
  TsallisScaledResponse->Setup ( IncPtResponse->Hmeasured(), IncPtResponse->Htruth(), tsallisscaledresponse );

  new TCanvas; gPad->SetLogy();
  IncPtResponse->Hresponse()->ProjectionY("py")->Draw();
  TsallisScaledResponse->Hresponse()->ProjectionY("bpy")->Draw("same");

  new TCanvas; gPad->SetLogy();
  IncPtResponse->Hresponse()->ProjectionX("px")->Draw();
  TsallisScaledResponse->Hresponse()->ProjectionX("bpx")->Draw("same");

  return 0;
  
							   
  
  TH1D** TsallisScaledPtBayesUnfolded = new TH1D*[MaxnBayes1D];
  // new TCanvas; gPad->SetLogy();
  DefaultPtBayesUnfolded[0]->DrawCopy("");
  new TCanvas; 
  TH1D* dummy = DefaultPtBayesUnfolded[0]->Clone("dummy");
  dummy->Reset();
  dummy->SetAxisRange (0, 2,"y");
  dummy->Draw();
  
  for ( int nBayes1D=1; nBayes1D<=MaxnBayes1D; ++nBayes1D ){
    cout << endl << "=== Unfolding distribution with nBayes1D=" << nBayes1D << " ===" << endl;
    RooUnfoldBayes TsallisScaledPtBayesUnfold ( TsallisScaledResponse, IncPtTestMeas, nBayes1D );

    TsallisScaledPtBayesUnfold.SetVerbose(1);
    TsallisScaledPtBayesUnfolded[nBayes1D-1] = (TH1D*) TsallisScaledPtBayesUnfold.Hreco( RooUnfold::kCovariance ); // RooUnfold::kCovariance seems to be the default
     
    TString hname = "TsallisScaledBayesUnfolded_"; hname+=nBayes1D;
    TsallisScaledPtBayesUnfolded[nBayes1D-1] ->SetName(hname);

    TsallisScaledPtBayesUnfolded[nBayes1D-1]->SetLineColor( nBayes1D+1 );
    // TsallisScaledPtBayesUnfolded[nBayes1D-1]->DrawCopy("same");
    // TH1D* ratio = DefaultPtBayesUnfolded[1]->Clone("ratio");
    TH1D* ratio = DefaultPtBayesUnfolded[nBayes1D-1]->Clone("ratio");
    ratio->Divide(TsallisScaledPtBayesUnfolded[nBayes1D-1]);
    ratio->SetLineColor( nBayes1D+1 );
    ratio->DrawCopy("same");
  }

  // Not too shabby!
  return 0;
  
  // Same problem. Oh well, do it by hand
  // TF1* myfunc = new TF1("myfunc","[0] * exp(-x/[1]) + [2] * pow(x,-[3])",0.0,60.0);
  // myfunc->SetParNames( "A","T","B","n");
  // myfunc->SetParameters( 1e11, 0.2, 1e10, 6);

  // TF1* myfunc = new TF1("myfunc","[0] * exp(-x/[1]) + [2] * exp(-x/[3])",0.0,60.0);
  // myfunc->SetParNames( "A","T","B","T2");
  // myfunc->SetParameters( 1e11, 0.2, 1e7, 0.4);

  TF1* myfunc = new TF1("myfunc","[0] * pow(x,-[1]) + [2] * pow(x,-[3])",0.0,60.0);
  myfunc->SetParNames( "A","n1","B","n2");
  myfunc->SetParameters( 1e13, 6.2, 1e16, 9.1);

  // // Smoothly broken power law
  // TF1* myfunc = new TF1("myfunc","[0] * ( pow(x,-[1]*[3]) + pow(x,-[2]*[3]) )",0.0,60.0);
  // myfunc->SetParNames( "A","n1","n2","S");
  // myfunc->SetParameters( 1e13, 6.2, 9.1, 2);

  // myfunc->FixParameter(0,0);  myfunc->FixParameter(1,0);
  new TCanvas; gPad->SetLogy();
  DefaultPtBayesUnfolded[0]->Fit( myfunc,"","", 5,40 );
  
  
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


// ---------------------------------------------------------------------------
Double_t LevyFitFunc_pT(Double_t* x_val, Double_t* par)
{
  // One over pT term is removed -> original pT distribution
  // Fit function for d2N/(2pi dpT dy)
  // taken from here: http://sampa.if.usp.br/~suaide/blog/files/papers/PLB6372006.pdf
  Double_t pT, y, B, T, n, m0;
  B    = par[0];
  T    = par[1];
  n    = par[2];
  m0   = par[3];
  pT   = x_val[0];
  Double_t mT = TMath::Sqrt(pT*pT+m0*m0);
  y = pT*B/TMath::Power(1.0+(mT-m0)/(n*T),n);
    return y;
}
