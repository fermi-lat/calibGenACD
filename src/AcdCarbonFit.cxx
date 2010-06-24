#define AcdCarbonFit_cxx

#include "AcdCarbonFit.h"

#include "AcdCalibEnum.h"
#include "AcdHistCalibMap.h"
#include "AcdGainFit.h"

#include "TF1.h"




Int_t AcdCarbonFitLibrary::fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder,
			       CalibData::AcdCalibObj* ref) {

  TH1& hist = const_cast<TH1&>(*(holder.getHist(0)));
 
  Int_t returnCode = CalibData::AcdCalibObj::NOFIT;
  if ( hist.GetEntries() < 30 ) return returnCode;
  
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
    returnCode = fitGauss(result,hist,ref);
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
  Int_t status = AcdGainFitLibrary::extractFeatures(hist,4,ped,min,peak,halfMax);
  if ( status != 0 ) return CalibData::AcdCalibObj::PREFIT_FAILED;
  //Float_t pedValue = hist.GetBinCenter(4*ped);
  //Float_t minValue = hist.GetBinCenter(4*min);
  Float_t peakValue = hist.GetBinCenter(4*peak);
  Float_t endValue = hist.GetBinCenter(4*halfMax);

  Float_t width = endValue - peakValue;
  result.setVals(peakValue,width,CalibData::AcdCalibObj::USED_FALLBACK_1);
  return CalibData::AcdCalibObj::OK;
}

Int_t AcdCarbonFitLibrary::fitGauss(CalibData::AcdCalibObj& result, const TH1& hist, const CalibData::AcdCalibObj* seed) {

  Int_t ped, min, peak, halfMax;
  Int_t rebin(4);
  Int_t status(0);

  TH1& theHist = const_cast<TH1&>(hist);

  Float_t valAtMax(0.);
  Float_t peakValue(0.);
  Float_t minValue(0.);
  Float_t endValue(0.);
  Float_t width(0.);

  if ( seed == 0 ) {
    status = AcdGainFitLibrary::extractFeatures(hist,rebin,ped,min,peak,halfMax);
    if ( status != 0 ) return status;
    valAtMax = hist.GetBinContent(rebin*peak);
    peakValue = hist.GetBinCenter(rebin*peak);
    minValue = hist.GetBinCenter(rebin*min);
    endValue = hist.GetBinCenter(rebin*halfMax);
    width = 0.5*(endValue-minValue);
  } else {
    peakValue = seed->operator[](0);
    width = seed->operator[](1);
    valAtMax = hist.GetMaximum();
    std::cout << "Using ref " << peakValue << ' ' << width << std::endl;
    minValue = TMath::Max(20., peakValue - width);
    minValue = TMath::Min(minValue, 0.5* peakValue);
    endValue = TMath::Min(3*peakValue, 1400.);
  }

  TF1 gauss("gauss","[0] * TMath::Gaus(x,[1],[2])",minValue,endValue);
  gauss.SetParameter(0,valAtMax);
  gauss.SetParameter(1,peakValue);
  gauss.SetParLimits(1,0.5*peakValue,2.*peakValue);
  gauss.SetParameter(2,width);
  gauss.SetParLimits(2,0.2*width,2*width);


  status = theHist.Fit(&gauss,"","",minValue,endValue); //applies polynomial fit
  // if ( status != 0 ) return fallback(result,hist);
  if ( status != 0 ) return status;
  
  peakValue = gauss.GetParameter(1);
  width = gauss.GetParameter(2);
  result.setVals(peakValue,width,(CalibData::AcdCalibObj::STATUS)status);
  return status;
}
