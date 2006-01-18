#ifndef AcdCalibVersion_h
#define AcdCalibVersion_h

#include "Rtypes.h"

class AcdCalibVersion {

public:

  static const char* version() {
    return "v1r0p0";
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
