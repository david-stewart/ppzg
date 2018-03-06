// Based on 
// $ROOTSYS/tutorials/graphics/canvas2.C

#include "ZgPaperConsts.hxx"

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

int FigRatioPanel(
		  // --- Latest Run 6 ---
		  // TString inname="Results/TotalzgSystematics_Recut_Pp_HT54_NoEff_NoBgSub__With_AEff0_PtSmear0_ATow0_SystGeant_NoEff_NoBg_HT54_WithMisses_WithFakes_TrainedWith_Recut_McGeant_NoEff_NoBg_MB.root"
		  // --- Use this ---
		  // TString inname="Results/TotalzgSystematics_ForPaper_Pp12_JP2_NoEff_NoBgSub__With_AEff0_PtSmear0_ATow0_SystGeant12_NoEff_NoBg_JP2_WithMisses_WithFakes.root"
		  // --- Different R
		  // TString inname="Results/TotalzgSystematics_R0.2_ForPaper_Pp12_JP2_NoEff_NoBgSub__With_AEff0_PtSmear0_ATow0_R0.2_SystGeant12_NoEff_NoBg_JP2_WithMisses_WithFakes_TrainedWith_R0.2_McGeant12_NoEff_NoBg_all.root"
		  TString inname="Results/TotalzgSystematics_R0.6_ForPaper_Pp12_JP2_NoEff_NoBgSub__With_AEff0_PtSmear0_ATow0_R0.6_SystGeant12_NoEff_NoBg_JP2_WithMisses_WithFakes_TrainedWith_R0.6_McGeant12_NoEff_NoBg_all.root"
		  // Unrebinned versions
		  // --- Latest Run 6 ---
		  // TString inname="Results/TotalzgSystematics_NoRebin_Recut_Pp_HT54_NoEff_NoBgSub__With_AEff0_PtSmear0_ATow0_SystGeant_NoEff_NoBg_HT54_WithMisses_WithFakes_TrainedWith_Recut_McGeant_NoEff_NoBg_MB.root"
		  // --- Use one of these ---
		  // TString inname="Results/TotalzgSystematics_NoRebin_ForPaper_Pp12_JP2_NoEff_NoBgSub__With_AEff0_PtSmear0_ATow0_SystGeant12_NoEff_NoBg_JP2_WithMisses_WithFakes.root"
		  //// Not this one really TString inname="Results/TotalzgSystematics_NoRebin_ForPaper_Pp12_HT54_JP2_NoEff_NoBgSub__With_AEff0_PtSmear0_ATow0_SystGeant12_NoEff_NoBg_HT54_JP2_WithMisses_WithFakes.root"
		  ){
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

  Float_t small = 1e-5; // for panel setup
  
  bool ShowNoHadro = false;

  // Load histos
  TFile* infile = new TFile(inname,"READ");
  TObjArray histos( 6 );
  histos.Add( infile->Get("Incunfold_1015_2") );
  histos.Add( infile->Get("Incunfold_1520_2") );
  histos.Add( infile->Get("Incunfold_2025_2") );
  histos.Add( infile->Get("Incunfold_2530_2") );
  histos.Add( infile->Get("Incunfold_3040_2") );
  histos.Add( infile->Get("Incunfold_4060_2") );

  TObjArray SysUnfold( 6 );
  SysUnfold.Add( infile->Get("SysIncunfold_1015") );
  SysUnfold.Add( infile->Get("SysIncunfold_1520") );
  SysUnfold.Add( infile->Get("SysIncunfold_2025") );
  SysUnfold.Add( infile->Get("SysIncunfold_2530") );
  SysUnfold.Add( infile->Get("SysIncunfold_3040") );
  SysUnfold.Add( infile->Get("SysIncunfold_4060") );

  TObjArray SysRest( 6 );
  SysRest.Add( infile->Get("Incunfold_1015_2_minmax") );
  SysRest.Add( infile->Get("Incunfold_1520_2_minmax") );
  SysRest.Add( infile->Get("Incunfold_2025_2_minmax") );
  SysRest.Add( infile->Get("Incunfold_2530_2_minmax") );
  SysRest.Add( infile->Get("Incunfold_3040_2_minmax") );
  SysRest.Add( infile->Get("Incunfold_4060_2_minmax") );

  bool SuppresSystematics = ( SysRest.At(0)==0 );

  // Number of PADS
  // const Int_t Nx = 2;
  // const Int_t Ny = 3;
  const Int_t Nx = 3;
  const Int_t Ny = 2;

  if (histos.GetEntries()!=Nx*Ny) {
    cerr << "Incompatible number of histos." << endl;
    return -1;
  }

  // Margins etc.
  // // Good for Nx=2, Ny=3, 300,220;
  // float lMargin = 0.10;
  // float rMargin = 0.05;
  // float bMargin = 0.08;
  // float tMargin = 0.05;
  // float xloffset=0.02;
  // float xtoffset=3;
  // int   ndivx= 505; 
  // float yloffset=0.02;
  // float ytoffset=3;
  // int   ndivy= 505; 
  // float xtickbase = 0.05;
  // float ytickbase = 0.03;
  

  // // // Good for Nx=3, Ny=2, 300,220;
  // float lMargin = 0.07;
  // float rMargin = 0.03;
  // float bMargin = 0.11;
  // float tMargin = 0.03;
  // float xloffset=0.02;
  // float xtoffset=2;
  // int   ndivx= 505; 
  // float yloffset=0.02;
  // float ytoffset=2;
  // int   ndivy= 505; 
  // float xtickbase = 0.04;
  // float ytickbase = 0.04;
  
  // // Good for Nx=3, Ny=2, 300,240;
  float lMargin = 0.07;
  float rMargin = 0.03;
  float bMargin = 0.13;
  float tMargin = 0.03;
  float xloffset=0.02;
  float xtoffset=2.2;
  int   ndivx= 505; 
  float yloffset=0.02;
  float ytoffset=1.75;
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

  // Load Pythia8 comparison
  TString nP8default = "Results/CutUp_ForPaper_Pythia8_NoEff_NoBgSub.root";
  TString nP8nohadro = "Results/CutUp_ForPaper_Pythia8_NoEff_NoBgSub_NoHadronization.root";

  // Load Herwig comparison
  TString nHWdefault = "Results/CutUp_ForPaper_Herwig_NoEff_NoBgSub.root";
  TString nHWnohadro = "Results/CutUp_ForPaper_Herwig_NoEff_NoBgSub_NoHadro.root";
    
  if ( inname.Contains("R0.2") ){
    nP8default.ReplaceAll( "CutUp_ForPaper","CutUp_R0.2_ForPaper");
    nHWdefault.ReplaceAll( "CutUp_ForPaper","CutUp_R0.2_ForPaper");
    nP8nohadro.ReplaceAll( "CutUp_ForPaper","CutUp_R0.2_ForPaper");
    nHWnohadro.ReplaceAll( "CutUp_ForPaper","CutUp_R0.2_ForPaper");
  }
  if ( inname.Contains("R0.6") ){
    nP8default.ReplaceAll( "CutUp_ForPaper","CutUp_R0.6_ForPaper");
    nHWdefault.ReplaceAll( "CutUp_ForPaper","CutUp_R0.6_ForPaper");
    nP8nohadro.ReplaceAll( "CutUp_ForPaper","CutUp_R0.6_ForPaper");
    nHWnohadro.ReplaceAll( "CutUp_ForPaper","CutUp_R0.6_ForPaper");
  }
  TFile* fP8default = new TFile( nP8default , "READ");
  TFile* fHWdefault = new TFile( nHWdefault , "READ");
  TFile* fP8nohadro = new TFile( nP8nohadro , "READ");
  TFile* fHWnohadro = new TFile( nHWnohadro , "READ");

  TLatex latex;
  // latex.SetNDC();
  
  TLegend* leg;
  TLegend* leg2;
  TString s, name;

  
  
  // Build and draw. Reordered.
  for (Int_t j=Ny-1;j>=0;j--) {
    for (Int_t i=0;i<Nx;i++) {
      C->cd(0);
      pad[i][j]->cd();
      TH1D* h = (TH1D*) histos.At(histcount);
      TH1D* sysunfold;
      TH1D* sysrest;
      TH1D* systot;
      TH1D* rsystot;
      if ( !SuppresSystematics ){
	sysunfold = (TH1D*) SysUnfold.At(histcount);
        sysrest = (TH1D*) SysRest.At(histcount);
	systot = (TH1D*) sysrest->Clone( TString("tot")+sysrest->GetName() );
	rsystot = (TH1D*) systot->Clone(  TString(systot->GetName()) + "_r" );
	for (int ii=1; ii<=systot->GetNbinsX() ; ++ii ){
	  systot->SetBinError (ii, sqrt( pow( sysunfold->GetBinError(ii), 2) + pow( sysrest->GetBinError(ii), 2) ));
	  rsystot->SetBinContent (ii, 1);
	  if ( systot->GetBinContent (ii )> 1e-6 ) rsystot->SetBinError (ii, systot->GetBinError ( ii ) / systot->GetBinContent (ii ) );
	}
      }      

      char hname[16];
      sprintf(hname,"dummy_%i_%i",i,j);

      TH1D* dummy = new TH1D( hname,"", 20, 0.05, 0.55);
      dummy->GetXaxis()->SetTitle(h->GetXaxis()->GetTitle());
      // dummy->GetYaxis()->SetTitle(h->GetYaxis()->GetTitle());
      dummy->GetYaxis()->SetTitle( "Data / MC");
      dummy->Reset();
      dummy->SetLineColor(0);      dummy->SetLineWidth(0);
      dummy->Draw();

      // Size factors
      Float_t xFactor = pad[0][0]->GetAbsWNDC()/pad[i][j]->GetAbsWNDC();
      Float_t yFactor = pad[0][0]->GetAbsHNDC()/pad[i][j]->GetAbsHNDC();

      // y axis range
      float yaxismin = 0.6;
      float yaxismax = 1.6;
      
      SetupDummy ( dummy,
		   xFactor, yFactor,
		   yaxismin, yaxismax,
		   xloffset, xtoffset,
		   xtickbase, ndivx,
		   yloffset, ytoffset,
		   ytickbase, ndivy );

      h->SetAxisRange(0.1 + 0.001, 0.5 - 0.001);
	    
      if ( !SuppresSystematics){
	systot->SetAxisRange(0.1 + 0.001, 0.5 - 0.001);
	systot->SetMarkerStyle(20);      systot->SetMarkerSize(0);      systot->SetLineWidth( 0 );
	systot->SetFillStyle(1001);
	
	sysunfold->SetAxisRange(0.1 + 0.001, 0.5 - 0.001);
	sysunfold->SetMarkerStyle(20);      sysunfold->SetMarkerSize(0);      sysunfold->SetLineWidth( 0 );
	sysunfold->SetFillStyle(1001);    
	
	sysrest->SetAxisRange(0.1 + 0.001, 0.5 - 0.001);
	sysrest->SetMarkerStyle(20);      sysrest->SetMarkerSize(0);      sysrest->SetLineWidth( 0 );
	sysrest->SetFillStyle(3001);
	sysrest->SetFillColor( kGray+2 );

	rsystot->SetAxisRange(0.1 + 0.001, 0.5 - 0.001);
	rsystot->SetMarkerStyle( h->GetMarkerStyle() );      rsystot->SetMarkerSize( h->GetMarkerSize() );
	rsystot->SetLineWidth( h->GetMarkerColor() );
	rsystot->SetLineColor( 0 );
	rsystot->SetLineStyle( 1 );
	rsystot->SetFillStyle(1001);    

      }

      float ptleft=0;
      float ptright=0;

      leg = new TLegend( 0.2, 1.38, 0.5, 1.58, "", "br" );
      
      leg->SetBorderSize(0);
      leg->SetTextFont(43);
      leg->SetTextSize(legs);
  
      leg->SetFillStyle(0);
      // leg->SetMargin(0.1);

      switch (histcount){
      case 0 :
	ptleft=10; ptright=15;	      
	break;
      case 1 :
	ptleft=15; ptright=20;
	break;
      case 2 :
	ptleft=20; ptright=25;
	break;
      case 3 :
	ptleft=25; ptright=30;
	break;	
      case 4 :
	ptleft=30; ptright=40;
	break; 
      case 5 :
	ptleft=40; ptright=60;
	break;
      default :
	cerr << "Unsupported number of histos" << endl;
	return -1;
	break; // ROOT can be stupid
      }

      h->SetMarkerStyle( zgconsts::datamark[histcount] );
      h->SetMarkerSize( zgconsts::datamarksize[histcount] );
      h->SetMarkerColor( zgconsts::datacol[histcount] );
      h->SetLineColor( zgconsts::datacol[histcount] );
      h->SetLineWidth( 2 );
      if ( !SuppresSystematics){
	sysunfold->SetFillColor( zgconsts::syscol[histcount] );
	systot->SetFillColor( zgconsts::syscol[histcount]  );
	rsystot->SetFillColor( zgconsts::syscol[histcount]  );
      }

      TLine l;
      l.SetLineStyle(2);
      l.SetLineColor(kBlack);
      l.SetLineWidth(1);
      // Systematics in the back
      if ( !SuppresSystematics){
	// sysunfold->Draw("9e2same");
	// sysrest->Draw("9e2same");
	// h->Draw("9E0X0same");       
	rsystot->Draw("9e2same");
	l.DrawLine(0.05,1,0.55,1);
      }
            
      s="p_{T}="; s+=int(ptleft); s+="-"; s+=int(ptright); s+=" GeV/c";
      leg->AddEntry(rsystot->GetName(), s,"f");

      name = "p8_Incmeas_"; name += int(ptleft+0.01); name += int(ptright+0.01);
      // name = "AS_P8_SoftJets_MB_"; name += int(ptleft+0.01); name+="_"; name += int(ptright+0.01);
      TH1D* hP8 = (TH1D*) fP8default->Get(name);      
      TH1D* rp = (TH1D*) hP8->Clone(TString(hP8->GetName())+"_rp");
      rp->Divide( h );

      // 

      // hP8->SetLineStyle(1);
      rp->SetAxisRange(0.11, 0.49);
      rp->SetMarkerStyle ( 21 );
      // rp->SetMarkerColor( h->GetLineColor() );
      // rp->SetLineColor( kBlack );
      rp->SetMarkerColor( kBlack );
      rp->SetLineColor( h->GetLineColor() );
      rp->SetLineWidth( 1 );
      rp->SetLineStyle( 1 );
      rp->Draw("9same");
      
      
      TH1D* hHW = (TH1D*) fHWdefault->Get(name);
      hHW->SetName(TString(hHW->GetName())+"_herwig");
      TH1D* rh = (TH1D*) hHW->Clone(TString(hHW->GetName())+"_rh");
      rh->Divide( h );

      rh->SetAxisRange(0.11, 0.49);
      rh->SetMarkerStyle ( 20 );
      // rh->SetMarkerColor( h->GetLineColor() );
      // rh->SetLineColor( kRed );
      rh->SetMarkerColor( kRed );
      rh->SetLineColor( h->GetLineColor() );
      rh->SetLineWidth( 1 );
      rh->SetLineStyle( 1 );
      rh->Draw("9same");
      
      leg->Draw();

      latex.SetTextColor( kAzure-6 );      latex.SetTextSize(0.07*yFactor);
      latex.SetTextFont( 62 ); // 42: helvetica, 62: helvetica bold
      latex.DrawLatex( .1,1.25, "STAR");
      
      if ( histcount==3 ){
      	leg2 = new TLegend( 0.12, 1.15, 0.5, 1.35, "", "br" );
      	leg2->SetBorderSize(0);
      	leg2->SetTextFont(63);
      	leg2->SetTextSize(legs*0.8);
      	leg2->SetFillStyle(0);
      	leg2->AddEntry((TObject*)0, "p+p, #sqrt{s}=200 GeV", "");
      	s = "anti-k_{T}, R=0.4";
      	if ( inname.Contains("R0.2") ) s = "anti-k_{T}, R=0.2";
      	if ( inname.Contains("R0.6") ) s = "anti-k_{T}, R=0.6";
	
      	leg2->AddEntry((TObject*)0, s, "");
      	leg2->Draw();
      }
	
      if ( histcount==1 ){
      	leg2 = new TLegend( 0.2, 1.25, 0.5, 1.35, "", "br" );
      	leg2->SetBorderSize(0);
      	leg2->SetTextFont(43);
      	leg2->SetTextSize(legs);
      	leg2->SetFillStyle(0);

	leg2->AddEntry(rp->GetName(), "Pythia8","lp");
	// if ( hP8nohadro && ShowNoHadro ){
	//   leg2->AddEntry(hP8nohadro->GetName(), "Pythia8 NoHadro","l");
	// }
	leg2->Draw();
      }
      // } 

      if ( histcount==2 ){
      	leg2 = new TLegend( 0.2, 1.25, 0.5, 1.35, "", "br" );
      	leg2->SetBorderSize(0);
      	leg2->SetTextFont(43);
      	leg2->SetTextSize(legs);
      	leg2->SetFillStyle(0);
	
	leg2->AddEntry(hHW->GetName(), "Herwig","lp");
	// if ( hHWnohadro && ShowNoHadro ){
	//   leg2->AddEntry(hHWnohadro->GetName(), "Herwig NoHadro","l");
	// }
	  
	leg2->Draw();
      }
      
      histcount++; 
    }    
  }
      


  C->cd();  
  // C->SaveAs(plotpath+"delme.pdf");
  // C->SaveAs(plotpath+"delme2.pdf");
  
  TString plotname=gSystem->BaseName(inname);
  plotname.Prepend(plotpath+"FigRatioPanel_");
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

