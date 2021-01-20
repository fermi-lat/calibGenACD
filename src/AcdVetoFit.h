
#ifndef AcdVetoFit_h
#define AcdVetoFit_h

#include "AcdCalibFit.h"

#include "CalibData/Acd/AcdVeto.h"

#include <string>

#include "TH1.h"
class AcdHistCalibMap;

/** 
 * @class AcdVetoFitLibrary
 *
 * @brief Veto calibration fitting library
 *
 * Algorithms are:
 *  - Counting:  find first bin above 50% of maximum value. 
 *      Accuracy depends mainly on bin size
 *  - Erf:       fit using Minuit and ERF
 *      Accuracy depends mainly on good seed value (NOT implemented)
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdVetoFit.h,v 1.8 2008/08/01 00:08:26 echarles Exp $
 */

class AcdVetoFitLibrary : public  AcdCalibFit{

public:

  /// Find the first bin in a histogram above a given value
  static Int_t findFirstBinAboveVal(const TH1& hist, Float_t val);

public:

  enum FitType { None = 0, 
		 Counting = 1,
		 Erf = 2 };

public:

  /// Standard c'tor, specify fit type
  AcdVetoFitLibrary(FitType type)
    :AcdCalibFit(&CalibData::AcdVetoFitDesc::instance()),
    _type(type){}

  AcdVetoFitLibrary(){}

  virtual ~AcdVetoFitLibrary() {;}
  
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
  Int_t counting(CalibData::AcdCalibObj& result, const TH1& hist);

  /// Fit using ERF and Minuit (FIXME not implemented)  
  Int_t fitErf(CalibData::AcdCalibObj& result, const TH1& hist);

private:

  /// Algorithm to use
  FitType _type;

};

#endif
