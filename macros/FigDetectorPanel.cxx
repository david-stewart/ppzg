// Based on 
// $ROOTSYS/tutorials/graphics/canvas2.C

#include"ZgPaperConsts.hxx"

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

int FigDetectorPanel( ){
  int rebinzg = 2;
  TString plotpath="./figs/";
  
  // gROOT->Reset();
  
  gStyle->SetHistLineWidth(2);
  gStyle->SetFrameLineWidth(2);
  gStyle->SetOptStat(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);

  gStyle->SetTitleX(0.1f);
  gStyle->SetTitleW(0.8f);
  gStyle->SetTitleBorderSize(0);	//Title box border thickness

  TFile * ppfile = new TFile("Results/ForUnfolding_ForPaper_Pp12_JP2_NoEff_NoBgSub.root");
  TFile * geantfile = new TFile("Results/AEff0_PtSmear0_ATow0_SystGeant12_NoEff_NoBg_JP2_WithMisses_WithFakes_TrainedWith_McGeant12_NoEff_NoBg_all.root");
    

  // Load 2d histos, make 1D histos
  // ------------------------------
  TH2D* pp2d = (TH2D*) ppfile->Get("IncMeas2D");
  pp2d->SetName("pp2d");
  
  TH2D* geant2d = (TH2D*) geantfile->Get("IncMeas2D");
  geant2d->SetName("geant2d");

  TObjArray pphistos( 6 );
  TObjArray geanthistos( 6 );
  for ( int i=0 ; i<zgconsts::nIncbins ; ++i ){
    float ptleft  = zgconsts::Incbins[i];
    float ptright = zgconsts::Incbins[i+1];

    TString name = "ppmeas_"; name += int(ptleft+0.01); name += int(ptright+0.01);
    TH1D* ppmeas =
      (TH1D*) pp2d->ProjectionY(name, pp2d->GetXaxis()->FindBin( ptleft+0.01), pp2d->GetXaxis()->FindBin( ptright-0.01));

    TH1D* h=ppmeas;
    h->Rebin(rebinzg);
    h->Scale ( 1./ h->Integral(h->FindBin(0.1+0.001),h->FindBin(0.5-0.001)) / h->GetXaxis()->GetBinWidth(1));
    pphistos.Add( h );

    TString name = "geantmeas_"; name += int(ptleft+0.01); name += int(ptright+0.01);
    TH1D* geantmeas =
      (TH1D*) geant2d->ProjectionY(name, geant2d->GetXaxis()->FindBin( ptleft+0.01), geant2d->GetXaxis()->FindBin( ptright-0.01));

    TH1D* h=geantmeas;
    h->Rebin(rebinzg);
    h->Scale ( 1./ h->Integral(h->FindBin(0.1+0.001),h->FindBin(0.5-0.001)) / h->GetXaxis()->GetBinWidth(1));
    geanthistos.Add( h );

  }

  // Number of PADS
  // const Int_t Nx = 2;
  // const Int_t Ny = 3;
  const Int_t Nx = 3;
  const Int_t Ny = 2;

  if (pphistos.GetEntries()!=Nx*Ny || geanthistos.GetEntries()!=Nx*Ny ) {
    cerr << "Incompatible number of histos." << endl;
    return -1;
  }
  
  // // Good for Nx=3, Ny=2, 300,240;
  float lMargin = 0.07;
  float rMargin = 0.03;
  float bMargin = 0.13;
  float tMargin = 0.03;
  float xloffset=0.02;
  float xtoffset=2.2;
  int   ndivx= 505; 
  float yloffset=0.02;
  float ytoffset=1.5;
  int   ndivy= 505; 
  float xtickbase = 0.04;
  float ytickbase = 0.04;

  float legs = 20;

  // Canvas setup
  // TCanvas* C =new TCanvas("C","", Nx*300,Ny*220);
  TCanvas* C =new TCanvas("C","", Nx*300,Ny*240);
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
  TString s;
  
  // And draw. Reordered.
  // for (Int_t i=0;i<Nx;i++) {
  //   for (Int_t j=Ny-1;j>=0;j--) {
  for (Int_t j=Ny-1;j>=0;j--) {
    for (Int_t i=0;i<Nx;i++) {
      C->cd(0);
      pad[i][j]->cd();
      TH1D* pph = (TH1D*) pphistos.At(histcount);
      TH1D* geanth = (TH1D*) geanthistos.At(histcount);

      char hname[16];
      sprintf(hname,"dummy_%i_%i",i,j);

      TH1D* dummy = new TH1D( hname,"", 20, 0.05, 0.55);
      dummy->GetXaxis()->SetTitle(pph->GetXaxis()->GetTitle());
      dummy->GetYaxis()->SetTitle(pph->GetYaxis()->GetTitle());
      dummy->Reset();
      dummy->SetLineColor(0);      dummy->SetLineWidth(0);
      dummy->SetTitle( ";z_{g}^{Det};1/N dN/dz_{g}^{Det}" );	  
      dummy->Draw();

      // Size factors
      Float_t xFactor = pad[0][0]->GetAbsWNDC()/pad[i][j]->GetAbsWNDC();
      Float_t yFactor = pad[0][0]->GetAbsHNDC()/pad[i][j]->GetAbsHNDC();

      // y axis range
      float yaxismin = -0.5;
      float yaxismax = 7;
      if ( rebinzg==1 ) yaxismax = 8.5;

      SetupDummy ( dummy,
		   xFactor, yFactor,
		   yaxismin, yaxismax,
		   xloffset, xtoffset,
		   xtickbase, ndivx,
		   yloffset, ytoffset,
		   ytickbase, ndivy );

      pph->SetAxisRange(0.1 + 0.001, 0.5 - 0.001);
	    
      float ptleft=0;
      float ptright=0;
    
      switch (histcount){
      case 0 :
	pph->SetMarkerStyle( 20 );
	pph->SetMarkerSize( 1 );
	pph->SetMarkerColor( kBlack ); pph->SetLineColor( kBlack );
	geanth->SetMarkerStyle( 34 );
	geanth->SetMarkerSize( 2 );
	geanth->SetMarkerColor( kGray+2 ); geanth->SetLineColor( kGray+2 );
	ptleft=10; ptright=15;	      
	break;
      case 1 :
	pph->SetMarkerStyle( 27 );
	pph->SetMarkerSize( 1.5 );
	pph->SetMarkerColor( kRed ); pph->SetLineColor( kRed );
	geanth->SetMarkerStyle( 34 );
	geanth->SetMarkerSize( 2 );
	geanth->SetMarkerColor( kGray+2 ); geanth->SetLineColor( kGray+2 );
	ptleft=15; ptright=20;
	break;
      case 2 :
	pph->SetMarkerStyle( 29 );
	pph->SetMarkerSize( 1.6 );
	pph->SetMarkerColor( kBlue ); pph->SetLineColor( kBlue );
	geanth->SetMarkerStyle( 34 );
	geanth->SetMarkerSize( 2 );
	geanth->SetMarkerColor( kGray+2 ); geanth->SetLineColor( kGray+2 );
	
	ptleft=20; ptright=25;
	break;
      case 3 :
	pph->SetMarkerStyle( 24 );
	pph->SetMarkerSize( 1 );
	pph->SetMarkerColor( kGreen+1 ); pph->SetLineColor( kGreen+1 );
	geanth->SetMarkerStyle( 34 );
	geanth->SetMarkerSize( 2 );
	geanth->SetMarkerColor( kGray+2 ); geanth->SetLineColor( kGray+2 );
	ptleft=25; ptright=30;
	break;	
      case 4 :
	pph->SetMarkerStyle( 33 );
	pph->SetMarkerSize( 1.6 );
	pph->SetMarkerColor( kOrange+7 ); pph->SetLineColor( kOrange+7 );
	geanth->SetMarkerStyle( 34 );
	geanth->SetMarkerSize( 2 );
	geanth->SetMarkerColor( kGray+2 ); geanth->SetLineColor( kGray+2 );
	ptleft=30; ptright=40;
	break; 
      case 5 :
	pph->SetMarkerStyle( 30 );
	pph->SetMarkerSize( 1.3 );
	pph->SetMarkerColor( kMagenta+1 ); pph->SetLineColor( kMagenta+1 );
	geanth->SetMarkerStyle( 34 );
	geanth->SetMarkerSize( 2 );
	geanth->SetMarkerColor( kGray+2 ); geanth->SetLineColor( kGray+2 );
	ptleft=40; ptright=60;
	break;
      default :
	cerr << "Unsupported number of histos" << endl;
	return -1;
	break; // ROOT can be stupid
      }
      // DEBUG
      geanth->SetFillColor(kGray);
      geanth->SetMarkerStyle( 1 );
      geanth->SetMarkerSize( 0 );
      geanth->SetMarkerColor( kGray );
      geanth->SetLineColor( kGray );
      
      pph->SetLineWidth( 2 );
      pph->Draw("9same");
      geanth->SetAxisRange( 0.1+0.001, 0.5 - 0.001, "x");
      // geanth->Draw("9same");
      // geanth->Draw("9histsame");
      geanth->Draw("9e3same");
      pph->Draw("9same");
	    
      s="p_{T}^{Det}="; s+=int(ptleft); s+="-"; s+=int(ptright); s+=" GeV/c";
      leg = new TLegend( 0.22, 3.6, 0.5, 6.7, "", "br" );
      leg->SetBorderSize(0);
      leg->SetTextFont(43);
      leg->SetTextSize(legs);
      leg->SetHeader(s);

      leg->AddEntry(pph->GetName(), "p+p Measured","lp");
      // leg->AddEntry(geanth->GetName(), "Pythia6@STAR","lp");
      leg->AddEntry(geanth->GetName(), "Pythia6@STAR","f");

      leg->Draw();
      
      histcount++; 
    }    
  }
      


  C->cd();  
  // C->SaveAs(plotpath+"delme.pdf");
  // C->SaveAs(plotpath+"delme2.pdf");
  
  TString plotname=gSystem->BaseName("DetLevelComparison");
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

