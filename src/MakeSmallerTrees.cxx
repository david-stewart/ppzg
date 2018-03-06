// #include "AjAnalysis.hh"
// #include "AjParameters.hh"

#include "JetAnalyzer.hh"

#include "TStarJetPicoReader.h"
#include "TStarJetPicoEvent.h"
#include "TStarJetPicoEventHeader.h"
#include "TStarJetPicoEventCuts.h"

#include "TStarJetPicoPrimaryTrack.h"
#include "TStarJetPicoTrackCuts.h"
#include "TStarJetPicoTowerCuts.h"

#include "TStarJetVectorContainer.h"
#include "TStarJetVector.h"
#include "TStarJetPicoTriggerInfo.h"
#include "TStarJetPicoUtils.h"

#include "TSystem.h"
#include "TTree.h"
#include "TFile.h"
#include "TChain.h"

#include <iostream>
#include <exception>

#include <stdlib.h>     // for getenv

using namespace std;
using namespace fastjet;

bool readinbadrunlist(vector<int> & badrun, TString csvfile);

int main ( int argc, const char** argv ) {

  // TString inname="Data/y14_AuAu_HT2/101-105/AuAu14Pico_2DE36CC733533CA27F75404C86640322_99.root";
  TString inname="Data/P17id/AuAu_200_production_2014/HT/AuAu_200_HT_76_94_161.root";
  TString outname="Y14TEST.root";
  TString TriggerName="HT2HT3";
  // TString TriggerName="HT";

  // Long64_t CustomnEvents=1000;
  Long64_t CustomnEvents=-1;

  switch (argc){
  case 1 :
    // use default values
    break;
  case 2 :
    inname = argv[1];
    outname=gSystem->BaseName(inname);
    // outname.Prepend("/Users/kkauder/SmallNPE18/Small_");
    break;
  case 3 :
    inname  = argv[1];
    outname = argv[2];
    break;
  case 4 :
    inname  = argv[1];
    outname = argv[2];
    CustomnEvents = atoi(argv[3]);
    break;
  default :
    cerr << "Incompatible arguments." << endl;
    cerr << "argc = " << argc << endl;  
    return -1;
    break;
  }

  cout << "Reading from " << inname << endl;
  cout << "Writing to " << outname << endl;
    
  // Load input
  // ----------
  TChain* chain = new TChain ( "JetTree");
  chain->Add( inname );

  double RefMultCut = 0;
  double MinJetPt=16;
  TStarJetPicoReader reader;
  reader.SetInputChain (chain);
  

  // Event and track selection
  // -------------------------
  TStarJetPicoEventCuts* evCuts = reader.GetEventCuts();
  evCuts->SetTriggerSelection( TriggerName ); //All, MB, HT, pp, ppHT, ppJP
  // Additional cuts 
  evCuts->SetVertexZCut ( 30 );
  evCuts->SetRefMultCut ( RefMultCut );
  evCuts->SetVertexZDiffCut( 100 ); // SHOULD be smaller, like 3-6
  evCuts->SetMinEventEtCut ( -1.0 ); // Could go to 5.5, 6?

  // Keep things inclusive! Keep electrons and use MIP correction for jet finding
  reader.SetRejectTowerElectrons( kFALSE );
  reader.SetApplyFractionHadronicCorrection(kFALSE);

  // bad run list
  TString csvfile= TString( getenv("STARPICOPATH" )) + "/P17id_bad_runs.txt";
  vector<int> badruns;
  if ( readinbadrunlist( badruns, csvfile) == false ){
    cerr << "Problems reading bad run list" << endl;
    return -1;
  }
  reader.AddMaskedRuns (badruns);
  
  // Tracks cuts
  TStarJetPicoTrackCuts* trackCuts = reader.GetTrackCuts();
  trackCuts->SetDCACut( 2 ); //  SHOULD be 1?
  trackCuts->SetMinNFitPointsCut( 20 );
  trackCuts->SetFitOverMaxPointsCut( 0.52 );
  trackCuts->SetMaxPtCut ( 1000 ); // SHOULD be smaller, like 15-30?

  cout << "Using these track cuts:" << endl;
  cout << " dca : " << trackCuts->GetDCACut(  ) << endl;
  cout << " nfit : " <<   trackCuts->GetMinNFitPointsCut( ) << endl;
  cout << " nfitratio : " <<   trackCuts->GetFitOverMaxPointsCut( ) << endl;
  cout << " maxpt : " << trackCuts->GetMaxPtCut (  ) << endl;
  
  // Towers
  TStarJetPicoTowerCuts* towerCuts = reader.GetTowerCuts();
  towerCuts->SetMaxEtCut( 1000 ); // SHOULD be smaller, say 45
  // towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/badTowerList_y11.txt");
  // towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/OrigY7MBBadTowers.txt");
  // towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/QuickAndDirty_y14.txt");
  towerCuts->AddBadTowers( TString( getenv("STARPICOPATH" )) + "/P17id_bad_towers.txt");

  cout << "Using these tower cuts:" << endl;
  cout << "  GetMaxEtCut = " << towerCuts->GetMaxEtCut() << endl;
  cout << "  Gety8PythiaCut = " << towerCuts->Gety8PythiaCut() << endl;
  
  // V0s: Turn off
  reader.SetProcessV0s(false);
  TStarJetPicoDefinitions::SetDebugLevel(0);

  Long64_t nEvents=-1; // -1 for all
  if ( CustomnEvents > 0 ) nEvents=CustomnEvents;
  reader.Init(nEvents);

  Int_t nAccepted=0;
  Int_t n10=0; // Just out of curiosity, count how many above 10 GeV
  
  // Set up output
  // -------------
  TFile * fout = new TFile (outname, "RECREATE");
  // TTree::SetMaxTreeSize(40000000LL ); // 10000000LL == 10 MB
  // TTree::SetMaxTreeSize(200000000LL ); // 200 MB
  // TTree::SetMaxTreeSize(500000000LL ); // 0.5GB
  TTree::SetMaxTreeSize(1000000000LL ); // 1GB
  // TTree::SetMaxTreeSize( 1000000LL); // 1MB
  TTree* outTree=new TTree("JetTree","Preselected Pico Tree for Jet");
  TStarJetPicoEvent *mEvPico = new TStarJetPicoEvent();
  outTree->Branch("PicoJetTree","TStarJetPicoEvent",&mEvPico);

  // Set up jet finding
  // ------------------
  double PtConsHi = 2.0;
  double max_track_rap=1.0;

  // allow two Rs. Smaller R should be a subset of larger R,
  // but it scans out to larger rapidity
  double R1 = 0.4;
  double R2 = 0.2;
  JetDefinition jet_def1 = fastjet::JetDefinition(fastjet::antikt_algorithm, R1);
  JetDefinition jet_def2 = fastjet::JetDefinition(fastjet::antikt_algorithm, R2);
  
  Selector select_track_rap = fastjet::SelectorAbsRapMax(max_track_rap);
  Selector select_hipt      = fastjet::SelectorPtMin( PtConsHi );
  Selector shi     = select_track_rap * select_hipt;
  
  vector<PseudoJet> particles;
  PseudoJet pj;
  TStarJetVectorContainer<TStarJetVector>* container;
  TStarJetVector* sv; // TLorentzVector* would be sufficient. 

  
  try{
    while ( reader.NextEvent() ) {
      reader.PrintStatus(10);
      
      // eventid = header->GetEventId();
      // runid   = header->GetRunId();
      // refmult=0;
      // if ( isAuAu ) refmult=header->GetGReferenceMultiplicity();
      
      // Load event
      // ----------
      container = reader.GetOutputContainer();
      
      // Make particle vector
      // --------------------
      particles.clear();
    
      for (int ip = 0; ip<container->GetEntries() ; ++ip ){
	sv = container->Get(ip);  // Note that TStarJetVector  contains more info, such as charge;
	// mostly for demonstration, add charge information
	pj=MakePseudoJet( sv );
	pj.set_user_info ( new JetAnalysisUserInfo( 3*sv->GetCharge() ) );
	particles.push_back ( pj );
      }    

      // find high constituent pT jets
      // -----------------------------
      vector<PseudoJet> pHi = shi( particles );
      JetAnalyzer JA1 ( pHi, jet_def1 ); // NO background subtraction
      JetAnalyzer JA2 ( pHi, jet_def2 ); // NO background subtraction
      vector<PseudoJet> JAResult1 = fastjet::sorted_by_pt( JA1.inclusive_jets() );     // NO jet selector
      vector<PseudoJet> JAResult2 = fastjet::sorted_by_pt( JA2.inclusive_jets() );     // NO jet selector
      if ( JAResult1.size()<1 && JAResult2.size()<1  )         continue;
	    
      if ( JAResult1.at(0).pt()>10 )         n10++;


      if ( ( JAResult1.size()>0 && JAResult1.at(0).pt()> MinJetPt ) ||
	   ( JAResult2.size()>0 && JAResult2.at(0).pt()> MinJetPt )     ){

	nAccepted++;
	mEvPico=(TStarJetPicoEvent*) reader.GetEvent();
	outTree->Fill();
      } // has trigger
      
    } // while NextEvent
  } catch ( exception& e) {
    cerr << "Caught " << e.what() << endl;
    return -1;
  }
  
  outTree->Write();  

  cout << "Started with " << chain->GetEntries() << " events." << endl;
  cout << "Accepted " << nAccepted << " events." << endl;
  cout << "There were " << n10 << " events with a jet above 10 GeV." << endl;
  cout << "Done." << endl;
  
  return 0;
  
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

