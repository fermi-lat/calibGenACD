
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
 * @brief Ribbon MIP peak calibration fitting library
 *
 * Algorithms are same as for gain fit, but done for each segment seperately.
 * Calibration parameters are ratios relative to central segment
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdRibbonFit.h,v 1.3 2008/08/01 00:08:26 echarles Exp $
 */

class AcdRibbonFitLibrary : public AcdGainFitLibrary {

public:

  static Float_t getLocalX(int id, Float_t x, Float_t y, Float_t z);

  static Int_t getBin(int id, Float_t localY);

public:

  /// Standard c'tor, specify fit type
  AcdRibbonFitLibrary(FitType type)
    :AcdGainFitLibrary(&CalibData::AcdRibbonFitDesc::instance(), type){}

  virtual ~AcdRibbonFitLibrary() {;}

  /**
   * @brief Fit a single channel and store the result
   * @param result is the result of the fit
   * @param holder is the set of histograms to be fit
   * @param ref is an optional reference result that may be use to seed the fit
   * @return 0 for success, a failure code otherwise  
   **/ 
  virtual Int_t fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder,
		    CalibData::AcdCalibObj* ref = 0);

protected:

private:
  
};


#endif
