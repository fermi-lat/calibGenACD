
#ifndef AcdCarbonFit_h
#define AcdCarbonFit_h

// Base classes
#include "AcdCalibFit.h"

#include "CalibData/Acd/AcdCarbon.h"

// stl includes
#include <string>

// ROOT includes
#include "TH1.h"

// forward declares
class AcdHistCalibMap;

/** 
 * @class AcdCarbonFitLibrary
 *
 * @brief ACD carbon peak calibration fitting library
 *
 * Algorithms are:
 * - Stats:
 *    - Just use mean and rms
 * - Fallback:
 *    - Scan histogram for features, can be used to seed other methods
 * - Gauss:
 *    - Fit peak region to a gaussian
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdCarbonFit.h,v 1.4 2010/06/24 23:52:42 echarles Exp $
 */


class AcdCarbonFitLibrary : public  AcdCalibFit{

public:

  enum FitType { None = 0, 
		 Stats = 1,
		 Fallback = 2,
		 Gauss = 3 };
  
public:

  /// Standard c'tor, specify fit type
  AcdCarbonFitLibrary(FitType type)
    :AcdCalibFit(&CalibData::AcdCarbonFitDesc::instance()),
    _type(type){
  }

  AcdCarbonFitLibrary(){}

  virtual ~AcdCarbonFitLibrary() {;}

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
    static const char* names[4] = {"None","Stats","Fallback","Gauss"};
    return names[_type];
  }
  
 protected:
  
  /// Just return mean, rms.  really stupid, but can be used as seed for various other fits
  Int_t stats(CalibData::AcdCalibObj& result, const TH1& hist);
  /// Extract various feature, can be used as seed for various other fits
  Int_t fallback(CalibData::AcdCalibObj& result, const TH1& hist);  
  /// Fit peak to a gaussian
  Int_t fitGauss(CalibData::AcdCalibObj& result, const TH1& hist, const CalibData::AcdCalibObj* seed);

private:

  /// Algorithm to use
  FitType _type;

};


#endif
