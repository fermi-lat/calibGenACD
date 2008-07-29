
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
 * $Header$
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

  /// Do the fit, return the status
  virtual Int_t fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder,
		    CalibData::AcdCalibObj* ref = 0);

  inline FitType fitType() const { return _type; };
  inline void setFitType(FitType type) { _type = type; };

  /// return the name of the algorithm  
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

#ifdef AcdRangeFit_cxx

#endif // #ifdef AcdRangeFit_cxx
