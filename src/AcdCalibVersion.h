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
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdCalibVersion.h,v 1.9 2010/06/24 23:52:41 echarles Exp $
 */

class AcdCalibVersion {

public:

  static const char* fmtVersion() {
    return "v3r2";
  }
  static const char* dtdVersion() {
    return "v3r2";
  }
  
public:

  AcdCalibVersion( ){;};
  virtual ~AcdCalibVersion(){;};

private:

};

#endif

#ifdef AcdCalibVersion_cxx

 
#endif
