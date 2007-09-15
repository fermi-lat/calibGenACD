
#ifndef AcdCalibFit_h
#define AcdCalibFit_h

#include "Rtypes.h"

// forward declares
class AcdHistCalibMap;
class AcdCalibHistHolder;
class AcdCalibResult;
class AcdCalibMap;
class AcdCalibDescription;
class TH1;


class AcdCalibFit {

public:

  AcdCalibFit(const AcdCalibDescription* desc);
  AcdCalibFit();

  virtual ~AcdCalibFit() {;}
  
  virtual Int_t fit(AcdCalibResult& result, const AcdCalibHistHolder& holder);

  virtual UInt_t fitChannel(AcdCalibMap& result, AcdHistCalibMap& input, UInt_t key);

  void fitAll(AcdCalibMap& results, AcdHistCalibMap& hists);

  virtual const char* algorithm() const {
    static const char* def("Default");
    return def;
  }

  const AcdCalibDescription* desc() const { return _desc; }

private:
  
  const AcdCalibDescription* _desc;
  
  ClassDef(AcdCalibFit,0) ;
};


#endif

#ifdef AcdCalibFit_cxx

#endif // #ifdef AcdCalibFit_cxx
