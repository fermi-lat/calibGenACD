
#ifndef AcdGainFit_h
#define AcdGainFit_h

// Base classes
#include "AcdCalibFit.h"

#include "CalibData/Acd/AcdGain.h"


// stl includes
#include <string>

// ROOT includes
#include "TH1.h"

// forward declares
class AcdHistCalibMap;

/** 
 * @class AcdGainFitLibrary
 *
 * @brief MIP peak calibration fitting library
 *
 * Algorithms are:
 * - Stats:
 *    - Just return mean, rms.  really stupid, but can be used as seed for various other fits
 * - Fallback:
 *    - Extract various feature, can be used as seed for various other fits
 * - Landau:
 *    - Fit to a landau distribution.  Too narrow
 * - P7:
 *    - Fit to a 7th order polynomial.  Only point is to smooth peak
 * - P5:
 *    - Fit to a 5th order polynomial.  Only point is to smooth peak
 * - LogNormal:
 *    - Fit to a lognormal distribution.  Slow, requires good seed
 * - GaussP1 
 *    - Fit to a Gaussian and a line.  Slow, requires good seed
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdGainFit.h,v 1.13 2008/08/01 00:08:25 echarles Exp $
 */

class AcdGainFitLibrary : public AcdCalibFit {

public:
  
  /// Find first bin with entries
  static Int_t findLowestNonZeroBin(const TH1& hist);
  /// Scan until next local MAX
  static Int_t findNextLocalMax(const TH1& hist, Int_t startBin);
  /// Scan until next local MIN
  static Int_t findNextLocalMin(const TH1& hist, Int_t startBin); 
  /// Scan until 1/2 down from MAX
  static Int_t findHalfMaxHigh(const TH1& hist, Int_t maxBin);
  /// Scan until 1/2 down from MAX
  static Int_t findHalfMaxLow(const TH1& hist, Int_t maxBin);
  /// Find the highest bin, starting at startBin
  static Int_t findMaxAfter(const TH1& hist, Int_t startBin);

  /**
   * @brief Scan distribution to find some feautres
   * @param ped bin at pedestal value
   * @param min  bin at minimum between pedestal and peak
   * @param peak bin at peak value
   * @param halfMax bin at half-max past peak
   * @return success code
   **/
  static Int_t extractFeatures(const TH1& hist, Int_t rebin, 
			       Int_t& ped, Int_t& min, Int_t& peak, Int_t& halfMax);

public:

  enum FitType { None = 0, 
		 Stats = 1,
		 Fallback = 2,
		 Landau = 3,
		 P7 = 4,
		 P3 = 5,
		 LogNormal = 6, 
		 GaussP1 = 7};

public:

  /// Standard c'tor, specify fit type
  AcdGainFitLibrary(const CalibData::AcdCalibDescription* desc, FitType type)
    :AcdCalibFit(desc),
     _type(type){}

  /// D'tor is a no-op
  virtual ~AcdGainFitLibrary() {;}

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
    static const char* names[8] = {"None","Stats","Fallback","Landau","P7","P3","LogNormal","GaussP1"};
    return names[_type];
  }

protected:

  /// Just return mean, rms.  really stupid, but can be used as seed for various other fits
  Int_t stats(CalibData::AcdCalibObj& result, const TH1& hist);
  /// Extract various feature, can be used as seed for various other fits
  Int_t fallback(CalibData::AcdCalibObj& result, const TH1& hist);
  /// Fit to a landau distribution.  Too narrow
  Int_t fitLandau(CalibData::AcdCalibObj& result, const TH1& hist);
  /// Fit to a 7th order polynomial.  Only point is to smooth peak
  Int_t fitP7(CalibData::AcdCalibObj& result, const TH1& hist);
  /// Fit to a 3rd order polynomial.  Only point is to smooth peak
  Int_t fitP3(CalibData::AcdCalibObj& result, const TH1& hist);
  /// Fit to a lognormal distribution.  Slow, requires good seed
  Int_t fitLogNormal(CalibData::AcdCalibObj& result, const TH1& hist);
  /// Fit to a Gaussian and a line
  Int_t fitGaussP1(CalibData::AcdCalibObj& result, const TH1& hist, const CalibData::AcdCalibObj* seed);

private:
  
  /// Algorithm to use
  FitType _type;

};


#endif
