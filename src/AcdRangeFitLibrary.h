
#ifndef AcdRangeFitLibrary_h
#define AcdRangeFitLibrary_h

#include "AcdRangeFit.h"
#include "TH1.h"

class AcdHistCalibMap;

class AcdRangeFitLibrary : public AcdRangeFit {

public:

  enum FitType { None = 0, 
		 Counting = 1 };

public:

  AcdRangeFitLibrary(FitType type)
    :AcdRangeFit(),
    _type(type){}

  AcdRangeFitLibrary(){}

  virtual ~AcdRangeFitLibrary() {;}
  
  virtual Int_t fit(AcdRangeFitResult& result, const TH1& lowhist, const TH1& highhist);

  inline FitType fitType() const { return _type; };
  inline void setFitType(FitType type) { _type = type; };

  virtual const char* algorithm() const {
    static const char* names[2] = {"None","Counting"};
    return names[_type];
  }

protected:

private:
  
  FitType _type;

  ClassDef(AcdRangeFitLibrary,0) ;
};


#endif

#ifdef AcdRangeFitLibrary_cxx

#endif // #ifdef AcdRangeFitLibrary_cxx
