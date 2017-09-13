// Based on 
// $ROOTSYS/tutorials/graphics/canvas2.C

#include <string>
#include <vector>
using namespace std;

void CanvasPartition(TCanvas *C,const Int_t Nx = 2,const Int_t Ny = 2,
                     Float_t lMargin = 0.15, Float_t rMargin = 0.05,
                     Float_t bMargin = 0.15, Float_t tMargin = 0.05);

void SetupDummy ( TH1* hFrame,
		  float xFactor, float yFactor,
		  float yaxismin, float yaxismax,
		  float xloffset, float xtoffset,
		  float xtickbase, int ndivx,
		  float yloffset, float ytoffset,
		  float ytickbase, int ndivy);

int SystematicsVisualizer()
{

  
  gStyle->SetHistLineWidth(2);
  gStyle->SetFrameLineWidth(2);
  gStyle->SetOptStat(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);

  gStyle->SetTitleX(0.1f);
  gStyle->SetTitleW(0.8f);
  gStyle->SetTitleBorderSize(0);	//Title box border thickness

  Float_t small = 1e-5; // for panel setup

  TString plotpath="./figs/";
  
  gStyle->SetHistLineWidth(3);
  TH1::SetDefaultSumw2(true);
  TH2::SetDefaultSumw2(true);

  TFile * fErrors     = new TFile ("Results/TotalzgSystematics_ForPaper_Pp12_JP2_NoEff_NoBgSub__With_AEff0_PtSmear0_ATow0_SystGeant12_NoEff_NoBg_JP2_WithMisses_WithFakes.root","READ");
  
  // Number of PADS
  // const Int_t Nx = 2;
  // const Int_t Ny = 3;
  const Int_t Nx = 1;
  const Int_t Ny = 5;

  // Good for Nx=3, Ny=2, 300,240;
  float lMargin = 0.2;
  float rMargin = 0.03;
  float bMargin = 0.09;
  float tMargin = 0.03;
  float xloffset=0.02;
  float xtoffset=4.0;
  int   ndivx= 505; 
  float yloffset=0.02;
  float ytoffset=1.5;
  int   ndivy= 305; 
  float xtickbase = 0.04;
  float ytickbase = 0.04;

  float legs = 20;


  // Demonstrate
  // -----------
  // Canvas setup
  TCanvas* C =new TCanvas("C","", Nx*360,Ny*120);
  CanvasPartition(C,Nx,Ny,lMargin,rMargin,bMargin,tMargin);
  TPad *pad[Nx][Ny];

  int histcount=0;
  
  // Set up the pads previously created.
  for (Int_t i=0;i<Nx;i++) {
    for (Int_t j=0;j<Ny;j++) {
      C->cd(0);
      char pname[16];
      sprintf(pname,"pad_%i_%i",i,j);
      pad[i][j] = (TPad*) gROOT->FindObject(pname);
      pad[i][j]->Draw();
      pad[i][j]->cd();
      gPad->SetTicky();      gPad->SetTickx();
      gPad->SetGridx(0);     gPad->SetGridy(0);
      
    }
  }

  TLegend* leg;


  TString pivot   = "Incunfold_2530_2";
  TH1D* stat  = (TH1D*) fErrors->Get( pivot )->Clone("stat");
  TH1D* tot  = (TH1D*) fErrors->Get( pivot+"_minmax" )->Clone("tot");
  // TH1D* e  = (TH1D*) fErrors->Get( pivot+"_E" )->Clone("e");
  // TH1D* r  = (TH1D*) fErrors->Get( pivot+"_R" )->Clone("r");
  // TH1D* t  = (TH1D*) fErrors->Get( pivot+"_T" )->Clone("t");
  // TH1D* h  = (TH1D*) fErrors->Get( pivot+"_H" )->Clone("h");

  TH1D* e  = (TH1D*) fErrors->Get( pivot+"_minmax_E_ratio" )->Clone("e");
  TH1D* r  = (TH1D*) fErrors->Get( pivot+"_minmax_R_ratio" )->Clone("r");
  TH1D* t  = (TH1D*) fErrors->Get( pivot+"_minmax_T_ratio" )->Clone("t");
  TH1D* hc  = (TH1D*) fErrors->Get( pivot+"_minmax_H_ratio" )->Clone("hc");

  
  
  
  for (int i=1; i<=stat->GetNbinsX(); ++i ){
    if ( stat->GetBinCenter(i) < 0.1 ){
      stat->SetBinContent(i, 0 );      stat->SetBinError(i, 0 );
      tot->SetBinContent(i, 0 );       tot->SetBinError(i, 0 );
      e->SetBinContent(i, 0 );         e->SetBinError(i, 0 );
      r->SetBinContent(i, 0 );         r->SetBinError(i, 0 );
      t->SetBinContent(i, 0 );         t->SetBinError(i, 0 );
      hc->SetBinContent(i, 0 );         hc->SetBinError(i, 0 );
    }
    stat->SetBinError(i, stat->GetBinError(i) / stat->GetBinContent(i) );
    stat->SetBinContent(i, 1 );
    tot->SetBinError(i, tot->GetBinError(i) / tot->GetBinContent(i) );
    tot->SetBinContent(i, 1 );
    // e->SetBinError(i, e->GetBinError(i) / e->GetBinContent(i) );
    // e->SetBinContent(i, 1 );
    // r->SetBinError(i, r->GetBinError(i) / r->GetBinContent(i) );
    // r->SetBinContent(i, 1 );
    // t->SetBinError(i, t->GetBinError(i) / t->GetBinContent(i) );
    // t->SetBinContent(i, 1 );
    // h->SetBinError(i, h->GetBinError(i) / h->GetBinContent(i) );
    // h->SetBinContent(i, 1 );
    e->SetBinError(i, e->GetBinContent(i) );    e->SetBinContent(i, 1 );
    r->SetBinError(i, r->GetBinContent(i) );    r->SetBinContent(i, 1 );
    t->SetBinError(i, t->GetBinContent(i) );    t->SetBinContent(i, 1 );
    hc->SetBinError(i, hc->GetBinContent(i) );    hc->SetBinContent(i, 1 );

  }
  stat->SetAxisRange(0.1 + 0.001, 0.5 - 0.001);
  tot->SetAxisRange(0.1 + 0.001, 0.5 - 0.001);
  e->SetAxisRange(0.1 + 0.001, 0.5 - 0.001);
  r->SetAxisRange(0.1 + 0.001, 0.5 - 0.001);
  t->SetAxisRange(0.1 + 0.001, 0.5 - 0.001);
  hc->SetAxisRange(0.1 + 0.001, 0.5 - 0.001);
    
  float dy = 0.05;
  float yaxismin = 1 - dy;
  float yaxismax = 1 + dy*1.2;

  TObjArray histos;
  // histos.Add(stat);
  histos.Add(tot);
  histos.Add(e);
  histos.Add(r);
  histos.Add(t);
  histos.Add(hc);

  int histcount=0;    
  for (Int_t j=Ny-1;j>=0;j--) {
    for (Int_t i=0;i<Nx;i++) {
      C->cd(0);
      pad[i][j]->cd();

      TH1D* h = (TH1D*) histos.At(histcount);
      
      // Size factors
      Float_t xFactor = pad[0][0]->GetAbsWNDC()/pad[i][j]->GetAbsWNDC();
      Float_t yFactor = pad[0][0]->GetAbsHNDC()/pad[i][j]->GetAbsHNDC();

      char hname[16];
      sprintf(hname,"dummy_%i_%i",i,j);

      TH1D* dummy = new TH1D( hname,"", 20, 0.05, 0.55);
      dummy->GetXaxis()->SetTitle("z_{g}");
      dummy->GetYaxis()->SetTitle("");
      dummy->Reset();
      dummy->SetLineColor(0);      dummy->SetLineWidth(0);


      // dummy->SetTitle(";z_{g};rel. uncertainty");
      dummy->SetTitle(";z_{g};");
      SetupDummy ( dummy,
		   xFactor, yFactor,
		   yaxismin, yaxismax,
		   xloffset, xtoffset,
		   xtickbase, ndivx,
		   yloffset, ytoffset,
		   ytickbase, ndivy );

  
      
      dummy->DrawCopy("AXIS");
      // tot->Draw("same");
      // stat->SetLineColor(kBlack);
      // stat->Draw("9same");
      // stat->Draw("9same");

      TString title;
      Color_t color = kBlack;
      if ( TString(h->GetName()) == "tot" ){
	title = "Quadrature Sum";
	color = kGreen+1;
      }
      if ( TString(h->GetName()) == "e" ){
	title = "Tracking Efficiency";
	color = kRed;
      }
      if ( TString(h->GetName()) == "r" ){
	title = "Tracking Resolution";
	color = kBlue;
      }
      if ( TString(h->GetName()) == "t" ){
	title = "Tower Gain Calibration";
	color = kOrange+1;
      }
      if ( TString(h->GetName()) == "hc" ){
	title = "Hadronic Correction";
	color = kMagenta+1;
      }


      h->SetLineColor(color);
      h->SetFillColor(color);
      h->SetMarkerColor(color);
      h->Draw("9e2same");

      if ( TString(h->GetName()) == "tot" ){
	stat->Draw("9same");
      }
	      

      leg = new TLegend( 0.05, 1+dy*.6, 0.5, 1+dy*.95, "", "br" );
      leg->SetBorderSize(0);
      leg->SetTextFont(43);
      leg->SetTextSize(legs);  
      leg->SetFillStyle(0);
      leg->SetMargin(0.1);

      leg->AddEntry(h->GetName(),title,"f");
      leg->Draw();
      
      histcount++; 
    }
  }

  TString plotname=gSystem->BaseName("SystematicsOverview");
  plotname.Prepend(plotpath+"Fig");
  C->SaveAs(plotname + ".pdf");

  
  return 0;

}
// -----------------------------------------------------------------------------


void CanvasPartition(TCanvas *C,const Int_t Nx,const Int_t Ny,
                     Float_t lMargin, Float_t rMargin,
                     Float_t bMargin, Float_t tMargin)
{
  if (!C) return;

  // Setup Pad layout:
  Float_t vSpacing = 0.0;
  Float_t vStep  = (1.- bMargin - tMargin - (Ny-1) * vSpacing) / Ny;

  Float_t hSpacing = 0.0;
  Float_t hStep  = (1.- lMargin - rMargin - (Nx-1) * hSpacing) / Nx;

  Float_t vposd,vposu,vmard,vmaru,vfactor;
  Float_t hposl,hposr,hmarl,hmarr,hfactor;

  for (Int_t i=0;i<Nx;i++) {

    if (i==0) {
      hposl = 0.0;
      hposr = lMargin + hStep;
      hfactor = hposr-hposl;
      hmarl = lMargin / hfactor;
      hmarr = 0.0;
    } else if (i == Nx-1) {
      hposl = hposr + hSpacing;
      hposr = hposl + hStep + rMargin;
      hfactor = hposr-hposl;
      hmarl = 0.0;
      hmarr = rMargin / (hposr-hposl);
    } else {
      hposl = hposr + hSpacing;
      hposr = hposl + hStep;
      hfactor = hposr-hposl;
      hmarl = 0.0;
      hmarr = 0.0;
    }

    for (Int_t j=0;j<Ny;j++) {

      if (j==0) {
	vposd = 0.0;
	vposu = bMargin + vStep;
	vfactor = vposu-vposd;
	vmard = bMargin / vfactor;
	vmaru = 0.0;
      } else if (j == Ny-1) {
	vposd = vposu + vSpacing;
	vposu = vposd + vStep + tMargin;
	vfactor = vposu-vposd;
	vmard = 0.0;
	vmaru = tMargin / (vposu-vposd);
      } else {
	vposd = vposu + vSpacing;
	vposu = vposd + vStep;
	vfactor = vposu-vposd;
	vmard = 0.0;
	vmaru = 0.0;
      }

      C->cd(0);

      char name[16];
      sprintf(name,"pad_%i_%i",i,j);
      TPad *pad = (TPad*) gROOT->FindObject(name);
      if (pad) delete pad;
      pad = new TPad(name,"",hposl,vposd,hposr,vposu);
      pad->SetLeftMargin(hmarl);
      pad->SetRightMargin(hmarr);
      pad->SetBottomMargin(vmard);
      pad->SetTopMargin(vmaru);

      pad->SetFrameBorderMode(0);
      pad->SetBorderMode(0);
      pad->SetBorderSize(0);

      pad->Draw();
    }
  }
}

// -----------------------------------------------------------------------------
void SetupDummy ( TH1* hFrame,
		  float xFactor, float yFactor,
		  float yaxismin, float yaxismax,
		  float xloffset, float xtoffset,
		  float xtickbase, int ndivx,
		  float yloffset, float ytoffset,
		  float ytickbase, int ndivy
		  )
{
  // y axis range
  hFrame->GetYaxis()->SetRangeUser(yaxismin,yaxismax);
  float ls = 20;
  float ts = 20;
  
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
}

