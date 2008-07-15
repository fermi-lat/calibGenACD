#define AcdCnoFit_cxx

#include "AcdCnoFit.h"
#include "AcdHistCalibMap.h"

#include "TF1.h"


Int_t AcdCnoFitLibrary::findFirstBinAboveVal(const TH1& hist, Float_t val) {
  UInt_t nB = hist.GetNbinsX();
  // skip the first seven bins...
  for ( UInt_t i(8); i <= nB; i++ ) {
    if ( hist.GetBinContent(i) > val ) {
      return (Int_t)i;
    }
  }
  return -1;   
}

Int_t AcdCnoFitLibrary::fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder) {

  TH1& hist = const_cast<TH1&>(*(holder.getHist(0)));
  
  Int_t returnCode = CalibData::AcdCalibObj::NOFIT;
  switch ( _type ) {
  case None:
    result.setVals(0.,0.,CalibData::AcdCalibObj::NOFIT);
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

Int_t AcdCnoFitLibrary::counting(CalibData::AcdCalibObj& result, const TH1& hist) {

  //TH1& theHist = const_cast<TH1&>(hist);
  
  Int_t i_25 = findFirstBinAboveVal(hist,0.25);
  Float_t f_25 = hist.GetBinCenter(i_25);
  Int_t i_95 = findFirstBinAboveVal(hist,0.75);
  Float_t f_95 = hist.GetBinCenter(i_95);
  Float_t width = f_25 - f_95;
  result.setVals(f_95,width,CalibData::AcdCalibObj::OK);
  return CalibData::AcdCalibObj::OK;
}


Int_t AcdCnoFitLibrary::fitErf(CalibData::AcdCalibObj& /* result */, const TH1& /* hist */) {
  
  return CalibData::AcdCalibObj::NOFIT;

}


