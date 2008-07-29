
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
 * $Header$
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

  /// Do the fit, return the status
  virtual Int_t fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder, 
		    CalibData::AcdCalibObj* ref = 0);

  inline FitType fitType() const { return _type; };
  inline void setFitType(FitType type) { _type = type; };

  /// return the name of the algorithm
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

#ifdef AcdPedestalFit_cxx

#endif // #ifdef AcdPedestalFit_cxx
