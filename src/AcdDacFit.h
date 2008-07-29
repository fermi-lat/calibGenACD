
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
 *  - Counting:  find first bin above 50% of maximum value. 
 *      Accuracy depends mainly on bin size
 *  - Erf:       fit using Minuit and ERF
 *      Accuracy depends mainly on good seed value
 *
 * @author Eric Charles
 * $Header$
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
  
  /// Do the fit, return the status
  virtual Int_t fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder,
		    CalibData::AcdCalibObj* ref = 0);

  inline FitType fitType() const { return _type; };
  inline void setFitType(FitType type) { _type = type; };

  /// return the name of the algorithm
  virtual const char* algorithm() const {
    static const char* names[2] = {"None","Lienar"};
    return names[_type];
  }

protected:

  /// Fit using counting algorithm
  Int_t fitLinear(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& hist);

private:

  /// Algorithm to use
  FitType _type;

};

#endif

#ifdef AcdDacFit_cxx

#endif // #ifdef AcdDacFit_cxx
