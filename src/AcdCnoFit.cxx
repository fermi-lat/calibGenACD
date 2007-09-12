#define AcdCnoFit_cxx

#include "AcdHistCalibMap.h"
#include "AcdCnoFit.h"
#include "AcdMap.h"

#include "AcdXmlUtil.h"
#include "DomElement.h"

#include <iostream>
#include <fstream>

ClassImp(AcdCnoFitResult) ;

AcdCnoFitResult::AcdCnoFitResult(Float_t cno, Float_t width, STATUS status) 
  :AcdCalibResult(status),
   _cno(cno),_width(width){
}

AcdCnoFitResult::AcdCnoFitResult()
  :AcdCalibResult(),
   _cno(0.),_width(0.){
}

void AcdCnoFitResult::makeXmlNode(DomElement& node) const {
  DomElement cnoNode = AcdXmlUtil::makeChildNode(node,"acdCno");
  AcdXmlUtil::addAttribute(cnoNode,"cno",_cno);
  AcdXmlUtil::addAttribute(cnoNode,"width",_width);
  AcdXmlUtil::addAttribute(cnoNode,"status",getStatus());
};

void AcdCnoFitResult::printTxtLine(ostream& os) const {
  os << _cno << ' ' << _width << ' ' << getStatus();     
};

Bool_t AcdCnoFitResult::readTxt(istream& is) { 
  Float_t cno, width;
  Int_t stat;
  is >> cno >> width >> stat;
  setVals(cno,width,(STATUS)stat);
  return kTRUE;
};  


ClassImp(AcdCnoFitMap) ;

AcdCnoFitMap::AcdCnoFitMap(){;}

AcdCnoFitMap::~AcdCnoFitMap(){;}

ClassImp(AcdCnoFit) ;

AcdCnoFit::AcdCnoFit() {;}

AcdCnoFit::~AcdCnoFit() {;}

Int_t AcdCnoFit::fit(AcdCnoFitResult& result, const TH1& /* hist */) {
  result.setVals(0.,0.,AcdCnoFitResult::NOFIT);
  return result.getStatus();
}

void AcdCnoFit::fitAll(AcdCnoFitMap& results, AcdHistCalibMap& hists) {
  TList& theHists = const_cast<TList&>(hists.histograms());
  TListIter itr(&theHists);
  while ( TObject* obj = itr() ) {
    TH1* hist = static_cast<TH1*>(obj);
    UInt_t key = hist->GetUniqueID();
    AcdCnoFitResult* theResult = new AcdCnoFitResult;
    fit(*theResult,*hist);
    results.add(key,*theResult);
  } 
}

Int_t AcdCnoFit::fitChannel(AcdCnoFitMap& result, AcdHistCalibMap& input, UInt_t key) {
  
  TH1* hist = input.getHist(key);
  if ( hist == 0 ) {
    std::cerr << "No histogram w/ key " << key << " to fit" << std::endl;
    return 0;
  }
  AcdCnoFitResult* theResult = static_cast<AcdCnoFitResult*>(result.get(key));
  if ( theResult == 0 ) {
    theResult = new AcdCnoFitResult;
    result.add(key,*theResult);
  }
  
  return fit(*theResult,*hist);

}
