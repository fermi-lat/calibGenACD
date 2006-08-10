#define AcdCoherentNoiseFit_cxx

#include "AcdHistCalibMap.h"
#include "AcdCoherentNoiseFit.h"
#include "AcdMap.h"
#include "AcdCalibUtil.h"

#include "TF1.h"

#include <iostream>
#include <fstream>

ClassImp(AcdCoherentNoiseFitResult) ;

AcdCoherentNoiseFitResult::AcdCoherentNoiseFitResult(Float_t min, Float_t minTime, Float_t max, Float_t maxTime, 
						     const char* fitPars, STATUS status) 
  :AcdCalibResult(status),
   _min(min), _minTime(minTime), _max(max), _maxTime(maxTime),
   _fitPars(fitPars){
}

AcdCoherentNoiseFitResult::AcdCoherentNoiseFitResult()
  :AcdCalibResult(),
   _min(0), _minTime(0), _max(0), _maxTime(0),
   _fitPars("NONE"){
}

void AcdCoherentNoiseFitResult::printXmlLine(ostream& os) const {
  
  os << "<acdStrip min=\"" << _min
     << "\" minTime=\"" << _minTime
     << "\" max=\"" << _max
     << "\" maxTime=\"" << _maxTime
     << "\" fitPars=\"" << _fitPars
     << "\" status=\"" << getStatus()
     << "\"/>" << std::endl;
};

void AcdCoherentNoiseFitResult::printTxtLine(ostream& os) const {
  os << _min << ' ' << _minTime << ' ' << _max << ' ' << _maxTime << ' ' 
     << '"' << _fitPars << '"' << ' ' << getStatus();     
};

Bool_t AcdCoherentNoiseFitResult::readTxt(istream& is) { 
  Float_t min, minTime, max, maxTime;
  Int_t stat;
  TString fitPars;
  is >> min >> minTime >> max >> maxTime >> fitPars >> stat;
  setVals(min,minTime,max,maxTime,fitPars,(STATUS)stat);
  return kTRUE;
};  


ClassImp(AcdCoherentNoiseFitMap) ;

AcdCoherentNoiseFitMap::AcdCoherentNoiseFitMap(){;}

AcdCoherentNoiseFitMap::~AcdCoherentNoiseFitMap(){;}


ClassImp(AcdCoherentNoiseFit) ;

AcdCoherentNoiseFit::AcdCoherentNoiseFit(const char* /* name */, 
					 Int_t method)
  :m_method(method){
}

AcdCoherentNoiseFit::~AcdCoherentNoiseFit() {;}

Int_t AcdCoherentNoiseFit::fit(AcdCoherentNoiseFitResult& result, const TH1& in) {

  Float_t min = 1e9; 
  Float_t minTime = 0.;
  Float_t max = -1e9;
  Float_t maxTime = 0.;

  Int_t nBin = in.GetNbinsX();
  for ( Int_t i = 1; i <= nBin; i++ ) {
    Float_t val = in.GetBinContent(i);
    if ( val < min ) {
      min = val;
      minTime = in.GetBinCenter(i);
    }
    if ( val > max ) {
      max = val;
      maxTime = in.GetBinCenter(i);	
    }
  }

  TF1 ring("ring","[0] * exp(-x/[1]) * sin(x*[2] + [3])");

  ring.SetParameter(0,40.);   // amplitude
  ring.SetParLimits(0,0.,100.);  

  ring.SetParameter(1,1000.); // decay const
  ring.SetParLimits(1,100.,2000.);  

  ring.FixParameter(2,0.0054); // frequency

  ring.SetParameter(3,0);     // phase
  ring.SetParLimits(3,-5.,5.);

  TH1& nch = const_cast<TH1&>(in);
  Int_t status = nch.Fit(&ring,"","");
  

  Float_t amp = ring.GetParameter(0);
  Float_t decay = ring.GetParameter(1);
  Float_t freq = ring.GetParameter(2);
  Float_t phase = ring.GetParameter(3);

  char fitPars[100];
  sprintf(fitPars,"%3.1f %4.1f %5.4f %4.3f",amp,decay,freq,phase);
  
  result.setVals(min,minTime,max,maxTime,fitPars,(AcdCoherentNoiseFitResult::STATUS)status);
  return result.getStatus();
}

void AcdCoherentNoiseFit::fitAll(AcdCoherentNoiseFitMap& results, AcdHistCalibMap& hists) {
  TList& theHists = const_cast<TList&>(hists.histograms());
  TListIter itr(&theHists);
  while ( TObject* obj = itr() ) {
    TH1* hist = static_cast<TH1*>(obj);
    UInt_t key = hist->GetUniqueID();
    AcdCoherentNoiseFitResult* theResult = new AcdCoherentNoiseFitResult;
    fit(*theResult,*hist);
    results.add(key,*theResult);
  } 
}

Int_t AcdCoherentNoiseFit::fitChannel(AcdCoherentNoiseFitMap& result, AcdHistCalibMap& input, UInt_t key) {
  
  TH1* hist = input.getHist(key);
  if ( hist == 0 ) {
    std::cerr << "No histogram w/ key " << key << " to fit" << std::endl;
    return 0;
  }
  AcdCoherentNoiseFitResult* theResult = static_cast<AcdCoherentNoiseFitResult*>(result.get(key));
  if ( theResult == 0 ) {
    theResult = new AcdCoherentNoiseFitResult;
    result.add(key,*theResult);
  }
  return fit(*theResult,*hist);
}
