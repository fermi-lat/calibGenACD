
#ifndef AcdCalibFit_h
#define AcdCalibFit_h

#include "Rtypes.h"

// forward declares
class AcdHistCalibMap;
class AcdCalibHistHolder;
class AcdCalibMap;
class TH1;

namespace CalibData {
  class AcdCalibDescription;
  class AcdCalibObj;
}

class AcdCalibFit {

public:

  AcdCalibFit(const CalibData::AcdCalibDescription* desc);
  AcdCalibFit();

  virtual ~AcdCalibFit() {;}
  
  virtual Int_t fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder);

  virtual UInt_t fitChannel(AcdCalibMap& result, AcdHistCalibMap& input, UInt_t key);

  void fitAll(AcdCalibMap& results, AcdHistCalibMap& hists);

  virtual const char* algorithm() const {
    static const char* def("Default");
    return def;
  }

  const CalibData::AcdCalibDescription* desc() const { return _desc; }

private:
  
  const CalibData::AcdCalibDescription* _desc;
  
};


#endif

#ifdef AcdCalibFit_cxx

#endif // #ifdef AcdCalibFit_cxx
