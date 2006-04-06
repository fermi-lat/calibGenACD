
#ifndef AcdVetoFitLibrary_h
#define AcdVetoFitLibrary_h

#include "AcdVetoFit.h"
#include "TH1.h"

class AcdHistCalibMap;

class AcdVetoFitLibrary : public AcdVetoFit {

public:
  
  static Int_t findFirstBinAboveVal(const TH1& hist, Float_t val);

public:

  enum FitType { None = 0, 
		 Counting = 1,
		 Erf = 2 };

public:

  AcdVetoFitLibrary(FitType type)
    :_type(type){}

  AcdVetoFitLibrary(){}

  virtual ~AcdVetoFitLibrary() {;}
  
  virtual Int_t fit(AcdVetoFitResult& result, const TH1& hist);

  inline FitType fitType() const { return _type; };
  inline void setFitType(FitType type) { _type = type; };

  virtual const char* algorithm() const {
    static const char* names[3] = {"None","Counting","Erf"};
    return names[_type];
  }

protected:

  Int_t counting(AcdVetoFitResult& result, const TH1& hist);
  Int_t fitErf(AcdVetoFitResult& result, const TH1& hist);

private:
  
  FitType _type;

  ClassDef(AcdVetoFitLibrary,0) ;
};


#endif

#ifdef AcdVetoFitLibrary_cxx

#endif // #ifdef AcdVetoFitLibrary_cxx
