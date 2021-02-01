
#ifndef AcdPedestalFit_h
#define AcdPedestalFit_h

// Base classes
#include "AcdCalibFit.h"

#include "CalibData/Acd/AcdPed.h"
#include "CalibData/Acd/AcdHighPed.h"

// stl includes
#include <string>

// ROOT includes
#include "TH1.h"

// forward declares
class AcdHistCalibMap;

/** 
 * @class AcdPedestalFitLibrary
 *
 * @brief ACD pedestal calibration fitting library
 *
 * Algorithms are:
 * - PeakValue:
 *    - Find the Mode of the distribution
 * - MeanValue:
 *    - Find the Mean of the distribution
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdPedestalFit.h,v 1.10 2008/08/01 00:08:26 echarles Exp $
 */


class AcdPedestalFitLibrary : public AcdCalibFit {

public:

  enum FitType { None = 0, 
		 PeakValue = 1,
		 MeanValue = 2 };

public:

  /// Standard c'tor, specify fit type
  AcdPedestalFitLibrary(FitType type, Bool_t high = kFALSE)
    :AcdCalibFit(high ? 
		 (CalibData::AcdCalibDescription*)(&CalibData::AcdHighPedestalFitDesc::instance()) : 
		 (CalibData::AcdCalibDescription*)(&CalibData::AcdPedestalFitDesc::instance())),
    _type(type){}

  AcdPedestalFitLibrary(){}

  virtual ~AcdPedestalFitLibrary() {;}

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
    static const char* names[3] = {"None","PeakValue","MeanValue"};
    return names[_type];
  }

protected:

  /// Find the Mean of the distribution
  UInt_t fitMean(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder);
  /// Find the Mode of the distribution
  UInt_t fitPeak(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder);

private:

  /// Algorithm to use
  FitType _type;

};


#endif
