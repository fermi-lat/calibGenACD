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
 *  - Minuit:      fit using Minuit and form y = [0] * exp(-x/[1]) * sin(x*[2] + [3]
 *      Success depends mainly on good seed value
 *
 * @author Eric Charles
 * $Header$
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

  /// Do the fit, return the status
  virtual Int_t fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder,
		    CalibData::AcdCalibObj* ref );

  inline FitType fitType() const { return _type; };
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

#ifdef AcdHighRangeFit_cxx

#endif // #ifdef AcdHighRangeFit_cxx
