/** @file PpZgAnalysis.cxx
    @author Kolja Kauder
    @version Revision 0.1
    @brief Class for A<SUB>J</SUB> analysis
    @details Uses JetAnalyzer objects to perform A<SUB>J</SUB> analysis.
    @date Mar 02, 2015
*/

#include "PpZgAnalysis.hh"
#include <stdlib.h>     // for getenv, atof, atoi

using std::cout;
using std::cerr;
using std::endl;

// Standard ctor
PpZgAnalysis::PpZgAnalysis ( const int argc, const char** const argv )
{
  // Parse arguments
  // ---------------
  vector<string> arguments(argv + 1, argv + argc);
  bool argsokay=true;
  bool forcedgeantnum=false;
  NEvents=-1;
  for ( auto parg = arguments.begin() ; parg!=arguments.end() ; ++parg){
    string arg=*parg;
    if ( arg == "-R" ){      
      if ( ++parg == arguments.end() ){ argsokay=false; break; }
      pars.R = atof( parg->data());
    } else if ( arg == "-lja" ){     
      if (++parg==arguments.end() ){ argsokay=false; break; }
      pars.LargeJetAlgorithm = AlgoFromString ( *parg);
    } else if ( arg == "-rcja" ){
      if (++parg==arguments.end() ){ argsokay=false; break; }
      pars.CustomRecluster=true;
      pars.CustomReclusterJetAlgorithm=AlgoFromString ( *parg);
      ReclusterJetAlgorithm = AlgoFromString ( *parg);
      ReclusterJetDef = JetDefinition( ReclusterJetAlgorithm, 2.0*pars.R );
      recluster = new Recluster( ReclusterJetDef, 2.0*pars.R );
    } else if ( arg == "-bg" ){      
      if (++parg==arguments.end() ){ argsokay=false; break; }
      pars.SubtractBg=BGTYPE(atoi(parg->data()));
      if ( pars.SubtractBg<0 || pars.SubtractBg>2 ){ argsokay=false; break; }
    } else if ( arg == "-Embbg" ){      
      if (++parg==arguments.end() ){ argsokay=false; break; }
      pars.EmbSubtractBg=BGTYPE(atoi(parg->data()));
      if ( pars.EmbSubtractBg<0 || pars.EmbSubtractBg>2 ){ argsokay=false; break; }
    } else if ( arg == "-b" ){
      if (++parg==arguments.end() ){ argsokay=false; break; }
      pars.beta = atof((parg)->data());
    } else if ( arg == "-z" ){
      if (++parg==arguments.end() ){ argsokay=false; break; }
      pars.z_cut = atof((parg)->data());
    } else if ( arg == "-pj" ){
      if (++parg ==arguments.end() ){ argsokay=false; break; }
      pars.PtJetMin = atof((parg)->data());
      if (++parg ==arguments.end() ){ argsokay=false; break; }
      pars.PtJetMax = atof((parg)->data());
    } else if ( arg == "-ec" ){      
      if (++parg ==arguments.end() ){ argsokay=false; break; }
      pars.EtaConsCut = atof((parg)->data());      
    } else if ( arg == "-pc" ){      
      if (++parg ==arguments.end() ){ argsokay=false; break; }
      pars.PtConsMin = atof((parg)->data());      
      if (++parg ==arguments.end() ){ argsokay=false; break; }
      pars.PtConsMax = atof((parg)->data());
    } else if ( arg == "-o" ){     
      if (++parg ==arguments.end() ){ argsokay=false; break; }
      pars.OutFileName=*parg;
    } else if ( arg == "-i" ){
      if (++parg ==arguments.end() ){ argsokay=false; break; }
      pars.InputName=*parg;
    } else if ( arg == "-embi" ){
      if (++parg ==arguments.end() ){ argsokay=false; break; }
      pars.EmbInputName=*parg;
      // Add a shortcut
      if (pars.EmbInputName == "FAKERHIC" ) {
	pars.EmbInputName = "Data/FakeAuAu20_*root";
	pars.Embintype = MCTREE;           ///< Embedding input type (can be a pico dst, a result tree, an MC tree)
	pars.EmbChainName = "tree";        ///< Name of the embedding input chain
	// pars.EmbTriggerName = "All";       ///< Embedding trigger type (All, MB, HT, pp, ppHT, ppJP)
      }
    } else if ( arg == "-c" ){
      if (++parg ==arguments.end() ){ argsokay=false; break; }
      pars.ChainName=*parg;
    } else if ( arg == "-trig" ){
      if (++parg ==arguments.end() ){ argsokay=false; break; }
      pars.TriggerName=*parg;
    } else if ( arg == "-ht" ){      
      if ( ++parg == arguments.end() ){ argsokay=false; break; }
      float ptht =atof( parg->data());
      if ( ptht>0 ){
	pars.MinEventEtCut = ptht;
	pars.ManualHtCut = ptht;
      }
    // } else if ( arg == "-htled" ){      
    //   if ( ++parg == arguments.end() ){ argsokay=false; break; }
    //   istringstream is(parg->data());
    //   is >> boolalpha >> pars.HTled;
    } else if ( arg == "-embtrig" ){
      if (++parg ==arguments.end() ){ argsokay=false; break; }
      pars.EmbTriggerName=*parg;
    } else if ( arg == "-embc" ){
      if (++parg ==arguments.end() ){ argsokay=false; break; }
      pars.EmbChainName=*parg;
    } else if ( arg == "-nmix" ){      
      if (++parg==arguments.end() ){ argsokay=false; break; }
      pars.nMix=atoi(parg->data());
    } else if ( arg == "-intype" ){
      if (++parg ==arguments.end() ){ argsokay=false; break; }
      if ( *parg == "pico" ){
	pars.intype = INPICO;
	continue;
      }
      if ( *parg == "mcpico" ){
	pars.intype = MCPICO;
	continue;
      }
      if ( *parg == "mctree" ){
	pars.intype = MCTREE;
	continue;
      }
      if ( *parg == "tree" ){
	pars.intype = INTREE;
	continue;
      }
      argsokay=false;
      break;
    } else if ( arg == "-embintype" ){
      if (++parg ==arguments.end() ){ argsokay=false; break; }
      if ( *parg == "pico" ){
	pars.Embintype = INPICO;
	continue;
      }
      if ( *parg == "mcpico" ){
	pars.Embintype = MCPICO;
	continue;
      }
      if ( *parg == "mctree" ){
	pars.Embintype = MCTREE;
	continue;
      }
      if ( *parg == "tree" ){
	pars.Embintype = INTREE;
	continue;
      }
      argsokay=false;
      break;
    } else if ( arg == "-N" ){      
      if (++parg==arguments.end() ){ argsokay=false; break; }
      NEvents=atoi(parg->data());
    } else if ( arg == "-geantnum" ){      
      if ( ++parg == arguments.end() ){ argsokay=false; break; }
      if ( *parg != "0" && *parg != "1" ){ argsokay=false; break; }
      forcedgeantnum=true;
      pars.UseGeantNumbering = bool ( atoi(parg->data()));      
    } else {
      argsokay=false;
      break;
    }
  }
  
  if ( !argsokay ) {
    cerr << "usage: " << argv[0] << endl
	 << " [-o OutFileName]"  << endl
      	 << " [-N Nevents (<0 for all)]" << endl
      	 << " [-lja LargeJetAlgorithm]" << endl
	 << " [-rcja ReclusteringJetAlgorithm]" << endl
	 << " [-bg BackgroundSubtraction (NONE=0, AREA=1, CONSTSUBPRE=2, CONSTSUBPOST=3)]" << endl
      	 << " [-Embbg EmbBackgroundSubtraction (NONE=0, AREA=1, CONSTSUBPRE=2, CONSTSUBPOST=3)]" << endl
      	 << " [-i infilepattern]" << endl
      	 << " [-c chainname]" << endl
      	 << " [-intype pico|mcpico|tree|mctree]" << endl
	 << " [-trig trigger name (e.g. HT)]" << endl
      	 << " [-ht offline high tower cut]" << endl
	 << " [-ht snap to high tower true|false ]" << endl
      	 << " [-embi embedding infilepattern]" << endl
      	 << " [-embc embedding chainname]" << endl
      	 << " [-embintype embedding pico|mcpico|tree|mctree]" << endl
	 << " [-nmix nMix]" << endl
	 << " [-R radius]" << endl
	 << " [-b beta]" << endl
	 << " [-z z_cut]" << endl
	 << " [-pj PtJetMin PtJetMax]" << endl
	 << " [-ec EtaConsCut]" << endl
	 << " [-pc PtConsMin PtConsMax]" << endl
      	 << " [-psc PtSubConsMin PtSubConsMax]" << endl
      	 << " [--geantnum true|false] (Force Geant run event id hack)" << endl      
	 << endl << endl
	 << "NOTE: Wildcarded file patterns should be in single quotes." << endl
	 << endl;      
    throw std::runtime_error("Not a valid list of options");
  }

  // Consistency checks
  // ------------------
  if ( pars.SubtractBg==CONSTSUBPOST || pars.EmbSubtractBg==CONSTSUBPOST ){
    throw std::runtime_error("Constituent Subtraction after jetfinding not yet implemented, sorry.");
  }

  if ( !forcedgeantnum ){
    if ( pars.InputName.Contains( "Geant" ) || pars.InputName.Contains( "EmbedPythiaRun12" ) ){
      pars.UseGeantNumbering=true;
    }
  }

  // if ( ( pars.intype!=MCPICO && pars.intype!=INPICO )
  //      && (pars.MinEventEtCut > 0.1 || pars.ManualHtCut > 0.1) ){
  //   throw std::runtime_error("HT cuts don't work for the selected input type.");
  // }
    
  if ( pars.PtJetMin<=0 ){
    throw std::runtime_error("PtJetMin needs to be positive (0.001 will work).");
  }

  if ( pars.intype==MCPICO ){
    // This refers to the JetTreeMc branch
    if ( pars.ChainName!= "JetTreeMc" ){
      throw std::runtime_error("Unsuitable chain name for pars.intype==MCPICO");
    }
  }          
  if ( pars.ChainName == "JetTreeMc" ){
    if ( pars.intype !=MCPICO ){
      throw std::runtime_error("Unsuitable chain name for pars.intype==MCPICO");
    }
  }

  // Derived rapidity cuts
  // ---------------------
  EtaJetCut     = pars.EtaConsCut - pars.R;
  EtaGhostCut   = EtaJetCut  + 2.0*pars.R;
  
  // Jet candidate selectors
  // -----------------------
  // select_jet_eta     = SelectorAbsRapMax( EtaJetCut );
  select_jet_eta     = SelectorAbsEtaMax( EtaJetCut );
  select_jet_pt      = SelectorPtRange( pars.PtJetMin, pars.PtJetMax );
  select_jet         = select_jet_eta * select_jet_pt;     
  
  // Repeat on subjets?
  // ------------------
  pars.Recursive = pars.InputName.Contains("Pythia") && false;

  // Initialize jet finding
  // ----------------------
  AreaSpec = GhostedAreaSpec ( EtaGhostCut, pars.GhostRepeat, pars.GhostArea );
  AreaDef = AreaDefinition (fastjet::active_area_explicit_ghosts, AreaSpec);
  JetDef    = JetDefinition( pars.LargeJetAlgorithm, pars.R );

  SelectClose = fastjet::SelectorCircle( pars.R );

  cout << " R = " << pars.R << endl;
  cout << " beta = " << pars.beta << endl;
  cout << " z_cut = " << pars.z_cut << endl;
  cout << " Original jet algorithm : "<< pars.LargeJetAlgorithm << endl;
  if ( pars.CustomRecluster )
    cout << " Reclustering with a different algorithm "<< ReclusterJetAlgorithm << endl;
  cout << " PtJetMin = " << pars.PtJetMin << endl;
  cout << " PtJetMax = " << pars.PtJetMax << endl;
  cout << " PtConsMin = " << pars.PtConsMin << endl;
  cout << " PtConsMax = " << pars.PtConsMax << endl;
  cout << " Constituent eta cut = " << pars.EtaConsCut << endl;
  cout << " Jet eta cut = " << EtaJetCut << endl;
  cout << " Ghosts out to eta = " << EtaGhostCut << endl;
  cout << " Reading tree named \""<< pars.ChainName << "\" from " << pars.InputName << endl;
  cout << " intype = " << pars.intype << endl;
  cout << " Writing to " << pars.OutFileName << endl;
  if ( Embedding ) {
    cout << " Embedding into tree named \""<< pars.EmbChainName << "\" from " << pars.EmbInputName << endl;
    cout << " Embintype = " << pars.Embintype << endl;
    cout << " nMix = " << pars.nMix << endl;
  }
  cout << " ----" << endl;
  cout << " Writing to " << pars.OutFileName << endl;
  cout << " ----------------------------" << endl;

  // Quick and dirty QA histos
  // -------------------------
  QA_TowerEt = new TH2D( "QA_TowerEt","", 4800, 0.5, 4800.5, 80, 0, 80);
    
}
//----------------------------------------------------------------------
PpZgAnalysis::~PpZgAnalysis(){
  if (pJA){
    delete pJA; pJA=0;
  }  
  if ( pConstituentBackgroundSubtractor ){  
    // delete pConstituentBackgroundSubtractor; // TAKEN CARE OFF IN pJA
    pConstituentBackgroundSubtractor =  0;
  }
  if ( pBackgroundSubtractor ){
    // delete pBackgroundSubtractor; // TAKEN CARE OFF IN pJA
    pBackgroundSubtractor =  0;
  }
}

//----------------------------------------------------------------------
bool PpZgAnalysis::InitChains(){

  // For trees of TStarJetVector
  // (like a previous result)
  // ------------------------
  Events = new TChain(pars.ChainName);
  Events->Add(pars.InputName);
  if ( NEvents<0 ) NEvents = INT_MAX;

  if ( pars.intype==INTREE || pars.intype==MCTREE ){
    assert ( Events->GetEntries()>0 && "Something went wrong loading events.");
    NEvents=min(NEvents,Events->GetEntries() );
    
    pFullEvent = new TClonesArray("TStarJetVector");
    Events->GetBranch("PythiaParticles")->SetAutoDelete(kFALSE);
    Events->SetBranchAddress("PythiaParticles", &pFullEvent);
    
    // these are frustratingly mixed between int and uint
    // Use GetLeaf()->GetValue() instead
    // Events->SetBranchAddress("eventid", &ueventid);
    // Events->SetBranchAddress("runid", &urunid);

    // pFullEvent = new TClonesArray("TStarJetVector");
    // Events->GetBranch("PythiaParticles")->SetAutoDelete(kFALSE);
    // Events->SetBranchAddress("PythiaParticles", &pFullEvent);

    if ( pars.intype==MCTREE ){
      pHardPartons= new TClonesArray("TStarJetVector");
      Events->GetBranch("HardPartons")->SetAutoDelete(kFALSE);
      Events->SetBranchAddress("HardPartons", &pHardPartons);

      pHardPartonNames= new TClonesArray("TObjString");
      Events->GetBranch("HardPartonNames")->SetAutoDelete(kFALSE);
      Events->SetBranchAddress("HardPartonNames", &pHardPartonNames);
      
      pSavedHardPartons= new TClonesArray("TStarJetVector");
      pSavedHardPartonNames= new TClonesArray("TObjString");
    }
    
  }
  
  // For picoDSTs
  // -------------
  if ( pars.intype==INPICO || pars.intype==MCPICO ){
    pReader = SetupReader( Events, pars );

    InitializeReader(  pReader, pars.InputName, NEvents, PicoDebugLevel );
    if ( pReader && pars.InputName.Contains( "picoDst_4_5" ) ) pReader->SetUseRejectAnyway( true );

    if ( pars.intype==MCPICO )      TurnOffCuts ( pReader );

    pReader->SetApplyFractionHadronicCorrection(kTRUE);
    pReader->SetFractionHadronicCorrection(0.9999);
    pReader->SetRejectTowerElectrons( kFALSE );

    cout << "Don't forget to set tower cuts for pReader!!" << endl;
    
  }

  Embedding = (pars.EmbInputName !="" && pars.EmbInputName!="NONE");
  pEmbReader=0;
  
  cout << "N = " << NEvents << endl;

  if ( Embedding ){
    EmbEvents = new TChain(pars.EmbChainName);
    EmbEvents->Add(pars.EmbInputName);    
    assert ( EmbEvents->GetEntries()>0 && "Something went wrong loading the embedding data.");
    if ( NEmbEvents<0 ) NEmbEvents = EmbEvents->GetEntries();
    NEmbEvents=min(NEmbEvents,EmbEvents->GetEntries() );
    gRandom->SetSeed(0);
    Embevi = gRandom->Integer(NEmbEvents); // Start at a random point

    if ( pars.Embintype==MCTREE ){

      pEmbEvent = new TClonesArray("TStarJetVector");
      EmbEvents->GetBranch("McParticles")->SetAutoDelete(kFALSE);
      EmbEvents->SetBranchAddress("McParticles", &pEmbEvent);
    } else if ( pars.Embintype==INTREE ){      

      pEmbEvent = new TClonesArray("TStarJetVector");
      EmbEvents->GetBranch("FullEvent")->SetAutoDelete(kFALSE);
      EmbEvents->SetBranchAddress("FullEvent", &pEmbEvent);

    } else if ( pars.Embintype==INPICO || pars.Embintype==MCPICO ){
      pEmbReader = SetupReader( EmbEvents, pars );
      InitializeReader(  pEmbReader, pars.EmbInputName, NEmbEvents, PicoDebugLevel );

      if ( pEmbReader && pars.InputName.Contains( "picoDst_4_5" ) ) pEmbReader->SetUseRejectAnyway( true );      
      if ( pars.Embintype==MCPICO )      TurnOffCuts ( pEmbReader );

      pEmbReader->SetApplyFractionHadronicCorrection(kTRUE);
      pEmbReader->SetFractionHadronicCorrection(0.9999);
      pEmbReader->SetRejectTowerElectrons( kFALSE );      

      pEmbReader->ReadEvent( Embevi );
    } else {
      throw std::runtime_error("Unknown embedding type.");
    }

    cout << "Don't forget to set tower cuts for pReader!!" << endl;
    cout << "Starting Embedding with event number " << Embevi << endl;
    
  }  
    
  cout << "Done initializing chains. " << endl;
  return true;
}
//----------------------------------------------------------------------
// Main routine for one event.
EVENTRESULT PpZgAnalysis::RunEvent (){
  // cout << "-----------------------" << endl;
  // cout << "Entering PpZgAnalysis::RunEvent " << endl;
  TStarJetVector* sv;

  TStarJetPicoEventHeader* header=0;

  UInt_t filehash = 0;
  TString cname = "";
  
  // Reset results (from last event)
  // -------------------------------
  GroomingResult.clear();
  weight=1;
  partons.clear();
  particles.clear();
  if ( pHardPartons ) pHardPartons->Clear();
  if ( pHardPartonNames ) pHardPartonNames->Clear();
  pHT=0;

  switch (pars.intype) {
    // =====================================================
  case INPICO :      
  case MCPICO :      
    if ( !pReader->NextEvent() ) {
      // cout << "Can't find a next event" << endl;
      // done=true;
      pReader->PrintStatus();
      return EVENTRESULT::ENDOFINPUT;
      break;
    }
    pReader->PrintStatus(10);
    
    // cout << pReader->GetOutputContainer()->GetEntries() << endl; 
    pFullEvent = pReader->GetOutputContainer()->GetArray();
    
    header = pReader->GetEvent()->GetHeader();
    refmult=header->GetProperReferenceMultiplicity();
      
    if ( pars.intype==INPICO ) pHT = pReader->GetHighTower();

    eventid = header->GetEventId();
    runid   = header->GetRunId();

    // For GEANT: Need to devise a runid that's unique but also
    // reproducible to match Geant and GeantMc data.
    if ( pars.UseGeantNumbering ){
      TString cname = gSystem->BaseName( pReader->GetInputChain()->GetCurrentFile()->GetName() );
      UInt_t filehash = cname.Hash();
      while ( filehash > INT_MAX - 100000 ) filehash -= INT_MAX / 4; // some random large number
      if ( filehash < 1000000 ) filehash += 1000001;
      runid = filehash;
      // Sigh. Apparently also need to uniquefy the event id
      // since some are the same in the same file. Grr.
      // The following isn't great, because it uses the number in the chain, but it should get the job done
      eventid = pReader->GetNOfCurrentEvent();
    }  

    break;      
    // =====================================================
  case INTREE :
  case MCTREE :
    if ( evi>= NEvents ) {
      return EVENTRESULT::ENDOFINPUT;
      break;
    }
    
    if ( !(evi%200) ) cout << "Working on " << evi << " / " << NEvents << endl;
    Events->GetEntry(evi);

    cname = Events->GetCurrentFile()->GetName();
    eventid = Events->GetLeaf("eventid")->GetValue();
    runid = Events->GetLeaf("runid")->GetValue();

    if (pars.intype == MCTREE ){
      filehash = cname.Hash();
      while ( filehash > INT_MAX - 100000 ) filehash /= 10;
      if ( filehash < 1000000 ) filehash += 1000001;
      runid += filehash;
    }    

    ++evi;
    break;
    // =====================================================
  default:
    cerr << "Unknown intype " << pars.intype << endl;
    return EVENTRESULT::PROBLEM;
  }
    
  // // FIXME: May (will) not work as intended unless both inputs are picoDSTs!
  // runevent = ULong64_t(runid)*10000000LL + eventid;
    
  // Fill particle container
  // -----------------------

  // MANUAL HT cut evaluated here
  // ---------------------------------
  bool HasManualHighTower=false;
  if ( pars.intype==INPICO ) HasManualHighTower=true;
  
  for ( int i=0 ; i<pFullEvent->GetEntries() ; ++i ){
    sv = (TStarJetVector*) pFullEvent->At(i);
    // Ensure kinematic similarity
    // cerr << "Pseudo" << endl;
    // cerr << "Pseudo " << sv->Eta() << "  " << sv->Pt() << endl;
    // cerr << "Pseudo worked" << endl;
		   
    if ( sv->Pt()< pars.PtConsMin )             continue;
    if ( fabs( sv->Eta() )>pars.EtaConsCut )    continue;
    particles.push_back( PseudoJet (*sv ) );
    particles.back().set_user_info ( new JetAnalysisUserInfo( 3*sv->GetCharge(),"",sv->GetTowerID() ) );
    
    if ( !sv->GetCharge()) QA_TowerEt->Fill ( sv->GetTowerID(), sv->Et() );

    if (
	( pars.intype==MCPICO || pars.intype==MCTREE || pars.intype==INTREE )
	 && sv->GetCharge()==0
	 && fabs(sv->Eta())<1.0
	 && sv->Pt()>pars.ManualHtCut
	 ) {
      // cout << sv->Pt() << "  " << sv->Phi() << "  " << sv->Eta() << "  " << sv->GetCharge() << endl;
      HasManualHighTower=true;
      // if ( HTled ) pHT = sv; // DEBUG: This is slightly FALSE, use to recreate QM, HP
      if ( !pHT || (pHT && sv->Pt()>pHT->Pt()) ) pHT = sv; // This is more correct, use to reproduce new code in ppzg
    }
  }
  
  if ( !HasManualHighTower) {
    // cerr << "Skipped due to manual ht cut" << endl;
    return EVENTRESULT::NOTACCEPTED;
  }

  if ( particles.size()==0 ) return EVENTRESULT::NOCONSTS;

  // For pythia, use cross section as weight
  // ---------------------------------------
  if ( TParameter<double>* sigmaGen=(TParameter<double>*) Events->GetCurrentFile()->Get("sigmaGen") ){
    weight=sigmaGen->GetVal();
  }
  
  // For GEANT data
  // --------------
  if ( pars.InputName.Contains("Geant") ){
    TString currentfile = pReader->GetInputChain()->GetCurrentFile()->GetName();
    weight=LookupXsec ( currentfile );
    if ( fabs(weight-1)<1e-4){
      throw std::runtime_error("mcweight unchanged!");
    }
  }

  if ( pars.InputName.Contains("EmbedPythiaRun12pp200") ){
    TString currentfile = pReader->GetInputChain()->GetCurrentFile()->GetName();
    weight=LookupRun12Xsec ( currentfile );
    if ( fabs(weight-1)<1e-4){
      throw std::runtime_error("mcweight unchanged!");
    }
  }

  
  // For pythia, fill leading parton container
  // -----------------------------------------  
  if ( pHardPartons ){
    for ( int i=0 ; i<pHardPartons->GetEntries() ; ++i ){
      sv = (TStarJetVector*) pHardPartons->At(i);
      
      PseudoJet pj = PseudoJet (*sv );
      
      // flavor info
      TString& s = ((TObjString*)(pHardPartonNames->At(i)))->String();
      int qcharge=-999;
      if ( s=="g" ) qcharge = 0;
      
      if ( s(0)=='u' || s(0)=='c' || s(0)=='t' ) qcharge  = 2;
      if ( s(0)=='d' || s(0)=='s' || s(0)=='b' ) qcharge = -1;
      if ( s.Contains("bar") ) qcharge*=-1;
      
      if ( abs ( qcharge ) >3 ) cout<< s << endl;
      
      
      pj.set_user_info ( new JetAnalysisUserInfo( qcharge ) );
      partons.push_back( pj );
      
      // Save them too
      // Update info here, may not be complete in original tree
      sv->SetType(TStarJetVector::_PYTHIA);
      sv->SetCharge(qcharge ); // Less than ideal, we're revamping charge to mean quark charge
      sv->SetMatch(-1); // Necessary for older TStarJetVectors
      TStarJetVector* ppart = (TStarJetVector*) pSavedHardPartons->ConstructedAt( i );
      *ppart = *sv;
      TObjString* saves = (TObjString*) pSavedHardPartonNames->ConstructedAt( i );
      *saves=s.Data();
    }
  }

  
  // Run analysis
  // ------------
  if (pJA){
    delete pJA; pJA=0;
  }
  // 
  if ( pConstituentBackgroundSubtractor ){
    // delete pConstituentBackgroundSubtractor; // TAKEN CARE OFF IN pJA
    pConstituentBackgroundSubtractor =  0;
  }
  if ( pBackgroundSubtractor ){
    // delete pBackgroundSubtractor;// TAKEN CARE OFF IN pJA
    pBackgroundSubtractor =  0;
  }
  rho=0;
  

  // cout << particles.size() << endl; //return true;
  // if ( particles.size()>40 ) {
  //   for (auto&p : particles)    cout << p << endl;
  // }
    
  switch ( pars.SubtractBg ){
  case AREA:
    pJA = new JetAnalyzer( particles, JetDef, AreaDef, select_jet_eta * (!fastjet::SelectorNHardest(2)) ) ;
    pBackgroundSubtractor = pJA->GetBackgroundSubtractor();
    rho = pJA->GetBackgroundEstimator()->rho();
  break;
  case CONSTSUBPRE :
    // estimate the background    
    pJA = new JetAnalyzer( particles, JetDef, AreaDef, select_jet_eta * (!fastjet::SelectorNHardest(2)) ) ;
    pConstituentBackgroundSubtractor =  pJA->GetConstituentBackgroundSubtractor();
    // cout << pConstituentBackgroundSubtractor->description() << endl;

    // if ( ForceRho >=0 ) {
    //   std::cerr << "Can't currently handle ForceRho" << std::endl;
    //   throw(-1);
    //   // BackgroundSubtractor = new fastjet::contrib::ConstituentSubtractor( ForceRho );
    // }

    // Now correct the full event and create a new JetAnalyzer with these
    rho = pJA->GetBackgroundEstimator()->rho();     // save rho first
    particles = pConstituentBackgroundSubtractor->subtract_event(particles, pars.EtaConsCut);
    delete pJA; pJA=0;
    pJA = new JetAnalyzer( particles, JetDef, AreaDef ) ;
    
    break;
  case NONE:
    pJA = new JetAnalyzer( particles, JetDef ) ;
    // return EVENTRESULT::JETSFOUND;
    break;
  default :
    cerr << "Unsupported Background subtraction method" << endl;
    return EVENTRESULT::PROBLEM;
    break;      

  }

  JetAnalyzer& JA = *pJA;      
  vector<PseudoJet> JAResult = sorted_by_pt( select_jet ( JA.inclusive_jets() ) );
  // for ( auto& jet : JAResult ){
  //   if ( fabs(jet.eta())>0.6 ){
  //     cout << jet << endl;
  //   }
  // }
  
  if ( JAResult.size()==0 ) {
    return EVENTRESULT::NOJETS;
  }

  // cout << "-----------------------" << endl;
  // cout << "We have jets. " << endl;
  
  contrib::SoftDrop sd( pars.beta, pars.z_cut);
  if ( pars.CustomRecluster ) {
    sd.set_reclustering(true, recluster);
  }
  
  if ( pars.SubtractBg == AREA) {
    sd.set_subtractor(pBackgroundSubtractor);
    sd.set_input_jet_is_subtracted( false );
  }
  // cout << pBackgroundSubtractor->description() << endl;
  // sd.set_input_jet_is_subtracted( true );
  // sd.set_subtractor( 0 );
  // contrib::SoftDrop::_verbose=true;

  int njets = JAResult.size();
  
  for (unsigned ijet = 0; ijet < JAResult.size(); ijet++) {
    // Run SoftDrop and examine the output
    PseudoJet& CurrentJet = JAResult[ijet];
    PseudoJet sd_jet = sd( CurrentJet );
    // cout << " Grooming Result: " << CurrentJet.pt() << "  --> " << sd_jet.pt() << endl << endl;

    if ( sd_jet == 0){
      // cout <<  " FOREGROUND Original Jet:   " << CurrentJet << endl;
      // if ( pBackgroundSubtractor ) cout <<  " FOREGROUND rho A: " << JA.GetBackgroundEstimator()->rho() * CurrentJet.area() << endl;	  
      // if ( pBackgroundSubtractor ) cout <<  " FOREGROUND Subtracted Jet: " << (*pBackgroundSubtractor)( CurrentJet ) << endl;	  
      // cout << " --- Skipped. Something caused SoftDrop to return 0 ---" << endl;
      continue;
    }
    // 	// Use groomed==original in this case. Not ideal, but probably the best choice.
    if ( pars.SubtractBg == AREA) {
      // cout << "hello subtractor " << ijet << endl;
      CurrentJet = (*pBackgroundSubtractor)( CurrentJet );	
    }
    double zg = sd_jet.structure_of<contrib::SoftDrop>().symmetry();
    //CurrentResult = GroomingResultStruct ( CurrentJet, sd_jet, zg);
    // GroomingResult.push_back ( GroomingResultStruct ( &CurrentJet, &sd_jet, zg) );
    GroomingResult.push_back ( GroomingResultStruct ( CurrentJet, sd_jet, zg) );
    
  }  
  // By default, sort for original jet pt
  sort ( GroomingResult.begin(), GroomingResult.end(), GroomingResultStruct::origptgreater);
  
  return EVENTRESULT::JETSFOUND;
}
//----------------------------------------------------------------------
void InitializeReader(  std::shared_ptr<TStarJetPicoReader> pReader, const TString InputName, const Long64_t NEvents, const int PicoDebugLevel ){

  TStarJetPicoReader& reader = *pReader;
  reader.SetApplyFractionHadronicCorrection(kTRUE);
  reader.SetFractionHadronicCorrection(0.9999);
  reader.SetRejectTowerElectrons( kFALSE );
    
  // Run 11: Use centrality cut
  if ( InputName.Contains("NPE") ){
    TStarJetPicoEventCuts* evCuts = reader.GetEventCuts();
    evCuts->SetReferenceCentralityCut (  6, 8 ); // 6,8 for 0-20%
  }
        
  reader.Init(NEvents);
  TStarJetPicoDefinitions::SetDebugLevel(PicoDebugLevel);
}
//----------------------------------------------------------------------
// Helper to deal with repetitive stuff
shared_ptr<TStarJetPicoReader> SetupReader ( TChain* chain, const PpZgParameters& pars ){
  TStarJetPicoDefinitions::SetDebugLevel(0); // 10 for more output

  shared_ptr<TStarJetPicoReader> pReader = make_shared<TStarJetPicoReader>();
  TStarJetPicoReader& reader = *pReader;
  reader.SetInputChain (chain);

  // Event and track selection
  // -------------------------
  TStarJetPicoEventCuts* evCuts = reader.GetEventCuts();
  evCuts->SetTriggerSelection( pars.TriggerName ); //All, MB, HT, pp, ppHT, ppJP
  // Additional cuts 
  evCuts->SetVertexZCut (pars.VzCut);
  evCuts->SetRefMultCut ( pars.RefMultCut );
  evCuts->SetVertexZDiffCut( pars.VzDiffCut );
  evCuts->SetMaxEventPtCut ( pars.MaxEventPtCut );
  evCuts->SetMaxEventEtCut ( pars.MaxEventEtCut );

  evCuts->SetMinEventEtCut ( pars.MinEventEtCut );

  std::cout << "Using these event cuts:" << std::endl;
  std::cout << " Vz: " << evCuts->GetVertexZCut() << std::endl;
  std::cout << " Refmult: " << evCuts->GetRefMultCutMin() << " -- " << evCuts->GetRefMultCutMax() << std::endl;
  std::cout << " Delta Vz:  " << evCuts->GetVertexZDiffCut() << std::endl;
  std::cout << " MaxEventPt:  " << evCuts->GetMaxEventPtCut() << std::endl;
  std::cout << " MaxEventEt:  " << evCuts->GetMaxEventEtCut() << std::endl;
  
  // This method does NOT WORK for GEANT MC trees because everything is in the tracks...
  // Do it by hand later on, using pars.ManualHtCut;
  // Also doesn't work for general trees, but there it can't be fixed
    
  // // TESTING ONLY:
  // evCuts->SetMaxEventPtCut ( 20000000. );
  // evCuts->SetMaxEventEtCut ( 20000000. );
  // evCuts->SetReferenceCentralityCut (  6, 8 ); // 6,8 for 0-20%
  // evCuts->SetMinEventEtCut ( -1.0 );
  // evCuts->SetMinEventEtCut ( 6.0 );


  // Tracks cuts
  TStarJetPicoTrackCuts* trackCuts = reader.GetTrackCuts();
  trackCuts->SetDCACut( pars.DcaCut );
  trackCuts->SetMinNFitPointsCut( pars.NMinFit );
  trackCuts->SetFitOverMaxPointsCut( pars.FitOverMaxPointsCut );
  trackCuts->SetMaxPtCut ( pars.MaxTrackPt );

  std::cout << "Using these track cuts:" << std::endl;
  std::cout << " dca : " << trackCuts->GetDCACut(  ) << std::endl;
  std::cout << " nfit : " <<   trackCuts->GetMinNFitPointsCut( ) << std::endl;
  std::cout << " nfitratio : " <<   trackCuts->GetFitOverMaxPointsCut( ) << std::endl;
  std::cout << " maxpt : " << trackCuts->GetMaxPtCut (  ) << std::endl;
  
  // Towers
  TStarJetPicoTowerCuts* towerCuts = reader.GetTowerCuts();
  towerCuts->SetMaxEtCut(pars.MaxEtCut);

  std::cout << "Using these tower cuts:" << std::endl;
  std::cout << "  GetMaxEtCut = " << towerCuts->GetMaxEtCut() << std::endl;
  std::cout << "  Gety8PythiaCut = " << towerCuts->Gety8PythiaCut() << std::endl;
  
  // V0s: Turn off
  reader.SetProcessV0s(false);

  // cout << "towerCuts=" << towerCuts << endl;
  // cout << "trackCuts=" << trackCuts << endl;
  return pReader;
}
//----------------------------------------------------------------------
void TurnOffCuts ( std::shared_ptr<TStarJetPicoReader> pReader ){

  pReader->SetProcessTowers ( false );
  TStarJetPicoEventCuts* evCuts = pReader->GetEventCuts();
  evCuts->SetTriggerSelection( "All" ); //All, MB, HT, pp, ppHT, ppJP
  // No Additional cuts -- but keep the Vz cut
  // evCuts->SetVertexZCut ( 30 );
  // evCuts->SetVertexZCut (99999);
  evCuts->SetRefMultCut (0);
  evCuts->SetVertexZDiffCut(999999);
  
  evCuts->SetMaxEventPtCut (99999);
  evCuts->SetMaxEventEtCut (99999);
  evCuts->SetPVRankingCutOff();		//  Use SetPVRankingCutOff() to turn off vertex ranking cut.  default is OFF
  
  // Tracks cuts
  TStarJetPicoTrackCuts* trackCuts = pReader->GetTrackCuts();
  trackCuts->SetDCACut(99999);
  trackCuts->SetMinNFitPointsCut(-1);
  trackCuts->SetFitOverMaxPointsCut(-1);
  trackCuts->SetMaxPtCut (99999);
  
  // Towers: should be no tower in MC. All (charged or neutral) are handled in track
  TStarJetPicoTowerCuts* towerCuts = pReader->GetTowerCuts();
  towerCuts->SetMaxEtCut(99999);

  cout << " TURNED OFF ALL CUTS" << endl;
}
  
  
//----------------------------------------------------------------------
double LookupXsec( TString filename ){
  // Some data for geant
  // -------------------
  //cross-sections for simulated GEANT data sample
  // From Renee.
  // also available via
  // http://people.physics.tamu.edu/sakuma/star/jets/c101121_event_selection/s0150_mclist_001/web.php
  // Double_t MinbXsec=28.12;
  // Double_t Xsec[12];
  // Xsec[0]=28.11;//2
  // Xsec[1]=1.287;//3
  // Xsec[2]=0.3117;//4
  // Xsec[3]=0.1360;//5
  // Xsec[4]=0.02305;//7
  // Xsec[5]=0.005494;//9
  // Xsec[6]=0.002228;//11
  // Xsec[7]=0.0003895;//15
  // Xsec[8]=0.00001016;//25
  // Xsec[9]=0.0000005010;//35
  // Xsec[10]=0.0000000283;//45
  // Xsec[11]=0.000000001443;//55
  
  static const Double_t MinbXsec=28.12;
  // static const Double_t Xsec[12] = {
  //   28.11,		// 2-3
  //   1.287,		// 3-4
  //   0.3117,		// 4-5
  //   0.1360,		// 5-7
  //   0.02305,		// 7-9
  //   0.005494,		// 9-11
  //   0.002228,		// 11-15
  //   0.0003895,		// 15-25
  //   0.00001016,		// 25-35
  //   0.0000005010,	// 35-45
  //   0.0000000283,	// 45-55
  //   0.000000001443	// 55-65
  // };

  static const Double_t Xsec[12] = {
    1.0,        // Placeholder for 2-3
    1.30E+09,	// 3-4
    3.15E+08,	// 4-5
    1.37E+08,	// 5-7
    2.30E+07,	// 7-9
    5.53E+06,	// 9-11
    2.22E+06,	// 11-15
    3.90E+05,	// 15-25
    1.02E+04,	// 25-35
    5.01E+02,	// 35-45
    2.86E+01,	// 45-55
    1.46E+00	// 55-65
  };

  static const Double_t Nmc[12] = {
    1,			// 2-3
    672518,		// 3-4
    672447,		// 4-5
    393498,		// 5-7
    417659,		// 7-9
    412652,		// 9-11
    419030,		// 11-15
    396744,		// 15-25
    399919,		// 25-35
    119995,		// 35-45
    117999,		// 45-55
    119999		// 55-65
  };

  Double_t w[12];
  for ( int i=0; i<12 ; ++i ){
    w[i] = Xsec[i] / Nmc[i];
    // w[i] = Nmc[i] / Xsec[i] ;
  }

  // static const Double_t w[12] = {
  //   1,			// Placeholder
  //   1.90E+03,
  //   6.30E+02,
  //   3.43E+02,
  //   5.49E+01,
  //   1.33E+01,
  //   5.30E+00,
  //   9.81E-01,
  //   2.56E-02,
  //   4.56E-03,
  //   2.43E-04,
  //   1.20E-05
  // };
    
  if ( filename.Contains("picoDst_3_4") ) return w[1];
  if ( filename.Contains("picoDst_4_5") ) return w[2];
  if ( filename.Contains("picoDst_5_7") ) return w[3];
  if ( filename.Contains("picoDst_7_9") ) return w[4];
  if ( filename.Contains("picoDst_9_11") ) return w[5];
  if ( filename.Contains("picoDst_11_15") ) return w[6];
  if ( filename.Contains("picoDst_15_25") ) return w[7];
  if ( filename.Contains("picoDst_25_35") ) return w[8];
  if ( filename.Contains("picoDst_35_45") ) return w[9];
  if ( filename.Contains("picoDst_45_55") ) return w[10];
  if ( filename.Contains("picoDst_55_65") ) return w[11];

  return 1;

}

//----------------------------------------------------------------------
double LookupRun12Xsec( TString filename ){
  
  const int NUMBEROFPT = 11;
  // const char *PTBINS[NUMBEROFPT]={"2_3","3_4","4_5","5_7","7_9","9_11","11_15","15_20","20_25","25_35","35_-1"};
  const static float XSEC[NUMBEROFPT] = {9.00581646, 1.461908221, 0.3544350863, 0.1513760388, 0.02488645725, 0.005845846143, 0.002304880181, 0.000342661835, 4.562988397e-05, 9.738041626e-06, 5.019978175e-07};
  const static float NUMBEROFEVENT[NUMBEROFPT] = {2100295, 600300, 600300, 300289, 300289, 300289, 160295, 100302, 80293, 76303, 23307};

  const static vector<string> vptbins={"pp12Pico_pt2_3","pp12Pico_pt3_4","pp12Pico_pt4_5","pp12Pico_pt5_7","pp12Pico_pt7_9","pp12Pico_pt9_11","pp12Pico_pt11_15","pp12Pico_pt15_20","pp12Pico_pt20_25","pp12Pico_pt25_35","35_-1"};
  for ( int i=0; i<vptbins.size(); ++i ){
    if ( filename.Contains(vptbins.at(i).data())) return XSEC[i] / NUMBEROFEVENT[i];
  }

  throw std::runtime_error("Not a valid filename");
  return -1;
  
}

//----------------------------------------------------------------------
/** 
    convenient output
*/
ostream & operator<<(ostream & ostr, const PseudoJet& jet) {
  if (jet == 0) {
    ostr << " 0 ";
  } else {
    ostr << " pt = " << jet.pt()
         << " m = " << jet.m()
         << " y = " << jet.rap()
         << " phi = " << jet.phi()
         << " ClusSeq = " << (jet.has_associated_cs() ? "yes" : "no");
  }
  return ostr;
}
//----------------------------------------------------------------------
