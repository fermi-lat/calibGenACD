
#ifndef AcdRangeFit_h
#define AcdRangeFit_h

// Base classes
#include "AcdCalibFit.h"

#include "CalibData/Acd/AcdRange.h"

// stl includes
#include <string>

// ROOT includes
#include "TH1.h"

// forward declares
class AcdHistCalibMap;

/** 
 * @class AcdRangeFitLibrary
 *
 * @brief ACD range crossover calibration fitting library
 *
 * Algorithms are:
 * - Counting:
 *    - Scan histograms, looking for first and last bins with entries
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdRangeFit.h,v 1.6 2008/08/01 00:08:26 echarles Exp $
 */


class AcdRangeFitLibrary : public  AcdCalibFit{

public:

  enum FitType { None = 0, 
		 Counting = 1 };

public:

  /// Standard c'tor, specify fit type
  AcdRangeFitLibrary(FitType type)
    :AcdCalibFit(&CalibData::AcdRangeFitDesc::instance()),
    _type(type){}

  AcdRangeFitLibrary(){}

  virtual ~AcdRangeFitLibrary() {;}

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
    static const char* names[2] = {"None","Counting"};
    return names[_type];
  }

protected:

private:

  /// Algorithm to use
  FitType _type;

};


#endif
