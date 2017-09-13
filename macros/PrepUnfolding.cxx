#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TFile.h>
#include <TProfile.h>
#include <TLine.h>

#include <TLorentzVector.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TObjArray.h>
#include <TClonesArray.h>
#include <TString.h>
#include <TChain.h>
#include <TBranch.h>
#include <TMath.h>
#include <TRandom.h>
#include <TSystem.h>


#include "TStarJetVectorContainer.h"
#include "TStarJetVector.h"
#include "TStarJetVectorJet.h"
#include "TStarJetPicoUtils.h"

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <cmath>
#include <exception>

#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#include "RooUnfoldSvd.h"
#include "RooUnfoldTUnfold.h"

using namespace std;

int PrepUnfolding ( ) {
  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.1f);
  gStyle->SetTitleW(0.8f);
  gStyle->SetTitleBorderSize(0);	//Title box border thickness
  gStyle->SetHistLineWidth(2);

  TLegend* leg = 0;
  
  float RCut = 0.4;
  float EtaCut = 1.0-RCut;

    
  // Input
  // -----
  TString PpLevelFile = "Results/Recut_Pp_HT54_MIP_NoEff_NoBgSub.root"; // pp-like events
  // --- Use one of these for Run 12: ---
  // TString PpLevelFile = "Results/ForPaper_Pp12_HT54_JP2_NoEff_NoBgSub.root"; // pp-like events
  // TString PpLevelFile = "Results/ForPaper_Pp12_JP2_NoEff_NoBgSub.root"; // pp-like events
  // --- MIP or other hadronic correction cross check ---
  // TString PpLevelFile = "Results/ForPaper_Pp12_JP2_MIP_NoEff_NoBgSub.root"; // pp-like events
  // --- Pythia8 ---
  // TString PpLevelFile = "Results/ForPaper_Pythia8_NoEff_NoBgSub.root";
  

  // Output
  // ------
  TString OutFileName = "Results/ForUnfolding_";
  if ( PpLevelFile.Contains ("ythia") ) OutFileName = "Results/Prepped_";
  OutFileName += gSystem->BaseName(PpLevelFile);
  

  // Refmult Cut
  // -----------
  int RefMultCut=0;

  // Set up pp events
  // ----------------
  // TChains don't work (well)!
  // "The index values from the first tree should be less then all the index values from the second tree, and so on."
  TFile* Ppf = new TFile( PpLevelFile );
  TTree* PpChain = (TTree*) Ppf->Get("ResultTree");
  PpChain->BuildIndex("runid","eventid");

  TClonesArray* PpJets = new TClonesArray("TStarJetVectorJet");
  PpChain->GetBranch("Jets")->SetAutoDelete(kFALSE);
  PpChain->SetBranchAddress("Jets", &PpJets);

  TClonesArray* PpGroomedJets = new TClonesArray("TStarJetVectorJet");
  PpChain->GetBranch("GroomedJets")->SetAutoDelete(kFALSE);
  PpChain->SetBranchAddress("GroomedJets", &PpGroomedJets);

  // For MC, and in general without an explicit HT cut, these are most likely meaningless
  TStarJetVector* PpTriggerHT = new TStarJetVector(); // The HT particle
  PpChain->SetBranchAddress("TriggerHT", &PpTriggerHT);
  
  TStarJetVectorJet* PpHTJet = new TStarJetVectorJet(); // The jet containing it
  PpChain->SetBranchAddress("HTJet", &PpHTJet);  


  int ppeventid;
  int pprunid;
  double ppweight;     // Double-check, should be the same as below
  PpChain->SetBranchAddress("eventid", &ppeventid);
  PpChain->SetBranchAddress("runid", &pprunid);
  PpChain->SetBranchAddress("weight",&ppweight );

  int ppnjets=0;
  PpChain->SetBranchAddress("njets", &ppnjets );

  double ppzg[1000];
  PpChain->SetBranchAddress("zg", ppzg );

  // Output and histograms
  // ----------------------
  TFile* fout = new TFile( OutFileName, "RECREATE");
  
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);

  // Set up Data
  // ----------------------
  int nZgBinsTrue = 20;
  float zgminTrue = 0.00;
  float zgmaxTrue = 0.5;
  // int nZgBinsTrue = 20;
  // float zgminTrue = 0.05;
  // float zgmaxTrue = 0.55;
  int nZgBinsMeas = nZgBinsTrue;
  float zgminMeas = zgminTrue;
  float zgmaxMeas = zgmaxTrue;

  int nPtBins = 80;
  float ptmin=0;
  float ptmax=80;
  int nPtBinsTrue =  nPtBins;
  float ptminTrue =  ptmin;
  float ptmaxTrue =  ptmax;

  int nPtBinsMeas =  60;
  float ptminMeas =  0;
  float ptmaxMeas =  60;

  // Only need one of these
  TH2D* IncMeas2D  = new TH2D( "IncMeas2D", "Measured z_{g} vs. p_{T};p_{T};z_{g}", nPtBinsMeas, ptminMeas, ptmaxMeas, nZgBinsMeas, zgminMeas, zgmaxMeas);
  TH2D* IncTestMeas2D  = new TH2D( "IncTestMeas2D", "TEST Measured z_{g} vs. p_{T};p_{T};z_{g}", nPtBinsMeas, ptminMeas, ptmaxMeas, nZgBinsMeas, zgminMeas, zgmaxMeas);

  // TH2D* TrigMeas2D  = new TH2D( "TrigMeas2D", "TRAIN z_{g}^{lead} vs. p_{T}^{lead}, Pythia in MC", nPtBinsMeas, ptminMeas, ptmaxMeas, nZgBinsMeas, zgminMeas, zgmaxMeas);
  // TH2D* TrigTestMeas2D  = new TH2D( "TrigTestMeas2D", "TEST z_{g}^{lead} vs. p_{T}^{lead}, Pythia in MC", nPtBinsMeas, ptminMeas, ptmaxMeas, nZgBinsMeas, zgminMeas, zgmaxMeas);

  // TH2D* RecoilMeas2D  = new TH2D( "RecoilMeas2D", "TRAIN z_{g}^{lead} vs. p_{T}^{lead}, Pythia in MC", nPtBinsMeas, ptminMeas, ptmaxMeas, nZgBinsMeas, zgminMeas, zgmaxMeas);
  // TH2D* RecoilTestMeas2D  = new TH2D( "RecoilTestMeas2D", "TEST z_{g}^{lead} vs. p_{T}^{lead}, Pythia in MC", nPtBinsMeas, ptminMeas, ptmaxMeas, nZgBinsMeas, zgminMeas, zgmaxMeas);

  // ------------------------
  // Loop over measured level
  // ------------------------

  cout << "Loop over measured data" << endl;
  cout << "------------------------" << endl;

  for ( Long64_t ppEvi = 0; ppEvi< PpChain->GetEntries() ; ++ppEvi ){
    if ( !(ppEvi%10000) ) cout << "Working on " << ppEvi << " / " << PpChain->GetEntries() << endl;

    PpChain->GetEntry(ppEvi);

    for (int j=0; j<ppnjets; ++j ){
      TStarJetVectorJet* ppjet = (TStarJetVectorJet*) PpJets->At(j);
      TStarJetVectorJet* ppgjet = (TStarJetVectorJet*) PpGroomedJets->At(j);
      
      // Record Measured
      // ---------------
      if ( fabs ( ppjet->Eta() ) < EtaCut ) {
	IncMeas2D->Fill( ppjet->Pt(), ppzg[j], ppweight );
	IncTestMeas2D->Fill( ppjet->Pt(), ppzg[j], ppweight );
      }
    }

    
  }

  TH1D* PpIncPt = (TH1D*)IncMeas2D->ProjectionX("PpIncPt");
    
  new TCanvas ( "c0","");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  gPad->SetLogy();
  PpIncPt->SetLineColor(kRed);
  PpIncPt->Draw("9");
  leg = new TLegend( 0.55, 0.75, 0.89, 0.9, "" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);
  leg->AddEntry( PpIncPt->GetName(), "Inclusive Jets");
  leg->Draw();

  TH1D* PpIncZg2030 = (TH1D*)IncMeas2D->ProjectionY("PpIncZg2030", IncMeas2D->GetXaxis()->FindBin(20+0.01), IncMeas2D->GetXaxis()->FindBin(30-0.01) );
  new TCanvas ( "c1","");
  gPad->SetGridx(0);  gPad->SetGridy(0);
  PpIncZg2030->SetLineColor(kRed);
  PpIncZg2030->Draw("9");
  leg = new TLegend( 0.55, 0.75, 0.89, 0.9, "" );
  leg->SetBorderSize(0);
  leg->SetLineWidth(10);
  leg->SetFillStyle(0);
  leg->SetMargin(0.1);
  leg->AddEntry( PpIncZg2030->GetName(), "Inclusive Jets");
  leg->Draw();

  fout->Write();

  cout << " Wrote to" << endl << OutFileName << endl;
  return 0;
}
