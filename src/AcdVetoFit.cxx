#define AcdVetoFit_cxx

#include "AcdVetoFit.h"

#include "AcdHistCalibMap.h"
#include "TF1.h"



Int_t AcdVetoFitLibrary::findFirstBinAboveVal(const TH1& hist, Float_t val) {
  UInt_t nB = hist.GetNbinsX();
  // skip the first bin...
  for ( UInt_t i(2); i <= nB; i++ ) {
    if ( hist.GetBinContent(i) > val ) {
      return (Int_t)i;
    }
  }
  return -1;   
}

Int_t AcdVetoFitLibrary::fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder,
			     CalibData::AcdCalibObj* /* ref */ ) {

  TH1& hist = const_cast<TH1&>(*(holder.getHist(2)));
  
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

Int_t AcdVetoFitLibrary::counting(CalibData::AcdCalibObj& result, const TH1& hist) {

  //TH1& theHist = const_cast<TH1&>(hist);
  
  Int_t i_10 = findFirstBinAboveVal(hist,0.1);
  Float_t f_10 = hist.GetBinCenter(i_10);
  Int_t i_50 = findFirstBinAboveVal(hist,0.5);
  Float_t f_50 = hist.GetBinCenter(i_50);
  Float_t width = f_50 - f_10;
  result.setVals(f_50,width,CalibData::AcdCalibObj::OK);
  return CalibData::AcdCalibObj::OK;
}


Int_t AcdVetoFitLibrary::fitErf(CalibData::AcdCalibObj& /* result */, const TH1& /* hist */) {
  
  return CalibData::AcdCalibObj::NOFIT;

}


