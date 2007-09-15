#ifndef AcdCoherentNoiseFit_h
#define AcdCoherentNoiseFit_h

// Base classes
#include "AcdCalibResult.h"
#include "AcdCalibFit.h"

// stl includes
#include <string>

// ROOT includes
#include "TH1.h"

// forward declares
class AcdHistCalibMap;



class AcdCoherentNoiseFitDesc : public AcdCalibDescription {

public:

  static const AcdCoherentNoiseFitDesc& ins();

private:
  
  static const std::string s_calibType; // "ACD_CoherentNoise";
  static const std::string s_txtFormat; // "TILE PMT MIN MIN_TIME MAX MAX_TIME FIT_PARAMETERS STATUS";

public:
  AcdCoherentNoiseFitDesc();
  virtual ~AcdCoherentNoiseFitDesc(){;};

private:

  ClassDef(AcdCoherentNoiseFitDesc,1);
};



class AcdCoherentNoiseFitLibrary : public AcdCalibFit {

public:

  enum FitType { None = 0, 
		 Minuit = 1 };

public:

  AcdCoherentNoiseFitLibrary(FitType type)
    :AcdCalibFit(&AcdCoherentNoiseFitDesc::ins()),
    _type(type){}

  AcdCoherentNoiseFitLibrary(){}

  virtual ~AcdCoherentNoiseFitLibrary() {;}
  
  virtual Int_t fit(AcdCalibResult& result, const AcdCalibHistHolder& holder);

  inline FitType fitType() const { return _type; };
  inline void setFitType(FitType type) { _type = type; };

  virtual const char* algorithm() const {
    static const char* names[2] = {"None","Minuit"};
    return names[_type];
  }

protected:

private:
  
  FitType _type;

  ClassDef(AcdCoherentNoiseFitLibrary,0) ;
};




#endif

#ifdef AcdCoherentNoiseFit_cxx

#endif // #ifdef AcdCoherentNoiseFit_cxx
