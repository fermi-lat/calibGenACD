#define AcdVetoFit_cxx

#include "AcdHistCalibMap.h"
#include "AcdVetoFit.h"
#include "AcdMap.h"

#include <iostream>
#include <fstream>

ClassImp(AcdVetoFitResult) ;

AcdVetoFitResult::AcdVetoFitResult(Float_t veto, Float_t width, STATUS status) 
  :AcdCalibResult(status),
   _veto(veto),_width(width){
}

AcdVetoFitResult::AcdVetoFitResult()
  :AcdCalibResult(),
   _veto(0.),_width(0.){
}

void AcdVetoFitResult::printXmlLine(ostream& os) const {
  
  os << "<acdVeto veto=\"" << _veto
     << "\" width=\"" << _width    
     << "\" status=\"" << getStatus()
     << "\"/>" << std::endl;
};

void AcdVetoFitResult::printTxtLine(ostream& os) const {
  os << _veto << ' ' << _width << ' ' << getStatus();     
};

Bool_t AcdVetoFitResult::readTxt(istream& is) { 
  Float_t veto, width;
  Int_t stat, type;
  is >> veto >> width >> stat;
  setVals(veto,width,(STATUS)stat);
  return kTRUE;
};  


ClassImp(AcdVetoFitMap) ;

AcdVetoFitMap::AcdVetoFitMap(){;}

AcdVetoFitMap::~AcdVetoFitMap(){;}

ClassImp(AcdVetoFit) ;

AcdVetoFit::AcdVetoFit() {;}

AcdVetoFit::~AcdVetoFit() {;}

Int_t AcdVetoFit::fit(AcdVetoFitResult& result, const TH1& hist) {
  result.setVals(0.,0.,AcdVetoFitResult::NOFIT);
  return result.getStatus();
}

void AcdVetoFit::fitAll(AcdVetoFitMap& results, AcdHistCalibMap& hists) {
  TList& theHists = const_cast<TList&>(hists.histograms());
  TListIter itr(&theHists);
  while ( TObject* obj = itr() ) {
    TH1* hist = static_cast<TH1*>(obj);
    UInt_t key = hist->GetUniqueID();
    AcdVetoFitResult* theResult = new AcdVetoFitResult;
    fit(*theResult,*hist);
    results.add(key,*theResult);
  } 
}

Int_t AcdVetoFit::fitChannel(AcdVetoFitMap& result, AcdHistCalibMap& input, UInt_t key) {
  
  TH1* hist = input.getHist(key);
  if ( hist == 0 ) {
    std::cerr << "No histogram w/ key " << key << " to fit" << std::endl;
    return 0;
  }
  AcdVetoFitResult* theResult = static_cast<AcdVetoFitResult*>(result.get(key));
  if ( theResult == 0 ) {
    theResult = new AcdVetoFitResult;
    result.add(key,*theResult);
  }
  
  return fit(*theResult,*hist);

}
