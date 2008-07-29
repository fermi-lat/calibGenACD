#define AcdHighRangeFit_cxx

#include "AcdHighRangeFit.h"
#include "AcdCalibEnum.h"

#include "AcdHistCalibMap.h"

#include <TF1.h>


Int_t AcdHighRangeFitLibrary::fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder,
				  CalibData::AcdCalibObj* /* ref */) {

  TH1& in = *(holder.getHist(0));
  std::cout << "AcdHighRangeFitLibrary::fit " << in.GetUniqueID() << std::endl;

  TF1 calib("hrCalib","[0] + ([1]*[2]*x/([1]*x + [2]))");

  Float_t ped = in.GetBinContent(1);
  calib.FixParameter(0,ped);
  calib.SetParLimits(1,0.,10.);
  calib.SetParameter(1,5.);
  calib.FixParameter(2,2000.);

  TH1& nch = const_cast<TH1&>(in);
  Int_t status = nch.Fit(&calib,"W","");

  Float_t slope = calib.GetParameter(1);
  Float_t satur = calib.GetParameter(2);
  result.setVals(ped,slope,satur,(CalibData::AcdCalibObj::STATUS)status);
  return result.getStatus();
}


