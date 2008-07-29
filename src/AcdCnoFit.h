
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

/** 
 * @class AcdCnoFitLibrary
 *
 * @brief CNO calibration fitting library
 *
 * Algorithms are:
 *  - Counting:  find first bin above 50% of maximum value. 
 *      Accuracy depends mainly on bin size
 *  - Erf:       fit using Minuit and ERF
 *      Accuracy depends mainly on good seed value
 *
 * @author Eric Charles
 * $Header$
 */

class AcdCnoFitLibrary : public AcdCalibFit {

public:
  
  /// Find the first bin in a histogram above a given value
  static Int_t findFirstBinAboveVal(const TH1& hist, Float_t val, Int_t start);

public:

  enum FitType { None = 0, 
		 Counting = 1,
		 Erf = 2 };

public:

  /// Standard c'tor, specify fit type
  AcdCnoFitLibrary(FitType type)
    :AcdCalibFit(&CalibData::AcdCnoFitDesc::instance()),
    _type(type){}

  AcdCnoFitLibrary(){}
  virtual ~AcdCnoFitLibrary() {;}
  
  /// Do the fit, return the status
  virtual Int_t fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder,
		    CalibData::AcdCalibObj* ref = 0);

  inline FitType fitType() const { return _type; };
  inline void setFitType(FitType type) { _type = type; };

  /// return the name of the algorithm
  virtual const char* algorithm() const {
    static const char* names[3] = {"None","Counting","Erf"};
    return names[_type];
  }

protected:

  /// Fit using counting algorithm
  Int_t counting(CalibData::AcdCalibObj& result, const TH1& hist, Int_t start);

  /// Fit using ERF and Minuit (FIXME not implemented)
  Int_t fitErf(CalibData::AcdCalibObj& result, const TH1& hist);

private:
  
  /// Algorithm to use
  FitType _type;

};

#endif

#ifdef AcdCnoFit_cxx

#endif // #ifdef AcdCnoFit_cxx
