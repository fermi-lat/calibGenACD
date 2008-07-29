#define AcdDacFit_cxx

#include "AcdDacFit.h"

#include "AcdHistCalibMap.h"
#include <TF1.h>


Int_t AcdDacFitLibrary::fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder,
			    CalibData::AcdCalibObj* /* ref */ ) {
  
  Int_t returnCode(-1);
  switch ( _type ) {
  case None:
    result.setVals(0.,0.,CalibData::AcdCalibObj::NOFIT);
    break;
  case Linear:
    returnCode = fitLinear(result,holder);
    break;
  }
  
  return returnCode;
}

Int_t AcdDacFitLibrary::fitLinear(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder) {
  TH1& theHist = const_cast<TH1&>(*(holder.getHist(0)));  
  TF1 pol1("pol1","[0] +  x * [1]");
  pol1.SetParameter(0,64.);
  pol1.SetParameter(1,0.003);
  Int_t status = theHist.Fit(&pol1);  
  Double_t offset = pol1.GetParameter(0);
  Double_t slope = pol1.GetParameter(1);
  result.setVals(slope,offset,(CalibData::AcdCalibObj::STATUS)status);
  return status;
}

