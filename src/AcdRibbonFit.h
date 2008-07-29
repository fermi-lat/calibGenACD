
#ifndef AcdRibbonFit_h
#define AcdRibbonFit_h

// Base classes
#include "AcdGainFit.h"


// stl includes
#include <string>

// ROOT includes
#include "TH1.h"

// Local includes
#include "CalibData/Acd/AcdRibbon.h"

// forward declares
class AcdHistCalibMap;

/** 
 * @class AcdRibbonFitLibrary
 *
 * @brief MIP peak calibration fitting library
 *
 * Algorithms are same as for gain fit, but done for each segment seperately.
 * Calibration parameters are ratios relative to central segment
 *
 * @author Eric Charles
 * $Header$
 */

class AcdRibbonFitLibrary : public AcdGainFitLibrary {

public:

  static Float_t getLocalX(int id, Float_t x, Float_t y, Float_t z);

  static Int_t getBin(int id, Float_t localY);

public:

  /// Standard c'tor, specify fit type
  AcdRibbonFitLibrary(FitType type, Bool_t pedRemove = kTRUE)
    :AcdGainFitLibrary(&CalibData::AcdRibbonFitDesc::instance(), type, pedRemove){}

  AcdRibbonFitLibrary(){}

  virtual ~AcdRibbonFitLibrary() {;}

  /// Do the fit, return the status
  virtual Int_t fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder,
		    CalibData::AcdCalibObj* ref = 0);

protected:

private:
  
};


#endif

#ifdef AcdRibbonFit_cxx

#endif // #ifdef AcdRibbonFit_cxx
