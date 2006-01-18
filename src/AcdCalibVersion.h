#ifndef AcdCalibVersion_h
#define AcdCalibVersion_h

#include "Rtypes.h"
#include <string>

class AcdCalibVersion {

public:

  static const char* cvsName() {
    return "$Name$";
  }
  
  static const char* version() {
    const char* retVal = cvsName();
    retVal += 7;
    return retVal;
  }
  
public:

  AcdCalibVersion( ){;};
  virtual ~AcdCalibVersion(){;};

private:

  ClassDef(AcdCalibVersion,0) ;

};

#endif

#ifdef AcdCalibVersion_cxx

 
#endif
