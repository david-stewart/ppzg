{
  // gSystem->Load("./libMuMCEvent.so");

  // Style options
  gROOT->ForceStyle();
  gROOT->SetStyle("Plain");// Default white background for all plots
  gStyle->SetPalette(1);

  // The modes below are provided by Nick van Eijndhoven <Nick@phys.uu.nl>
  // from Alice.
  gStyle->SetCanvasColor(10);
  gStyle->SetStatColor(10);
  gStyle->SetTitleFillColor(10);
  gStyle->SetPadColor(10);

  // Settings for statistics information
  gStyle->SetOptFit(0);
  // gStyle->SetOptStat(0);

  // SetPaperSize wants width & height in cm: A4 is 20,26 & US is 20,24
  gStyle->SetPaperSize(20,24);

  // Positioning of axes labels
  gStyle->SetTitleOffset(1.2);
  // grid
  gStyle->SetPadGridX(0);
  gStyle->SetPadGridY(0);

  //  Set date/time for plot
  gStyle->SetOptDate(0);
  
  // To read and deal with TStarJetPicoDst's
  Int_t ierr = 0;
  const char *clibs[] = {
    "libPhysics.so",
    // "libRIO.so", // Unnecessary? Throws weird errors
    "libHist.so",
    "libEG.so",
    "libTree.so",    
    "$STARPICOPATH/libTStarJetPico.so",
    0
  };



  Int_t i = 0;
  while ( clibs[i++] ) {
    ierr = gSystem->Load(clibs[i-1]);
    if ( ierr != 0) {
      cerr <<  "Unable to load " << clibs[i-1] << endl;
    }
  }

  TString STARPICOPATH=gSystem->Getenv("STARPICOPATH");
  gSystem->AddIncludePath(" -I" + STARPICOPATH);

  TString __ROOUNFOLD=gSystem->Getenv("ROOUNFOLD");
  if (__ROOUNFOLD!=""){
    gSystem->AddIncludePath(" -I" + __ROOUNFOLD + "/src");
    gInterpreter->AddIncludePath(__ROOUNFOLD + "/src");
    gSystem->Load("$ROOUNFOLD/libRooUnfold.so");
  }


}
