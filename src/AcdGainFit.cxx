#define AcdGainFit_cxx

#include "AcdHistCalibMap.h"
#include "AcdGainFit.h"
#include "AcdMap.h"

#include <iostream>
#include <fstream>

ClassImp(AcdGainFitResult) ;

AcdGainFitResult::AcdGainFitResult(Float_t peak, Float_t width, STATUS status) 
  :AcdCalibResult(status),
   _peak(peak),_width(width){
}

AcdGainFitResult::AcdGainFitResult()
  :AcdCalibResult(),
   _peak(0.),_width(0.){
}

void AcdGainFitResult::printXmlLine(ostream& os) const {
  
  os << "<acdGain peak=\"" << _peak
     << "\" width=\"" << _width    
     << "\" status=\"" << getStatus()
     << "\"/>" << std::endl;
};

void AcdGainFitResult::printTxtLine(ostream& os) const {
  os << _peak << ' ' << _width << ' ' << getStatus();
};

Bool_t AcdGainFitResult::readTxt(istream& is) { 
  Float_t peak, width;
  Int_t stat;
  is >> peak >> width >> stat;
  setVals(peak,width,(STATUS)stat);
  return kTRUE;
};  


ClassImp(AcdGainFitMap) ;

AcdGainFitMap::AcdGainFitMap(){;}

AcdGainFitMap::~AcdGainFitMap(){;}

ClassImp(AcdGainFit) ;

AcdGainFit::AcdGainFit() {;}

AcdGainFit::~AcdGainFit() {;}

Int_t AcdGainFit::fit(AcdGainFitResult& result, const TH1& /* hist */) {
  result.setVals(0.,0.,AcdGainFitResult::NOFIT);
  return result.getStatus();
}

void AcdGainFit::fitAll(AcdGainFitMap& results, AcdHistCalibMap& hists) {
  TList& theHists = const_cast<TList&>(hists.histograms());
  TListIter itr(&theHists);
  while ( TObject* obj = itr() ) {
    TH1* hist = static_cast<TH1*>(obj);
    UInt_t key = hist->GetUniqueID();
    AcdGainFitResult* theResult = new AcdGainFitResult;
    fit(*theResult,*hist);
    results.add(key,*theResult);
  } 
}

Int_t AcdGainFit::fitChannel(AcdGainFitMap& result, AcdHistCalibMap& input, UInt_t key) {
  
  TH1* hist = input.getHist(key);
  if ( hist == 0 ) {
    std::cerr << "No histogram w/ key " << key << " to fit" << std::endl;
    return 0;
  }
  AcdGainFitResult* theResult = static_cast<AcdGainFitResult*>(result.get(key));
  if ( theResult == 0 ) {
    theResult = new AcdGainFitResult;
    result.add(key,*theResult);
  }
  
  return fit(*theResult,*hist);

}
