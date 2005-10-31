
#ifndef AcdGainFitLibrary_h
#define AcdGainFitLibrary_h

#include "AcdGainFit.h"
#include "TH1.h"

class AcdHistCalibMap;

class AcdGainFitLibrary : public AcdGainFit {

public:
  
  static Int_t findLowestNonZeroBin(const TH1& hist);
  static Int_t findNextLocalMax(const TH1& hist, Int_t startBin);
  static Int_t findNextLocalMin(const TH1& hist, Int_t startBin); 
  static Int_t findHalfMaxHigh(const TH1& hist, Int_t maxBin);
  static Int_t findHalfMaxLow(const TH1& hist, Int_t maxBin);

  static Int_t extractFeatures(const TH1& hist, Int_t rebin, Int_t& ped, Int_t& min, Int_t& peak, Int_t& halfMax);

public:

  enum FitType { None = 0, 
		 Stats = 1,
		 Fallback = 2,
		 Landau = 3,
		 P7 = 4,
		 LogNormal = 5 };

public:

  AcdGainFitLibrary(FitType type)
    :_type(type){}

  AcdGainFitLibrary(){}

  virtual ~AcdGainFitLibrary() {;}
  
  virtual Int_t fit(AcdGainFitResult& result, const TH1& hist);

  inline FitType fitType() const { return _type; };
  inline void setFitType(FitType type) { _type = type; };

protected:

  Int_t stats(AcdGainFitResult& result, const TH1& hist);
  Int_t fallback(AcdGainFitResult& result, const TH1& hist);
  Int_t fitLandau(AcdGainFitResult& result, const TH1& hist);
  Int_t fitP7(AcdGainFitResult& result, const TH1& hist);
  Int_t fitLogNormal(AcdGainFitResult& result, const TH1& hist);

private:
  
  FitType _type;

  ClassDef(AcdGainFitLibrary,0) ;
};


#endif

#ifdef AcdGainFitLibrary_cxx

#endif // #ifdef AcdGainFitLibrary_cxx
