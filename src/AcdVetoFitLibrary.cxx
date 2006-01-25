#define AcdVetoFitLibrary_cxx
#include "AcdVetoFitLibrary.h"
#include "AcdHistCalibMap.h"

#include "TF1.h"

ClassImp(AcdVetoFitLibrary) ;


Int_t AcdVetoFitLibrary::findFirstBinAboveVal(const TH1& hist, Float_t val) {
  UInt_t nB = hist.GetNbinsX();
  for ( UInt_t i(1); i <= nB; i++ ) {
    if ( hist.GetBinContent(i) > val ) {
      return (Int_t)i;
    }
  }
  return -1;   
}

Int_t AcdVetoFitLibrary::fit(AcdVetoFitResult& result, const TH1& hist) {
  
  Int_t returnCode = AcdVetoFitResult::NOFIT;
  switch ( _type ) {
  case None:
    result.setVals(0.,0.,AcdVetoFitResult::NOFIT,_type);
    break;
  case Counting:
    returnCode = counting(result,hist);
    break;
  case Erf:
    returnCode = fitErf(result,hist);
    break;
  }  
  return returnCode;
}

Int_t AcdVetoFitLibrary::counting(AcdVetoFitResult& result, const TH1& hist) {

  TH1& theHist = const_cast<TH1&>(hist);
  
  Int_t i_10 = findFirstBinAboveVal(hist,0.1);
  Float_t f_10 = hist.GetBinCenter(i_10);
  Int_t i_50 = findFirstBinAboveVal(hist,0.5);
  Float_t f_50 = hist.GetBinCenter(i_50);
  Float_t width = f_50 - f_10;
  result.setVals(f_50,width,AcdVetoFitResult::OK,_type);
  return AcdVetoFitResult::OK;
}


Int_t AcdVetoFitLibrary::fitErf(AcdVetoFitResult& result, const TH1& hist) {
  
  return AcdVetoFitResult::OK;

}


