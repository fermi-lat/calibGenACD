#ifndef AcdCalibVersion_h
#define AcdCalibVersion_h

#include "Rtypes.h"

class AcdCalibVersion {

public:

  static const char* fmtVersion() {
    return "v1r1";
  }
  static const char* dtdVersion() {
    return "v1r1";
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
