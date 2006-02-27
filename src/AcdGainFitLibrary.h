
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

  static Int_t extractFeatures(Bool_t removePed, const TH1& hist, Int_t rebin, 
			       Int_t& ped, Int_t& min, Int_t& peak, Int_t& halfMax);

public:

  enum FitType { None = 0, 
		 Stats = 1,
		 Fallback = 2,
		 Landau = 3,
		 P7 = 4,
		 P5 = 5,
		 LogNormal = 6 };

public:

  AcdGainFitLibrary(FitType type, Bool_t pedRemove = kTRUE)
    :_type(type),_pedRemove(pedRemove){}

  AcdGainFitLibrary(){}

  virtual ~AcdGainFitLibrary() {;}
  
  virtual Int_t fit(AcdGainFitResult& result, const TH1& hist);

  inline FitType fitType() const { return _type; };
  inline void setFitType(FitType type) { _type = type; };

  inline Bool_t pedRemove() const { return _pedRemove; };
  inline void setPedRemove(Bool_t val) { _pedRemove = val; };

protected:

  Int_t stats(AcdGainFitResult& result, const TH1& hist);
  Int_t fallback(AcdGainFitResult& result, const TH1& hist);
  Int_t fitLandau(AcdGainFitResult& result, const TH1& hist);
  Int_t fitP7(AcdGainFitResult& result, const TH1& hist);
  Int_t fitP5(AcdGainFitResult& result, const TH1& hist);
  Int_t fitLogNormal(AcdGainFitResult& result, const TH1& hist);

private:
  
  FitType _type;
  Bool_t _pedRemove;

  ClassDef(AcdGainFitLibrary,0) ;
};


#endif

#ifdef AcdGainFitLibrary_cxx

#endif // #ifdef AcdGainFitLibrary_cxx
