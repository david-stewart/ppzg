
namespace zgconsts{

  // Binning
  // -------
  const int nIncbins = 6;
  double Incbins[nIncbins+1] = { 10, 15, 20, 25, 30, 40, 60};
  
  Color_t datacol[nIncbins] = {
    kBlack,
    kRed,
    kBlue,
    kGreen+1,
    kOrange+7,
    kMagenta+1
  };

  Marker_t datamark[nIncbins] = {
    20,
    27,
    29,
    24,
    33,
    30
  };

  Float_t datamarksize[nIncbins] = {
    1,
    1.5,
    1.6,
    1,
    1.6,
    1.3
  };
  
  Color_t syscol[nIncbins] = {
    kGray,
    kRed-10,
    kBlue-10,
    kGreen-10,
    kOrange-9,
    kMagenta-10
  };
    
    
};



