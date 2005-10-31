#define AcdPedestalFitLibrary_cxx
#include "AcdPedestalFitLibrary.h"
#include "AcdHistCalibMap.h"


ClassImp(AcdPedestalFitLibrary) ;


UInt_t AcdPedestalFitLibrary::fit(AcdPedestalFitResult& result, const TH1& hist) {
  
  UInt_t returnCode(0);
  switch ( _type ) {
  case None:
    result.setVals(0.,0.,0);
    break;
  case PeakValue:
    returnCode = fitPeak(result,hist);
    break;
  case MeanValue:
    returnCode = fitMean(result,hist);  
    break;
  }
  
  return returnCode;
}

UInt_t AcdPedestalFitLibrary::fitMean(AcdPedestalFitResult& result, const TH1& hist) {
  TH1& theHist = const_cast<TH1&>(hist);
  float av = theHist.GetMean(); 
  float rms = theHist.GetRMS();
  theHist.SetAxisRange(av-5*rms,av+5*rms);
  av = theHist.GetMean(); rms = theHist.GetRMS();
  theHist.SetAxisRange(av-5*rms,av+5*rms);
  av = theHist.GetMean(); rms = theHist.GetRMS();
  result.setVals(av,rms,1);
  return 1;
}


UInt_t AcdPedestalFitLibrary::fitPeak(AcdPedestalFitResult& result, const TH1& hist) {
  Long64_t maxbin = hist.GetMaximumBin(); //finds tallest bin
  float pedestal = hist.GetBinCenter(maxbin); 
  result.setVals(pedestal,0.,1);
  return 1;
}
