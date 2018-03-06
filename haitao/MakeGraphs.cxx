#include<iostream>
#include<fstream>
#include<vector>

// Hai Tao Li, Ivan Vitev. Dec 29, 2017. 5 pp.
// e-Print: arXiv:1801.00008 [hep-ph] | PDF

int MakeGraphs()
{

  double x,u,l;
  double v,e;

  // leading ratio
  // -------------
  vector<double> rlead_zg;
  vector<double> rlead_v;
  vector<double> rlead_e;
  ifstream frlead ("light_zg_ratio_leading_jet.dat");
  
  // The format is  {zg,  upper value of the band, lower value of the band}.    
  while (frlead.good()) {
    frlead >> x >> u >> l;
    v = 0.5*(u+l);
    e = u - v;    
    rlead_zg.push_back(x);
    rlead_v.push_back(v);
    rlead_e.push_back(e);
  }
  TGraphErrors* rlead = new TGraphErrors ( rlead_zg.size(), &rlead_zg.front(), &rlead_v.front(),0,&rlead_e.front());
  
  // subleading ratio
  // -------------
  vector<double> rsublead_zg;
  vector<double> rsublead_v;
  vector<double> rsublead_e;
  ifstream frsublead ("light_zg_ratio_subleading_jet.dat");
  
  // The format is  {zg,  upper value of the band, lower value of the band}.    
  while (frsublead.good()) {
    frsublead >> x >> u >> l;
    v = 0.5*(u+l);
    e = u - v;    
    rsublead_zg.push_back(x);
    rsublead_v.push_back(v);
    rsublead_e.push_back(e);
  }
  TGraphErrors* rsublead = new TGraphErrors ( rsublead_zg.size(), &rsublead_zg.front(), &rsublead_v.front(),0,&rsublead_e.front());
  
  //  MLL zg distribution for the subleading jet
  // -------------------------------------------
  vector<double> rdist_zg;
  vector<double> rdist_v;
  vector<double> rdist_e;
  ifstream frdist ("light_zg_distributions_Medium.dat");
  
  // The format is  {zg,  upper value of the band, lower value of the band}.    
  while (frdist.good()) {
    frdist >> x >> u >> l;
    v = 0.5*(u+l);
    e = u - v;    
    rdist_zg.push_back(x);
    rdist_v.push_back(v);
    rdist_e.push_back(e);
  }
  TGraphErrors* rdist = new TGraphErrors ( rdist_zg.size(), &rdist_zg.front(), &rdist_v.front(),0,&rdist_e.front());
    
  rlead->Draw("APL");
  rsublead->Draw("*same");

  new TCanvas;
  rdist->Draw("APL");
  // rlead->Draw("Ae3");
  
  TFile * fout = new TFile( "li_vitev.root", "RECREATE");
  rlead->Write("rlead");
  rsublead->Write("rsublead");
  rdist->Write("rdist");

  return 0;
}
