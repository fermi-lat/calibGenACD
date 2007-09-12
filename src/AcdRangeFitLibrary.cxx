#define AcdRangeFitLibrary_cxx
#include "AcdRangeFitLibrary.h"
#include "AcdHistCalibMap.h"

#include "TF1.h"

ClassImp(AcdRangeFitLibrary) ;


Int_t AcdRangeFitLibrary::fit(AcdRangeFitResult& result, const TH1& lowhist, const TH1& highhist) {  
  Int_t returnCode = AcdRangeFitResult::NOFIT;
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
  returnCode = AcdRangeFitResult::OK;
  result.setVals(lowRange,hiRange,AcdRangeFitResult::OK);
  return returnCode;
}




