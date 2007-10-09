
#ifndef AcdPedestalFit_h
#define AcdPedestalFit_h

// Base classes
#include "AcdCalibFit.h"

#include "CalibData/Acd/AcdPed.h"

// stl includes
#include <string>

// ROOT includes
#include "TH1.h"

// forward declares
class AcdHistCalibMap;



class AcdPedestalFitLibrary : public AcdCalibFit {

public:

  enum FitType { None = 0, 
		 PeakValue = 1,
		 MeanValue = 2 };

public:

  AcdPedestalFitLibrary(FitType type)
    :AcdCalibFit(&CalibData::AcdPedestalFitDesc::instance()),
    _type(type){}

  AcdPedestalFitLibrary(){}

  virtual ~AcdPedestalFitLibrary() {;}
  
  virtual Int_t fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder);

  inline FitType fitType() const { return _type; };
  inline void setFitType(FitType type) { _type = type; };

  virtual const char* algorithm() const {
    static const char* names[3] = {"None","PeakValue","MeanValue"};
    return names[_type];
  }

protected:

  UInt_t fitMean(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder);
  UInt_t fitPeak(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder);

private:
  
  FitType _type;

};


#endif

#ifdef AcdPedestalFit_cxx

#endif // #ifdef AcdPedestalFit_cxx
