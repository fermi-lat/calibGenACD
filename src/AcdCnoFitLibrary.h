
#ifndef AcdCnoFitLibrary_h
#define AcdCnoFitLibrary_h

#include "AcdCnoFit.h"
#include "TH1.h"

class AcdHistCalibMap;

class AcdCnoFitLibrary : public AcdCnoFit {

public:
  
  static Int_t findFirstBinAboveVal(const TH1& hist, Float_t val);

public:

  enum FitType { None = 0, 
		 Counting = 1,
		 Erf = 2 };

public:

  AcdCnoFitLibrary(FitType type)
    :_type(type){}

  AcdCnoFitLibrary(){}

  virtual ~AcdCnoFitLibrary() {;}
  
  virtual Int_t fit(AcdCnoFitResult& result, const TH1& hist);

  inline FitType fitType() const { return _type; };
  inline void setFitType(FitType type) { _type = type; };

  virtual const char* algorithm() const {
    static const char* names[3] = {"None","Counting","Erf"};
    return names[_type];
  }

protected:

  Int_t counting(AcdCnoFitResult& result, const TH1& hist);
  Int_t fitErf(AcdCnoFitResult& result, const TH1& hist);

private:
  
  FitType _type;

  ClassDef(AcdCnoFitLibrary,0) ;
};


#endif

#ifdef AcdCnoFitLibrary_cxx

#endif // #ifdef AcdCnoFitLibrary_cxx
