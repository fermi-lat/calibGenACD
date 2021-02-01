#ifndef AcdHighRangeFit_h
#define AcdHighRangeFit_h

// Base classes
#include "AcdCalibFit.h"

#include "CalibData/Acd/AcdHighRange.h"


// stl includes
#include <string>

// ROOT includes
#include "TH1.h"

// forward declares
class AcdHistCalibMap;


/** 
 * @class AcdHighRangeFitLibrary
 *
 * @brief CNO calibration fitting library
 *
 * Algorithms are:
 *  - Convert:    fit using Minuit and form PHA = [ped] + ( [slope]*[saturation]*q / ([slope]*q + [saturation]) )
 *      Fit is fully constrained by using 2 points and saturation == 2000 pha counts
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdHighRangeFit.h,v 1.3 2008/08/01 00:08:25 echarles Exp $
 */

class AcdHighRangeFitLibrary : public AcdCalibFit {

public:

  enum FitType { None = 0, 
		 Convert = 1 };
public:

  /// Standard c'tor, specify fit type
  AcdHighRangeFitLibrary(FitType type)
    :AcdCalibFit(&CalibData::AcdHighRangeFitDesc::instance()),
    _type(type){}

  AcdHighRangeFitLibrary(){}

  virtual ~AcdHighRangeFitLibrary() {;}

  /**
   * @brief Fit a single channel and store the result
   * @param result is the result of the fit
   * @param holder is the set of histograms to be fit
   * @param ref is an optional reference result that may be use to seed the fit
   * @return 0 for success, a failure code otherwise  
   **/ 
  virtual Int_t fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder,
		    CalibData::AcdCalibObj* ref );

  /// return the fitting alogrithm
  inline FitType fitType() const { return _type; };

  ///  set the fitting algoritm
  inline void setFitType(FitType type) { _type = type; };

  /// return the name of the algorithm
  virtual const char* algorithm() const {
    static const char* names[2] = {"None","Convert"};
    return names[_type];
  }

protected:

private:

  /// Algorithm to use
  FitType _type;

};

#endif


