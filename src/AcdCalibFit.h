
#ifndef AcdCalibFit_h
#define AcdCalibFit_h

#include "Rtypes.h"

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
 *
 * @author Eric Charles
 * $Header$
 */


class AcdCalibFit {

public:

  /// Build from a calibration description
  AcdCalibFit(const CalibData::AcdCalibDescription* desc);
  /// Null c'tor
  AcdCalibFit();

  /// Trivial d'tor
  virtual ~AcdCalibFit() {;}
  
  /// Fit a single channel, store the result and return a status code
  virtual Int_t fit(CalibData::AcdCalibObj& result, const AcdCalibHistHolder& holder, 
		    CalibData::AcdCalibObj* ref = 0);

  /// Fit a single channel, store the result and return a status code
  virtual UInt_t fitChannel(AcdCalibMap& result, AcdHistCalibMap& input, UInt_t key);

  /// Fit all the channels
  void fitAll(AcdCalibMap& results, AcdHistCalibMap& hists);

  /// Return the name of the fitting algorithm
  virtual const char* algorithm() const {
    static const char* def("Default");
    return def;
  }

  /// Return the calibration description
  const CalibData::AcdCalibDescription* desc() const { return _desc; }

private:
  
  /// A desciption of the calibration data
  const CalibData::AcdCalibDescription* _desc;
  
};


#endif

#ifdef AcdCalibFit_cxx

#endif // #ifdef AcdCalibFit_cxx
