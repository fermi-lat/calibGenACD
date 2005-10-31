
#ifndef AcdPedestalFitLibrary_h
#define AcdPedestalFitLibrary_h

#include "AcdPedestalFit.h"
#include "TH1.h"

class AcdHistCalibMap;

class AcdPedestalFitLibrary : public AcdPedestalFit {

public:

  enum FitType { None = 0, 
		 PeakValue = 1,
		 MeanValue = 2 };

public:

  AcdPedestalFitLibrary(FitType type)
    :_type(type){}

  AcdPedestalFitLibrary(){}

  virtual ~AcdPedestalFitLibrary() {;}
  
  virtual UInt_t fit(AcdPedestalFitResult& result, const TH1& hist);

  inline FitType fitType() const { return _type; };
  inline void setFitType(FitType type) { _type = type; };

protected:

  UInt_t fitMean(AcdPedestalFitResult& result, const TH1& hist);
  UInt_t fitPeak(AcdPedestalFitResult& result, const TH1& hist);

private:
  
  FitType _type;

  ClassDef(AcdPedestalFitLibrary,0) ;
};


#endif

#ifdef AcdPedestalFitLibrary_cxx

#endif // #ifdef AcdPedestalFitLibrary_cxx
