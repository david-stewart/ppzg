// HAS to be compiled,
// root -l macros/MatchGeantToPythia.cxx+


#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TFile.h>
#include <TProfile.h>
#include <TLine.h>

#include <TROOT.h>
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

#include <TStarJetVector.h>
#include <TStarJetVectorJet.h>

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

// Load helper macro
#include "NewGeantWeightReject.hh"

// ----------------------------------------------------
class RootGroomingResultStruct{
public:
  TStarJetVectorJet orig;
  TStarJetVectorJet groomed;
  double zg;
  RootGroomingResultStruct ( TStarJetVectorJet orig, TStarJetVectorJet groomed, double zg ) : orig(orig), groomed(groomed),zg(zg){};
  
  // static bool origptgreater( GroomingResultStruct const & a, GroomingResultStruct const & b) { 
  //   return a.orig.pt()>b.orig.pt();
  // };
  
  // static bool groomedptgreater( GroomingResultStruct const & a, GroomingResultStruct const & b) { 
  //   return a.groomed.pt()>b.groomed.pt();
  // }; 

  ClassDef(RootGroomingResultStruct,1)

};

typedef pair<RootGroomingResultStruct,RootGroomingResultStruct> MatchedRootGroomingResultStruct;

int MatchGeantToPythia (
			// --- Latest Run 6 --- 
			// TString PpLevelFile = "Results/AEff0_PtSmear0_ATow0_SystGeant_NoEff_NoBg_HT54.root",
			// TString McLevelFile = "Results/Recut_McGeant_NoEff_NoBg_MB.root" // Reference (particle level) jets
			// --- Default for Run 12: ---
			TString PpLevelFile = "Results/AEff0_PtSmear0_ATow0_SystGeant12_NoEff_NoBg_JP2.root",
			TString McLevelFile = "Results/McGeant12_NoEff_NoBg_all.root" // Reference (particle level) jets
			// // --- MIP or other hadronic correction cross check ---
			// TString PpLevelFile = "Results/AEff0_PtSmear0_ATow0_SystGeant12_MIP_NoEff_NoBg_JP2.root",
			// TString McLevelFile = "Results/McGeant12_NoEff_NoBg_all.root" // Reference (particle level) jets
			// // // --- Different R --- 
			// TString PpLevelFile = "Results/AEff0_PtSmear0_ATow0_R0.6_SystGeant12_NoEff_NoBg_JP2.root",
			// TString McLevelFile = "Results/R0.6_McGeant12_NoEff_NoBg_all.root" // Reference (particle level) jets
			) {
  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.1f);
  gStyle->SetTitleW(0.8f);
  gStyle->SetTitleBorderSize(0);	//Title box border thickness
  gStyle->SetHistLineWidth(2);
  TLegend* leg = 0;

  bool PrepClosure=false;
    
  float MinJetPt = 5.0;
  bool UseMiss=true;
  bool UseFakes=true;
  // The embedding data has some strange quirks,
  // specifically things like 10 GeV "true" jets reconstructed at 50 GeV
  // My suspicion is that some pythia particles were not properly recorded.
  // This is supported by the fact that these low pT leading jets come
  // from quite high pHat bins.
  // Example:
  // pHat bin 35-45
  // Trigger match with dR=0.334396 pT_Part= 2.88517 pT_Det= 28.9917
  // Visualization:
  // TFile *_file0 = TFile::Open("Data/AddedGeantPythia/picoDst_35_45_4.root");
  // new TCanvas;
  // JetTree->Draw("fPrimaryTracks.GetEta():fPrimaryTracks.GetPhi()","fPrimaryTracks.GetPt()*(Entry$==5570  && abs(fPrimaryTracks.GetEta())<1.2 && fPrimaryTracks.GetPt()<30  && fPrimaryTracks.GetPt()>0.2  )","colz");
  // new TCanvas;
  // JetTree->Draw("fTowers.GetEta():fTowers.GetPhi()","fTowers.GetEnergy()/TMath::CosH(fTowers.GetEta())*( Entry$==5570  )","colz");
  // new TCanvas;
  // JetTreeMc->Draw("fPrimaryTracks.GetEta():fPrimaryTracks.GetPhi()","fPrimaryTracks.GetPt()*(Entry$==5570  && abs(fPrimaryTracks.GetEta())<1.2  && fPrimaryTracks.GetPt()>0.2  )","colz");  
  // Here's an option to reject them based on a simple cut.    
  bool RejectOutliers= false; // NOT working
  bool RejectHiweights= true;
  
  float RCut = 0.4;
  float EtaCut = 1.0-RCut;

  // Output
  // ------
  TString OutFileName = "Results/";
  if ( PrepClosure )    OutFileName += "ForClosure_";
  OutFileName += gSystem->BaseName(PpLevelFile);
  OutFileName.ReplaceAll(".root","_");
  if ( UseMiss ) OutFileName += "WithMisses_";
  else OutFileName += "NoMisses_";
  if ( UseFakes ) OutFileName += "WithFakes_";
  else OutFileName += "NoFakes_";
  if ( RejectOutliers ) OutFileName += "NoOutliers_";
  if ( !RejectHiweights ) OutFileName += "WithHiWeights_";  
    
  OutFileName += "TrainedWith_";
  OutFileName += gSystem->BaseName(McLevelFile);
  
  TString PlotBase = "plots/";
  if ( PrepClosure ) PlotBase += "ForClosure_";
  PlotBase += gSystem->BaseName(PpLevelFile);
  PlotBase.ReplaceAll(".root","_");
  if ( UseMiss ) PlotBase += "WithMisses_";
  else PlotBase += "NoMisses_";
  if ( RejectOutliers ) PlotBase += "NoOutliers_";
  PlotBase += "Trained_With";
  PlotBase += gSystem->BaseName(McLevelFile);
  PlotBase.ReplaceAll(".root","");
  
  // Refmult Cut
  // -----------
  int RefMultCut=0;

  // Load MC level tree
  // ------------------
  // TChains don't work (well)!
  // "The index values from the first tree should be less then all the index values from the second tree, and so on."
  // TChain* McChain = new TChain("ResultTree");
  // McChain->Add ( McLevelFile );

  TFile* Mcf = new TFile( McLevelFile );
  TTree* McChain = (TTree*) Mcf->Get("ResultTree");
  McChain->BuildIndex("runid","eventid");
  // McChain->GetTreeIndex()->Print();

  TClonesArray* McJets = new TClonesArray("TStarJetVectorJet");
  McChain->GetBranch("Jets")->SetAutoDelete(kFALSE);
  McChain->SetBranchAddress("Jets", &McJets);

  TClonesArray* McGroomedJets = new TClonesArray("TStarJetVectorJet");
  McChain->GetBranch("GroomedJets")->SetAutoDelete(kFALSE);
  McChain->SetBranchAddress("GroomedJets", &McGroomedJets);
  
  // For MC, and in general without an explicit HT cut, these are most likely meaningless
  TStarJetVector* McTriggerHT = new TStarJetVector(); // The HT particle
  McChain->SetBranchAddress("TriggerHT", &McTriggerHT);
  
  TStarJetVectorJet* McHTJet = new TStarJetVectorJet(); // The jet containing it
  McChain->SetBranchAddress("HTJet", &McHTJet);  
  
  int mceventid;
  int mcrunid;
  double mcweight;     // Double-check, should be the same as below
  McChain->SetBranchAddress("eventid", &mceventid);
  McChain->SetBranchAddress("runid", &mcrunid);
  McChain->SetBranchAddress("weight",&mcweight );

  int mcnjets=0;
  McChain->SetBranchAddress("njets", &mcnjets );

  double mczg[1000];
  McChain->SetBranchAddress("zg", mczg );


  // Set up pp events
  // ----------------
  // TChain* PpChain = new TChain("ResultTree");
  // PpChain->Add ( PpLevelFile );

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

  // Load different pt shape for systematics
  // ---------------------------------------
  TFile* fPtWeights = new TFile( "TsallisScaler.root","READ");
  TH1D* hPtWeights = (TH1D*) fPtWeights->Get("TsallisScaler");
  hPtWeights->SetTitle("hPtWeights");
    
  // Output and histograms
  // ----------------------
  TFile* fout = new TFile( OutFileName, "RECREATE");
  
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);


  // Set up response matrix
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

  RooUnfoldResponse IncPtResponse    ( nPtBinsMeas, ptminMeas, ptmaxMeas, nPtBinsTrue, ptminTrue, ptmaxTrue );

  RooUnfoldResponse VisualIncPtResponse    ( 10*nPtBinsMeas, ptminMeas, ptmaxMeas, 10*nPtBinsTrue, ptminTrue, ptmaxTrue );
  float ptminforvisualzg = 20;
  float ptmaxforvisualzg = 30;
  RooUnfoldResponse VisualIncZgResponse    ( 10*nZgBinsMeas, zgminMeas, zgmaxMeas, 10*nZgBinsTrue, zgminTrue, zgmaxTrue );

  // 2D unfolding:
  TH2D* hTrue= new TH2D ("hTrue", "Truth", nPtBinsTrue, ptminTrue, ptmaxTrue, nZgBinsTrue, zgminTrue, zgmaxTrue);
  TH2D* hMeas= new TH2D ("hMeas", "Measured", nPtBinsMeas, ptminMeas, ptmaxMeas, nZgBinsMeas, zgminMeas, zgmaxMeas);

  RooUnfoldResponse IncPtZgResponse2D;
  IncPtZgResponse2D.Setup (hMeas, hTrue );


  // Various responses with truth shapes reweighted to represent a different prior
  RooUnfoldResponse IncBentPtResponse    ( nPtBinsMeas, ptminMeas, ptmaxMeas, nPtBinsTrue, ptminTrue, ptmaxTrue );

  RooUnfoldResponse IncBentPtZgResponse2D;
  IncBentPtZgResponse2D.Setup (hMeas, hTrue );

  RooUnfoldResponse IncPtBentZgResponse2D;
  IncPtBentZgResponse2D.Setup (hMeas, hTrue );
  
  RooUnfoldResponse IncBentPtBentZgResponse2D;
  IncBentPtBentZgResponse2D.Setup (hMeas, hTrue );


  TH2D* IncTruth2D = new TH2D( "IncTruth2D", "TRAIN z_{g}^{lead} vs. p_{T}^{lead}, Pythia6;p_{T}^{lead};z_{g}^{lead}", nPtBinsTrue, ptminTrue, ptmaxTrue, nZgBinsTrue, zgminTrue, zgmaxTrue);
  TH2D* IncMeas2D  = new TH2D( "IncMeas2D", "TRAIN z_{g}^{lead} vs. p_{T}^{lead}, Pythia6 #oplus GEANT;p_{T}^{lead};z_{g}^{lead}", nPtBinsMeas, ptminMeas, ptmaxMeas, nZgBinsMeas, zgminMeas, zgmaxMeas);
  TH2D* IncTestTruth2D = new TH2D( "IncTestTruth2D", "TEST z_{g}^{lead} vs. p_{T}^{lead}, Pythia6;p_{T}^{lead};z_{g}^{lead}", nPtBinsTrue, ptminTrue, ptmaxTrue, nZgBinsTrue, zgminTrue, zgmaxTrue);
  TH2D* IncTestMeas2D  = new TH2D( "IncTestMeas2D", "TEST z_{g}^{lead} vs. p_{T}^{lead}, Pythia6 #oplus GEANT;p_{T}^{lead};z_{g}^{lead}", nPtBinsMeas, ptminMeas, ptmaxMeas, nZgBinsMeas, zgminMeas, zgmaxMeas);

  // TH2D* TrigTruth2D = new TH2D( "TrigTruth2D", "TRAIN z_{g}^{lead} vs. p_{T}^{lead}, Pythia6;p_{T}^{lead};z_{g}^{lead}", nPtBinsTrue, ptminTrue, ptmaxTrue, nZgBinsTrue, zgminTrue, zgmaxTrue);
  // TH2D* TrigMeas2D  = new TH2D( "TrigMeas2D", "TRAIN z_{g}^{lead} vs. p_{T}^{lead}, Pythia6 #oplus GEANT;p_{T}^{lead};z_{g}^{lead}", nPtBinsMeas, ptminMeas, ptmaxMeas, nZgBinsMeas, zgminMeas, zgmaxMeas);
  // TH2D* TrigTestTruth2D = new TH2D( "TrigTestTruth2D", "TEST z_{g}^{lead} vs. p_{T}^{lead}, Pythia6;p_{T}^{lead};z_{g}^{lead}", nPtBinsTrue, ptminTrue, ptmaxTrue, nZgBinsTrue, zgminTrue, zgmaxTrue);
  // TH2D* TrigTestMeas2D  = new TH2D( "TrigTestMeas2D", "TEST z_{g}^{lead} vs. p_{T}^{lead}, Pythia6 #oplus GEANT;p_{T}^{lead};z_{g}^{lead}", nPtBinsMeas, ptminMeas, ptmaxMeas, nZgBinsMeas, zgminMeas, zgmaxMeas);

  // TH2D* RecoilTruth2D = new TH2D( "RecoilTruth2D", "TRAIN z_{g}^{lead} vs. p_{T}^{lead}, Pythia6;p_{T}^{lead};z_{g}^{lead}", nPtBinsTrue, ptminTrue, ptmaxTrue, nZgBinsTrue, zgminTrue, zgmaxTrue);
  // TH2D* RecoilMeas2D  = new TH2D( "RecoilMeas2D", "TRAIN z_{g}^{lead} vs. p_{T}^{lead}, Pythia6 #oplus GEANT;p_{T}^{lead};z_{g}^{lead}", nPtBinsMeas, ptminMeas, ptmaxMeas, nZgBinsMeas, zgminMeas, zgmaxMeas);
  // TH2D* RecoilTestTruth2D = new TH2D( "RecoilTestTruth2D", "TEST z_{g}^{lead} vs. p_{T}^{lead}, Pythia6;p_{T}^{lead};z_{g}^{lead}", nPtBinsTrue, ptminTrue, ptmaxTrue, nZgBinsTrue, zgminTrue, zgmaxTrue);
  // TH2D* RecoilTestMeas2D  = new TH2D( "RecoilTestMeas2D", "TEST z_{g}^{lead} vs. p_{T}^{lead}, Pythia6 #oplus GEANT;p_{T}^{lead};z_{g}^{lead}", nPtBinsMeas, ptminMeas, ptmaxMeas, nZgBinsMeas, zgminMeas, zgmaxMeas);

  // TH2D* McPpTriggerZg2030 = new TH2D( "McPpTriggerZg2030",";z_{g}^{Part};z_{g}^{Det}", nZgBinsTrue, zgminTrue, zgmaxTrue, nZgBinsMeas, zgminMeas, zgmaxMeas );
  // TH2D* McPpTriggerZg10plus = new TH2D( "McPpTriggerZg10plus",";z_{g}^{Part};z_{g}^{Det}", nZgBinsTrue, zgminTrue, zgmaxTrue, nZgBinsMeas, zgminMeas, zgmaxMeas );
  // TH3D* McPpTriggerZg3d = new TH3D( "McPpTriggerZg3d",";z_{g}^{Part};z_{g}^{Det};p_{T}^{truth}", nZgBinsTrue, zgminTrue, zgmaxTrue, nZgBinsMeas, zgminMeas, zgmaxMeas, 12, 0, 60 );

  TH2D* DeltaZgvsPt = new TH2D( "DeltaZgvsPt", "DeltaZg vs. p_{T}, Pythia6", nPtBinsTrue, ptminTrue, ptmaxTrue, 75*3, -0.5, 0.5);

  int nDPTGBinsMeas = nPtBins;
  float dptgminMeas = -ptmax;
  // float dptgmaxMeas = ptmax;
  float dptgmaxMeas = 0;
  
  int nRPTGBinsMeas = 100;
  float rptgminMeas = 0 + 0.5/nRPTGBinsMeas;
  float rptgmaxMeas = 1 + 0.5/nRPTGBinsMeas;
  // ------------------------
  // Loop over particle level
  // ------------------------

  cout << "Loop over particle level" << endl;
  cout << "------------------------" << endl;

  int missed=0;
  int N = McChain->GetEntries();
  //  N=500000;
  for ( Long64_t mcEvi = 0; mcEvi<N  ; ++mcEvi ){
    if ( !(mcEvi%10000) ) cout << "Working on " << mcEvi << " / " << N << endl;
    McChain->GetEntry(mcEvi);

    if ( McJets->GetEntries() != mcnjets ){
      cerr << "McJets->GetEntries() != mcnjets" << endl;
      return -1;
    }
    
    // Fill results in vectors for easier manipulation
    // -----------------------------------------------
    // Also check whether there's something true in the acceptance
    bool TruthInAcceptance=false;
    vector<RootGroomingResultStruct> mcresult;
    for (int j=0; j<mcnjets; ++j ){
      TStarJetVectorJet* mcjet = (TStarJetVectorJet*) McJets->At(j);
      TStarJetVectorJet* mcgjet = (TStarJetVectorJet*) McGroomedJets->At(j);

      // Skip high weight outliers
      // -------------------------
      if ( RejectHiweights && NewGeantWeightReject ( 0.4, mcjet->Pt(), mcweight, 2 ) )  {
	cout << "Skipping JET with pt=" << mcjet->Pt() << " due to high weight" << endl;
	continue;
      }

      // // DEBUG, for now
      // // force truth to have positive zg
      // if ( mczg[j] < 0.1 ) continue;

      // Debug: Can this happen?
      if ( mczg[j] < 0.0 || mczg[j] >= 0.5 ) {
	cerr << "!!!!!! mczg[j] = " << mczg[j] << endl;
	return -1;
      }
	
      // Ok, record
      if ( fabs ( mcjet->Eta() ) < EtaCut ) {
	mcresult.push_back( RootGroomingResultStruct(*mcjet, *mcgjet, mczg[j]) );
	TruthInAcceptance=true;
      }
    }

    if ( !TruthInAcceptance ) {
      // Skip this event, but don't count it as a loss
      continue;
    }



    // Record Truth
    // ------------
    for ( vector<RootGroomingResultStruct>::iterator mcit = mcresult.begin(); mcit != mcresult.end(); ++mcit ){
      if ( !PrepClosure || mcEvi%2 == 0)    IncTruth2D->Fill( mcit->orig.Pt(), mcit->zg, mcweight );
      if ( !PrepClosure || mcEvi%2 == 1)    IncTestTruth2D->Fill( mcit->orig.Pt(), mcit->zg, mcweight );
    }

    // Get corresponding pp event
    // --------------------------
    Long64_t ppevi=-1;
    ppevi = PpChain->GetEntryNumberWithIndex( mcrunid, mceventid );

    if ( ppevi < 0 ){      
      // Here is where we for the first time could file for loss
      if ( UseMiss ){
      	if ( !PrepClosure || mcEvi%2 == 0 ){
	  for ( vector<RootGroomingResultStruct>::iterator mcit = mcresult.begin(); mcit != mcresult.end(); ++mcit ){
	    IncPtResponse.Miss( mcit->orig.Pt(), mcweight );
	    IncPtZgResponse2D.Miss2D( mcit->orig.Pt(), mcit->zg, mcweight );

	    VisualIncPtResponse.Miss ( mcit->orig.Pt(), mcweight );
	    if ( mcit->orig.Pt() > ptminforvisualzg && mcit->orig.Pt() < ptmaxforvisualzg ){
	      VisualIncZgResponse.Miss ( mcit->zg, mcweight );
	    }
	    
	  }
	}
	// Skip this event
	// cout << " Skipped" << endl;
	missed++;
	continue;
      }
    }

    
    // Get pp
    // ------
    PpChain->GetEntry(ppevi);

    // Fill results in vectors for easier manipulation
    // -----------------------------------------------
    vector<RootGroomingResultStruct> ppresult;
    for (int j=0; j<ppnjets; ++j ){
      TStarJetVectorJet* ppjet = (TStarJetVectorJet*) PpJets->At(j);
      TStarJetVectorJet* ppgjet = (TStarJetVectorJet*) PpGroomedJets->At(j);
      
      if ( RejectHiweights && NewGeantWeightReject ( 0.4, ppjet->Pt(), mcweight, 12 ) )  {
	cout << "Skipping RECO JET with pt=" << ppjet->Pt() << " due to high weight" << endl;
	continue;
      }
      
      // // DEBUG, for now
      // // force truth to have positive zg
      // if ( ppzg[j] < 0.1 ) continue; 

      // Ok, record
      if ( fabs ( ppjet->Eta() ) < EtaCut ) {
	ppresult.push_back( RootGroomingResultStruct(*ppjet, *ppgjet, ppzg[j]) );
      }
    }

    // Record Measured
    // ---------------
    for ( vector<RootGroomingResultStruct>::iterator ppit = ppresult.begin(); ppit != ppresult.end(); ++ppit ){
      if ( !PrepClosure || mcEvi%2 == 0)    IncMeas2D->Fill( ppit->orig.Pt(), ppit->zg, ppweight );
      if ( !PrepClosure || mcEvi%2 == 1)    IncTestMeas2D->Fill( ppit->orig.Pt(), ppit->zg, ppweight );
    }

    // cout << mcresult.size() << " + " << ppresult.size() << " = " << mcresult.size() + ppresult.size() << endl;
    
    // Sort them together
    // ------------------
    vector<MatchedRootGroomingResultStruct> MatchedResult;
    for ( vector<RootGroomingResultStruct>::iterator mcit = mcresult.begin(); mcit != mcresult.end(); ){
      bool matched=false;
      for ( vector<RootGroomingResultStruct>::iterator ppit = ppresult.begin(); ppit != ppresult.end(); ){
	if ( mcit->orig.DeltaR( ppit->orig )< RCut ){
	  MatchedResult.push_back ( MatchedRootGroomingResultStruct ( *mcit, *ppit ) );
	  ppit = ppresult.erase( ppit );
	  matched=true;
	  break;
	} else{
	  ++ppit;
	}
      }
      if ( matched ) {
	mcit = mcresult.erase( mcit );
      } else {
	++mcit;
      }
    }

    // cout << "  --> " << 2*MatchedResult.size() << " + " << mcresult.size() << " + " << ppresult.size() << " = " << 2*MatchedResult.size() + mcresult.size() + ppresult.size() << endl;

    // Now MatchedResult contains all matches, the remainder in mcresult is missed, the remainder in ppresult is fake
    // cout << "ppresult.size() = " << ppresult.size() << endl;
    // if ( ppresult.size() > 0 ){
    //   cout << " =================== " << endl;
    //   for (int i=0; i<ppresult.size(); ++i ){
    // 	cout << "Missed: " << endl;
    // 	ppresult.at(i).orig.Print();
    // 	for (int j=0; j<MatchedResult.size(); ++j ){
    // 	  cout << j << ": " << ppresult.at(i).orig.Pt() << "  " << ppresult.at(i).orig.DeltaR( MatchedResult.at(j).first.orig ) << endl;
    // 	}
    //   }      
    // }

    // Fill Response
    // -------------
    for (vector<MatchedRootGroomingResultStruct>::iterator res = MatchedResult.begin(); res != MatchedResult.end(); ++res ){
      if ( !PrepClosure || mcEvi%2 == 0){
	IncPtZgResponse2D.Fill( res->second.orig.Pt(), res->second.zg, res->first.orig.Pt(), res->first.zg, mcweight );
	IncPtResponse.Fill( res->second.orig.Pt(), res->first.orig.Pt(), mcweight );

	VisualIncPtResponse.Fill( res->second.orig.Pt(), res->first.orig.Pt(), mcweight );
	if ( res->second.orig.Pt() > ptminforvisualzg && res->second.orig.Pt() < ptmaxforvisualzg ){
	  VisualIncZgResponse.Fill ( res->second.zg, res->first.zg, mcweight );
	}

	float truept = res->first.orig.Pt();
	float ptweight = hPtWeights->GetBinContent( hPtWeights->FindBin(truept) );
	IncBentPtResponse.Fill ( res->second.orig.Pt(), res->first.orig.Pt(), mcweight*ptweight );
	IncBentPtZgResponse2D.Fill( res->second.orig.Pt(), res->second.zg, res->first.orig.Pt(), res->first.zg, mcweight*ptweight );

	DeltaZgvsPt->Fill ( truept,  res->second.zg-res->first.zg, mcweight);
	  

      }
    }
    
    // Fill misses and fakes
    if ( UseMiss ){
      if ( !PrepClosure || mcEvi%2 == 0){
	for ( vector<RootGroomingResultStruct>::iterator mcit = mcresult.begin(); mcit != mcresult.end(); ++mcit ){
	  IncPtResponse.Miss( mcit->orig.Pt(), mcweight );
	  IncPtZgResponse2D.Miss2D( mcit->orig.Pt(), mcit->zg, mcweight );
	  
	  VisualIncPtResponse.Miss( mcit->orig.Pt(), mcweight );
	  if ( mcit->orig.Pt() > ptminforvisualzg && mcit->orig.Pt() < ptmaxforvisualzg ){
	    VisualIncZgResponse.Miss( mcit->zg, mcweight );
	  }
	  
	  float truept = mcit->orig.Pt();
	  float ptweight = hPtWeights->GetBinContent( hPtWeights->FindBin(truept) );
	  IncBentPtResponse.Miss( mcit->orig.Pt(), mcweight*ptweight );
	  IncBentPtZgResponse2D.Miss2D( mcit->orig.Pt(), mcit->zg, mcweight*ptweight );
	}
      }
    }
    
    if ( UseFakes ){
      if ( !PrepClosure || mcEvi%2 == 0){
	for ( vector<RootGroomingResultStruct>::iterator ppit = ppresult.begin(); ppit != ppresult.end(); ++ppit ){
	  IncPtResponse.Fake( ppit->orig.Pt(), ppweight );
	  IncPtZgResponse2D.Fake2D( ppit->orig.Pt(), ppit->zg, ppweight );

	  if ( ppit->orig.Pt() > ptminforvisualzg && ppit->orig.Pt() < ptmaxforvisualzg ){
	    VisualIncZgResponse.Fake( ppit->zg, ppweight );
	  }		
	}  
      }
    }
    
    continue;

    // // Record Truth
    // // ------------
    // if ( !PrepClosure || mcEvi%2 == 0)    TrigTruth2D->Fill( McT->Pt(), mczgtriglo, mcweight );
    // if ( !PrepClosure || mcEvi%2 == 1)    TrigTestTruth2D->Fill( McT->Pt(), mczgtriglo, mcweight );
    // if ( HasAway ){
    //   if ( !PrepClosure || mcEvi%2 == 0)    RecoilTruth2D->Fill( McA->Pt(), ppzgawaylo, mcweight );
    //   if ( !PrepClosure || mcEvi%2 == 1)    RecoilTestTruth2D->Fill( McA->Pt(), ppzgawaylo, mcweight );
    // }

    // // Get the det level jets
    // // ----------------------
    // if ( PpTrigger->GetEntries()!=1 ){
    //   cout << " No trigger jet in data?!" << endl;
    //   return -1;
    // } 
    
    // TLorentzVector* PpT = (TLorentzVector*) PpTrigger->At(0);
    // TLorentzVector* PpA = 0;
    // if (PpAwayJet->GetEntries()!=0 ) PpA = (TLorentzVector*) PpAwayJet->At(0);

    // if ( PpT->Pt()<15 && PpT->Pt()>10 && ( !RejectHiweights || !GeantWeightReject ( 0.4, PpT->Pt(), mcweight, 10 ) ) )
    // 	 hhh->Fill( ppzgtriglo, mcweight );

    // // TRIGGER: Find best match to MC
    // // ------------------------------
    // TLorentzVector* MatchT = 0;
    // if ( McT->DeltaR( *PpT ) < RCut ) MatchT = PpT;
    // // else if ( PpA && McT->DeltaR( *PpA ) < RCut ) MatchT = PpA;

    // if ( !MatchT ) {
    //   // lost trigger jet
    //   if ( UseMiss ){
    // 	if ( !PrepClosure || mcEvi%2 == 0){
    // 	  TrigPtResponse.Miss( McT->Pt(), mcweight );
    // 	  TrigPtZgResponse2D.Miss( McT->Pt(), mczgtriglo, mcweight );
    // 	}
    // 	if ( HasAway ) {
    // 	  if ( !PrepClosure || mcEvi%2 == 0) {
    // 	    RecoilPtResponse.Miss( McA->Pt(), mcweight );
    // 	    RecoilPtZgResponse2D.Miss( McA->Pt(), mczgawaylo, mcweight );
    // 	  }
    // 	}
    //   }
    //   continue;
    // }

    // if ( RejectHiweights && GeantWeightReject ( 0.4, MatchT->Pt(), mcweight, 10 ) )  {
    //   cout << "Skipping RECO TRIGGER with pt=" << McT->Pt() << " due to high weight" << endl;
    //   continue;
    // }

    // // Skip other outliers
    // // -------------------
    // if ( RejectOutliers ){
    //   if ( (McT->Pt() - MatchT->Pt()) / McT->Pt() < -0.4 )
    // 	continue;
    // }


    // // Fill truth, smeared, and delta pT
    // McTriggerPt->Fill( McT->Pt(), mcweight);
    // PpTriggerPt->Fill( MatchT->Pt(), mcweight);
    // if ( fabs( McT->Pt() - MatchT->Pt() ) > McT->Pt() ) {
    //   cerr << " ------------------------------------------------------------" << endl;
    //   cerr << "Trigger match with dR=" << McT->DeltaR( *MatchT)
    // 	   << " pT_Part= " << McT->Pt() << " pT_Det= " << MatchT->Pt() << endl;
    //   cerr << "mcrunid=" << mcrunid << "  " << "mceventid=" << mceventid << endl ;
    //   cerr << "pprunid=" << pprunid << "  " << "ppeventid=" << ppeventid << endl ;
    //   cerr << "mcweight=" << mcweight << "   ppweight=" << ppweight << endl ;
    //   cerr << " ------------------------------------------------------------" << endl;
    // }
    // McPpTriggerPt->Fill( McT->Pt(), MatchT->Pt(), mcweight);
 
    // DeltaTriggerPt->Fill( McT->Pt(), McT->Pt() - MatchT->Pt(), mcweight);
    // RelDeltaTriggerPt->Fill( McT->Pt(), (McT->Pt() - MatchT->Pt()) / McT->Pt(), mcweight);
    
    // if ( !PrepClosure || mcEvi%2 == 0) {
    //   TrigPtResponse.Fill( MatchT->Pt(), McT->Pt(), mcweight );
    //   TrigPtZgResponse2D.Fill( MatchT->Pt(), ppzgtriglo, McT->Pt(), mczgtriglo, mcweight );
    //   McPpTriggerZg3d->Fill( ppzgtriglo, mczgtriglo, McT->Pt(), mcweight);
    //   if ( MatchT->Pt() > 10 ) McPpTriggerZg10plus->Fill( ppzgtriglo, mczgtriglo, mcweight);
      
    //   if ( MatchT->Pt() > 20 && MatchT->Pt() < 30 ){
    // 	McPpTriggerZg2030->Fill( ppzgtriglo, mczgtriglo, mcweight);
    //   }
    // }
    
    // if ( !PrepClosure || mcEvi%2 == 0)    TrigMeas2D->Fill( MatchT->Pt(), ppzgtriglo, mcweight );
    // if ( !PrepClosure || mcEvi%2 == 1)    TrigTestMeas2D->Fill( MatchT->Pt(), ppzgtriglo, mcweight );
    
    // // RECOIL: Find best match to MC
    // // -----------------------------
    // if ( !HasAway ) {
    //   // no recoil jet
    //   continue;
    // }
    
    // TLorentzVector* MatchA = 0;
    // if ( McA->DeltaR( *PpA ) < RCut ) MatchA = PpA;
    // // else if ( McA->DeltaR( *PpT ) < RCut ) MatchA = PpT;
    
    // if ( !MatchA ) {
    //   // lost recoil jet
    //   if ( UseMiss ){
    // 	if ( !PrepClosure || mcEvi%2 == 0){
    // 	  RecoilPtResponse.Miss( McA->Pt(), mcweight );
    // 	  RecoilPtZgResponse2D.Miss( McA->Pt(), mczgawaylo, mcweight );
    // 	}
    //   }
    //   continue;
    // }
    
    // if ( MatchA && RejectHiweights && GeantWeightReject ( 0.4, MatchA->Pt(), mcweight, 11 ) )  {
    //   cout << "Skipping RECO RECOIL with pt=" << McA->Pt() << " due to high weight" << endl;
    //   continue;
    // }

    // // if ( fabs( McA->Pt() - MatchA->Pt() ) > McA->Pt() ) {
    // //   cerr << "Recoil match with dR=" << McA->DeltaR( *MatchA)
    // // 	   << " pT_Part= " << McA->Pt() << " pT_Det= " << MatchA->Pt() << "  " << mcrunid << "  " << pprunid << endl ;
    // // }

    // if ( !PrepClosure || mcEvi%2 == 0){
    //   RecoilPtResponse.Fill( MatchA->Pt(), McA->Pt(), mcweight );
    //   RecoilPtZgResponse2D.Fill( MatchA->Pt(), ppzgawaylo, McA->Pt(), mczgawaylo, mcweight );
    //   RecoilPtZgResponse2D.Fill( MatchA->Pt(), ppzgawaylo, McA->Pt(), mczgawaylo, mcweight );
    // }

    // if ( !PrepClosure || mcEvi%2 == 0)   RecoilMeas2D->Fill( MatchA->Pt(), ppzgawaylo, mcweight );
    // if ( !PrepClosure || mcEvi%2 == 1)   RecoilTestMeas2D->Fill( MatchA->Pt(), ppzgawaylo, mcweight );

  }
  cout << "Misssed " << missed << endl;
  // return 0;

  // new TCanvas ( "c0","");
  // gPad->SetGridx(0);  gPad->SetGridy(0);
  // gPad->SetLogy();
  // // McTriggerPt->Draw();
  // // PpTriggerPt->Draw("same");
  // TH1D* TrigTruthPt = (TH1D*) TrigTruth2D->ProjectionX("TrigTruthPt");
  // TH1D* TrigMeasPt = (TH1D*) TrigMeas2D->ProjectionX("TrigMeasPt");
  // TH1D* TrigTestTruthPt = (TH1D*) TrigTestTruth2D->ProjectionX("TrigTestTruthPt");
  // TH1D* TrigTestMeasPt = (TH1D*) TrigTestMeas2D->ProjectionX("TrigTestMeasPt");

  // leg = new TLegend( 0.6, 0.6, 0.89, 0.9, "Leading Jet p_{T}" );
  // leg->SetBorderSize(0);
  // leg->SetLineWidth(10);
  // leg->SetFillStyle(0);
  // leg->SetMargin(0.1);

  // TrigTruthPt->Draw("9same");
  // TrigMeasPt->Draw("9same");
  
  // TrigMeasPt->SetLineColor(kBlue);
  // leg->AddEntry(TrigMeasPt, "Training GEANT Result");
  // TrigTruthPt->SetLineColor(kOrange);
  // leg->AddEntry(TrigTruthPt, "Training Matched Truth");

  // TrigTestMeasPt->SetLineColor(kMagenta);
  // TrigTestMeasPt->Draw("9same");
  // leg->AddEntry(TrigTestMeasPt, "TESTING GEANT Result");
  // TrigTestTruthPt->SetLineColor(kRed);
  // TrigTestTruthPt->Draw("9same");
  // leg->AddEntry(TrigTestTruthPt, "TESTING Matched Truth");
  // leg->Draw("same");
  
  // // PpTriggerPt->SetLineColor(kBlue);
  // // leg->AddEntry(PpTriggerPt, "Training GEANT Result");
  // // McTriggerPt->SetLineColor(kOrange);
  // // leg->AddEntry(McTriggerPt, "Matched Training Truth");
  // leg->Draw();

  // gPad->SaveAs(PlotBase+"_TrigSpec.png");

  // // new TCanvas ( "c1","",500, 500);
  // new TCanvas ( "c1","");
  // // gPad->SetCanvasSize(450,450);
  // // gPad->DrawFrame();
  // DeltaTriggerPt->Draw("colz");
  // // capped byt ptdet = ptpart-10
  // TF1* f = new TF1("f","x-10", ptmin, ptmax);
  // f->SetLineStyle(7);
  // f->SetLineWidth(1);
  // f->Draw("same");
  // gPad->SaveAs(PlotBase+"_DeltaPt.png");


  // new TCanvas ( "c2","",500, 500);
  // // gPad->SetCanvasSize(450,450);
  // RelDeltaTriggerPt->Draw("colz");
  // gPad->SaveAs(PlotBase+"_RelDeltaPt.png");

  // // new TCanvas ( "c22","",500, 500);
  // // // gPad->SetCanvasSize(450,450);
  // // RelDeltaRecoilPt->Draw("colz");
  // // gPad->SaveAs(PlotBase+"_RelDeltaPt.png");

  // new TCanvas ( "c3","",500, 500);
  // // gPad->SetCanvasSize(450,450);
  // DeltaTriggerPt->ProfileX("_pfx",1,-1,"s")->Draw();
  // gPad->SaveAs(PlotBase+"_DeltaProfile.png");

  // new TCanvas ( "c4","",500, 500);
  // gPad->SetCanvasSize(450,450);
  // McPpTriggerPt->Draw("colz");
  // TLine l;
  // l.SetLineStyle(7);
  // l.DrawLine( ptmin, ptmin, ptmax, ptmax );
  // gPad->SaveAs(PlotBase+"_ResponseHist.png");

  // // new TCanvas ( "c5","",500, 500);
  // // gPad->SetCanvasSize(450,450);
  // // McPpTriggerZg2030->Draw("colz");
  // // l.SetLineStyle(7);
  // // l.DrawLine( 0.05, 0.55, 0.05, 0.55 );
  // // gPad->SaveAs(PlotBase+"_ZgResponseHist.png");

  // new TCanvas ( "c5","",500, 500);
  // gPad->SetCanvasSize(450,450);
  // McPpTriggerZg10plus->Draw("colz");
  // l.SetLineStyle(7);
  // l.DrawLine( 0.05, 0.55, 0.05, 0.55 );
  // gPad->SaveAs(PlotBase+"_ZgResponseHist.png");
  
  // new TCanvas ( "c6","",500, 500);
  // gPad->SetCanvasSize(450,450);

  // gPad->SaveAs(PlotBase+"_ZgResponses.pdf[");
  // for ( int i=1; i<=McPpTriggerZg3d->GetNbinsZ(); ++i ){
  //   McPpTriggerZg3d->GetZaxis()->SetRange(i,i);
  //   TH2D* h2=(TH2D*)McPpTriggerZg3d->Project3D("yx");
  //   TString s=McPpTriggerZg3d->GetName(); s+=i;
  //   h2->SetName(s);
  //   s="p_{T}^{truth}=";
  //   s+= int(McPpTriggerZg3d->GetZaxis()->GetBinLowEdge(i)+0.01);
  //   s+=" - ";
  //   s+= int(McPpTriggerZg3d->GetZaxis()->GetBinLowEdge(i+1)+0.01);
  //   s+=" GeV/c ";
  //   h2->SetTitle(s);    
  //   h2->Draw("colz");
  //   l.SetLineStyle(7);
  //   l.DrawLine( 0.05, 0.55, 0.05, 0.55 );
  //   // if (i==1) gPad->SaveAs(PlotBase+"_ZgResponses.pdf[");
  //   // else if (i==McPpTriggerZg3d->GetNbinsZ()) gPad->SaveAs(PlotBase+"_ZgResponses.pdf]");
  //   // else
  //   gPad->SaveAs(PlotBase+"_ZgResponses.pdf");
  // }
  // gPad->SaveAs(PlotBase+"_ZgResponses.pdf]");
 
  // // new TCanvas ( "c5","",500, 500);
  // // gPad->SetCanvasSize(450,450);
  // // TrigPtZgResponse2D->Hmeasured()->Draw("colz");
  // // l.SetLineStyle(7);
  // // l.DrawLine( 0.05, 0.55, 0.05, 0.55 );
  // // gPad->SaveAs(PlotBase+"_ZgResponseHist.png");

  
    
  // Done
  // ----
  fout->Write();
  IncPtResponse.SetName("IncPtResponse");
  IncPtZgResponse2D.SetName("IncPtZgResponse2D");
  IncPtResponse.Write();
  IncPtZgResponse2D.Write();

  IncBentPtResponse.SetName("IncBentPtResponse");
  IncBentPtResponse.Write();

  IncBentPtZgResponse2D.SetName("IncBentPtZgResponse2D");
  IncBentPtZgResponse2D.Write();

  VisualIncPtResponse.SetName("VisualIncPtResponse");
  VisualIncPtResponse.Write();

  VisualIncZgResponse.SetName("VisualIncZgResponse");
  VisualIncZgResponse.Write();

  cout << " Wrote to" << endl << OutFileName << endl;


  // new TCanvas;     hhh->DrawNormalized();
  // hhh->SaveAs("hhh.root");

  return 0;

}


