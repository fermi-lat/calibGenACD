#define AcdRangeFit_cxx

#include "AcdRangeFit.h"

#include "AcdCalibEnum.h"
#include "AcdHistCalibMap.h"

#include "TF1.h"

ClassImp(AcdRangeFitDesc) ;

const std::string AcdRangeFitDesc::s_calibType("ACD_Range");
const std::string AcdRangeFitDesc::s_txtFormat("TILE PMT LOW_MAX HIGH_MIN STATUS");

const AcdRangeFitDesc& AcdRangeFitDesc::ins() {
  static const AcdRangeFitDesc desc;
  return desc;
}

AcdRangeFitDesc::AcdRangeFitDesc()
  :AcdCalibDescription(AcdCalib::RANGE,s_calibType,s_txtFormat){
  addVarName("low_max");
  addVarName("high_min");
}


ClassImp(AcdRangeFitLibrary) ;


Int_t AcdRangeFitLibrary::fit(AcdCalibResult& result, const AcdCalibHistHolder& holder) {

  TH1& lowhist = const_cast<TH1&>(*(holder.getHist(0)));
  TH1& highhist = const_cast<TH1&>(*(holder.getHist(1)));
 
  Int_t returnCode = AcdCalibResult::NOFIT;
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
  returnCode = AcdCalibResult::OK;
  result.setVals(lowRange,hiRange,AcdCalibResult::OK);
  return returnCode;
}




