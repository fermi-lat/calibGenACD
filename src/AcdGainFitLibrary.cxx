#define AcdGainFitLibrary_cxx
#include "AcdGainFitLibrary.h"
#include "AcdHistCalibMap.h"

#include "TF1.h"

ClassImp(AcdGainFitLibrary) ;


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

Int_t AcdGainFitLibrary::extractFeatures(const TH1& hist, Int_t rebin, Int_t& ped, Int_t& min, Int_t& peak, Int_t& halfMax) {

  TH1F copy((TH1F&)hist);
  copy.Rebin(rebin);

  ped = findLowestNonZeroBin(copy);
  if ( ped < 0 ) return AcdGainFitResult::PREFIT_FAILED;
  Int_t ped_1 = findNextLocalMax(copy,ped);
  if ( ped_1 < 0 ) return AcdGainFitResult::PREFIT_FAILED;
  min = findNextLocalMin(copy,ped_1);
  if ( min < 0 ) return AcdGainFitResult::PREFIT_FAILED;
  peak = findNextLocalMax(copy,min);
  if ( peak < 0 ) return AcdGainFitResult::PREFIT_FAILED;
  halfMax = findHalfMaxHigh(copy,peak);
  halfMax = halfMax < 0 ? copy.GetNbinsX() : halfMax;
  return 0;
}


Int_t AcdGainFitLibrary::fit(AcdGainFitResult& result, const TH1& hist) {
  
  Int_t returnCode = AcdGainFitResult::NOFIT;
  switch ( _type ) {
  case None:
    result.setVals(0.,0.,0.,AcdGainFitResult::NOFIT,_type);
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
  case LogNormal:
    returnCode = fitLogNormal(result,hist);
    break;
  }  
  return returnCode;
}

Int_t AcdGainFitLibrary::stats(AcdGainFitResult& result, const TH1& hist) {

  TH1& theHist = const_cast<TH1&>(hist);

  float ave = theHist.GetMean();
  float rms = theHist.GetRMS();
  result.setVals(0,ave,rms,AcdGainFitResult::OK,Stats);
  return AcdGainFitResult::OK;
}


Int_t AcdGainFitLibrary::fallback(AcdGainFitResult& result, const TH1& hist) {
  Int_t ped, min, peak, halfMax;
  Int_t status = extractFeatures(hist,4,ped,min,peak,halfMax);

  if ( status != 0 ) return AcdGainFitResult::PREFIT_FAILED;
  Float_t pedValue = hist.GetBinCenter(4*ped);
  //Float_t minValue = hist.GetBinCenter(4*min);
  Float_t peakValue = hist.GetBinCenter(4*peak);
  Float_t endValue = hist.GetBinCenter(4*halfMax);

  Float_t width = endValue - peakValue;

  result.setVals(pedValue,peakValue,width,AcdGainFitResult::OK,Fallback);
  return AcdGainFitResult::OK;

}

Int_t AcdGainFitLibrary::fitLandau(AcdGainFitResult& result, const TH1& hist) {

  TH1& theHist = const_cast<TH1&>(hist);

  float ave = theHist.GetMean();
  float rms = theHist.GetRMS();
  Int_t status = theHist.Fit("landau", "", "", ave-2*rms, ave+3*rms);

  if ( status != 0 ) return fallback(result,hist);

  double* par = (theHist.GetFunction("landau"))->GetParameters();
  float mean = *(par+1); 
  float sigma = *(par+2);
  result.setVals(0,mean,sigma,(AcdGainFitResult::STATUS)status,Landau);
  return status;
}


Int_t AcdGainFitLibrary::fitP7(AcdGainFitResult& result, const TH1& hist) {

  Int_t ped, min, peak, halfMax;
  Int_t status = extractFeatures(hist,4,ped,min,peak,halfMax);

  TH1& theHist = const_cast<TH1&>(hist);

  if ( status != 0 ) return status;
  Float_t pedValue = hist.GetBinCenter(4*ped);
  Float_t minValue = hist.GetBinCenter(4*min);
  Float_t endValue = hist.GetBinCenter(4*halfMax);

  status = theHist.Fit("pol7","","",minValue,endValue); //applies polynomial fit
  if ( status != 0 ) return fallback(result,hist);

  TF1* fit = theHist.GetFunction("pol7"); 
 
  Float_t peakValue = fit->GetMaximumX();
  Float_t width = endValue - peakValue;

  result.setVals(pedValue,peakValue,width,(AcdGainFitResult::STATUS)status,P7);
  return status;
}


Int_t AcdGainFitLibrary::fitLogNormal(AcdGainFitResult& result, const TH1& hist) {
  
  Int_t ped, min, peak, halfMax;
  Int_t status = extractFeatures(hist,4,ped,min,peak,halfMax);

  if ( status != 0 ) return status;
  Float_t pedValue = hist.GetBinCenter(4*ped);
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
  result.setVals(pedValue,peakValue,width,(AcdGainFitResult::STATUS)status,LogNormal);

  return status;

}


