#define AcdRangeFit_cxx

#include "AcdHistCalibMap.h"
#include "AcdRangeFit.h"
#include "AcdMap.h"

#include "AcdXmlUtil.h"
#include "DomElement.h"

#include <iostream>
#include <fstream>

ClassImp(AcdRangeFitResult) ;

AcdRangeFitResult::AcdRangeFitResult(UInt_t low, UInt_t high, STATUS status) 
  :AcdCalibResult(status),
   _low(low),
   _high(high){
}

AcdRangeFitResult::AcdRangeFitResult()
  :AcdCalibResult(),
   _low(0),
   _high(0){
}

void AcdRangeFitResult::makeXmlNode(DomElement& node) const {
  DomElement rangeNode = AcdXmlUtil::makeChildNode(node,"acdRange");
  AcdXmlUtil::addAttribute(rangeNode,"low",(Int_t)_low);
  AcdXmlUtil::addAttribute(rangeNode,"high",(Int_t)_high);
  AcdXmlUtil::addAttribute(rangeNode,"status",getStatus());
};

void AcdRangeFitResult::printTxtLine(ostream& os) const {
  os << _low << ' ' << _high << ' ' << getStatus();     
};

Bool_t AcdRangeFitResult::readTxt(istream& is) { 
  UInt_t low, high;
  Int_t stat;
  is >> low >> high >> stat;
  setVals(low, high,(STATUS)stat);
  return kTRUE;
};  


ClassImp(AcdRangeFitMap) ;

AcdRangeFitMap::AcdRangeFitMap(){;}

AcdRangeFitMap::~AcdRangeFitMap(){;}

ClassImp(AcdRangeFit) ;

AcdRangeFit::AcdRangeFit(){;}

AcdRangeFit::~AcdRangeFit() {;}

Int_t AcdRangeFit::fit(AcdRangeFitResult& result, const TH1& /* low */, const TH1& /* high */) {
  result.setVals(0.,0.,AcdRangeFitResult::NOFIT);
  return result.getStatus();
}

void AcdRangeFit::fitAll(AcdRangeFitMap& results, AcdHistCalibMap& lowHists, AcdHistCalibMap& highHists) {
  TList& theHists = const_cast<TList&>(lowHists.histograms());
  TListIter itr(&theHists);
  while ( TObject* obj = itr() ) {
    TH1* lowhist = static_cast<TH1*>(obj);
    UInt_t key = lowhist->GetUniqueID();
    TH1* highhist = highHists.getHist(key);
    if ( highhist == 0 ) {
      std::cerr << "No High histogram w/ key " << key << " to fit" << std::endl;
      return;
    }
    AcdRangeFitResult* theResult = new AcdRangeFitResult;
    fit(*theResult,*lowhist,*highhist);
    results.add(key,*theResult);
  } 
}

Int_t AcdRangeFit::fitChannel(AcdRangeFitMap& result, AcdHistCalibMap& low, AcdHistCalibMap& high, UInt_t key) {
  
  TH1* lowhist = low.getHist(key);
  TH1* highhist = high.getHist(key);
  
  if ( lowhist == 0 ) {
    std::cerr << "No histogram w/ key " << key << " to fit" << std::endl;
    return 0;
  }
  AcdRangeFitResult* theResult = static_cast<AcdRangeFitResult*>(result.get(key));
  if ( theResult == 0 ) {
    theResult = new AcdRangeFitResult;
    result.add(key,*theResult);
  }
  
  return fit(*theResult,*lowhist,*highhist);

}
