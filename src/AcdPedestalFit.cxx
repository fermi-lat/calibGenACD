#define AcdPedestalFit_cxx
#include "AcdPedestalFit.h"
#include "AcdHistCalibMap.h"
#include "AcdMap.h"

#include <iostream>
#include <fstream>

ClassImp(AcdPedestalFitResult) ;

AcdPedestalFitResult::AcdPedestalFitResult(Float_t mean, Float_t rms, STATUS status) 
  :AcdCalibResult(status),
   _mean(mean),_rms(rms){
}

AcdPedestalFitResult::AcdPedestalFitResult()
  :AcdCalibResult(),
   _mean(0.),_rms(0.){
}


void AcdPedestalFitResult::printXmlLine(ostream& os) const {
  
  os << "<acdPed mean=\"" << _mean
     << "\" width=\"" << _rms    
     << "\" status=\"" << getStatus()
     << "\"/>" << std::endl;
};


void AcdPedestalFitResult::printTxtLine(ostream& os) const {
  os << _mean << ' ' << _rms << ' ' << getStatus();
};

Bool_t AcdPedestalFitResult::readTxt(istream& is) { 
  Float_t mean, rms;
  Int_t stat;
  is >> mean >> rms >> stat;
  setVals(mean,rms,(STATUS)stat);
  return kTRUE;
};  

ClassImp(AcdPedestalFitMap) ;

AcdPedestalFitMap::~AcdPedestalFitMap() {
}



//void AcdPedestalFitMap::writeXmlHeader(ostream& ped, 
//				       const char* system,
//				       const char* instrument,
//				       const char* timestamp,
//				       const char* version) const {  
//  using std::endl;
//  ped << "<!DOCTYPE acdCalib SYSTEM \"" << system << "\" [] >" << endl;
//  ped << "<acdCalib>" << endl;
//  ped << "<generic instrument=\"" << instrument 
//      << "\" timestamp=\"" << timestamp 
//      << "\" calibType=\"ACD_Ped\" "
//      << "fmtVersion=\"" << version 
//      << "\">" << endl;
//  ped << "  <inputSample startTime=\"" << 0
//      << "\" stopTime=\"" << 0
//      << "\" triggers=\"" << 0
//      << "\" source=\"" << 0
//      << "\" mode=\"" << 0
//      << "\"/>" << endl;
// ped << "</generic>" << endl;
//  ped << "<dimension nFace=\"7\" nRow=\"5\" nCol=\"5\"/>" << endl;    
//}

ClassImp(AcdPedestalFit) ;

AcdPedestalFit::AcdPedestalFit() {
}

UInt_t AcdPedestalFit::fit(AcdPedestalFitResult& result, const TH1& /* hist */) {
  result.setVals(0.,0.,AcdCalibResult::NOFIT);
  return result.getStatus();
}

void AcdPedestalFit::fitAll(AcdPedestalFitMap& results, AcdHistCalibMap& hists) {
  TList& theHists = const_cast<TList&>(hists.histograms());
  TListIter itr(&theHists);
  while ( TObject* obj = itr() ) {
    TH1* hist = static_cast<TH1*>(obj);
    UInt_t key = hist->GetUniqueID();
    AcdPedestalFitResult* theResult = new AcdPedestalFitResult;
    fit(*theResult,*hist);
    results.add(key,*theResult);
  } 
}

UInt_t AcdPedestalFit::fitChannel(AcdPedestalFitMap& result, AcdHistCalibMap& input, UInt_t key) {
  
  TH1* hist = input.getHist(key);
  if ( hist == 0 ) {
    std::cerr << "No histogram w/ key " << key << " to fit" << std::endl;
    return 0;
  }
  AcdPedestalFitResult* theResult = static_cast<AcdPedestalFitResult*>(result.get(key));
  if ( theResult == 0 ) {
    theResult = new AcdPedestalFitResult;
    result.add(key,*theResult);
  }
  
  return fit(*theResult,*hist);

}
