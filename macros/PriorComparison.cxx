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

#include <vector>
#include <string>

int PriorComparison(){

  gStyle->SetHistLineWidth(2);
  gStyle->SetFrameLineWidth(2);
  gStyle->SetOptStat(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);

  gStyle->SetTitleX(0.1f);
  gStyle->SetTitleW(0.8f);
  gStyle->SetTitleBorderSize(0);	//Title box border thickness

  TString sGood = "Results/GoodPrior.root"; 
  // TString sBent = "Results/TruePtBentZgPrior.root";
  // TString sBent = "Results/BentPtTrueZgPrior.root";
  // TString sBent = "Results/BentPtBentZgPrior.root";

  TString sBent = "Results/NewTruePtBentZgPrior.root";
  // TString sBent = "Results/NewBentPtTrueZgPrior.root";
  // TString sBent = "Results/NewBentPtBentZgPrior.root";
  // TString sBent = "Results/TsallisNewBentPtTrueZgPrior.root";
  // TString sBent = "Results/TsallisNewBentPtBentZgPrior.root";

  TString PlotBase = "plots/PriorComparison_";
  PlotBase += gSystem->BaseName(sBent);
  PlotBase.ReplaceAll(".root","");
  int itdefault = 2;

  TFile* fGood = new TFile( sGood, "READ");
  TFile* fBent = new TFile( sBent, "READ");

  // pT spectrum
  // -----------
  TH1D* ptGood = (TH1D*) fGood->Get( TString("Incunfoldpt_")+TString::Itoa(itdefault,10) );

  new TCanvas;
  TLegend* leg = new TLegend( 0.2, 0.55, 0.49, 0.9, "Inclusive Jet p_{T}" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);

  gPad->SaveAs( PlotBase + "_Everything.pdf[");
  
  for ( int it = 1; it<=4 ; ++it ){
    TH1D* ptBent = (TH1D*) fBent->Get( TString("Incunfoldpt_")+TString::Itoa(it,10) );
    ptBent->SetName( TString("ptBent_")+TString::Itoa(it,10) );
    TH1D* ptRatio = (TH1D*) ptGood->Clone ( TString("ptRatio_")+TString::Itoa(it,10) );
    ptRatio->Divide( ptBent );
    ptRatio->SetAxisRange(0.0,2.0,"y");
    ptRatio->SetAxisRange(0.0,60.0,"x");
    ptRatio->SetTitle(";p_{T} [GeV/c];ratio");
    ptRatio->SetLineColor( it+1 );
    if ( it==1 ) ptRatio->Draw("hist");
    else         ptRatio->Draw("histsame");
    
    TString title = "N = ";
    title +=it;
    leg->AddEntry(ptRatio->GetName(), title);
  }
  leg->Draw();
  
  gPad->SaveAs( PlotBase + "_Everything.pdf");

  // zg
  // -----------
  vector<string> ptranges;
  ptranges.push_back ( "1015" );
  ptranges.push_back ( "1520" );
  ptranges.push_back ( "2025" );
  ptranges.push_back ( "2530" );
  ptranges.push_back ( "3040" );
  ptranges.push_back ( "4060" );


  for ( int pti = 0; pti<ptranges.size() ; ++pti ){
    cout << " =========== Working on " << pti << " pt = " << ptranges.at(pti) << " =============" << endl;
	  
    TString hname = TString("Incunfold_")+TString(ptranges.at(pti).c_str()) + TString("_") + TString::Itoa(itdefault,10) ;
    TH1D* zgGood = (TH1D*) fGood->Get( hname );
    
    new TCanvas;
    leg = new TLegend( 0.2, 0.55, 0.7, 0.9, TString ("Inclusive Jet z_{g}, p_{T} = ") + TString(ptranges.at(pti).c_str()) );
    leg->SetBorderSize(0);
    leg->SetLineWidth(10);
    leg->SetFillStyle(0);
    leg->SetMargin(0.1);
    float chibest = -1;
    int Nbest = 0;
    TH1D* RatioBest=0;
    for ( int it = 1; it<=4 ; ++it ){
      TString hname = TString("Incunfold_")+TString(ptranges.at(pti).c_str()) + TString("_") + TString::Itoa(it,10) ;
      TH1D* zgBent = (TH1D*) fBent->Get( hname );
      
      zgBent->SetName( TString("zgBent_")+TString(ptranges.at(pti).c_str()) + TString("_") + TString::Itoa(it,10) );
      TH1D* zgRatio = (TH1D*) zgGood->Clone ( TString("zgRatio_")+TString(ptranges.at(pti).c_str()) + TString("_") + TString::Itoa(it,10) );
      zgRatio->Divide( zgBent );
      zgRatio->SetAxisRange(0.5,1.5,"y");
      zgRatio->SetAxisRange(0.0,0.55,"x");
      zgRatio->SetTitle(";z_{g};ratio");
      zgRatio->SetLineColor( it+1 );
      if ( it==1 ) zgRatio->Draw("hist");
      else         zgRatio->Draw("histsame");
      
      TString title = "N = ";
      title +=it;
      leg->AddEntry(zgRatio->GetName(), title);

      // Ignore 0-bin
      zgGood->SetAxisRange( 0.1+0.001, 0.5-0.001 );      
      //cout << title << "  " << zgGood->Chi2Test( zgBent, "WP" ) << endl;
      float chicurr =zgGood->Chi2Test( zgBent, "W" );
      if ( chicurr > chibest ){
	// cout << "Updating chibest" << endl;
	chibest = chicurr;
	Nbest = it;
	RatioBest = zgRatio;
      }

    }
    leg->Draw();

    if ( Nbest < 1 || chibest<0 ){
      cerr << "chisquare never improved??" << endl;
      return -1;
    }
    cout << "Choosing N = " << Nbest << " with chibest = " << chibest << endl;
    float errmax = 0;
    float err = 0;
    int nbinscounted=0;
    for (int i=RatioBest->FindBin (0.1+0.001) ; i<=RatioBest->GetNbinsX() ; ++i){
      errmax = TMath::Max ( errmax, fabs ( RatioBest->GetBinContent(i) - 1 ) );
      // cout <<i << "  " <<  RatioBest->GetBinCenter(i) << "  " << RatioBest->GetBinContent(i)<< "  " << errmax << endl ;
      err += fabs ( RatioBest->GetBinContent(i) - 1 );
      nbinscounted++;
    }
    cout << " Maximum relative error is " << errmax << endl;
    cout << " Average relative error is " << err / nbinscounted << endl << endl;
    //if ( pti > 1 )      return 0;
	
    // gPad->SaveAs( PlotBase + "_Everything.pdf");

  }

  gPad->SaveAs( PlotBase + "_Everything.pdf]");
  
}
