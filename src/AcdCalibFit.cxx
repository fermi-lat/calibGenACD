#define AcdCalibFit_cxx

#include "AcdCalibFit.h"

#include "AcdKey.h"
#include "AcdXmlUtil.h"
#include "DomElement.h"

#include "AcdHistCalibMap.h"
#include "AcdCalibMap.h"

#include "CalibData/Acd/AcdCalibObj.h"

#include <iostream>
#include <fstream>


AcdCalibFit::AcdCalibFit(const CalibData::AcdCalibDescription* desc)
  :_desc(desc){
}

AcdCalibFit::AcdCalibFit()
  :_desc(0){
}

Int_t AcdCalibFit::fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& /* holder */,
		       CalibData::AcdCalibObj* /* ref */ ){
  result.setStatus(CalibData::AcdCalibObj::NOFIT);
  return result.getStatus();
}

void AcdCalibFit::fitAll(AcdCalibMap& results, AcdHistCalibMap& hists,AcdKey::ChannelSet cSet) {

  const AcdCalibMap* ref = results.theReference();

  std::cout << "Fitting All " << ref << std::endl;

  for ( std::map<UInt_t,AcdCalibHistHolder>::const_iterator itr = hists.theMap().begin();
	itr != hists.theMap().end(); itr++ ) {
    UInt_t key = itr->first;
    const AcdCalibHistHolder& holder = itr->second;
    CalibData::AcdCalibObj* theResult = results.get(key);
    if ( theResult == 0 ) {
      theResult = results.makeNew();
    }
    CalibData::AcdCalibObj* refFit = ref ? const_cast<CalibData::AcdCalibObj*>(ref->get(key)) : 0;
    if ( AcdKey::useChannel( AcdKey::getId(key), cSet ) ) {
      fit(*theResult, holder,refFit);
    } else {
      if ( refFit ) {
	theResult->update(refFit);
      }
    }
    results.add(key,*theResult);
  }
}

UInt_t AcdCalibFit::fitChannel(AcdCalibMap& result, AcdHistCalibMap& input, UInt_t key) {
  
  AcdCalibHistHolder* holder = input.getHolder(key);
  if ( holder == 0 ) {
    std::cerr << "No histogram w/ key " << key << " to fit" << std::endl;
    return 0;
  }
  CalibData::AcdCalibObj* theResult = result.get(key);
  if ( theResult == 0 ) {
    theResult = result.makeNew();
    result.add(key,*theResult);
  }
  
  return fit(*theResult,*holder);

}
