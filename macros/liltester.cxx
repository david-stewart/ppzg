#include <TFile.h>
#include <TSystem.h>
#include <TChain.h>
#include <TTree.h>
#include <TLeaf.h>

#include <map>
#include <iostream>
#include <climits>
using namespace std;

typedef map< pair<int,int>,int> mpi;

int liltester()
{
  TChain * JetTree = new TChain("JetTree");
  TChain * JetTreeMc = new TChain("JetTreeMc");

  // TString s = "Data/AddedGeantPythia/picoDst_15_25_*.root";
  TString s = "Data/AddedGeantPythia/picoDst_*.root";
  JetTree->Add( s );
  JetTreeMc->Add( s );  

  if ( JetTree->GetEntries() != JetTreeMc->GetEntries() ){
    cerr << "Already in trouble." << endl;
    return -1;
  }

  int eventid;
  int runid;

  int mceventid;
  int mcrunid;

  mpi m2;
  pair<int,int> p;

  int localcounter=0;
  int lasttree=-1;
  for ( Long64_t mcEvi = 0; mcEvi< JetTreeMc->GetEntries() ; ++mcEvi ){
    if ( !(mcEvi%10000) ) cout << "Working on " << mcEvi << " / " << JetTreeMc->GetEntries() << endl;
    JetTreeMc->GetEntry(mcEvi);
    JetTree->GetEntry(mcEvi);
    
    mceventid = JetTree->GetLeaf("fEventHeader.fEventId")->GetValue();
    mcrunid = JetTree->GetLeaf("fEventHeader.fRunId")->GetValue();
    eventid = JetTree->GetLeaf("fEventHeader.fEventId")->GetValue();
    runid = JetTree->GetLeaf("fEventHeader.fRunId")->GetValue();    

    if ( mceventid != eventid ){
      cerr << "Trouble. mceventid = " << mceventid << "  eventid = " << eventid<< endl;
      // return;
    }

    int currenttree = JetTree->GetTreeNumber();
    if ( currenttree!=lasttree ){
      localcounter=0;
      lasttree=currenttree;
    } else {
      ++localcounter;
    }
    
    TString cname = gSystem->BaseName( JetTree->GetFile()->GetName());
    UInt_t filehash = cname.Hash();
    while ( filehash > INT_MAX - 100000 ) filehash -= INT_MAX / 4; // some random large number
    if ( filehash < 1000000 ) filehash += 1000001;
    runid = filehash;
    // Sigh. Apparently also need to uniquefy the event id
    // since some are the same in the same file. Grr.
    eventid = localcounter;

    // if ( !(mcEvi%10000) ) cout << runid << "  " << eventid << endl;
    p.first=runid;
    p.second=eventid;
    m2[p]++;

  }
    
  cout << "Testing " << m2.size() << " entries." << endl;
  int mcEvi=0;
  for ( mpi::iterator it=m2.begin(); it!=m2.end(); it++ ){
    if ( it->second != 1 ){
      cout << "ALARUM WITHIN! " << it->first.first << "  " << it->first.second << "  " << it->second << endl;
    }
    if ( !(mcEvi%10000) ) cout << "Working on " << mcEvi << " / " << JetTreeMc->GetEntries() << "  --> " << it->first.first << "  " << it->first.second << "  " << it->second << endl;
    mcEvi++;  
  }

  return 0;
}
