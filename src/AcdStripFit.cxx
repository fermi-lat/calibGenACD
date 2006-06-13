#define AcdStripFit_cxx

#include "AcdHistCalibMap.h"
#include "AcdStripFit.h"
#include "AcdMap.h"
#include "AcdCalibUtil.h"

#include <iostream>
#include <fstream>

ClassImp(AcdStripFitResult) ;

AcdStripFitResult::AcdStripFitResult(Float_t mean, Float_t rms, Float_t min, Float_t max, STATUS status) 
  :AcdCalibResult(status),
   _mean(mean),_rms(rms), _min(min), _max(max) {
}

AcdStripFitResult::AcdStripFitResult()
  :AcdCalibResult(),
   _mean(0.),_rms(0.), _min(0.), _max(0.) {
}

void AcdStripFitResult::printXmlLine(ostream& os) const {
  
  os << "<acdStrip mean=\"" << _mean
     << "\" rms=\"" << _rms    
     << "\" min=\"" << _min
     << "\" max=\"" << _max
     << "\" status=\"" << getStatus()
     << "\"/>" << std::endl;
};

void AcdStripFitResult::printTxtLine(ostream& os) const {
  os << _mean << ' ' << _rms << ' ' << _min << ' ' << _max << ' ' << getStatus();     
};

Bool_t AcdStripFitResult::readTxt(istream& is) { 
  Float_t mean, rms, min, max;
  Int_t stat;
  is >> mean >> rms >> min >> max >> stat;
  setVals(mean,rms,min,max,(STATUS)stat);
  return kTRUE;
};  


ClassImp(AcdStripFitMap) ;

AcdStripFitMap::AcdStripFitMap(){;}

AcdStripFitMap::~AcdStripFitMap(){;}

Bool_t AcdStripFitMap::test(Float_t lo, Float_t hi, const char* msg, const char* testName) const {

  Bool_t retVal(kTRUE);

  std::cout << "Results of " << testName << '.' << std::endl
	    << "---------------------------------------------" << std::endl;
  
  for ( std::map<UInt_t,AcdCalibResult*>::const_iterator itr = theMap().begin();
	itr != theMap().end(); itr++ ) {    
    const AcdStripFitResult* theFit = static_cast<const AcdStripFitResult*>(itr->second);
    if ( theFit->min() < lo ) {
      std::cout << msg << "  Channel " << AcdMap::getId(itr->first) << ':' << AcdMap::getPmt(itr->first)
		<< " is below min level.  " << theFit->min() << " < " << lo << std::endl;
      retVal = kFALSE;
    }
    if ( theFit->max() > hi ) {
      std::cout << msg << "  Channel " << AcdMap::getId(itr->first) << ':' << AcdMap::getPmt(itr->first)
		<< " is above max level.  " << theFit->max() << " > " << hi << std::endl;
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


ClassImp(AcdStripFit) ;

AcdStripFit::AcdStripFit(const char* name, 
			 Int_t method, 
			 Int_t nBin, Float_t min, Float_t max,
			 Float_t refVal, Float_t scale) 
  :_fitted(name,nBin,min,max),
   m_method(method),m_refVal(refVal),m_scale(scale){;}

AcdStripFit::~AcdStripFit() {;}

Int_t AcdStripFit::fit(AcdStripFitResult& result, const TH1& in, TH1*& out) {

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
  Float_t ref = m_method == AcdCalibUtil::PLAIN ? m_refVal : mean;

  AcdCalibUtil::chi2Dist(in,out,m_method,ref,m_scale);
  Float_t rms = out->GetRMS();

  switch ( m_method ) {
  case AcdCalibUtil::PLAIN:
    break;
  case AcdCalibUtil::MEAN_ABSOLUTE:
    min -= mean;
    max -= mean;
    break;
  case AcdCalibUtil::MEAN_RELATIVE:
    min -= mean;
    if ( mean > 1e-9 ) { min /= mean; }
    max -= mean;
    if ( mean > 1e-9 ) { max /= mean; }
    break;
  case AcdCalibUtil::MEAN_SIGMA:
    min -= mean;
    if ( minErr > 1e-9 ) { min /= minErr; }
    max -= mean;
    if ( maxErr > 1e-9 ) { max /= maxErr; }
    break;
  }
  result.setVals(mean,rms,min,max,AcdStripFitResult::OK);
  return result.getStatus();
}

void AcdStripFit::fitAll(AcdStripFitMap& results, AcdHistCalibMap& hists) {
  TList& theHists = const_cast<TList&>(hists.histograms());
  TListIter itr(&theHists);
  while ( TObject* obj = itr() ) {
    TH1* hist = static_cast<TH1*>(obj);
    UInt_t key = hist->GetUniqueID();
    AcdStripFitResult* theResult = new AcdStripFitResult;
    TH1* outHist = _fitted.getHist(key);
    fit(*theResult,*hist,outHist);
    results.add(key,*theResult);
  } 
}

Int_t AcdStripFit::fitChannel(AcdStripFitMap& result, AcdHistCalibMap& input, UInt_t key) {
  
  TH1* hist = input.getHist(key);
  if ( hist == 0 ) {
    std::cerr << "No histogram w/ key " << key << " to fit" << std::endl;
    return 0;
  }
  AcdStripFitResult* theResult = static_cast<AcdStripFitResult*>(result.get(key));
  if ( theResult == 0 ) {
    theResult = new AcdStripFitResult;
    result.add(key,*theResult);
  }
  TH1* outHist = _fitted.getHist(key);
  return fit(*theResult,*hist,outHist);

}
