#ifndef AcdCalibUtil_h
#define AcdCalibUtil_h 

#include "TList.h"

class AcdHistCalibMap;
class AcdPedestalFit;
class AcdPedestalFitMap;
class AcdGainFit;
class AcdGainFitMap;
class TH1;

class AcdCalibUtil {

public :

  // Standard ctor
  AcdCalibUtil();
  
  virtual ~AcdCalibUtil();  
   
  // save a list of canvases to files
  static void saveCanvases(TList& cl, const char* filePrefix = "", const char* suffix = ".ps");

  // draw mip peaks onto canvaes
  static void drawMips(TList& cl, AcdHistCalibMap& h, AcdGainFitMap& gains, 
		       Bool_t onLog = kTRUE, const char* prefix = "");

  static UShort_t gemId(UInt_t id);

  static Float_t efficDivide(TH1& out, const TH1& top, const TH1& bot, Bool_t inEffic);

  static Float_t width(UInt_t id);

private:

  ClassDef(AcdCalibUtil,0) ;
    
};

#endif
