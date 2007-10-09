#define AcdRangeFit_cxx

#include "AcdRangeFit.h"

#include "AcdCalibEnum.h"
#include "AcdHistCalibMap.h"

#include "TF1.h"



Int_t AcdRangeFitLibrary::fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder) {

  TH1& lowhist = const_cast<TH1&>(*(holder.getHist(0)));
  TH1& highhist = const_cast<TH1&>(*(holder.getHist(1)));
 
  Int_t returnCode = CalibData::AcdCalibObj::NOFIT;
  if ( _type == None ) {
    return returnCode;
  }
  UInt_t nB = lowhist.GetNbinsX();
  UInt_t hiRange(0);
  for ( UInt_t i(1); i <= nB; i++ ) {
    if ( highhist.GetBinContent(i) > 0 ) {
      hiRange = i;
      break;
    }
  }
  UInt_t lowRange(0);
  for ( UInt_t j(nB); j > 0; j-- ) {
    if ( lowhist.GetBinContent(j) > 0 ) {
     lowRange = j;
     break;
    }
  }
  returnCode = CalibData::AcdCalibObj::OK;
  result.setVals(lowRange,hiRange,CalibData::AcdCalibObj::OK);
  return returnCode;
}




