#include <vector>

#define REBIN 1

TH1D* minmax ( vector<TFile*> files, TString which, TString namehelper,  vector<TH1D*>& histos, 
	       vector<TH1D*>& ratios, vector<TH1D*>& minandmax );


  
int PrepSystematics( TString R="0.4"  )
{

  // --- Latest Run6 ---
  // TString Base = "Results/Unfolded_ForUnfolding_Recut_Pp_HT54_NoEff_NoBgSub__With_AEff0_PtSmear0_ATow0_SystGeant_NoEff_NoBg_HT54_WithMisses_WithFakes_TrainedWith_Recut_McGeant_NoEff_NoBg_MB.root";  

  // --- Run 12 ---
  TString Base = "Results/Unfolded_ForUnfolding_ForPaper_Pp12_JP2_NoEff_NoBgSub__With_AEff0_PtSmear0_ATow0_SystGeant12_NoEff_NoBg_JP2_WithMisses_WithFakes_TrainedWith_McGeant12_NoEff_NoBg_all.root";  

  // Unrebinned versions
  // --- Latest Run6 ---
  // TString Base = "Results/Unfolded_NoRebin_ForUnfolding_Recut_Pp_HT54_NoEff_NoBgSub__With_AEff0_PtSmear0_ATow0_SystGeant_NoEff_NoBg_HT54_WithMisses_WithFakes_TrainedWith_Recut_McGeant_NoEff_NoBg_MB.root";  

  // --- Run 12 ---
  // TString Base = "Results/Unfolded_NoRebin_ForUnfolding_ForPaper_Pp12_JP2_NoEff_NoBgSub__With_AEff0_PtSmear0_ATow0_SystGeant12_NoEff_NoBg_JP2_WithMisses_WithFakes_TrainedWith_McGeant12_NoEff_NoBg_all.root";  

  TString outname = Base;
  outname.ReplaceAll("Results/Unfolded_ForUnfolding_","Results/TotalzgSystematics_");
  outname.ReplaceAll("Results/Unfolded_NoRebin_ForUnfolding_","Results/TotalzgSystematics_NoRebin_");  
  outname.ReplaceAll("__With_AEff0_PtSmear1_ATow0","_Using");
  outname.ReplaceAll("_TrainedWith_McGeant12_NoEff_NoBg_all","");

  TString fe0r0t0 = Base;

  TString s;
  s = Base; s.ReplaceAll("AEff0_PtSmear0_ATow0","AEff-1_PtSmear0_ATow0");
  TString feMr0t0 = s;

  s = Base; s.ReplaceAll("AEff0_PtSmear0_ATow0","AEff0_PtSmear1_ATow0");
  TString fe0rSt0 = s;

  s = Base; s.ReplaceAll("AEff0_PtSmear0_ATow0","AEff0_PtSmear0_ATow-1");
  TString fe0r0tM = s;

  s = Base; s.ReplaceAll("AEff0_PtSmear0_ATow0","AEff0_PtSmear0_ATow1");
  TString fe0r0tP = s;

  s = Base;
  s.ReplaceAll("Pp12_HT54_JP2_","Pp12_HT54_JP2_MIP_");
  s.ReplaceAll("Pp12_JP2_","Pp12_JP2_MIP_");
  s.ReplaceAll("SystGeant12_","SystGeant12_MIP_");  
    
  s.ReplaceAll("Pp_HT54_","Pp_HT54_MIP_");
  s.ReplaceAll("SystGeant_","SystGeant_MIP_");  
  TString fHC = s;
  
  vector<TFile*> Efiles;
  Efiles.push_back(new TFile( fe0r0t0, "READ" ) );
  Efiles.push_back(new TFile( feMr0t0, "READ" ) );

  bool UsePtMinMax=false;
  vector<TFile*> Rfiles;
  Rfiles.push_back(new TFile( fe0r0t0, "READ" ) );
  Rfiles.push_back(new TFile( fe0rSt0, "READ" ) );

  // if ( UsePtMinMax ){
  //   // Option 1: Min/Max
  //   s = Base; s.ReplaceAll("AEff0_PtRes0_ATow0_","AEff0_PtRes-1_ATow0_");
  //   TString fe0rMt0 = s;
  //   s = Base; s.ReplaceAll("AEff0_PtRes0_ATow0_","AEff0_PtRes0_ATow1_");
  //   TString fe0r0tP = s;
  //   Rfiles.push_back(new TFile( fe0r0t0, "READ" ) );
  //   Rfiles.push_back(new TFile( fe0rMt0, "READ" ) );
  //   Rfiles.push_back(new TFile( fe0rPt0, "READ" ) );
  // } else {
  //   // Option 2: Smear
  //   // outname.ReplaceAll("TotalzgSystematics_","TotalzgSystematics_PtSmear_");
  //   s = Base; s.ReplaceAll("AEff0_PtRes0_ATow0_Syst","AEff0_PtRes10_ATow0_PtSmearSyst");
  //   TString fe0rSt0 = s;
  //   Rfiles.push_back(new TFile( fe0r0t0, "READ" ) );
  //   Rfiles.push_back(new TFile( fe0rSt0, "READ" ) );
  // }
 
  vector<TFile*> Tfiles;
  Tfiles.push_back(new TFile( fe0r0t0, "READ" ) );
  Tfiles.push_back(new TFile( fe0r0tM, "READ" ) );
  Tfiles.push_back(new TFile( fe0r0tP, "READ" ) );
    
  vector<TFile*> Hfiles;
  Hfiles.push_back(new TFile( fe0r0t0, "READ" ) );
  Hfiles.push_back(new TFile( fHC, "READ" ) );


  vector<TH1D*> Ehistos;
  vector<TH1D*> Rhistos;
  vector<TH1D*> Thistos;
  vector<TH1D*> Hhistos;
  // vector<TH1D*> AChistos;
  vector<TH1D*> ratios;
  vector<TH1D*> minandmax;

  bool visualize=true;
  if (visualize){
    TCanvas* c = new TCanvas("c");
    gPad->SetGridx(0);  gPad->SetGridy(0);
    gStyle->SetOptStat(0);
    gStyle->SetOptDate(0);    
  }
  TString plotname;
    
  TH1D* h_E=0;
  TH1D* h_R=0;
  TH1D* h_T=0;
  TH1D* h_H=0;
  TString which;  

  // Open outputfile
  // ---------------
  TFile* out = new TFile( outname, "RECREATE");

  vector<string> whiches;
  whiches.push_back ( "Incunfold_1015" );
  whiches.push_back ( "Incunfold_1520" );
  whiches.push_back ( "Incunfold_2025" );
  whiches.push_back ( "Incunfold_2530" );
  whiches.push_back ( "Incunfold_3040" );
  whiches.push_back ( "Incunfold_4060" );
  TString itmodifier="_2";
  
  for (int w = 0; w<whiches.size(); ++w ){
    which = whiches.at(w);
    cout << which << endl;

    // Forward values and unfolding systematics
    // Assume that 0th file in any list is the default    
    ((TH1*)Efiles.at(0)->Get(which+itmodifier))->SetDirectory(gDirectory);
    ((TH1*)Efiles.at(0)->Get("Sys"+which))->SetDirectory(gDirectory);

    // Now append the desired/chosen default iteration
    which+=itmodifier;
    

    // Tracking Efficiency
    h_E = minmax (  Efiles, which, "E", Ehistos, ratios, minandmax );
    h_E->SetName( which + "_E" );
    if (gPad){
      plotname = "plots/" + R + "_" + h_E->GetName() + ".png";
      gPad->SaveAs( plotname );
    }
    
    // Tracking Resolution
    // TODO: Do we really want this from smearing?
    // Tower gain is essentially the same thing, yet we treat it as a straight up/down
    h_R = minmax (  Rfiles, which, "R", Rhistos, ratios, minandmax );
    h_R->SetName( which + "_R" );
    if (gPad){
      plotname = "plots/" + R + "_" + h_R->GetName() + ".png";
      gPad->SaveAs( plotname );
    }

    // Tower Gain
    h_T = minmax (  Tfiles, which, "T", Thistos, ratios, minandmax );
    h_T->SetName( which + "_T" );
    if (gPad){
      plotname = "plots/" + R + "_" + h_T->GetName() + ".png";
      gPad->SaveAs( plotname );
    }

    // Hadronic Correction
    h_H = minmax (  Hfiles, which, "H", Hhistos, ratios, minandmax );
    h_H->SetName( which + "_H" );
    if (gPad){
      plotname = "plots/" + R + "_" + h_H->GetName() + ".png";
      gPad->SaveAs( plotname );
    }

    TH1D* h_minmax = h_H->Clone(which + "_minmax");
    TH1D* h_min = h_H->Clone(which + "_min");
    TH1D* h_max = h_H->Clone(which + "_max");
    
    TH1D* h_minmax_ratio = h_H->Clone(which + "_minmax_ratio");
    h_minmax_ratio->Reset();
    
    for (int i=1; i<=h_minmax->GetNbinsX() ; ++ i ){
      h_minmax->SetBinContent (i, Thistos.at(0)->GetBinContent(i) );
      
      // h_minmax->SetBinError (i, sqrt( pow( h_E->GetBinError(i), 2) +
      // 				      pow( h_R->GetBinError(i), 2) + 
      // 			              pow( h_T->GetBinError(i), 2)  )
      // 			     );
      h_minmax->SetBinError (i, sqrt( pow( h_E->GetBinError(i), 2) +
				      pow( h_R->GetBinError(i), 2) + 
			              pow( h_T->GetBinError(i), 2) +
			              pow( h_H->GetBinError(i), 2)  )
			     );

      if ( h_minmax->GetBinError(i)>0 ){
	h_minmax_ratio->SetBinContent (i, h_minmax->GetBinError(i) / h_minmax->GetBinContent(i) );
      }
      h_minmax_ratio->SetAxisRange(0.1+0.001,0.5 - 0.001,"x");
      h_minmax_ratio->SetAxisRange(0,0.1,"y");
  
	    
      
      h_min->SetBinContent (i, Thistos.at(0)->GetBinContent(i) - h_minmax->GetBinError (i) );
      h_min->SetBinError   (i, Thistos.at(0)->GetBinError(i) );
      
      h_max->SetBinContent (i, Thistos.at(0)->GetBinContent(i) + h_minmax->GetBinError (i) );
      h_max->SetBinError   (i, Thistos.at(0)->GetBinError(i) );
      
    }

    s = Thistos.at(0)->GetName();
    s.ReplaceAll( "T0","nom");
    TH1D* h_nom=Thistos.at(0)->Clone( s );
    h_nom->SetDirectory(0); // Already saved it
    h_minmax->SetMarkerSize(0);
    h_minmax->SetLineWidth( 0 );
    h_minmax->SetFillStyle(1001);
    h_minmax->SetFillColor( kGray );
    h_minmax->SetMarkerColor( kGray );

    h_nom->Draw("9");
    h_minmax->Draw("9E2same");
    // h_minmax->Draw("9E2");
    h_nom->Draw("9same");
    plotname = "plots/" + R + "_" + h_minmax->GetName() + ".png";
    gPad->SaveAs( plotname );

    h_minmax_ratio->SetAxisRange( 0.1,0.5,"x");
    h_minmax_ratio->SetAxisRange( -0.05,0.25,"y");
    h_minmax_ratio->Draw("9");
    plotname = "plots/" + R + "_" + h_minmax_ratio->GetName() + ".png";    
    gPad->SaveAs( plotname );



  }
    
  // Done. Save
  // ----------
  out->Write();  

  cout << "Wrote to " << endl << out->GetName() << endl;
  return 0;
  
}



// ===========================================================================
TH1D* minmax ( vector<TFile*> files, TString which, TString namehelper,  vector<TH1D*>& histos,
	       vector<TH1D*>& ratios, vector<TH1D*>& minandmax ){

  histos.clear();  
  TFile* file;
  TH1D* h1;
  
  for ( int f=0; f<files.size() ; ++f ){
    cout << "trying" << endl;
    file = files.at(f);
    cout << file->GetName() << endl;
    cout << which << endl;
    h1 = (TH1D*) file->Get(which);
    
    h1->Rebin(REBIN);
    cout << "Ok" << endl;
    // h1->Scale(1./REBIN);
    // h1->Scale(1./h1->Integral());
    // h1->Scale(1./h1->Integral(0, h1->GetNbinsX()+1)); // Include over/underflow

    histos.push_back( h1 );		      
  }

  
  TH1D* ret = histos.at(0)->Clone( which+"_minmax" );
  ret->Reset();

  TH1D* hmin = histos.at(0)->Clone( TString(histos.at(0)->GetName())+"_min" );
  hmin->Reset();
  hmin->SetDirectory(0);
    
  TH1D* hmax = histos.at(0)->Clone( TString(histos.at(0)->GetName())+"_max" );
  hmax->Reset();
  hmax->SetDirectory(0);

  double min, max;
  for (int i=1; i<=ret->GetNbinsX() ; ++ i ){
    min = 1e15;
    max = -1e15;
    
    for ( int j=0; j<histos.size() ; ++j ){
      if ( histos.at(j)->GetBinContent(i) < min ) min = histos.at(j)->GetBinContent(i);
      if ( histos.at(j)->GetBinContent(i) > max ) max = histos.at(j)->GetBinContent(i);
    }

    ret->SetBinContent( i, 0.5 * ( max+min ) );
    ret->SetBinError( i, 0.5 * ( max-min ) );   
    
    hmin->SetBinContent( i, min );
    hmin->SetBinError( i, histos.at(0)->GetBinError(i) );   

    hmax->SetBinContent( i, max );
    hmax->SetBinError( i, histos.at(0)->GetBinError(i) );       
  }

  minandmax.push_back(hmin);
  minandmax.push_back(hmax);

  // ratios. Assuming 0 is nominal
  // -----------------------------
  TH1D* nom = histos.at(0)->Clone( TString(histos.at(0)->GetName()) + "_nom");
  nom->SetDirectory(0);
  
  TH1D* ratio1 = histos.at(1)->Clone( TString(histos.at(1)->GetName()) + "_ratio");
  ratio1->SetDirectory(0);
  ratio1->GetYaxis()->SetTitle("ratio to nominal");
  ratio1->Divide( nom);
  ratios.push_back(ratio1);

  if ( files.size()==3 ){
    TH1D* ratio2 = histos.at(2)->Clone( TString(histos.at(2)->GetName()) + "_ratio");
    ratio2->SetDirectory(0);
    ratio2->GetYaxis()->SetTitle("ratio to nominal");
    ratio2->Divide( nom);
    ratios.push_back(ratio2);
  }

  TH1D* ratio = ret->Clone( TString(ret->GetName()) + "_" + namehelper + "_ratio");
  // ratio->SetDirectory(0);
  ratio->Reset();
  for (int i=1; i<=ratio->GetNbinsX() ; ++ i ){
    if ( ret->GetBinError(i)>0 ){
      ratio->SetBinContent (i, ret->GetBinError(i) / nom->GetBinContent(i) );
    }
  }
  ratio->GetYaxis()->SetTitle("ratio to nominal");
  ratio->SetAxisRange(0.1+0.001,0.5 - 0.001,"x");
  ratio->SetAxisRange(0,0.1,"y");
  ratios.push_back(ratio);

  float yaxismin = -0.5;
  float yaxismax = 7;
  // float ymax = 0.12*REBIN;
  if ( gPad ){      
    // histos.at(0)->SetTitle( "");
    // histos.at(0)->SetAxisRange(  0.0, 0.23, "y");
    // histos.at(0)->SetAxisRange(  0.0, 0.72, "x");
    // histos.at(0)->SetAxisRange( 0.0, ymax, "y");
    histos.at(0)->SetAxisRange( yaxismin, yaxismax, "y");
    // histos.at(0)->SetAxisRange(  0.05, 0.5, "x");
    histos.at(0)->SetAxisRange(  0.1, 0.5, "x");
    histos.at(0)->Draw();

    histos.at(1)->SetLineWidth(2);
    histos.at(1)->SetLineColor(kBlue);
    histos.at(1)->Draw("same");
    
    if ( files.size()==3 ){
      histos.at(2)->SetLineWidth(2);
      histos.at(2)->SetLineColor(kMagenta+1);
      histos.at(2)->Draw("same");
    }
    
    ret->SetFillStyle(3001 );
    ret->SetFillColor( kGray+1 );  
    
    ret->Draw("9E2same");

    TLegend* leg = new TLegend ( 0.6, 0.6, 0.88, 0.88 );
    leg->SetBorderSize(0);
    leg->SetLineWidth(10);
    leg->SetFillStyle(0);
    // leg->SetMargin(0.1);

    leg->AddEntry(histos.at(0)->GetName(), "Nominal", "l");
    if ( namehelper.Contains("T") ){
      leg->AddEntry(histos.at(1)->GetName(), "Tow Scale Max", "l");
      leg->AddEntry(histos.at(2)->GetName(), "Tow Scale Min", "l");
    } else if ( namehelper.Contains("R") ){
      if ( histos.size() == 3 ){
	leg->AddEntry(histos.at(1)->GetName(), "Track Res Max", "l");
	leg->AddEntry(histos.at(2)->GetName(), "Track Res Min", "l");
      } else {
	leg->AddEntry(histos.at(1)->GetName(), "Track Res Smeared", "l");
      }
    } else if ( namehelper.Contains("H") ){
      leg->AddEntry(histos.at(1)->GetName(), "MIP Correction", "l");
    } else {
      leg->AddEntry(histos.at(1)->GetName(), "Track Eff Min", "l");
    }
    
    leg->AddEntry(ret->GetName(), "Uncertainty", "f");
    leg->Draw("same");
    // cout << histos.at(0)->GetName() << endl;
  }
  return ret;

}

