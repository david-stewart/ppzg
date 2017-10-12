#include <TLorentzVector.h>
#include <TSystem.h>
#include <TClonesArray.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TBranch.h>
#include <TMath.h>
#include <TRandom.h>
#include <TParameter.h>
#include "TString.h"
#include "TObjString.h"

#include "TStarJetPicoReader.h"
#include "TStarJetPicoEvent.h"
#include "TStarJetPicoEventHeader.h"
#include "TStarJetPicoEventCuts.h"

#include "TStarJetPicoPrimaryTrack.h"
#include "TStarJetPicoTrackCuts.h"
#include "TStarJetPicoTowerCuts.h"
#include "TStarJetPicoTower.h"

#include "TStarJetVectorContainer.h"
#include "TStarJetVector.h"
#include "TStarJetPicoTriggerInfo.h"
#include "TStarJetPicoUtils.h"


#include<vector>
#include<algorithm>


using namespace std;

// int Pp12qa(TString fname="Data/ppJP2Run12/sum8.root")
int Pp12qa(TString fname="Data/ppJP2Run12/sum*.root")
{
  TChain * JetTree = new TChain("JetTree");
  JetTree->Add( fname );

  // // To read and deal with TStarJetPicoDst's
  // Int_t ierr = 0;
  // ierr = gSystem->Load("/nfs/rhi/STAR/software/eventStructuredAu/libTStarJetPico.so");
  // if ( ierr != 0) {
  //   cerr <<  "Unable to load " << ierr << endl;
  // }
  

  // 360   1.92719
  // 493   2.07367
  // 779   1.988
  // 1284   2.59772
  // 1306   2.84076
  // 1337   3.03485
  // 1438   1.95164
  // 1709   2.2092
  // 2027   3.21105
  // 2445   2.50519
  // 3407   4.97214
  // 3720   2.31654
  // 4217   1.98978
  // 4288   1.94671

    
  vector<int> questionable;
  questionable.push_back(360);
  questionable.push_back(493);
  questionable.push_back(779);
  questionable.push_back(1284);
  questionable.push_back(1306);
  questionable.push_back(1337);
  questionable.push_back(1438);
  questionable.push_back(1709);
  questionable.push_back(2027);
  questionable.push_back(2445);
  questionable.push_back(3407);
  questionable.push_back(3720);
  questionable.push_back(4217);
  questionable.push_back(4288);

  vector<int> reference;
  reference.push_back(1);
  reference.push_back(500);
  reference.push_back(1000);
  reference.push_back(1500);
  reference.push_back(2000);
  reference.push_back(2500);
  reference.push_back(3500);

  TFile* fout = new TFile("ETspectra.root","RECREATE");
  TH1D** hq = new TH1D*[questionable.size()];
  TH1D** hr = new TH1D*[reference.size()];

  for (int i=0; i<questionable.size(); ++i ){
    // cout << i << endl;
    TString name = "hq"; name+=i;
    TString title = "tower "; title+=questionable.at(i);
    hq[i] = new TH1D( name, title, 80, 0, 40);
    // cout << hq[i]->GetTitle() << endl;
  }

  for (int i=0; i<reference.size(); ++i ){
    TString name = "hr"; name+=i;
    TString title = "tower "; title+=reference.at(i);
    hr[i] = new TH1D( name, title, 80, 0, 40);
  }


  TBranch*           fBranch = new TBranch(); //! input branch    
  TStarJetPicoEvent* fEvent = new TStarJetPicoEvent(); //! input event

  fBranch = JetTree->GetBranch("PicoJetTree");
  fBranch->SetAddress(&fEvent);
  JetTree->SetBranchAddress("PicoJetTree", &fEvent, &fBranch);

  int id;
  TStarJetPicoTower *ptower;
  Long64_t NEvents=JetTree->GetEntries();
  // NEvents=100000;

  for (int ev=0; ev<NEvents; ++ev){
    JetTree->GetEntry(ev);
    if ( !(ev%50000) ) cout << "Working on " << ev << " / " << NEvents << endl;
    
    for (Int_t ntower = 0; ntower < fEvent->GetHeader()->GetNOfTowers(); ntower++) { 
      ptower = fEvent->GetTower(ntower);
      id = ptower->GetId();

      vector<int>::iterator qit = std::find(questionable.begin(), questionable.end(), id);
      // ptrdiff_t qit = std::find(questionable.begin(), questionable.end(), id);
      if ( qit != questionable.end()){
	int index = qit -questionable.begin();
	// cout << id << "  " << questionable.at(index) << endl;
	hq[index]->Fill( ptower->GetEt() );
      }

      vector<int>::iterator rit = std::find(reference.begin(), reference.end(), id);
      // ptrdiff_t rit = std::find(reference.begin(), reference.end(), id);
      if ( rit != reference.end()){
	int index = rit -reference.begin();
	// cout << id << "  " << reference.at(index) << endl;
	hr[index]->Fill( ptower->GetEt() );
      }

    }

    // cout << t->GetEntries() << endl;
  }
		   
  // JetTree->Draw("fTowers.GetEt():fTowers.fId>>h2","","goff");
  new TCanvas;
  gPad->SetLogy();
  gPad->SaveAs( "questionable.pdf[");
  for (int i=0; i<questionable.size(); ++i ){
    hq[i]->Draw();
    gPad->SaveAs( "questionable.pdf");
    // if (i==0)    gPad->SaveAs( "questionable.pdf(");
    // else if (i==questionable.size()-1)    gPad->SaveAs( "questionable.pdf)");
    // else gPad->SaveAs( "questionable.pdf");
  }
  gPad->SaveAs( "questionable.pdf]");

  for (int i=0; i<reference.size(); ++i ){
    hr[i]->Draw();
    if (i==0)    gPad->SaveAs( "reference.pdf(");
    else if (i==reference.size()-1)    gPad->SaveAs( "reference.pdf)");
    else gPad->SaveAs( "reference.pdf");
  }

  fout->Write();
  return 0;
}
