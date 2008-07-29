#define AcdStripFit_cxx

#include "AcdStripFit.h"

#include "AcdHistCalibMap.h"
#include "AcdCalibUtil.h"
#include "AcdCalibMap.h"


Bool_t AcdStripFitLibrary::test(AcdCalibMap& results, Float_t lo, Float_t hi, const char* msg, const char* testName) const {

  Bool_t retVal(kTRUE);

  std::cout << "Results of " << testName << '.' << std::endl
	    << "---------------------------------------------" << std::endl;
  
  for ( std::map<UInt_t,CalibData::AcdCalibObj*>::const_iterator itr = results.theMap().begin();
	itr != results.theMap().end(); itr++ ) {    
  
    const CalibData::AcdCalibObj* theFit = itr->second;
    if ( (*theFit)[2] < lo ) {
      std::cout << msg << "  Channel " << AcdKey::getId(itr->first) << ':' << AcdKey::getPmt(itr->first)
		<< " is below min level.  " << (*theFit)[2]  << " < " << lo << std::endl;
      retVal = kFALSE;
    }
    if ( (*theFit)[3] > hi ) {
      std::cout << msg << "  Channel " << AcdKey::getId(itr->first) << ':' << AcdKey::getPmt(itr->first)
		<< " is above max level.  " << (*theFit)[3] << " > " << hi << std::endl;
      retVal = kFALSE;
    }
  }  
  if ( retVal ) {
    std::cout << "Passed" << std::endl;
  } else {
    std::cout << "Failed" << std::endl;
  }
  std::cout << "---------------------------------------------" << std::endl << std::endl;

  return retVal;
}


Int_t AcdStripFitLibrary::fit(CalibData::AcdCalibObj& result, AcdCalibHistHolder& holder,
			      CalibData::AcdCalibObj* /* ref */ ) {

  TH1& in = *(holder.getHist(0));
  TH1* out(0);

  Float_t mean(0.);
  Float_t sum(0.);

  Float_t min = 1e9; 
  Float_t minErr = 0.;
  Float_t max = -1e9;
  Float_t maxErr = 0.;

  Int_t nBin = in.GetNbinsX();
  for ( Int_t i = 1; i <= nBin; i++ ) {
    Float_t val = in.GetBinContent(i);
    sum += val;
    if ( val < min ) {
      min = val;
      minErr = in.GetBinContent(i);
    }
    if ( val > max ) {
      max = val;
      maxErr = in.GetBinContent(i);
    }
  }
  mean = sum / ( (Double_t) nBin );
  Float_t ref = _method == AcdCalib::PLAIN ? _ref : mean;

  AcdCalibUtil::chi2Dist(in,out,_method,ref,_scale);
  holder.addHist(*out);

  Float_t rms = out->GetRMS();

  switch ( _method ) {
  case AcdCalib::PLAIN:
    break;
  case AcdCalib::MEAN_ABSOLUTE:
    min -= mean;
    max -= mean;
    break;
  case AcdCalib::MEAN_RELATIVE:
    min -= mean;
    if ( mean > 1e-9 ) { min /= mean; }
    max -= mean;
    if ( mean > 1e-9 ) { max /= mean; }
    break;
  case AcdCalib::MEAN_SIGMA:
    min -= mean;
    if ( minErr > 1e-9 ) { min /= minErr; }
    max -= mean;
    if ( maxErr > 1e-9 ) { max /= maxErr; }
    break;
  }
  result.setVals(mean,rms,min,max,CalibData::AcdCalibObj::OK);
  return result.getStatus();
}

