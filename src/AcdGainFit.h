
#ifndef AcdGainFit_h
#define AcdGainFit_h

// Base classes
#include "AcdCalibFit.h"

#include "CalibData/Acd/AcdGain.h"


// stl includes
#include <string>

// ROOT includes
#include "TH1.h"

// forward declares
class AcdHistCalibMap;


class AcdGainFitLibrary : public AcdCalibFit {

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
    :AcdCalibFit(&CalibData::AcdGainFitDesc::instance()),
     _type(type),_pedRemove(pedRemove){}

  AcdGainFitLibrary(){}

  virtual ~AcdGainFitLibrary() {;}
  
  virtual Int_t fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder);

  inline FitType fitType() const { return _type; };
  inline void setFitType(FitType type) { _type = type; };

  inline Bool_t pedRemove() const { return _pedRemove; };
  inline void setPedRemove(Bool_t val) { _pedRemove = val; };

  virtual const char* algorithm() const {
    static const char* names[7] = {"None","Stats","Fallback","Landau","P7","P5","LogNormal"};
    return names[_type];
  }

protected:

  Int_t stats(CalibData::AcdCalibObj& result, const TH1& hist);
  Int_t fallback(CalibData::AcdCalibObj& result, const TH1& hist);
  Int_t fitLandau(CalibData::AcdCalibObj& result, const TH1& hist);
  Int_t fitP7(CalibData::AcdCalibObj& result, const TH1& hist);
  Int_t fitP5(CalibData::AcdCalibObj& result, const TH1& hist);
  Int_t fitLogNormal(CalibData::AcdCalibObj& result, const TH1& hist);

private:
  
  FitType _type;
  Bool_t _pedRemove;

};


#endif

#ifdef AcdGainFit_cxx

#endif // #ifdef AcdGainFit_cxx
