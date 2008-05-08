#define AcdHighRangeFit_cxx

#include "AcdHighRangeFit.h"
#include "AcdCalibEnum.h"

#include "AcdHistCalibMap.h"

#include <TF1.h>


Int_t AcdHighRangeFitLibrary::fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder) {

  TH1& in = *(holder.getHist(0));

  TF1 calib("calib","[0]*[1]*x/([0]*x + [1])");

  calib.SetParLimits(0,0.,10.);
  calib.SetParLimits(1,100.,4000.);
  calib.SetParameter(0,1.);
  calib.SetParameter(1,1000.);

  TH1& nch = const_cast<TH1&>(in);
  Int_t status = nch.Fit(&calib);
  
  Float_t ped = result[0];
  Float_t slope = calib.GetParameter(0);
  Float_t satur = calib.GetParameter(1);
  result.setVals(ped,slope,satur,(CalibData::AcdCalibObj::STATUS)status);
  return result.getStatus();
}


