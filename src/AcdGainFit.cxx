#define AcdGainFit_cxx

#include "AcdGainFit.h"

#include "AcdCalibEnum.h"

#include "AcdHistCalibMap.h"

#include "TMath.h"
#include "TF1.h"

#include "TSystem.h"
#include "TMinuit.h"
#include "TClass.h"

Int_t AcdGainFitLibrary::findLowestNonZeroBin(const TH1& hist) {
  UInt_t nB = hist.GetNbinsX();
  for ( UInt_t i(1); i <= nB; i++ ) {
    if ( hist.GetBinContent(i) > 0.5 ) {
      return (Int_t)i;
    }
  }
  return -1;   
}

Int_t AcdGainFitLibrary::findNextLocalMax(const TH1& hist, Int_t startBin) {
  UInt_t nB = hist.GetNbinsX();
  Float_t lastValue = hist.GetBinContent(startBin);
  for ( UInt_t i = startBin+1; i <= nB; i++ ) {
    Float_t curValue = hist.GetBinContent(i);
    if ( curValue < lastValue ) {      
      return (Int_t)i;
    }
    lastValue = curValue;
  }
  return -1;
}


Int_t AcdGainFitLibrary::findNextLocalMin(const TH1& hist, Int_t startBin) {
  UInt_t nB = hist.GetNbinsX();
  Float_t lastValue = hist.GetBinContent(startBin);
  for ( UInt_t i = startBin+1; i <= nB; i++ ) {
    Float_t curValue = hist.GetBinContent(i);
    if ( curValue > lastValue ) {      
      return (Int_t)i;
    }
    lastValue = curValue;
  }
  return -1;
}

Int_t AcdGainFitLibrary::findHalfMaxHigh(const TH1& hist, Int_t maxBin) {
  UInt_t nB = hist.GetNbinsX();
  Float_t refVal = hist.GetBinContent(maxBin);
  refVal /= 2;
  for ( UInt_t i = maxBin+1; i <= nB; i++ ) {
    Float_t curValue = hist.GetBinContent(i);
    if ( curValue < refVal ) {
      return (Int_t)i;
    }
  }
  return -1;
}

Int_t AcdGainFitLibrary::findHalfMaxLow(const TH1& hist, Int_t maxBin) {
  Float_t refVal = hist.GetBinContent(maxBin);
  refVal /= 2;
  for ( UInt_t i = maxBin-1; i > 0; i-- ) {
    Float_t curValue = hist.GetBinContent(i);
    if ( curValue < refVal ) {
      return (Int_t)i;
    }
  }
  return -1;
}

Int_t AcdGainFitLibrary::findMaxAfter(const TH1& hist, Int_t startBin) {
  Int_t nB = hist.GetNbinsX();
  Float_t maxVal = hist.GetBinContent(startBin);
  Int_t maxBin(startBin);
  for ( Int_t i = startBin; i < nB; i++ ) {
    Float_t curValue = hist.GetBinContent(i);
    if ( curValue > maxVal ) {
      maxVal = curValue;
      maxBin = i;
    }
  }
  return maxBin;
}


Int_t AcdGainFitLibrary::extractFeatures(const TH1& hist, Int_t rebin, 
					 Int_t& ped, Int_t& min, Int_t& peak, Int_t& halfMax) {

  TH1F copy((TH1F&)hist);
  copy.Rebin(rebin);

  ped = 0;
  peak = findMaxAfter(copy,2);
  if ( peak < 0 ) return CalibData::AcdCalibObj::PREFIT_FAILED;
  min = findHalfMaxLow(copy,peak);
  if ( min < 0 ) min = 1;    
  halfMax = findHalfMaxHigh(copy,peak);
  halfMax = halfMax < 0 ? copy.GetNbinsX() : halfMax;
  return 0;
}


Int_t AcdGainFitLibrary::fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder,
			     CalibData::AcdCalibObj* ref ) {
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
  case Landau:
    returnCode = fitLandau(result,hist);
    break;
  case P7:
    returnCode = fitP7(result,hist);
    break;
  case P3:
    returnCode = fitP3(result,hist);
    break;
  case LogNormal:
    returnCode = fitLogNormal(result,hist);
    break;
  case GaussP1:
    returnCode = fitGaussP1(result,hist,ref);
    break;
  }  
  return returnCode;
}

Int_t AcdGainFitLibrary::stats(CalibData::AcdCalibObj& result, const TH1& hist) {

  TH1& theHist = const_cast<TH1&>(hist);

  float ave = theHist.GetMean();
  float rms = theHist.GetRMS();
  result.setVals(ave,rms,CalibData::AcdCalibObj::USED_FALLBACK_2);
  return CalibData::AcdCalibObj::OK;
}


Int_t AcdGainFitLibrary::fallback(CalibData::AcdCalibObj& result, const TH1& hist) {
  Int_t ped, min, peak, halfMax;
  Int_t status = extractFeatures(hist,4,ped,min,peak,halfMax);

  if ( status != 0 ) return CalibData::AcdCalibObj::PREFIT_FAILED;
  //Float_t pedValue = hist.GetBinCenter(4*ped);
  //Float_t minValue = hist.GetBinCenter(4*min);
  Float_t peakValue = hist.GetBinCenter(4*peak);
  Float_t endValue = hist.GetBinCenter(4*halfMax);

  Float_t width = endValue - peakValue;

  result.setVals(peakValue,width,CalibData::AcdCalibObj::USED_FALLBACK_1);
  return CalibData::AcdCalibObj::OK;

}

Int_t AcdGainFitLibrary::fitLandau(CalibData::AcdCalibObj& result, const TH1& hist) {

  TH1& theHist = const_cast<TH1&>(hist);

  float ave = theHist.GetMean();
  float rms = theHist.GetRMS();
  Int_t status = theHist.Fit("landau", "", "", ave-2*rms, ave+3*rms);

  if ( status != 0 ) return fallback(result,hist);

  double* par = (theHist.GetFunction("landau"))->GetParameters();
  float mean = *(par+1); 
  float sigma = *(par+2);
  result.setVals(mean,sigma,(CalibData::AcdCalibObj::STATUS)status);
  return status;
}


Int_t AcdGainFitLibrary::fitP7(CalibData::AcdCalibObj& result, const TH1& hist) {

  Int_t ped, min, peak, halfMax;
  Int_t status = extractFeatures(hist,4,ped,min,peak,halfMax);

  TH1& theHist = const_cast<TH1&>(hist);

  if ( status != 0 ) return status;
  //Float_t pedValue = hist.GetBinCenter(4*ped);
  Float_t minValue = hist.GetBinCenter(4*min);
  Float_t endValue = hist.GetBinCenter(4*halfMax);

  status = theHist.Fit("pol7","","",minValue,endValue); //applies polynomial fit
  if ( status != 0 ) return fallback(result,hist);

  TF1* fit = theHist.GetFunction("pol7"); 
 
  Float_t peakValue = fit->GetMaximumX();
  Float_t width = endValue - peakValue;

  result.setVals(peakValue,width,(CalibData::AcdCalibObj::STATUS)status);
  return status;
}


Int_t AcdGainFitLibrary::fitP3(CalibData::AcdCalibObj& result, const TH1& hist) {

  Int_t ped, min, peak, halfMax;
  Int_t status = extractFeatures(hist,4,ped,min,peak,halfMax);

  TH1& theHist = const_cast<TH1&>(hist);

  if ( status != 0 ) return status;
  //Float_t pedValue = hist.GetBinCenter(4*ped);
  Float_t minValue = hist.GetBinCenter(4*min);
  Float_t endValue = hist.GetBinCenter(4*halfMax);

  status = theHist.Fit("pol3","","",minValue,endValue); //applies polynomial fit
  // if ( status != 0 ) return fallback(result,hist);
  if ( status != 0 ) return status;

  TF1* fit = theHist.GetFunction("pol3"); 
 
  Float_t peakValue = fit->GetMaximumX();
  Float_t width = endValue - peakValue;

  result.setVals(peakValue,width,(CalibData::AcdCalibObj::STATUS)status);
  return status;
}


Int_t AcdGainFitLibrary::fitLogNormal(CalibData::AcdCalibObj& result, const TH1& hist) {
  
  Int_t ped, min, peak, halfMax;
  Int_t status = extractFeatures(hist,4,ped,min,peak,halfMax);

  if ( status != 0 ) return status;
  //Float_t pedValue = hist.GetBinCenter(4*ped);
  Float_t minValue = hist.GetBinCenter(4*min);
  Float_t endValue = hist.GetBinCenter(4*halfMax);
  Float_t integ = hist.Integral(4*min,4*halfMax);
      
  TF1 logNorm("logNorm","[0] * TMath::LogNormal(x,[1],[2],[3])",minValue,endValue);
  logNorm.SetParameter(0,integ);
  logNorm.SetParameter(1,0.5);
  logNorm.SetParLimits(1,0.,10.);
  logNorm.SetParameter(2,ped);
  logNorm.SetParLimits(2,0.,min-1.);
  logNorm.SetParameter(3,450.);
  logNorm.SetParLimits(3,20.,3000.);
  
  TH1& nch = const_cast<TH1&>(hist);
  status = nch.Fit(&logNorm,"","",minValue,endValue);

  if ( status != 0 ) return fallback(result,hist);

  Double_t peakValue = logNorm.GetMaximumX();
  
  Double_t sigma = logNorm.GetParameter(1);

  Double_t sigma2 = sigma*sigma;
  Double_t es2 = exp(sigma2);

  Double_t m = logNorm.GetParameter(3);
  
  Double_t width = m * es2 * ( es2 - 1 );
  result.setVals(peakValue,width,(CalibData::AcdCalibObj::STATUS)status);

  return status;

}



Int_t AcdGainFitLibrary::fitGaussP1(CalibData::AcdCalibObj& result, const TH1& hist, const CalibData::AcdCalibObj* seed) {
  
  Float_t peak(0.);
  Float_t width(0.);
  Float_t minVal(0.);
  Float_t maxVal(0.);
  if ( seed == 0 ) {
    peak = 1.;
    width = 0.25;
    minVal = 0.25;
    maxVal = 2.0;     
  } else {    
    peak = seed->operator[](0);
    width = seed->operator[](1);   
    minVal = TMath::Max(20., peak - width);
    minVal = TMath::Min(minVal, 0.5* peak);
    maxVal = 4000.;
  }  
  Float_t norm = hist.GetMaximum();
      
  std::cout << "Fit Gauss P1 " << hist.GetUniqueID() << std::endl;  

  static Bool_t first(kTRUE);
  if ( first ) {
    std::cout << "Init" << std::endl;
    TMinuit minuit;
    std::cout << minuit.IsA()->GetName() << std::endl;
    gSystem->Load("libMinuit");
    gSystem->ListLibraries();
    first = kFALSE;
  }

  TF1 gaussP1("gaussP1","[0] * (TMath::Gaus(x,[1],[2]) + [3] + [4] * x )",minVal,maxVal);
  gaussP1.SetParameter(0,norm);  
  gaussP1.SetParameter(1,peak);
  // gaussP1.SetParLimits(1,0.85*peak,1.15*peak);
  gaussP1.SetParLimits(1,0.5*peak,3*peak);
  gaussP1.SetParameter(2,width);
  gaussP1.SetParLimits(2,0.85*width,1.15*width);
  // gaussP1.SetParLimits(2,50.,2*width);
  gaussP1.SetParameter(3,0.2);  
  gaussP1.SetParLimits(3,0.,1.0);
  gaussP1.SetParameter(4,0.);
  gaussP1.SetParLimits(4,-0.01,0.01);

  std::cout << "Set Limits" << std::endl;

  TH1& nch = const_cast<TH1&>(hist);

  Int_t status = nch.Fit(&gaussP1,"0","",minVal,maxVal);
  std::cout << "Done Fit" << std::endl;

  peak = gaussP1.GetParameter(1);
  width = gaussP1.GetParameter(2);
  result.setVals(peak,width,(CalibData::AcdCalibObj::STATUS)status);

  return status;

}



