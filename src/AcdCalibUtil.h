#ifndef AcdCalibUtil_h
#define AcdCalibUtil_h 

#include "TList.h"

class AcdHistCalibMap;
class AcdPedestalFit;
class AcdPedestalFitMap;
class AcdGainFit;
class AcdGainFitMap;
class AcdGainFitResult;
class AcdCnoFitMap;
class AcdCnoFitResult;
class AcdVetoFitMap;
class AcdVetoFitResult;
class AcdPadMap;
class TVirtualPad;
class TH1;

class AcdCalibUtil {

public :

  enum STRIPTYPE{ PLAIN,
		  MEAN_ABSOLUTE,
		  MEAN_RELATIVE,
		  MEAN_SIGMA };

  // Standard ctor
  AcdCalibUtil();
  
  virtual ~AcdCalibUtil();  
   
  // save a list of canvases to files
  static void saveCanvases(TList& cl, const char* filePrefix = "", const char* suffix = ".ps");

  // draw a single mip plot.
  static void drawMipPlot(TVirtualPad& vp, TH1& hist, AcdGainFitResult* res, Bool_t onLog);

  // draw a single veto plot
  static void drawVetoPlot(TVirtualPad& vp, TH1& hVeto, TH1& hAll, AcdVetoFitResult* res);

  // draw a single veto plot
  static void drawCnoPlot(TVirtualPad& vp, TH1& hVeto, TH1& hAll, AcdCnoFitResult* res);

  // draw veto plots onto canvases
  static AcdPadMap* drawVetos(AcdHistCalibMap& hVeto, AcdHistCalibMap& hRaw,
			      AcdVetoFitMap& vetos, const char* prefix = "");

  // draw veto plots onto canvases
  static AcdPadMap* drawCnos(AcdHistCalibMap& hVeto, AcdHistCalibMap& hRaw,
			     AcdCnoFitMap& vetos, const char* prefix = "");

  // draw mip peaks onto canvases
  static AcdPadMap* drawMips(AcdHistCalibMap& h, AcdGainFitMap& gains, 
			     Bool_t onLog = kTRUE, const char* prefix = "");

  // draw strip charts onto canvases
  static AcdPadMap* drawStripCharts(AcdHistCalibMap& h, const char* prefix = "");

  // histogram the outliers 
  static void chi2Dist(const TH1& input, TH1*& output, Int_t method = PLAIN, 
		       Float_t refVal = 0., Float_t scale = 1.);

  static UShort_t gemId(UInt_t id);

  static Float_t efficDivide(TH1& out, const TH1& top, const TH1& bot, Bool_t inEffic, Float_t minBol = 0.5);

  static Float_t width(UInt_t id);

private:

  ClassDef(AcdCalibUtil,0) ;
    
};

#endif
