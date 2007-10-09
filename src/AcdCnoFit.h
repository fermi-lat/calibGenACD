
#ifndef AcdCnoFit_h
#define AcdCnoFit_h

// Base classes
#include "AcdCalibFit.h"

#include "CalibData/Acd/AcdCno.h"

// stl includes
#include <string>

// ROOT includes
#include "TH1.h"

// forward declares
class AcdHistCalibMap;


class AcdCnoFitLibrary : public AcdCalibFit {

public:
  
  static Int_t findFirstBinAboveVal(const TH1& hist, Float_t val);

public:

  enum FitType { None = 0, 
		 Counting = 1,
		 Erf = 2 };

public:

  AcdCnoFitLibrary(FitType type)
    :AcdCalibFit(&CalibData::AcdCnoFitDesc::instance()),
    _type(type){}

  AcdCnoFitLibrary(){}

  virtual ~AcdCnoFitLibrary() {;}
  
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

#ifdef AcdCnoFit_cxx

#endif // #ifdef AcdCnoFit_cxx
