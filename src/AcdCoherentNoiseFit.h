#ifndef AcdCoherentNoiseFit_h
#define AcdCoherentNoiseFit_h

// Base classes
#include "AcdCalibFit.h"

#include "CalibData/Acd/AcdCoherentNoise.h"


// stl includes
#include <string>

// ROOT includes
#include "TH1.h"

// forward declares
class AcdHistCalibMap;


/** 
 * @class AcdCoherentNoiseFitLibrary
 *
 * @brief CNO calibration fitting library
 *
 * Algorithms are:
 *  - Minuit:      fit using Minuit and form y = [0] * exp(-x/[1]) * sin(x*[2] + [3])
 *      Success depends mainly on good seed value
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdCoherentNoiseFit.h,v 1.8 2008/08/01 00:08:25 echarles Exp $
 */

class AcdCoherentNoiseFitLibrary : public AcdCalibFit {

public:

  enum FitType { None = 0, 
		 Minuit = 1 };

public:

  /// Standard c'tor, specify fit type
  AcdCoherentNoiseFitLibrary(FitType type)
    :AcdCalibFit(&CalibData::AcdCoherentNoiseFitDesc::instance()),
    _type(type){}

  AcdCoherentNoiseFitLibrary(){}

  virtual ~AcdCoherentNoiseFitLibrary() {;}

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
    static const char* names[2] = {"None","Minuit"};
    return names[_type];
  }

protected:

private:

  /// Algorithm to use
  FitType _type;

};




#endif

#ifdef AcdCoherentNoiseFit_cxx

#endif // #ifdef AcdCoherentNoiseFit_cxx
