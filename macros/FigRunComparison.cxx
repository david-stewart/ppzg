// Based on 
// $ROOTSYS/tutorials/graphics/canvas2.C

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

int FigRunComparison(
		     // --- Latest Run 6 ---
		     TString r6name="Results/TotalzgSystematics_Recut_Pp_HT54_NoEff_NoBgSub__With_AEff0_PtSmear0_ATow0_SystGeant_NoEff_NoBg_HT54_WithMisses_WithFakes_TrainedWith_Recut_McGeant_NoEff_NoBg_MB.root",
		     // --- Run 12 ---
		     TString r12name="Results/TotalzgSystematics_ForPaper_Pp12_JP2_NoEff_NoBgSub__With_AEff0_PtSmear0_ATow0_SystGeant12_NoEff_NoBg_JP2_WithMisses_WithFakes.root"
		     
		     // Unrebinned versions
		     // --- Latest Run 6 ---
		     // TString r6name="Results/TotalzgSystematics_NoRebin_Recut_Pp_HT54_NoEff_NoBgSub__With_AEff0_PtSmear0_ATow0_SystGeant_NoEff_NoBg_HT54_WithMisses_WithFakes_TrainedWith_Recut_McGeant_NoEff_NoBg_MB.root",
		     // --- Run 12 ---
		     // TString r12name="Results/TotalzgSystematics_NoRebin_ForPaper_Pp12_JP2_NoEff_NoBgSub__With_AEff0_PtSmear0_ATow0_SystGeant12_NoEff_NoBg_JP2_WithMisses_WithFakes.root"
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

  bool ShowHardProbes=false;
    

  // Load histos
  TFile* r12file = new TFile(r12name,"READ");
  TObjArray r12histos( 3 );
  r12histos.Add( r12file->Get("Incunfold_1015_2") );
  r12histos.Add( r12file->Get("Incunfold_2025_2") );
  r12histos.Add( r12file->Get("Incunfold_3040_2") );
  // r12histos.Add( r12file->Get("Incunfold_1520_2") );
  // r12histos.Add( r12file->Get("Incunfold_2530_2") );
  // r12histos.Add( r12file->Get("Incunfold_4060_2") );

  TObjArray r12SysUnfold( 3 );
  r12SysUnfold.Add( r12file->Get("SysIncunfold_1015") );
  r12SysUnfold.Add( r12file->Get("SysIncunfold_2025") );
  r12SysUnfold.Add( r12file->Get("SysIncunfold_3040") );
  // r12SysUnfold.Add( r12file->Get("SysIncunfold_1520") );
  // r12SysUnfold.Add( r12file->Get("SysIncunfold_2530") );
  // r12SysUnfold.Add( r12file->Get("SysIncunfold_4060") );

  TObjArray r12SysRest( 3 );
  r12SysRest.Add( r12file->Get("Incunfold_1015_2_minmax") );
  r12SysRest.Add( r12file->Get("Incunfold_2025_2_minmax") );
  r12SysRest.Add( r12file->Get("Incunfold_3040_2_minmax") );
  // r12SysRest.Add( r12file->Get("Incunfold_1520_2_minmax") );
  // r12SysRest.Add( r12file->Get("Incunfold_2530_2_minmax") );
  // r12SysRest.Add( r12file->Get("Incunfold_4060_2_minmax") );

  TH1D* h;  TString s;
  s="r12";
  for (int i=0 ; i<r12histos.GetEntries() ; ++i )    {h = (TH1D*) r12histos.At(i); h->SetName( s+h->GetName() );}
  for (int i=0 ; i<r12SysUnfold.GetEntries() ; ++i ) {h = (TH1D*) r12SysUnfold.At(i); h->SetName( s+h->GetName() );}
  for (int i=0 ; i<r12SysRest.GetEntries() ; ++i )   {h = (TH1D*) r12SysRest.At(i); h->SetName( s+h->GetName() );}

  TFile* r6file = new TFile(r6name,"READ");
  TObjArray r6histos( 3 );
  r6histos.Add( r6file->Get("Incunfold_1015_2") );
  r6histos.Add( r6file->Get("Incunfold_2025_2") );
  r6histos.Add( r6file->Get("Incunfold_3040_2") );
  // r6histos.Add( r6file->Get("Incunfold_1520_2") );
  // r6histos.Add( r6file->Get("Incunfold_2530_2") );
  // r6histos.Add( r6file->Get("Incunfold_4060_2") );

  TObjArray r6SysUnfold( 3 );
  r6SysUnfold.Add( r6file->Get("SysIncunfold_1015") );
  r6SysUnfold.Add( r6file->Get("SysIncunfold_2025") );
  r6SysUnfold.Add( r6file->Get("SysIncunfold_3040") );
  // r6SysUnfold.Add( r6file->Get("SysIncunfold_1520") );
  // r6SysUnfold.Add( r6file->Get("SysIncunfold_2530") );
  // r6SysUnfold.Add( r6file->Get("SysIncunfold_4060") );

  TObjArray r6SysRest( 3 );
  r6SysRest.Add( r6file->Get("Incunfold_1015_2_minmax") );
  r6SysRest.Add( r6file->Get("Incunfold_2025_2_minmax") );
  r6SysRest.Add( r6file->Get("Incunfold_3040_2_minmax") );
  // r6SysRest.Add( r6file->Get("Incunfold_1520_2_minmax") );
  // r6SysRest.Add( r6file->Get("Incunfold_2530_2_minmax") );
  // r6SysRest.Add( r6file->Get("Incunfold_4060_2_minmax") );

  s="r6";
  for (int i=0 ; i<r6histos.GetEntries() ; ++i )    {h = (TH1D*) r6histos.At(i); h->SetName( s+h->GetName() );}
  for (int i=0 ; i<r6SysUnfold.GetEntries() ; ++i ) {h = (TH1D*) r6SysUnfold.At(i); h->SetName( s+h->GetName() );}
  for (int i=0 ; i<r6SysRest.GetEntries() ; ++i )   {h = (TH1D*) r6SysRest.At(i); h->SetName( s+h->GetName() );}

  

  // Number of PADS
  const Int_t Nx = 3;
  const Int_t Ny = 1;

  if (r12histos.GetEntries()!=Nx*Ny || r6histos.GetEntries()!=Nx*Ny ) {
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

  // // // Good for Nx=3, Ny=1, 300,290;
  // float lMargin = 0.07;
  // float rMargin = 0.03;
  // float bMargin = 0.22;
  // float tMargin = 0.05;
  // float xloffset=0.02;
  // float xtoffset=1.25;
  // int   ndivx= 505; 
  // float yloffset=0.02;
  // float ytoffset=0.85;
  // int   ndivy= 505; 
  // float xtickbase = 0.04;
  // float ytickbase = 0.04;

  float legs = 20;

  // Canvas setup
  // TCanvas* C =new TCanvas("C","", Nx*300,Ny*290);
  // CanvasPartition(C,Nx,Ny,lMargin,rMargin,bMargin,tMargin);
  // TPad *pad[Nx][Ny];
  TCanvas* C =new TCanvas("C","", Nx*300,Ny*2*240);
  CanvasPartition(C,Nx,2*Ny,lMargin,rMargin,bMargin,tMargin);
  TPad *pad[Nx][2*Ny];

  int histcount=0;
  
  // Set up the pads previously created.
  // for (Int_t i=0;i<Nx;i++) {
  //   for (Int_t j=0;j<Ny;j++) {
  for (Int_t i=0;i<Nx;i++) {
    for (Int_t j=0;j<2*Ny;j++) {
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
  
  // Load Pythia8 and Hard Probes comparison
  TFile* fHP = new TFile( "~/BasicAj/AjResults/UnfoldedPpSystematics_Histos.root", "READ");
  // TFile* fP8 = new TFile( "~/BasicAj/AjResults/UnfoldedPpSystematics_Histos.root", "READ");
  TFile* fP8 = new TFile( "Results/CutUp_ForPaper_Pythia8_NoEff_NoBgSub.root", "READ");

  TLegend* leg;
  TString s;
  
  // And draw. Reordered.
  // for (Int_t i=0;i<Nx;i++) {
  //   for (Int_t j=Ny-1;j>=0;j--) {
  for (Int_t j=Ny-1;j>=0;j--) {
    for (Int_t i=0;i<Nx;i++) {
      C->cd(0);
      // pad[i][j]->cd();
      pad[i][j+1]->cd();
      TH1D* r12h = (TH1D*) r12histos.At(histcount);
      TH1D* r12sysunfold = (TH1D*) r12SysUnfold.At(histcount);
      TH1D* r12sysrest = (TH1D*) r12SysRest.At(histcount);
      TH1D* r12systot =  (TH1D*) r12sysrest->Clone( TString("tot")+r12sysrest->GetName() );
      for (int ii=1; ii<=r12systot->GetNbinsX() ; ++ii ){
	r12systot->SetBinError (ii, sqrt( pow( r12sysunfold->GetBinError(ii), 2) + pow( r12sysrest->GetBinError(ii), 2) ));
      }

      TH1D* r6h = (TH1D*) r6histos.At(histcount);
      TH1D* r6sysunfold = (TH1D*) r6SysUnfold.At(histcount);
      TH1D* r6sysrest = (TH1D*) r6SysRest.At(histcount);
      TH1D* r6systot =  (TH1D*) r6sysrest->Clone( TString("tot")+r6sysrest->GetName() );
      for (int ii=1; ii<=r6systot->GetNbinsX() ; ++ii ){
	r6systot->SetBinError (ii, sqrt( pow( r6sysunfold->GetBinError(ii), 2) + pow( r6sysrest->GetBinError(ii), 2) ));
      }
      
      char hname[16];
      sprintf(hname,"dummy_%i_%i",i,j);

      TH1D* dummy = new TH1D( hname,"", 20, 0.05, 0.55);
      dummy->GetXaxis()->SetTitle(r12h->GetXaxis()->GetTitle());
      dummy->GetYaxis()->SetTitle(r12h->GetYaxis()->GetTitle());
      dummy->Reset();
      dummy->SetLineColor(0);      dummy->SetLineWidth(0);
      dummy->Draw();

      // Size factors
      Float_t xFactor = pad[0][0]->GetAbsWNDC()/pad[i][j]->GetAbsWNDC();
      Float_t yFactor = pad[0][0]->GetAbsHNDC()/pad[i][j]->GetAbsHNDC();

      // y axis range
      // float yaxismin = 0.0001;
      // float yaxismax = 1.2*h->GetMaximum();
      // float yaxismin = 0;
      float yaxismin = -0.5;
      float yaxismax = 7;
      if ( r12name.Contains("NoRebin")) yaxismax = 8.5;

      SetupDummy ( dummy,
		   xFactor, yFactor,
		   yaxismin, yaxismax,
		   xloffset, xtoffset,
		   xtickbase, ndivx,
		   yloffset, ytoffset,
		   ytickbase, ndivy );

      r12h->SetAxisRange(0.1 + 0.001, 0.5 - 0.001);
	    
      r12systot->SetAxisRange(0.1 + 0.001, 0.5 - 0.001);
      r12systot->SetMarkerStyle(20);      r12systot->SetMarkerSize(0);      r12systot->SetLineWidth( 0 );
      r12systot->SetFillStyle(1001);    

      r12sysunfold->SetAxisRange(0.1 + 0.001, 0.5 - 0.001);
      r12sysunfold->SetMarkerStyle(20);      r12sysunfold->SetMarkerSize(0);      r12sysunfold->SetLineWidth( 0 );
      r12sysunfold->SetFillStyle(1001);    

      r12sysrest->SetAxisRange(0.1 + 0.001, 0.5 - 0.001);
      r12sysrest->SetMarkerStyle(20);      r12sysrest->SetMarkerSize(0);      r12sysrest->SetLineWidth( 0 );
      r12sysrest->SetFillStyle(3001);
      r12sysrest->SetFillColor( kGray+2 );

      float ptleft=0;
      float ptright=0;
      
  
      switch (histcount){
      case 0 :
	r12h->SetMarkerStyle( 20 );
	r12h->SetMarkerSize( 1 );
	r12h->SetMarkerColor( kBlack ); r12h->SetLineColor( kBlack );
	// r12sysunfold->SetFillColor( kGray );
	// r12systot->SetFillColor( kGray );
	// r12h->SetMarkerColor( kGray+3 ); r12h->SetLineColor( kGray+3 );
	r12sysunfold->SetFillColor( kGray );
	r12systot->SetFillColor( kGray );

	r6h->SetMarkerStyle( 24 );
	r6h->SetMarkerSize( 1 );
	// r6h->SetMarkerColor( kRed ); r6h->SetLineColor( kRed );
	// r6sysunfold->SetFillColor( kRed-10 );
	// r6systot->SetFillColor( kRed-10 );
	r6h->SetMarkerColor( kGreen+1 ); r6h->SetLineColor( kGreen+1 );
	r6sysunfold->SetFillColor( kGreen-10 );
	r6systot->SetFillColor( kGreen-10 );

	ptleft=10; ptright=15;	      
	break;
      case 1 :
	r12h->SetMarkerStyle( 29 );
	r12h->SetMarkerSize( 1.6 );
	r12h->SetMarkerColor( kBlue ); r12h->SetLineColor( kBlue );
	r12sysunfold->SetFillColor( kBlue-10 );
	r12systot->SetFillColor( kBlue-10 );
	// r12h->SetMarkerColor( kGray+3 ); r12h->SetLineColor( kGray+3 );
	// r12sysunfold->SetFillColor( kGray );
	// r12systot->SetFillColor( kGray );

	r6h->SetMarkerStyle( 30 );
	r6h->SetMarkerSize( 1.3 );
	r6h->SetMarkerColor( kGreen+1 ); r6h->SetLineColor( kGreen+1 );
	r6sysunfold->SetFillColor( kGreen-10 );
	r6systot->SetFillColor( kGreen-10 );

	ptleft=20; ptright=25;
	break;
      case 2 :
	r12h->SetMarkerStyle( 33 );
	r12h->SetMarkerSize( 1.6 );
	r12h->SetMarkerColor( kOrange+7 ); r12h->SetLineColor( kOrange+7 );
	r12sysunfold->SetFillColor( kOrange-9 );
	r12systot->SetFillColor( kOrange-9 );
	// r12h->SetMarkerColor( kGray+3 ); r12h->SetLineColor( kGray+3 );
	// r12sysunfold->SetFillColor( kGray );
	// r12systot->SetFillColor( kGray );

	r6h->SetMarkerStyle( 27 );
	r6h->SetMarkerSize( 1.5 );
	r6h->SetMarkerColor( kGreen+1 ); r6h->SetLineColor( kGreen+1 );
	r6sysunfold->SetFillColor( kGreen-10 );
	r6systot->SetFillColor( kGreen-10 );

	
	ptleft=30; ptright=40;
	break; 
      default :
	cerr << "Unsupported number of histos" << endl;
	return -1;
	break; // ROOT can be stupid
      }

      s="p_{T}="; s+=int(ptleft); s+="-"; s+=int(ptright); s+=" GeV/c";
      leg = new TLegend( 0.25, 3.6, 0.5, 6.7, "", "br" );
      leg->SetBorderSize(0);
      leg->SetTextFont(43);
      leg->SetTextSize(legs);
      leg->SetHeader(s);
  
      leg->SetFillStyle(0);
      // leg->SetMargin(0.1);

      r12h->SetLineWidth( 2 );
      r6h->SetLineWidth( 2 );

      r12systot->Draw("9e2same");
      r12h->Draw("9same");
      leg->AddEntry(r12h->GetName(), "Run 12 JP2","lp");

      r6systot->Draw("9e2same");
      r6h->Draw("9same");
      leg->AddEntry(r6h->GetName(), "Run 6 HT2","lp");

      leg->Draw();
      
      // Ratios
      // -------
      TH1D* statratio=(TH1D*) r12h->Clone(r12h->GetName()+TString("_statratio"));
      statratio->Divide(r6h);
      TH1D* systratio=(TH1D*) r12systot->Clone(r12systot->GetName()+TString("_systratio"));
      systratio->Divide(r6systot);
      
      C->cd(0);
      pad[i][j]->cd();


      sprintf(hname,"rdummy_%i_%i",i,j);
      TH1D* rdummy = new TH1D( hname,"", 20, 0.05, 0.55);
      rdummy->GetXaxis()->SetTitle(r12h->GetXaxis()->GetTitle());
      rdummy->GetYaxis()->SetTitle("Run12 / Run 6 ");
      rdummy->Reset();
      rdummy->SetLineColor(0);      rdummy->SetLineWidth(0);

      SetupDummy ( rdummy,
		   xFactor, yFactor,
		   yaxismin, yaxismax,
		   xloffset, xtoffset,
		   xtickbase, ndivx,
		   yloffset, 1.2*ytoffset,
		   ytickbase, ndivy );
	    
      rdummy->SetAxisRange(0.5,1.5, "y");
      rdummy->Draw("hist");

      TLine l;
      l.SetLineStyle(2);
      l.SetLineWidth(1);
      l.SetLineColor(kGray+2);
      l.DrawLine( 0.05, 1, 0.55,1);
      systratio->Draw("9e2same");
      statratio->Draw("9same");

      histcount++; 
    }    
  }
      


  C->cd();  
  // C->SaveAs(plotpath+"delme.pdf");
  // C->SaveAs(plotpath+"delme2.pdf");
  
  // TString plotname=gSystem->BaseName(r12name);
  TString plotname=gSystem->BaseName("Comparison");
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

