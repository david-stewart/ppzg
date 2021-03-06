#include "ZgPaperConsts.hxx"

float ls = 20;
float ts = 20;

void SetupHisto ( TH1* hFrame,
		  float xFactor, float yFactor,
		  float xloffset, float xtoffset,
		  float xtickbase, int ndivx,
		  float yloffset, float ytoffset,
		  float ytickbase, int ndivy
		  );

int FigDeltaZg( ){
  
  TString infilename="Results/AEff0_PtSmear0_ATow0_SystGeant12_NoEff_NoBg_JP2_WithMisses_WithFakes_TrainedWith_McGeant12_NoEff_NoBg_all.root";
  TFile* fin = 0;
  if ( gSystem->FindFile(".",infilename) ){
    fin = new TFile( infilename, "READ");
  } 

  TString plotpath="./figs/";

  int rebin=5*3; // 5*5*3*3
  
  gStyle->SetHistLineWidth(2);
  gStyle->SetFrameLineWidth(2);
  gStyle->SetOptStat(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);

  gStyle->SetTitleX(0.1f);
  gStyle->SetTitleW(0.8f);
  gStyle->SetTitleBorderSize(0);	//Title box border thickness


  // Good for 400,750 divided in two
  float lMargin = 0.12;
  float rMargin = 0.05;
  float bMargin = 0.12;
  float tMargin = 0.05;
  float xloffset=0.02;
  float xtoffset=1.3;
  int   ndivx= 505; 
  float yloffset=0.02;
  float ytoffset=1.3;
  int   ndivy= 505; 
  float xtickbase = 0.04;
  float ytickbase = 0.04;

  float legs = 20;
  // TCanvas* C =new TCanvas("C","", 400,750);
  TCanvas* C =new TCanvas("C");
  
  gPad->SetLogz();
  gPad->SetLeftMargin(lMargin);
  gPad->SetRightMargin(rMargin);
  gPad->SetTopMargin(tMargin);
  gPad->SetBottomMargin(bMargin);
  
  // Load 2d histos, make 1D histos
  // ------------------------------
  TH2D* DeltaZgvsPt;
  if ( fin ){
    DeltaZgvsPt = (TH2D*) fin->Get("DeltaZgvsPt");
    DeltaZgvsPt->SaveAs("DeltaZgvsPt.root");
  } else {
    TFile* f3 = new TFile ( "DeltaZgvsPt.root");
    DeltaZgvsPt = (TH2D*) f3->Get("DeltaZgvsPt");
  }
    
  TObjArray pphistos( 6 );
  // TObjArray geanthistos( 6 );

  TLegend* leg;
  TString s;
  leg = new TLegend( 0.6, 0.6, 0.95, 0.95 );
  leg->SetBorderSize(0);
  leg->SetTextFont(43);
  leg->SetTextSize(legs);  
  leg->SetFillStyle(0);

  for ( int i=0 ; i<zgconsts::nIncbins ; ++i ){
    float ptleft  = zgconsts::Incbins[i];
    float ptright = zgconsts::Incbins[i+1];

    TString name = "deltaZg_"; name += int(ptleft+0.01); name += int(ptright+0.01);
    TH1D* deltaZg =
      (TH1D*) DeltaZgvsPt->ProjectionY(name, DeltaZgvsPt->GetXaxis()->FindBin( ptleft+0.01), DeltaZgvsPt->GetXaxis()->FindBin( ptright-0.01));

    TH1D* h=deltaZg;
    switch ( i ){
    case 0:
    case 1:
      // h->Rebin(1);
      break;
    case 2:
    case 3:
      // h->Rebin(2);
      break;
    case 4:
    case 5:
      // h->Rebin(4);
      break;
    default :
      cerr << "Unsupported number of histos" << endl;
      return -1;
      break; // ROOT can be stupid
    }
    h->Rebin(rebin);
    h->Scale ( 1./ h->Integral(1,h->GetNbinsX(),"width") );
    h->SetAxisRange(-0.5,0.5,"x");
    h->SetAxisRange(0,6,"y");

    pphistos.Add( h );
    s="p_{T}^{Det}="; s+=int(ptleft); s+="-"; s+=int(ptright); s+=" GeV/c";
    leg->AddEntry(h, s,"lp");

    h->SetTitle (";#Deltaz_{g} = z_{g}^{Det}-z_{g}^{Part};1/N dN / d#Deltaz_{g}");
    h->SetMarkerStyle( zgconsts::datamark[i] );
    h->SetMarkerSize( 2*zgconsts::datamarksize[i] );
    h->SetMarkerColor( zgconsts::datacol[i] );
    h->SetLineColor( zgconsts::datacol[i] );
    h->SetLineWidth( 3 );

    SetupHisto ( h,
		 1, 1,
		 xloffset, xtoffset,
		 xtickbase, ndivx,
		 yloffset, ytoffset,
		 ytickbase, ndivy );
    if ( i==0 ) h->Draw("9");
    else h->Draw("9same");
    h->Draw("9histsame");
    // if ( i==0 ) h->Draw("9hist");
    // else h->Draw("9histsame");
  }

  // pphistos.At(0)->Draw("9histsame");
  leg->Draw();
  
  C->SaveAs(plotpath + "FigDeltaZg.pdf");

  return 0;
}
// -----------------------------------------------------------------------------
void SetupHisto ( TH1* hFrame,
		  float xFactor, float yFactor,
		  float xloffset, float xtoffset,
		  float xtickbase, int ndivx,
		  float yloffset, float ytoffset,
		  float ytickbase, int ndivy
		  )
{
  
  // Format for x axis
  hFrame->GetXaxis()->SetLabelFont(43);
  hFrame->GetXaxis()->SetLabelSize(ls);
  hFrame->GetXaxis()->SetLabelOffset(xloffset);
  hFrame->GetXaxis()->SetTitleFont(43);
  hFrame->GetXaxis()->SetTitleSize(ts);
  hFrame->GetXaxis()->SetTitleOffset(xtoffset);
  hFrame->GetXaxis()->CenterTitle( true );
  hFrame->GetXaxis()->SetNdivisions(ndivx);
  
  // TICKS X Axis
  hFrame->GetXaxis()->SetTickLength(yFactor*xtickbase/xFactor);
  
  // Format for y axis
  hFrame->GetYaxis()->SetLabelFont(43);
  hFrame->GetYaxis()->SetLabelSize(ls);
  hFrame->GetYaxis()->SetLabelOffset(yloffset);
  hFrame->GetYaxis()->SetTitleFont(43);
  hFrame->GetYaxis()->SetTitleSize(ls);
  hFrame->GetYaxis()->SetTitleOffset(ytoffset);
  
  hFrame->GetYaxis()->CenterTitle( true );
  hFrame->GetYaxis()->SetNdivisions(ndivy);
  
  // TICKS Y Axis
  hFrame->GetYaxis()->SetTickLength(xFactor*ytickbase/yFactor);      

  // Format for z axis
  hFrame->GetZaxis()->SetLabelFont(43);
  hFrame->GetZaxis()->SetLabelSize(ls);
  // hFrame->GetZaxis()->SetLabelOffset(zloffset);
  hFrame->GetZaxis()->SetTitleFont(43);
  hFrame->GetZaxis()->SetTitleSize(ls);
  // hFrame->GetZaxis()->SetTitleOffset(ztoffset);
  
  hFrame->GetZaxis()->CenterTitle( true );
  // hFrame->GetZaxis()->SetNdivisions(ndivz);

}

