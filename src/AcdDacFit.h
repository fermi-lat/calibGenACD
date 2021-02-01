
#ifndef AcdDacFit_h
#define AcdDacFit_h

#include "AcdCalibFit.h"

#include "CalibData/Acd/AcdVetoFit.h"
#include "CalibData/Acd/AcdCnoFit.h"

#include <string>

#include "TH1.h"
class AcdHistCalibMap;

/** 
 * @class AcdDacFitLibrary
 *
 * @brief Dac calibration fitting library
 *
 * Algorithms are:
 *  - Linear:  Fit the point to a line
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdDacFit.h,v 1.2 2008/08/01 00:08:25 echarles Exp $
 */

class AcdDacFitLibrary : public  AcdCalibFit{

public:

public:

  enum FitType { None = 0, 
		 Linear = 1 };

public:

  /// Standard c'tor, specify fit type
  AcdDacFitLibrary(FitType type, Bool_t cno = kFALSE)
    :AcdCalibFit(cno ? 
		 (CalibData::AcdCalibDescription*)(&CalibData::AcdCnoFitFitDesc::instance()) : 
		 (CalibData::AcdCalibDescription*)(&CalibData::AcdVetoFitFitDesc::instance())),
    _type(type){}

  AcdDacFitLibrary(){}

  virtual ~AcdDacFitLibrary() {;}
  
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
    static const char* names[2] = {"None","Lienar"};
    return names[_type];
  }

protected:

  /// Fit using linear algorithm
  Int_t fitLinear(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& hist);

private:

  /// Algorithm to use
  FitType _type;

};

#endif
