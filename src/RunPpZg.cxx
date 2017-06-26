/** @file RunPpZg.cxx
    @author Kolja Kauder
    @version Revision 0.1
    @brief Subjet Analysis for pythia and/or real events.
    @details Perform Subjet analysis on a chain of LorentzVectors or TStarJetPicoTrees.
    @date Mar 23, 2017
*/

// FOR MC:
// If using PicoReader class to read MC, it would be NOT be a
// problem as the number of towers is set to 0 and nothing could be
// loaded. and I think that is the way both Kolja and I read the Pico
// data instead of pulling out of branch by hand.

// #include "DevSubjetAnalysis.hh"
#include "PpZgParameters.hh"
#include "PpZgAnalysis.hh"
#include "TStarJetVectorJet.h"

#include <TLorentzVector.h>
#include <TClonesArray.h>
#include <TChain.h>
#include <TFile.h>
#include <TBranch.h>
#include <TMath.h>
#include <TRandom.h>
#include <TParameter.h>
#include "TString.h"
#include "TObjString.h"

#include <set>
#include <vector>
#include <algorithm>

#include <cmath>
#include <climits>

#include "fastjet/contrib/Recluster.hh"
#include "fastjet/contrib/SoftDrop.hh"

#include <exception>

using namespace std;
using namespace fastjet;
using namespace contrib;

// Mostly for run 12
bool readinbadrunlist(vector<int> & badrun, TString csvfile);
  
// DEBUG
void decluster (PseudoJet j);

/** 
    - Parse parameters
    - Set up input tree
    - Set up output histos and tree
    - Initialize SubjetAnalysis object
    - If needed, combine input from two sources
    - Loop through events
    \arg argv: flags.
    Display options with
    <BR><tt>% UnifiedSubjetWrapper -h </tt> 
    <BR>Note that wildcarded file patterns should be in single quotes.
*/

int main( int argc, const char** argv ){

  // shared_ptr<PpZgAnalysis> ppzg = nullptr;
  PpZgAnalysis* ppzg = nullptr; 
  try {
    // ppzg = make_shared<PpZgAnalysis>(argc, argv );
    ppzg = new PpZgAnalysis(argc, argv );
  } catch ( std::exception& e ){
    cerr << "Initialization failed with exception " << e.what() << endl;
    return -1;
  }
  
  if ( ppzg->InitChains() == false ){
    cerr << "Chain initialization failed" << endl;
    return -1;
  }
  
  // Get parameters we used
  // ----------------------
  const PpZgParameters pars  = ppzg->GetPars();

  // Explicitly choose bad tower list here
  // -------------------------------------
  // Otherwise too easy to hide somewhere and forget...
  shared_ptr<TStarJetPicoReader> pReader = ppzg->GetpReader();
  if ( pReader ){
    TStarJetPicoTowerCuts* towerCuts = pReader->GetTowerCuts();
    if ( pars.InputName.Contains("NPE") || pars.InputName.Contains("11picoMB") ){
      towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/badTowerList_y11.txt");
    } else if ( pars.InputName.Contains("Run12") ){
      // towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/pp200Y12_badtower.list");
      towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/Combined_pp200Y12_badtower.list");
    } else if ( pars.InputName.Contains("Y14") ){
      towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/Alternate_QuickAndDirty_y14.txt");
      towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/Combined_y7_PP_Nick.txt");
    // } else {
    //   cout << getenv("STARPICOPATH" ) << "/Combined_y7_AuAu_Nick.txt" << endl;
    //   towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/Combined_y7_AuAu_Nick.txt");
    //   cout << getenv("STARPICOPATH" ) << "/Combined_y7_PP_Nick.txt" << endl;
    //   towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/Combined_y7_PP_Nick.txt");
    // }
    } else {
      cout << getenv("STARPICOPATH" ) << "/Combined_y7_PP_Nick.txt" << endl;
      towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/Combined_y7_PP_Nick.txt");
    }
  }
  
  shared_ptr<TStarJetPicoReader> pEmbReader = ppzg->GetpEmbReader();
  if ( pEmbReader ){
    TStarJetPicoTowerCuts* towerCuts = pEmbReader->GetTowerCuts();
    if ( pars.EmbInputName.Contains("NPE") || pars.EmbInputName.Contains("11picoMB") ){
      towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/badTowerList_y11.txt");
    } else if ( pars.EmbInputName.Contains("Run12") ){
      towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/pp200Y12_badtower.list");      
    } else if ( pars.EmbInputName.Contains("Y14") ){
      towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/Alternate_QuickAndDirty_y14.txt");
      towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/Combined_y7_PP_Nick.txt");
    } else {
      // towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/OrigY7MBBadTowers.txt");    
      towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/Combined_y7_AuAu_Nick.txt");
      towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/Combined_y7_PP_Nick.txt");
    }
  }
  
  // Explicitly add bad run list here
  // --------------------------------
  if ( pReader ){
    if ( pars.InputName.Contains("Run12") ){
      TString csvfile= TString( getenv("STARPICOPATH" )) + "/pp200Y12_badrun.list";
      vector<int> badruns;
      if ( readinbadrunlist( badruns, csvfile) == false ){
	cerr << "Problems reading bad run list" << endl;
	return -1;
      }
      pReader->AddMaskedRuns (badruns);
    }
  }
  
  if ( pEmbReader ){
    if ( pars.EmbInputName.Contains("Run12") ){
      TString csvfile= TString( getenv("STARPICOPATH" )) + "/pp200Y12_badrun.list";
      vector<int> badruns;
      if ( readinbadrunlist( badruns, csvfile) == false ){
	cerr << "Problems reading bad run list" << endl;
	return -1;
      }
      pEmbReader->AddMaskedRuns (badruns);
    }
  }
      

  // Files and histograms
  // --------------------

  TFile* fout = new TFile( pars.OutFileName, "RECREATE");
  
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);
  TH3::SetDefaultSumw2(true);
    
  // TH1D* hzg      = new TH1D( "hzg", "z_{g}"              , 100*(pars.R+0.1),  0.0, (pars.R+0.1)  );
  // TH1D* hEmbzg   = new TH1D( "hEmbzg", "z_{g}, embedded" , 100*(pars.R+0.1),  0.0, (pars.R+0.1)  );
  TH3D* cptphieta = new TH3D("cptphieta","",500, 0.2, 50.2, 100, 0, TMath::TwoPi(), 100, -1, 1);
  TH3D* nptphieta = new TH3D("nptphieta","",500, 0.2, 50.2, 100, 0, TMath::TwoPi(), 100, -1, 1);
  
  // List of miscellaneous info
  // --------------------------
  TTree* info = new TTree("info", "Information");
  info->Branch("InputName"         , (void*)pars.InputName.Data()     , "InputName/C" );
  info->Branch("ChainName"         , (void*)pars.ChainName.Data()     , "ChainName/C" );
  bool Embedding = (pars.EmbInputName != "");
  if ( Embedding ){
    info->Branch("EmbInputName"         , (void*)pars.EmbInputName.Data()     , "EmbInputName/C" );
    info->Branch("EmbChainName"         , (void*)pars.EmbChainName.Data()     , "EmbChainName/C" );
  }
  info->Branch("R"                 , (void*) &pars.R                          , "R/D" );
  info->Branch("z_cut"             , (void*) &pars.z_cut                      , "z_cut/D" );
  info->Branch("beta"              , (void*) &pars.beta                       , "beta/D" );
  info->Branch("LargeJetAlgorithm" , (UInt_t*)&pars.LargeJetAlgorithm , "LargeJetAlgorithm/i" );

  if ( pars.CustomRecluster )
    info->Branch("ReclusterJetAlgorithm" , (UInt_t*)&pars.CustomReclusterJetAlgorithm , "ReclusterJetAlgorithm/i" );

  info->Branch("PtJetMin"          , (void*) &pars.PtJetMin                   , "PtJetMin/D" );
  info->Branch("PtJetMax"          , (void*) &pars.PtJetMax                   , "PtJetMax/D" );
  info->Branch("EtaConsCut"        , (void*) &pars.EtaConsCut                 , "EtaConsCut/D" );
  info->Branch("PtConsMin"         , (void*) &pars.PtConsMin                  , "PtConsMin/D" );
  info->Branch("PtConsMax"         , (void*) &pars.PtConsMax                  , "PtConsMax/D" );


  // Save results
  // ------------
  TTree* ResultTree=new TTree("ResultTree","Result Jets");
  
  TClonesArray HardPartons( "TStarJetVector" );
  ResultTree->Branch("HardPartons", &HardPartons );
  TClonesArray HardPartonNames( "TObjString" );
  ResultTree->Branch("HardPartonNames", &HardPartonNames );

  TStarJetVector HT;
  ResultTree->Branch("TriggerHT", &HT );
  // For convenience
  TStarJetVectorJet HTJet;
  ResultTree->Branch("HTJet", &HTJet );
  double HTJetZg;
  ResultTree->Branch("HTJetZg", &HTJetZg );

  TClonesArray Jets( "TStarJetVectorJet" );
  ResultTree->Branch("Jets", &Jets );
  TClonesArray GroomedJets( "TStarJetVectorJet" ); 
  ResultTree->Branch("GroomedJets", &GroomedJets );
  TClonesArray sj1( "TStarJetVectorJet" );
  ResultTree->Branch("sj1", &sj1 );
  TClonesArray sj2( "TStarJetVectorJet" );
  ResultTree->Branch("sj2", &sj2 );
  double refmult;
  ResultTree->Branch("refmult",&refmult, "refmult/d");  
  
  int njets=0;
  ResultTree->Branch("njets",   &njets, "njets/I" );
  double zg[1000];
  ResultTree->Branch("zg",       zg, "zg[njets]/D" );
  double delta_R[1000];
  ResultTree->Branch("delta_R",  delta_R, "delta_R[njets]/D" );
  double mu[1000];
  ResultTree->Branch("mu",       mu, "mu[njets]/D" );
  double rho=-1;
  ResultTree->Branch("rho",      &rho, "rho/D" );

  double zg1[1000];
  double zg2[1000];
  if ( pars.Recursive){
    ResultTree->Branch("zg1",       zg1, "zg1[njets]/D" );
    ResultTree->Branch("zg2",       zg2, "zg2[njets]/D" );
  }

  TClonesArray EmbJets( "TStarJetVectorJet" ); 
  ResultTree->Branch("EmbJets", &EmbJets );
  TClonesArray EmbGroomedJets( "TStarJetVectorJet" ); 
  ResultTree->Branch("EmbGroomedJets", &EmbGroomedJets );
  TClonesArray Embsj1( "TStarJetVectorJet" );
  ResultTree->Branch("Embsj1", &Embsj1 );
  TClonesArray Embsj2( "TStarJetVectorJet" );
  ResultTree->Branch("Embsj2", &Embsj2 );
  double Embrefmult;
  ResultTree->Branch("Embrefmult",&Embrefmult, "Embrefmult/d");
  
  double Embzg[1000];
  ResultTree->Branch("Embzg",       Embzg, "Embzg[njets]/D" );
  double Embdelta_R[1000];
  ResultTree->Branch("Embdelta_R",  Embdelta_R, "Embdelta_R[njets]/D" );
  double Embmu[1000];
  ResultTree->Branch("Embmu",       Embmu, "Embmu[njets]/D" );

  double Embzg1[1000];
  double Embzg2[1000];
  if ( pars.Recursive){
    ResultTree->Branch("Embzg1",       Embzg1, "Embzg1[njets]/D" );
    ResultTree->Branch("Embzg2",       Embzg2, "Embzg2[njets]/D" );
  }

  double weight=1;
  ResultTree->Branch("weight",      &weight, "weight/D" );

  double Embrho=-1;
  ResultTree->Branch("Embrho",      &Embrho, "Embrho/D" );

  // Give each event a unique ID to compare event by event with different runs
  int runid;
  int eventid;
  ResultTree->Branch("eventid", &eventid, "eventid/I");
  ResultTree->Branch("runid",   &runid, "runid/I");
  // And also contracted versions to index on both
  // ULong64_t runevent=0;
  // ResultTree->Branch("runevent", &runevent, "runevent/l");

  // ULong64_t Embrunevent=0;
  // // Give each event a unique ID to compare event by event with different runs
  // if ( Embedding ){
  //   ResultTree->Branch("Embeventid",(void*) &pars.Embeventid, "Embeventid/I");
  //   ResultTree->Branch("Embrunid",  (void*) &pars.Embrunid, "Embrunid/I");
  //   ResultTree->Branch("Embrunevent",&Embrunevent, "Embrunevent/l");
  // }

  // Helpers
  TStarJetVector* sv;
  TObjString* tobjs;
  
  // Go through events
  // -----------------
  // Long64_t Ntot=0;
  Long64_t Naccepted=0;
  // Long64_t Nrejected=0;
  cout << "Running analysis" << endl;
  try {
    // I understand that a break after continue or return is silly...
    // But it's necessary in nested switches in root and I don't want to lose the habit    
    bool ContinueReading = true;

    while ( ContinueReading ){

      HardPartons.Clear();
      Jets.Clear();
      GroomedJets.Clear();
      HTJet = TStarJetVectorJet();
      HT = TStarJetVector();
      HTJetZg = 0;
      //   sj1.Clear();
      //   sj2.Clear();
      rho=-1;
      refmult=0;
      runid   =-(INT_MAX-1);
      eventid =-(INT_MAX-1);
      //   EmbJets.Clear();
      //   EmbGroomedJets.Clear();
      //   Embsj1.Clear();
      //   Embsj2.Clear();
      //   Embrho=-1;
      Embrefmult=0;
      std::fill_n( zg, sizeof(zg), 0);
      EVENTRESULT ret=ppzg->RunEvent();

      // Understand what happened in the event
      switch (ret){
      case  EVENTRESULT::PROBLEM :
	cerr << "Encountered a serious issue" << endl;
	return -1;
	break;	
      case  EVENTRESULT::ENDOFINPUT :
	// cout << "End of Input" << endl;
	ContinueReading=false;
	continue;
	break;
      case  EVENTRESULT::NOTACCEPTED :
	// cout << "Event rejected" << endl;
	continue;
	break;
      case  EVENTRESULT::NOCONSTS :
	// cout << "Event empty." << endl;
	continue;
	break;
      case  EVENTRESULT::NOJETS :
	// cout << "No jets found." << endl;
	continue;
	break;
      case  EVENTRESULT::JETSFOUND:
	// The only way not to break out or go back to the top
	// Do Something
	Naccepted++;
	break;
      default :
	cerr << "Unknown return value." << endl;
	return -1;
	break;
      }
	  
      // cout << ++Ntot << endl;
      
      // Now we can pull out details and results
      // ---------------------------------------
      weight = ppzg->GetEventWeight();
      refmult = ppzg->GetRefmult();
      rho = ppzg->GetRho();
      runid = ppzg->GetRunid();
      eventid = ppzg->GetEventid();
      
      TClonesArray* pHardPartons =  ppzg->GetHardPartons();
      TClonesArray* pHardPartonNames =  ppzg->GetHardPartonNames();
      if ( pHardPartons ){
	sv = (TStarJetVector*) HardPartons.ConstructedAt( 0 ); *sv = *( (TStarJetVector*) pHardPartons->At(0));
	sv = (TStarJetVector*) HardPartons.ConstructedAt( 1 ); *sv = *( (TStarJetVector*) pHardPartons->At(1));
	tobjs = (TObjString*)  HardPartonNames.ConstructedAt( 0 ); *tobjs = *( (TObjString*) pHardPartonNames->At( 0 ));
	tobjs = (TObjString*)  HardPartonNames.ConstructedAt( 1 ); *tobjs = *( (TObjString*) pHardPartonNames->At( 1 ));
      }
      
      vector<GroomingResultStruct> GroomingResult = ppzg->GetGroomingResult(); 
      // sort ( GroomingResult.begin(), GroomingResult.end(), GroomingResultStruct::groomedptgreater);
      // sort ( GroomingResult.begin(), GroomingResult.end(), GroomingResultStruct::origptgreater);
      if ( auto pHT = ppzg->GetTrigger() )	HT = *pHT;
      
      // cout << "-----------------------" << endl;
      // for ( auto& gr : GroomingResult ){
      // 	TLorentzVector lv = MakeTLorentzVector(gr.orig);
      // 	cout << gr.orig.pt() << "  " << gr.groomed.pt() << "  " << gr.zg;
      // 	if ( pHT && pHT->DeltaR( lv ) <pars.R ){
      // 	  cout << " matched to trigger with pT=" << pHT->Pt() << endl;
      // 	} else {
      // 	  cout << endl;
      // 	}
      // }
      
      njets=GroomingResult.size();
      int ijet=0;
      for ( auto& gr : GroomingResult ){	
	zg[ijet]=gr.zg;
	new ( Jets[ijet] )               TStarJetVectorJet ( TStarJetVector( MakeTLorentzVector( gr.orig) ) );
	new ( GroomedJets[ijet] )        TStarJetVectorJet ( TStarJetVector( MakeTLorentzVector( gr.groomed) ) );
	zg[ijet] = gr.zg;
		  
	TStarJetVector sv = TStarJetVector( MakeTLorentzVector( gr.orig) );
	if ( HT.Pt()>1e-1 && HT.DeltaR( sv ) <pars.R ){
	  if ( HTJet.Pt()<sv.Pt() ) {
	    HTJet = sv;
	    HTJetZg = gr.zg;
	  }	  
      	} 

	if ( gr.orig.pt()>200 ){ // DEBUG
	  vector<PseudoJet> particles = sorted_by_pt(ppzg->GetConstituents());
	  vector<PseudoJet> c = sorted_by_pt(gr.orig.constituents());
	  for (auto& p : c ){
	    if ( p.has_user_info<JetAnalysisUserInfo>() )
	      cout << "WEIRD JET: charge = " << p.user_info<JetAnalysisUserInfo>().GetQuarkCharge()/3.0
		   << ", Id = " << p.user_info<JetAnalysisUserInfo>().GetNumber()
		   << ", pT = " << p.pt() << endl;
	    else
	      cout << "WEIRD JET: no charge info, " << p << endl;
	  }
	  // return -1;
	}
	
	ijet++;
      }
    // 	new ( GroomedJets[ijet] ) TStarJetVectorJet ( TStarJetVector(MakeTLorentzVector ( CurrentJet )) );

    // 	new ( sj1[ijet] )        TStarJetVectorJet ( );
    // 	new ( sj2[ijet] )        TStarJetVectorJet ( );

    // 	zg[ijet]=0;
    // 	delta_R[ijet]=0;
    // 	mu[ijet]=0;
	
    // 	((TStarJetVectorJet*)Jets[ijet])->SetArea( CurrentJet.area() );
    // 	((TStarJetVectorJet*)Jets[ijet])->SetArea4Vector( CurrentJet.area_4vector().four_mom()  );
    // 	// Save leading pt
    // 	((TStarJetVectorJet*)Jets[ijet])->SetLeadingParticle( 0, sorted_by_pt(CurrentJet.constituents()).at(0).pt() );
	
    // 	((TStarJetVectorJet*)GroomedJets[ijet])->SetArea( CurrentJet.area() );
    // 	((TStarJetVectorJet*)GroomedJets[ijet])->SetArea4Vector( CurrentJet.four_mom()  );

    //   } else {       
    // 	hzg->Fill(sd_jet.structure_of<contrib::SoftDrop>().symmetry());
    // 	zg[ijet]=sd_jet.structure_of<contrib::SoftDrop>().symmetry();
    // 	delta_R[ijet]=sd_jet.structure_of<contrib::SoftDrop>().delta_R();
    // 	mu[ijet]=sd_jet.structure_of<contrib::SoftDrop>().mu();

    // 	  // Now do background subtraction
    // 	if ( SubtractBg == AREA) {
    // 	  CurrentJet = (*pBackgroundSubtractor)( CurrentJet );
    // 	}

    // 	// save original and groomed
    // 	new ( Jets[ijet] )        TStarJetVectorJet ( TStarJetVector(MakeTLorentzVector ( CurrentJet )) );
    // 	new ( GroomedJets[ijet] ) TStarJetVectorJet ( TStarJetVector(MakeTLorentzVector ( sd_jet ))  );
      
      
      ResultTree->Fill();

      const vector<PseudoJet>& particles = ppzg->GetParticles();
      for (auto& p : particles ){
	if ( p.has_user_info<JetAnalysisUserInfo>() ){
	  if ( abs( p.user_info<JetAnalysisUserInfo>().GetQuarkCharge() )>0 ){
	    cptphieta->Fill(p.pt(),p.phi(),p.eta());
	  } else {
	    nptphieta->Fill(p.pt(),p.phi(),p.eta());
	  }
	}else{
	  cout << "constituent: no charge info" << endl;
	}
      }	     
    }
  } catch (std::string& s) {
    cerr << "RunEvent failed with string " << s << endl;
    return -1;
  } catch ( std::exception& e ){
    cerr << "RunEvent failed with exception " << e.what() << endl;
    return -1;
  }

  
  
  // while ( true ){    
  //   HardPartons.Clear();
  //   Jets.Clear();
  //   GroomedJets.Clear();
  //   sj1.Clear();
  //   sj2.Clear();
  //   rho=-1;
  //   EmbJets.Clear();
  //   EmbGroomedJets.Clear();
  //   Embsj1.Clear();
  //   Embsj2.Clear();
  //   Embrho=-1;
  //   refmult=0;

  //   TStarJetPicoEventHeader* header=0;
  //   switch (intype) {
  //     // =====================================================
  //   case INPICO :      
  //     if ( !pReader->NextEvent() ) {
  // 	// cout << "Can't find a next event" << endl;
  // 	done=true;
  // 	break;
  //     }
  //     // cout << "hello" << endl;
  //     pReader->PrintStatus(10);

  //     // cout << pReader->GetOutputContainer()->GetEntries() << endl; 
  //     pFullEvent = pReader->GetOutputContainer()->GetArray();

  //     header = pReader->GetEvent()->GetHeader();
  //     eventid = header->GetEventId();
  //     runid   = header->GetRunId();
  //     refmult=header->GetProperReferenceMultiplicity();
      
  //     break;      
  //     // =====================================================
  //   case INTREE :
  //   case MCTREE :
  //     if ( evi>= NEvents ) {
  // 	done=true;
  // 	break;
  //     }
  //     if ( !(evi%200) ) cout << "Working on " << evi << " / " << NEvents << endl;
  //     Events->GetEntry(evi);
  //     cname = Events->GetCurrentFile()->GetName();
  //     if (intype == MCTREE ){
  // 	filehash = cname.Hash();
  // 	while ( filehash > INT_MAX - 100000 ) filehash /= 10;
  // 	if ( filehash < 1000000 ) filehash += 1000001;
  // 	runid += filehash;
  //     }
  //     if (intype == INTREE ){
  // 	runid = urunid;
  // 	eventid = ueventid;
  //     }

  //     ++evi;
  //     break;
  //     // =====================================================
  //   default:
  //     cerr << "Unknown intype " << intype << endl;
  //     return(-1);
  //   }
  //   if ( done ) break;

  //   // FIXME: May (will) not work as intended unless both inputs are picoDSTs!
  //   runevent = ULong64_t(runid)*10000000LL + eventid;
    
  //   // Fill particle container
  //   // -----------------------
  //   particles.clear();
  //   for ( int i=0 ; i<pFullEvent->GetEntries() ; ++i ){
  //     sv = (TStarJetVector*) pFullEvent->At(i);
  //     // Ensure kinematic similarity
  //     if ( sv->Pt()< PtConsMin && sv->Pt()< PtSubConsMin ) continue;
  //     if ( fabs( sv->Eta() )>EtaConsCut ) continue;
  //     particles.push_back( PseudoJet (*sv ) );
  //     particles.back().set_user_info ( new JetAnalysisUserInfo( 3*sv->GetCharge() ) );
  //   }
    
  //   if ( particles.size()==0 ) continue;
    
  //   // For pythia, use cross section as weight
  //   // ---------------------------------------
  //   weight=1;
  //   if ( TParameter<double>* sigmaGen=(TParameter<double>*) Events->GetCurrentFile()->Get("sigmaGen") ){
  //     weight=sigmaGen->GetVal();
  //   }

  //   // For GEANT data
  //   // --------------
  //   if ( InputName.Contains("Geant") ){
  //     TString currentfile = pReader->GetInputChain()->GetCurrentFile()->GetName();
  //     weight=LookupXsec ( currentfile );
  //     if ( fabs(weight-1)<1e-4){
  // 	cerr << "mcweight unchanged!" << endl;
  // 	throw(-1);
  //     }
  //   }

  //   // For pythia, fill leading parton container
  //   // -----------------------------------------
  //   partons.clear();

  //   if ( pHardPartons ){
  //     for ( int i=0 ; i<pHardPartons->GetEntries() ; ++i ){
  // 	sv = (TStarJetVector*) pHardPartons->At(i);

  // 	PseudoJet pj = PseudoJet (*sv );
	
  // 	// flavor info
  // 	TString& s = ((TObjString*)(pHardPartonNames->At(i)))->String();
  // 	int qcharge=-999;
  // 	if ( s=="g" ) qcharge = 0;
	
  // 	if ( s(0)=='u' || s(0)=='c' || s(0)=='t' ) qcharge  = 2;
  // 	if ( s(0)=='d' || s(0)=='s' || s(0)=='b' ) qcharge = -1;
  // 	if ( s.Contains("bar") ) qcharge*=-1;
	
  // 	if ( abs ( qcharge ) >3 ) cout<< s << endl;
	

  // 	pj.set_user_info ( new JetAnalysisUserInfo( qcharge ) );
  // 	partons.push_back( pj );

  // 	// Save them too
  // 	// Update info here, may not be complete in original tree
  // 	sv->SetType(TStarJetVector::_PYTHIA);
  // 	sv->SetCharge(qcharge ); // Less than ideal, we're revamping charge to mean quark charge
  // 	sv->SetMatch(-1); // Necessary for older TStarJetVectors
  // 	new ( HardPartons[i] ) 	TStarJetVector (*sv);
  //     }
  //   }

    
    // // Run analysis
    // // ------------
    // // JetAnalyzer JA( particles, JetDef ); // WITHOUT background subtraction
    // // vector<PseudoJet> JAResult = sorted_by_pt( select_jet ( JA.inclusive_jets() ) );
    // if (pJA){
    //   delete pJA; pJA=0;
    // }
    // pJA=0;
    // pBackgroundSubtractor =  0;
    // switch ( SubtractBg ){
    // case AREA:
    //   pJA = new JetAnalyzer( particles, JetDef, AreaDef, select_jet_eta * (!fastjet::SelectorNHardest(2)) ) ;
    //   pBackgroundSubtractor = pJA->GetBackgroundSubtractor();
    //   break;
    // case NONE:
    //   pJA = new JetAnalyzer( particles, JetDef, AreaDef ) ;
    //   break;
    // default :
    //   cerr << "Unsupported Background subtraction method" << endl;
    //   return -1;
    //   break;      
    // }
    // JetAnalyzer& JA = *pJA;

    // // Can't subtract the background before SoftDrop
    // // But we can discard jets that get eliminated by BG subtraction
    // // and get the subtracted order right
    // vector<PseudoJet> JAResult = sorted_by_pt( select_jet ( JA.inclusive_jets() ) );
    // vector<PseudoJetPt> ResortedJAResult;
    // for (unsigned ijet = 0; ijet < JAResult.size(); ijet++) {
    //   PseudoJet j=JAResult.at(ijet);
    //   if ( SubtractBg == AREA) {
    // 	j = (*pBackgroundSubtractor)( j);
    //   } 
    //   if ( j.pt() > 0 ){
    // 	ResortedJAResult.push_back ( PseudoJetPt( JAResult.at(ijet), j.pt() ) );
    //   }
    // }
    // sort(ResortedJAResult.begin(), ResortedJAResult.end(),  PseudoJetPtGreater() );
    

    // if ( ResortedJAResult.size()==0 ) {
    //   continue;
    // }
    // contrib::SoftDrop sd(beta, z_cut);
    // if ( CustomRecluster ) {
    //   sd.set_reclustering(true, recluster);
    // }

    //   if ( SubtractBg == AREA) {
    //   sd.set_subtractor(pBackgroundSubtractor);
    //   sd.set_input_jet_is_subtracted( false );
    // } 
    // // sd.set_input_jet_is_subtracted( true );
    // // sd.set_subtractor( 0 );
    // // contrib::SoftDrop::_verbose=true;
    
    // njets = ResortedJAResult.size();
    // rho=0;
    // if ( pBackgroundSubtractor ) rho = JA.GetBackgroundEstimator()->rho();
    // for (unsigned ijet = 0; ijet < ResortedJAResult.size(); ijet++) {
    //   // Run SoftDrop and examine the output
    //   PseudoJet& CurrentJet = ResortedJAResult[ijet].first;
    //   PseudoJet sd_jet = sd( CurrentJet );
      
    //   zg[ijet]=0;
    //   zg1[ijet]=0;
    //   zg2[ijet]=0;
    //   delta_R[ijet]=0;      
    //   mu[ijet]=0;

    //   // assert(sd_jet != 0); //because soft drop is a groomer (not a tagger), it should always return a soft-dropped jet
    //   // HOWEVER, it seems the background subtractor may kill the sd_jet
    //   if ( sd_jet == 0){
    // 	cout <<  " FOREGROUND Original Jet:   " << CurrentJet << endl;
    // 	if ( pBackgroundSubtractor ) cout <<  " FOREGROUND rho A: " << JA.GetBackgroundEstimator()->rho() * CurrentJet.area() << endl;	  
    // 	if ( pBackgroundSubtractor ) cout <<  " FOREGROUND Subtracted Jet: " << (*pBackgroundSubtractor)( CurrentJet ) << endl;	  
    // 	cout << " --- Skipped. Something caused SoftDrop to return 0 ---" << endl;

    // 	// Use groomed==original in this case. Not ideal, but probably the best choice.
    // 	if ( SubtractBg == AREA) {
    // 	  CurrentJet = (*pBackgroundSubtractor)( CurrentJet );	
    // 	} 

    // 	new ( Jets[ijet] )        TStarJetVectorJet ( TStarJetVector(MakeTLorentzVector ( CurrentJet )) );
    // 	new ( GroomedJets[ijet] ) TStarJetVectorJet ( TStarJetVector(MakeTLorentzVector ( CurrentJet )) );

    // 	new ( sj1[ijet] )        TStarJetVectorJet ( );
    // 	new ( sj2[ijet] )        TStarJetVectorJet ( );

    // 	zg[ijet]=0;
    // 	delta_R[ijet]=0;
    // 	mu[ijet]=0;
	
    // 	((TStarJetVectorJet*)Jets[ijet])->SetArea( CurrentJet.area() );
    // 	((TStarJetVectorJet*)Jets[ijet])->SetArea4Vector( CurrentJet.area_4vector().four_mom()  );
    // 	// Save leading pt
    // 	((TStarJetVectorJet*)Jets[ijet])->SetLeadingParticle( 0, sorted_by_pt(CurrentJet.constituents()).at(0).pt() );
	
    // 	((TStarJetVectorJet*)GroomedJets[ijet])->SetArea( CurrentJet.area() );
    // 	((TStarJetVectorJet*)GroomedJets[ijet])->SetArea4Vector( CurrentJet.four_mom()  );

    //   } else {       
    // 	hzg->Fill(sd_jet.structure_of<contrib::SoftDrop>().symmetry());
    // 	zg[ijet]=sd_jet.structure_of<contrib::SoftDrop>().symmetry();
    // 	delta_R[ijet]=sd_jet.structure_of<contrib::SoftDrop>().delta_R();
    // 	mu[ijet]=sd_jet.structure_of<contrib::SoftDrop>().mu();

    // 	  // Now do background subtraction
    // 	if ( SubtractBg == AREA) {
    // 	  CurrentJet = (*pBackgroundSubtractor)( CurrentJet );
    // 	}

    // 	// save original and groomed
    // 	new ( Jets[ijet] )        TStarJetVectorJet ( TStarJetVector(MakeTLorentzVector ( CurrentJet )) );
    // 	new ( GroomedJets[ijet] ) TStarJetVectorJet ( TStarJetVector(MakeTLorentzVector ( sd_jet ))  );
	
    // 	if ( sd_jet.pieces().size() >0 ){
    // 	  new ( sj1[ijet] ) TStarJetVectorJet ( TStarJetVector(MakeTLorentzVector ( sd_jet.pieces().at(0) ))  );
    // 	  ((TStarJetVectorJet*)sj1[ijet])->SetMatch(-1);
    // 	  ((TStarJetVectorJet*)sj1[ijet])->SetArea( sd_jet.pieces().at(0).area() );
    // 	  ((TStarJetVectorJet*)sj1[ijet])->SetArea4Vector( sd_jet.pieces().at(0).area_4vector().four_mom()  );

    // 	  // Repeat on subjets?
    // 	  if ( Recursive) {
    // 	    if ( SubtractBg == AREA) {
    // 	      sd.set_subtractor( pBackgroundSubtractor);
    // 	      sd.set_input_jet_is_subtracted( true ); // this may be trouble...
    // 	    }
    // 	    PseudoJet rsd_jet = sd( sd_jet.pieces().at(0) );
    // 	    zg1[ijet] = rsd_jet.structure_of<contrib::SoftDrop>().symmetry();
    // 	  }

    // 	} else {
    // 	  new ( sj1[ijet] )        TStarJetVectorJet ( );
    // 	}
    // 	if ( sd_jet.pieces().size() >1 ){
    // 	  new ( sj2[ijet] ) TStarJetVectorJet ( TStarJetVector(MakeTLorentzVector ( sd_jet.pieces().at(1) ))  );
    // 	  ((TStarJetVectorJet*)sj2[ijet])->SetMatch(-1);
    // 	  ((TStarJetVectorJet*)sj2[ijet])->SetArea( sd_jet.pieces().at(1).area() );
    // 	  ((TStarJetVectorJet*)sj2[ijet])->SetArea4Vector( sd_jet.pieces().at(1).area_4vector().four_mom()  );
    // 	  // Repeat on subjets?
    // 	  if ( Recursive) {
    // 	    if ( SubtractBg == AREA) {
    // 	      sd.set_subtractor( pBackgroundSubtractor);
    // 	      sd.set_input_jet_is_subtracted( true ); // this may be trouble...
    // 	    }
    // 	    PseudoJet rsd_jet = sd( sd_jet.pieces().at(1) );
    // 	    zg2[ijet] = rsd_jet.structure_of<contrib::SoftDrop>().symmetry();
    // 	  }

    // 	} else {
    // 	  new ( sj2[ijet] )        TStarJetVectorJet ( );
    // 	}
	
    // 	((TStarJetVectorJet*)Jets[ijet])->SetArea( CurrentJet.area() );
    // 	((TStarJetVectorJet*)Jets[ijet])->SetArea4Vector( CurrentJet.area_4vector().four_mom()  );
    // 	// Save leading pt
    // 	((TStarJetVectorJet*)Jets[ijet])->SetLeadingParticle( 0, sorted_by_pt( CurrentJet.constituents()).at(0).pt() );
	
    // 	((TStarJetVectorJet*)GroomedJets[ijet])->SetArea( sd_jet.area() );
    // 	((TStarJetVectorJet*)GroomedJets[ijet])->SetArea4Vector( sd_jet.four_mom()  );
    //   }
      
    //   ((TStarJetVectorJet*)Jets[ijet])->SetMatch(-1);
    //   ((TStarJetVectorJet*)GroomedJets[ijet])->SetMatch(-1);

    //   // Make Jets[ijet].GetMatch() give the index of a matched hard parton
    //   // We have a choice here. 
    //   // - Use each hard parton exactly once, i.e. choose the best match (highest pT) when multiple options available
    //   // - Or match every jet to the best possible match, i.e., possibly use the targets multiple times
    //   // Using the first option
    //   // IMPORTANT: We do this by first-come first serve! The following assumes that we're working our way down in jet pT
    //   // Also, the partons vector is destroyed in the process
    //   for ( int ip=0; ip<partons.size(); ++ip){
    // 	if ( partons[ip].pt() == 0 ) continue;
    // 	if ( partons[ip].delta_R( CurrentJet )< R){
    // 	  ((TStarJetVectorJet*)Jets[ijet])->SetMatch( ip );
    // 	  ((TStarJetVectorJet*)GroomedJets[ijet])->SetMatch( ip );
    // 	  partons[ip] = PseudoJet();
    // 	  break;
    // 	} 
    //   }      
    // }
    
    // // Embedding?
    // // ----------
    // TStarJetPicoEventHeader* Embheader=0;
    // UInt_t seed;
    // if ( Embedding ){
    //   Embrefmult =0;
    //   // cout << " -------------- EMBEDDED ----------------- " << endl;

    //   for (int mix=0; mix < nMix; ++mix ){
    // 	switch (Embintype) {
    // 	  // =====================================================
    // 	case INPICO :
    // 	  // Need a unique uint from runid and eventid
    // 	  // runid: last 6 numbers are unique (in a given year at least), 7 to be safe
    // 	  // eventid: up to 10M events per runid aren't impossible
    // 	  // --> 123456710000000.
    // 	  // That's a bit more than uint_max |-(
    // 	  // haha! eventid*runid % UINT_MAX; works, at least for run 7
    // 	  // Try randtester.cxx for others first!!
    // 	  seed = (mix+1)*eventid*runid % UINT_MAX;
    // 	  gRandom->SetSeed(seed);
    // 	  Embevi = gRandom->Integer(NEmbEvents); // Start at a random but deterministic point
    // 	  pEmbReader->ReadEvent( Embevi );
    // 	  // if ( !pEmbReader->NextEvent() ) {
    // 	  //   cout << "Can't find a next embedding event, starting over." << endl;
    // 	  //   delete pEmbReader; pEmbReader=0;
    // 	  //   pEmbReader = SetupReader( EmbEvents, EmbTriggerName, PpZgParameters::RefMultCut );
    // 	  //   InitializeReader(  pEmbReader, EmbInputName, NEmbEvents, PicoDebugLevel );
    // 	  //   if ( !pEmbReader->NextEvent() ) {
    // 	  //     cerr << "Can't find a next embedding event after reset, bailing out." << endl;
    // 	  //     return -1;
    // 	  //   }
    // 	  // }
    // 	  // pEmbReader->PrintStatus(10);
    // 	  pEmbEvent = pEmbReader->GetOutputContainer()->GetArray();
	
    // 	  Embheader = pEmbReader->GetEvent()->GetHeader();
    // 	  Embrefmult = Embheader->GetProperReferenceMultiplicity();
    // 	  Embeventid = Embheader->GetEventId();
    // 	  Embrunid   = Embheader->GetRunId();
	
    // 	  // return -1;
    // 	  break;      
    // 	  // =====================================================
    // 	case INTREE :
    // 	case MCTREE :
    // 	  if ( Embevi>= NEmbEvents ) {
    // 	    // qnd: just cycle:
    // 	    Embevi=0;
    // 	    // done=true;
    // 	    // break;
    // 	  }
    // 	  EmbEvents->GetEntry(Embevi);
    // 	  // cout << Embrunid << "  " << Embeventid << "  -->  ";
	  
    // 	  if (Embintype == MCTREE ){
    // 	    cname = EmbEvents->GetCurrentFile()->GetName();
    // 	    filehash = cname.Hash();
    // 	    if ( filehash > UINT_MAX - 100000 ) filehash /= 2;
    // 	    if ( filehash < 1000000 ) filehash += 1000001;
    // 	    Embrunid += filehash;
    // 	  }
    // 	  if (Embintype == INTREE ){
    // 	    Embrunid = uEmbrunid;
    // 	    Embeventid = uEmbeventid;
    // 	  }

    // 	  // cout << Embrunid << "  " << Embeventid << endl;
    // 	  ++Embevi;
    // 	  break;
    // 	  // =====================================================
    // 	default:
    // 	  cerr << "Unknown intype " << intype << endl;
    // 	  return(-1);
    // 	}
    // 	// if ( done ) break;

    // 	// particles.clear(); // DEBUG!!      
    // 	for ( int i=0 ; i<pEmbEvent->GetEntries() ; ++i ){
    // 	  sv = (TStarJetVector*) pEmbEvent->At(i);
    // 	  // Ensure kinematic similarity
    // 	  if ( sv->Pt()< PtConsMin && sv->Pt()< PtSubConsMin ) continue;
    // 	  if ( fabs( sv->Eta() )>EtaConsCut ) continue;
    // 	  particles.push_back( PseudoJet (*sv ) );
    // 	  ptphieta->Fill(sv->Pt(),sv->Phi(),sv->Eta());
    // 	}
    // 	// FIXME: May (will) not work as intended unless both inputs are picoDSTs!
    // 	Embrunevent = ULong64_t(Embrunid)*10000000LL + Embeventid;

      
    // 	// Run analysis
    // 	// ------------
    // 	if (pEmbJA){
    // 	  delete pEmbJA; pEmbJA=0;
    // 	}
    // 	pEmbJA=0;
    // 	pEmbBackgroundSubtractor =  0;
    // 	Embrho=0;
    // 	switch ( EmbSubtractBg ){
    // 	case AREA:
    // 	  pEmbJA = new JetAnalyzer( particles, JetDef, AreaDef, select_jet_eta * (!fastjet::SelectorNHardest(2)) ) ;
    // 	  pEmbBackgroundSubtractor = pEmbJA->GetBackgroundSubtractor();
    // 	  Embrho=pEmbJA->GetBackgroundEstimator()->rho() ;
    // 	  break;
    // 	case NONE:
    // 	  pEmbJA = new JetAnalyzer( particles, JetDef, AreaDef ) ;
    // 	  break;
    // 	default :
    // 	  cerr << "Unsupported Background subtraction method" << endl;
    // 	  return -1;
    // 	  break;      
    // 	}
    // 	JetAnalyzer& EmbJA = *pEmbJA;

    // 	vector<PseudoJet> EmbJAResult = sorted_by_pt( select_jet( EmbJA.inclusive_jets() )) ;
    // 	vector<PseudoJetPt> EmbResortedJAResult;
    // 	for (unsigned ijet = 0; ijet < EmbJAResult.size(); ijet++) {
    // 	  PseudoJet j = EmbJAResult.at(ijet);
    // 	  if ( EmbSubtractBg == AREA) {
    // 	    j = (*pEmbBackgroundSubtractor)( j );
    // 	  }
    // 	  if ( j.pt() > 0 ){
    // 	    EmbResortedJAResult.push_back ( PseudoJetPt( EmbJAResult.at(ijet), j.pt() ) );
    // 	  }
    // 	}
    // 	sort(EmbResortedJAResult.begin(), EmbResortedJAResult.end(),  PseudoJetPtGreater() );
	
    
    // 	Embnjets = EmbResortedJAResult.size();      
    // 	if ( EmbSubtractBg == AREA) {
    // 	  sd.set_subtractor( pEmbBackgroundSubtractor );
    // 	  sd.set_input_jet_is_subtracted(false);
    // 	}

    // 	// contrib::SoftDrop unsubsd(beta, z_cut);
    // 	// unsubsd.set_input_jet_is_subtracted( false );

    // 	int j=0; // Counter for jets to save
    // 	for (unsigned ijet = 0; ijet < EmbResortedJAResult.size(); ijet++) {
    // 	  PseudoJet& EmbCurrentJet = EmbResortedJAResult[ijet].first;

    // 	  // Need to skip if this jet is unhealthy
    // 	  if ( EmbSubtractBg == AREA  && (*pEmbBackgroundSubtractor)( EmbCurrentJet ).pt() < 1 ) continue;
	
    // 	  // Run SoftDrop and examine the output
    // 	  // -----------------------------------
    // 	  PseudoJet sd_jet = sd( EmbCurrentJet );

    // 	  Embzg[j]=0;
    // 	  Embdelta_R[j]=0;      
    // 	  Embmu[j]=0;

    // 	  // assert(sd_jet != 0); //because soft drop is a groomer (not a tagger), it should always return a soft-dropped jet
    // 	  // HOWEVER, it seems the background subtractor may kill the sd_jet
    // 	  if ( sd_jet == 0){
    // 	    cout <<  " Original Jet:   " << EmbCurrentJet << endl;
    // 	    if (pEmbBackgroundSubtractor) cout <<  " Subtracted Jet: " << (*pEmbBackgroundSubtractor)( EmbCurrentJet ) << endl;	  
    // 	    cout << " --- Skipped. Something caused SoftDrop to return 0 ---" << endl;

    // 	    // Use groomed==original in this case. Not ideal, but probably the best choice.
    // 	    if ( EmbSubtractBg == AREA) {
    // 	      EmbCurrentJet = (*pEmbBackgroundSubtractor)( EmbCurrentJet );
    // 	    }
    // 	    new ( EmbJets[j] )        TStarJetVectorJet ( TStarJetVector(MakeTLorentzVector ( EmbCurrentJet )) );
    // 	    new ( EmbGroomedJets[j] ) TStarJetVectorJet ( TStarJetVector(MakeTLorentzVector ( EmbCurrentJet )) );

    // 	    new ( Embsj1[j] )        TStarJetVectorJet ( );
    // 	    new ( Embsj2[j] )        TStarJetVectorJet ( );

    // 	    Embzg[j]=0;
    // 	    Embdelta_R[j]=0;
    // 	    Embmu[j]=0;
	
    // 	    ((TStarJetVectorJet*)EmbJets[j])->SetArea( EmbCurrentJet.area() );
    // 	    ((TStarJetVectorJet*)EmbJets[j])->SetArea4Vector( EmbCurrentJet.area_4vector().four_mom()  );
    // 	    // Save leading pt
    // 	    ((TStarJetVectorJet*)EmbJets[j])->SetLeadingParticle( 0, sorted_by_pt( EmbCurrentJet.constituents()).at(0).pt() );
	
    // 	    ((TStarJetVectorJet*)EmbGroomedJets[j])->SetArea( EmbCurrentJet.area() );
    // 	    ((TStarJetVectorJet*)EmbGroomedJets[j])->SetArea4Vector( EmbCurrentJet.four_mom()  );

    // 	  } else {
	    
    // 	    // Now do background subtraction
    // 	    if ( EmbSubtractBg == AREA) {
    // 	      EmbCurrentJet = (*pEmbBackgroundSubtractor)( EmbCurrentJet );
    // 	    }
    // 	    // cout << "After subtraction pT = " << EmbCurrentJet << endl << endl;
	    
    // 	    hEmbzg->Fill(sd_jet.structure_of<contrib::SoftDrop>().symmetry());
    // 	    Embzg[ijet]=sd_jet.structure_of<contrib::SoftDrop>().symmetry();
    // 	    Embdelta_R[ijet]=sd_jet.structure_of<contrib::SoftDrop>().delta_R();
    // 	    Embmu[ijet]=sd_jet.structure_of<contrib::SoftDrop>().mu();
	    
    // 	    // save original and groomed
    // 	    new ( EmbJets[j] )        TStarJetVectorJet ( TStarJetVector(MakeTLorentzVector ( EmbCurrentJet ) ) );
    // 	    new ( EmbGroomedJets[j] ) TStarJetVectorJet ( TStarJetVector(MakeTLorentzVector ( sd_jet ) ) );
	    
    // 	    ((TStarJetVectorJet*)EmbJets[j])->SetMatch(-1);
    // 	    ((TStarJetVectorJet*)EmbGroomedJets[j])->SetMatch(-1);
	    
    // 	    ((TStarJetVectorJet*)EmbJets[j])->SetArea( EmbCurrentJet.area() );
    // 	    ((TStarJetVectorJet*)EmbJets[j])->SetArea4Vector( EmbCurrentJet.area_4vector().four_mom()  );
    // 	    ((TStarJetVectorJet*)EmbJets[j])->SetLeadingParticle( 0, sorted_by_pt( EmbCurrentJet.constituents()).at(0).pt() );
    // 	    ((TStarJetVectorJet*)EmbGroomedJets[j])->SetArea( sd_jet.area() );
    // 	    ((TStarJetVectorJet*)EmbGroomedJets[j])->SetArea4Vector( sd_jet.four_mom()  );
	    
    // 	    if ( sd_jet.pieces().size() >0 ){
    // 	      new ( Embsj1[j] ) TStarJetVectorJet ( TStarJetVector(MakeTLorentzVector ( sd_jet.pieces().at(0) ))  );
    // 	      ((TStarJetVectorJet*)Embsj1[j])->SetMatch(-1);
    // 	      ((TStarJetVectorJet*)Embsj1[j])->SetArea( sd_jet.pieces().at(0).area() );
    // 	      ((TStarJetVectorJet*)Embsj1[j])->SetArea4Vector( sd_jet.pieces().at(1).area_4vector().four_mom()  );
    // 	    } else {
    // 	      new ( Embsj1[j] )        TStarJetVectorJet ( );
    // 	    }
    // 	    if ( sd_jet.pieces().size() >1 ){
    // 	      new ( Embsj2[j] ) TStarJetVectorJet ( TStarJetVector(MakeTLorentzVector ( sd_jet.pieces().at(1) ))  );
    // 	      ((TStarJetVectorJet*)Embsj2[j])->SetMatch(-1);
    // 	      ((TStarJetVectorJet*)Embsj2[j])->SetArea( sd_jet.pieces().at(0).area() );
    // 	      ((TStarJetVectorJet*)Embsj2[j])->SetArea4Vector( sd_jet.pieces().at(1).area_4vector().four_mom()  );
    // 	    } else {
    // 	      new ( Embsj2[j] )        TStarJetVectorJet ( );
    // 	    }

    // 	  }

    // 	  // Match to an original jet. Similar considerations apply as for parton matching
    // 	  // The original jet vector is destroyed in the process
    // 	  for ( int iorig=0; iorig<ResortedJAResult.size(); ++iorig){
    // 	    PseudoJet& OrigJet = ResortedJAResult[iorig].first;
    // 	    if ( OrigJet.pt() == 0 ) continue;
    // 	    if ( OrigJet.delta_R( EmbCurrentJet )< R){
    // 	      ((TStarJetVectorJet*)EmbJets[j])->SetMatch( iorig );
    // 	      ((TStarJetVectorJet*)EmbGroomedJets[j])->SetMatch( iorig );
    // 	      // cout << "Is:     " << ((TStarJetVectorJet*)EmbJets[j])->Pt() << endl;
    // 	      // cout << "Compare to:     " << 	    JAResult[iorig].pt() << endl;
    // 	      // cout << "=============== " << endl;
    // 	      ResortedJAResult[iorig] = PseudoJetPt ( PseudoJet(), 0 );
    // 	      break;
    // 	    } 
    // 	  }
    // 	  j++;
    // 	} // for #jets
    // 	ResultTree->Fill();   
    //   } // for mix
    // } else {
    //   // Fill result tree
    //   ResultTree->Fill();   
    // }
  

  //  }  
  
  // info->Branch("Ntot"      , &Ntot      , "Ntot/L" );
  info->Branch("Naccepted" , &Naccepted , "Naccepted/L" );
  // info->Branch("Nrejected" , &Nrejected , "Nrejected/L" );
  info->Fill();
  
  fout->Write();

  if (ppzg->QA_TowerEt ) ppzg->QA_TowerEt->Write();

  cout << "Done." << endl;


  delete ppzg;
  return 0;
}
//----------------------------------------------------------------------

// DEBUG
void decluster (PseudoJet j){
  cout << " ### Declustering ### " << endl;
  cout << j.pt() << endl;

  PseudoJet piece1, piece2;

  static int level=0;
  if ( j.has_parents(piece1, piece2) ){
    cout << "level = " << level++ << endl;
    cout << "piece1.pt() = " << piece1.pt() << endl;
    cout << "piece2.pt() = " << piece2.pt() << endl;

    if (! piece1.is_pure_ghost() ) decluster(piece1);
    if (! piece2.is_pure_ghost() ) decluster(piece2);

  } else cout << " Done with this branch" << endl;
  return;
}

//----------------------------------------------------------------------
bool readinbadrunlist(vector<int> & badrun, TString csvfile) {
	
  // open infile
  std::string line;
  std::ifstream inFile (csvfile );
	
  std::cout<<"Loading bad run id from "<< csvfile.Data()<<std::endl;;
	        
  if ( !inFile.good() ) {
    std::cout<<"Can't open "<<csvfile.Data()<<std::endl;
    return false;
  }
	
  while (std::getline (inFile, line) ){
    if ( line.size()==0 ) continue; // skip empty lines
    if ( line[0] == '#' ) continue; // skip comments
	
    std::istringstream ss( line );
    while( ss ){
      std::string entry;
      std::getline( ss, entry, ',' );
      int ientry = atoi(entry.c_str());
      if (ientry) {
	badrun.push_back( ientry );
	std::cout<<"Added bad runid "<<ientry<<std::endl;
      }
    }
  }
	
  return true;
}

