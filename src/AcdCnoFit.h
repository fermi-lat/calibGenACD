
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
 *  - Erf:       fit using Minuit and ERF (NOT implemented)
 *      Accuracy depends mainly on good seed value
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdCnoFit.h,v 1.6 2008/08/01 00:08:25 echarles Exp $
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
  
  /**
   * @brief Fit a single channel and store the result
   * @param result is the result of the fit
   * @param holder is the set of histograms to be fit
   * @param ref is an optional reference result that may be use to seed the fit
   * @return 0 for success, a failure code otherwise  
   **/
  virtual Int_t fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder,
		    CalibData::AcdCalibObj* ref = 0);

  /// return the fitting alogrithm
  inline FitType fitType() const { return _type; };

  ///  set the fitting algoritm
  inline void setFitType(FitType type) { _type = type; };

  /// return the name of the fitting algorithm  
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
