#include<TTree.h>
#include<TString.h>

int AfterAddCleanup( TString in ="Data/AddedEmbedPythiaRun12pp200/pp12Pico_pt2_3_g0.root",
		     TString out="Data/AddedEmbedPythiaRun12pp200/Cleanpp12Pico_pt2_3_g0.root"
		     )
{
  TFile* fin = new TFile( in, "READ");
  TFile* fout = new TFile( out, "RECREATE");
  TTree* JetTree = ((TTree*) fin->Get("JetTree"))->CloneTree();
  TTree* JetTreeMc = ((TTree*) fin->Get("JetTreeMc"))->CloneTree();
  fout->cd();
  JetTree->Write();
  JetTreeMc->Write();
  
  return 0;
}
