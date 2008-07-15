#define AcdCarbonFit_cxx

#include "AcdCarbonFit.h"

#include "AcdCalibEnum.h"
#include "AcdHistCalibMap.h"
#include "AcdGainFit.h"

#include "TF1.h"




Int_t AcdCarbonFitLibrary::fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder) {

  TH1& hist = const_cast<TH1&>(*(holder.getHist(0)));
 
  Int_t returnCode = CalibData::AcdCalibObj::NOFIT;
  if ( hist.GetEntries() < 100 ) return returnCode;
  
  switch ( _type ) {
  case None:
    result.setVals(0.,0.,CalibData::AcdCalibObj::NOFIT);
    break;
  case Stats:
    returnCode = stats(result,hist);
    break;
  case Fallback:
    returnCode = fallback(result,hist);
    break;
  case Gauss:
    returnCode = fitGauss(result,hist);
    break;
  }  
  return returnCode;
}


Int_t AcdCarbonFitLibrary::stats(CalibData::AcdCalibObj& result, const TH1& hist) {

  TH1& theHist = const_cast<TH1&>(hist);

  float ave = theHist.GetMean();
  float rms = theHist.GetRMS();
  result.setVals(ave,rms,CalibData::AcdCalibObj::USED_FALLBACK_2);
  return CalibData::AcdCalibObj::OK;
}


Int_t AcdCarbonFitLibrary::fallback(CalibData::AcdCalibObj& result, const TH1& hist) {
  Int_t ped, min, peak, halfMax;
  Int_t status = AcdGainFitLibrary::extractFeatures(_pedRemove,hist,4,ped,min,peak,halfMax);
  if ( status != 0 ) return CalibData::AcdCalibObj::PREFIT_FAILED;
  //Float_t pedValue = hist.GetBinCenter(4*ped);
  //Float_t minValue = hist.GetBinCenter(4*min);
  Float_t peakValue = hist.GetBinCenter(4*peak);
  Float_t endValue = hist.GetBinCenter(4*halfMax);

  Float_t width = endValue - peakValue;
  result.setVals(peakValue,width,CalibData::AcdCalibObj::USED_FALLBACK_1);
  return CalibData::AcdCalibObj::OK;
}

Int_t AcdCarbonFitLibrary::fitGauss(CalibData::AcdCalibObj& result, const TH1& hist) {

  Int_t ped, min, peak, halfMax;
  Int_t rebin(4);
  Int_t status = AcdGainFitLibrary::extractFeatures(_pedRemove,hist,rebin,ped,min,peak,halfMax);

  TH1& theHist = const_cast<TH1&>(hist);

  if ( status != 0 ) return status;
  Float_t peakValue = hist.GetBinCenter(rebin*peak);
  Float_t minValue = hist.GetBinCenter(rebin*min);
  Float_t endValue = hist.GetBinCenter(rebin*halfMax);
  Float_t integ = hist.Integral(minValue,endValue);

  TF1 gauss("gauss","[0] * TMath::Gaus(x,[1],[2])",minValue,endValue);
  gauss.SetParameter(0,integ);
  gauss.SetParameter(1,peakValue);
  gauss.SetParLimits(1,minValue,endValue);
  gauss.SetParameter(2,endValue-minValue);

  status = theHist.Fit(&gauss,"","",minValue,endValue); //applies polynomial fit
  // if ( status != 0 ) return fallback(result,hist);
  if ( status != 0 ) return status;
  
  peakValue = gauss.GetParameter(1);
  Double_t width = gauss.GetParameter(2);
  result.setVals(peakValue,width,(CalibData::AcdCalibObj::STATUS)status);
  return status;
}
