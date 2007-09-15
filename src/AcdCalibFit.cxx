#define AcdCalibFit_cxx

#include "AcdCalibFit.h"

#include "AcdMap.h"
#include "AcdXmlUtil.h"
#include "DomElement.h"

#include "AcdHistCalibMap.h"
#include "AcdCalibResult.h"
#include "AcdCalibMap.h"

#include <iostream>
#include <fstream>


ClassImp(AcdCalibFit) ;

AcdCalibFit::AcdCalibFit(const AcdCalibDescription* desc)
  :_desc(desc){
}

AcdCalibFit::AcdCalibFit()
  :_desc(0){
}

Int_t AcdCalibFit::fit(AcdCalibResult& result, const AcdCalibHistHolder& /* holder */) {
  result.setStatus(AcdCalibResult::NOFIT);
  return result.getStatus();
}

void AcdCalibFit::fitAll(AcdCalibMap& results, AcdHistCalibMap& hists) {

  for ( std::map<UInt_t,AcdCalibHistHolder>::const_iterator itr = hists.theMap().begin();
	itr != hists.theMap().end(); itr++ ) {
    UInt_t key = itr->first;
    const AcdCalibHistHolder& holder = itr->second;
    AcdCalibResult* theResult = results.makeNew();
    fit(*theResult, holder);
    results.add(key,*theResult);
  }
}

UInt_t AcdCalibFit::fitChannel(AcdCalibMap& result, AcdHistCalibMap& input, UInt_t key) {
  
  AcdCalibHistHolder* holder = input.getHolder(key);
  if ( holder == 0 ) {
    std::cerr << "No histogram w/ key " << key << " to fit" << std::endl;
    return 0;
  }
  AcdCalibResult* theResult = result.get(key);
  if ( theResult == 0 ) {
    theResult = result.makeNew();
    result.add(key,*theResult);
  }
  
  return fit(*theResult,*holder);

}
