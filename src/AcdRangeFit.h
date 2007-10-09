
#ifndef AcdRangeFit_h
#define AcdRangeFit_h

// Base classes
#include "AcdCalibFit.h"

#include "CalibData/Acd/AcdRange.h"

// stl includes
#include <string>

// ROOT includes
#include "TH1.h"

// forward declares
class AcdHistCalibMap;



class AcdRangeFitLibrary : public  AcdCalibFit{

public:

  enum FitType { None = 0, 
		 Counting = 1 };

public:

  AcdRangeFitLibrary(FitType type)
    :AcdCalibFit(&CalibData::AcdRangeFitDesc::instance()),
    _type(type){}

  AcdRangeFitLibrary(){}

  virtual ~AcdRangeFitLibrary() {;}
  
  virtual Int_t fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder);

  inline FitType fitType() const { return _type; };
  inline void setFitType(FitType type) { _type = type; };

  virtual const char* algorithm() const {
    static const char* names[2] = {"None","Counting"};
    return names[_type];
  }

protected:

private:
  
  FitType _type;

};


#endif

#ifdef AcdRangeFit_cxx

#endif // #ifdef AcdRangeFit_cxx
