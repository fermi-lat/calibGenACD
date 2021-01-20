
#ifndef AcdCalibFit_h
#define AcdCalibFit_h

#include "Rtypes.h"
#include "AcdKey.h"

// forward declares
class AcdHistCalibMap;
class AcdCalibHistHolder;
class AcdCalibMap;
class TH1;

namespace CalibData {
  class AcdCalibDescription;
  class AcdCalibObj;
}

/** 
 * @class AcdCalibFit 
 *
 * @brief Base class for object that makes calibration fits
 *
 * The main point of this class is implement the fitAll() function, which 
 * just loops on all the channels and calls fit(), which is implemented in the various sub-classes
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdCalibFit.h,v 1.6 2008/09/05 22:57:50 echarles Exp $
 */


class AcdCalibFit {

public:

  /// Build from a calibration description
  AcdCalibFit(const CalibData::AcdCalibDescription* desc);
  /// Null c'tor
  AcdCalibFit();

  /// Trivial d'tor
  virtual ~AcdCalibFit() {;}
  
  /**
   * @brief Fit a single channel and store the result
   * @param result is the result of the fit
   * @param holder is the set of histograms to be fit
   * @param ref is an optional reference result that may be use to seed the fit
   * @return 0 for success, a failure code otherwise  
   **/
  virtual Int_t fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder, 
		    CalibData::AcdCalibObj* ref = 0);

  /**
   * @brief Fit a single channel and store the result
   * @param result is the result of the fit
   * @param input is the set of all the input histograms
   * @param key is the id of the channel to be fit
   * @return 0 for success, a failure code otherwise  
   **/
  virtual UInt_t fitChannel(AcdCalibMap& result, AcdHistCalibMap& input, UInt_t key);

  /**
   * @brief Fit all the channels and store the results
   * @param results are the result of all the fits
   * @param hists are the set of all the input histograms
   **/
  void fitAll(AcdCalibMap& results, AcdHistCalibMap& hists, AcdKey::ChannelSet cSet = AcdKey::All );

  /// return the name of the fitting algorith
  virtual const char* algorithm() const {
    static const char* def("Default");
    return def;
  }

  /// return the calibration description
  const CalibData::AcdCalibDescription* desc() const { return _desc; }

private:
  
  /// A desciption of the calibration data
  const CalibData::AcdCalibDescription* _desc;
  
};


#endif
