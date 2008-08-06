#ifndef AcdCalibVersion_h
#define AcdCalibVersion_h

#include "Rtypes.h"

/** 
 * @class AcdCalibVersion
 *
 * @brief Keeps track of version of ACD calibration
 *
 * FIXME
 * Not really sure how this works. 
 *
 * @author Eric Charles
 * $Header$
 */

class AcdCalibVersion {

public:

  static const char* fmtVersion() {
    return "v3r1";
  }
  static const char* dtdVersion() {
    return "v3r1";
  }
  
public:

  AcdCalibVersion( ){;};
  virtual ~AcdCalibVersion(){;};

private:

};

#endif

#ifdef AcdCalibVersion_cxx

 
#endif
