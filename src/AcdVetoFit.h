
#ifndef AcdVetoFit_h
#define AcdVetoFit_h

#include "AcdCalibFit.h"

#include "CalibData/Acd/AcdVeto.h"

#include <string>

#include "TH1.h"
class AcdHistCalibMap;



class AcdVetoFitLibrary : public  AcdCalibFit{

public:
  
  static Int_t findFirstBinAboveVal(const TH1& hist, Float_t val);

public:

  enum FitType { None = 0, 
		 Counting = 1,
		 Erf = 2 };

public:

  AcdVetoFitLibrary(FitType type)
    :AcdCalibFit(&CalibData::AcdVetoFitDesc::instance()),
    _type(type){}

  AcdVetoFitLibrary(){}

  virtual ~AcdVetoFitLibrary() {;}
  
  virtual Int_t fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder);

  inline FitType fitType() const { return _type; };
  inline void setFitType(FitType type) { _type = type; };

  virtual const char* algorithm() const {
    static const char* names[3] = {"None","Counting","Erf"};
    return names[_type];
  }

protected:

  Int_t counting(CalibData::AcdCalibObj& result, const TH1& hist);
  Int_t fitErf(CalibData::AcdCalibObj& result, const TH1& hist);

private:
  
  FitType _type;

};

#endif

#ifdef AcdVetoFit_cxx

#endif // #ifdef AcdVetoFit_cxx
