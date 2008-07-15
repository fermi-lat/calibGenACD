
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
 * @brief ACD range crossover calibration fitting library
 *
 * Algorithms are:
 * - Counting:
 *    - Scan histograms, looking for first and last bins with entries
 *
 * @author Eric Charles
 * $Header$
 */


class AcdCarbonFitLibrary : public  AcdCalibFit{

public:

  enum FitType { None = 0, 
		 Stats = 1,
		 Fallback = 2,
		 Gauss = 3 };
  
public:

  /// Standard c'tor, specify fit type
  AcdCarbonFitLibrary(FitType type,bool removePeds)
    :AcdCalibFit(&CalibData::AcdCarbonFitDesc::instance()),
    _type(type),
    _pedRemove(removePeds){
  }

  AcdCarbonFitLibrary(){}

  virtual ~AcdCarbonFitLibrary() {;}

  /// Do the fit, return the status
  virtual Int_t fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder);

  inline FitType fitType() const { return _type; };
  inline void setFitType(FitType type) { _type = type; };

  /// return the name of the algorithm  
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
  Int_t fitGauss(CalibData::AcdCalibObj& result, const TH1& hist);

private:

  /// Algorithm to use
  FitType _type;
  bool _pedRemove;

};


#endif

#ifdef AcdCarbonFit_cxx

#endif // #ifdef AcdCarbonFit_cxx
